#include "ini/ini_parse.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char* COMMENT_PREFIX = ";#";

static const char* ASSIGN_OPERATORS = "=:";

static Region
__region(const char* const str, int len)
{
    Region region = {str, len};
    return region;
}

static const char*
__skipLeftSpace(const char* str, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (!isspace(str[i]))
        {
            return str + i;
        }
    }
    return NULL;
}

static const char*
__skipRightSpace(const char* str, int len)
{
    for (int i = len - 1; i >= 0; --i)
    {
        if (!isspace(str[i]))
        {
            return str + i;
        }
    }
    return NULL;
}

static const char*
__strQuery(const char* str, int len, char ch)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] == ch)
        {
            return str + i;
        }
    }
    return NULL;
}

/*
 * find a pos in range[str,str+len) which value
 * contained in str
 */
static const char*
__strQuery2(const char* str, int len, const char* chars)
{
    const char* p = chars;
    while (*p)
    {
        const char* pos = __strQuery(str, len, *p);
        if (pos)
        {
            return pos;
        }
        ++p;
    }
    return NULL;
}

static bool
__parseSection(const char* line,
               int len,
               IniCallback callback,
               void* obj,
               bool allow_inline_comment)
{
    const char* end = line + len;
    const char* start = __skipLeftSpace(line + 1, end - line - 1);
    if (!start)
    {
        return false;
    }
    const char* comment_start = __strQuery2(start, end - start, COMMENT_PREFIX);
    if (comment_start)
    {
        /*
         * comment character found after [
         * section is commented
         */
        return false;
    }
    const char* finish = __strQuery(start, end - start, ']');
    /*
     * no ], incomplete section region
     */
    if (!finish)
    {
        return false;
    }
    finish = __skipRightSpace(start, finish - start) + 1;
    Region empty_region = {NULL, 0};
    Region section_region = {.str = start, .len = finish - start};
    if (callback)
    {
        callback(obj, section_region, empty_region, empty_region);
    }
    return true;
}

/*
 * section is always empty in here
 */
static bool
__parseSectionLine(const char* line,
                   int len,
                   IniCallback callback,
                   void* obj,
                   bool allow_inline_comment)
{
    const char* end = line + len;
    const char* start = __skipLeftSpace(line, end - line);
    if (!start)
    {
        return false;
    }
    const char* comment_start = __strQuery2(start, end - start, COMMENT_PREFIX);
    if (comment_start == start)
    {
        /*
         * comment line, noop
         */
        return true;
    }
    if (comment_start)
    {
        /*
         * inline comment, such as xxx;bbb
         */
        if (allow_inline_comment)
        {
            /*
             * ignore inline comment
             */
            end = comment_start;
        }
        else
        {
            return false;
        }
    }
    Region section_region = {NULL, 0};
    Region key_region = {NULL, 0};
    Region value_region = {NULL, 0};
    const char* sep = __strQuery2(start, end - start, ASSIGN_OPERATORS);
    if (sep)
    {
        /*
         * find key
         */
        const char* key_start = start;
        const char* key_finish
            = __skipRightSpace(key_start, sep - key_start) + 1;
        key_region = __region(key_start, key_finish - key_start);
    }
    /*
     * if :or= found, value start at sep+1
     * otherwise, key is empty and treat line as value
     */
    const char* value_start
        = sep ? __skipLeftSpace(sep + 1, end - sep - 1) : start;
    if (value_start)
    {
        const char* value_finish
            = __skipRightSpace(value_start, end - value_start) + 1;
        value_region = __region(value_start, value_finish - value_start);
    }
    if (callback)
    {
        callback(obj, section_region, key_region, value_region);
    }
    return true;
}

static bool
__Ini_processLine(Ini ini,
                  Region region,
                  IniCallback callback,
                  void* obj,
                  bool* has_section)
{
    const char* line = region.str;
    if (line[0] == '[')
    {
        /*
         * find a section hint, process section
         * has_section will be used in future calls
         */
        *has_section = __parseSection(line,
                                      region.len,
                                      callback,
                                      obj,
                                      ini.allow_inline_comment);
        if (!*has_section)
        {
            return false;
        }
    }
    else
    {
        if (__strQuery(COMMENT_PREFIX, strlen(COMMENT_PREFIX), line[0]))
        {
            /*
             * ignore comment line
             */
        }
        else
        {
            if (*has_section)
            {
                bool ret = __parseSectionLine(line,
                                              region.len,
                                              callback,
                                              obj,
                                              ini.allow_inline_comment);
                if (!ret)
                {
                    return false;
                }
            }
            else
            {
                /*
                 * parse section failed but find available paramter line
                 */
                return false;
            }
        }
    }
    return true;
}

/*
 * process ini data buf
 * return last progress start pos
 * or NULL if Err_get can not be ignored
 */
static const char*
__Ini_processRegion(Ini ini,
                    Region region,
                    IniCallback callback,
                    void* obj,
                    bool* has_section)
{
    const char* buf = region.str;
    const char* buf_end = buf + region.len;
    const char* cursor = buf;
    while (cursor < buf_end)
    {
        const char* line = __skipLeftSpace(cursor, buf_end - cursor);
        if (!line)
        {
            return buf_end;
        }
        const char* line_end = __strQuery(line, buf_end - line, '\n');
        if (!line_end)
        {
            return line;
        }
        if (!__Ini_processLine(ini,
                               __region(line, line_end - line),
                               callback,
                               obj,
                               has_section))
        {
            if (!ini.allow_error)
            {
                return NULL;
            }
        }
        cursor = line_end + 1;
    }
    return buf_end;
}

static bool
__Ini_parseFromReader(Ini ini,
                      void* stream,
                      IniReadFunc reader,
                      IniCallback callback,
                      void* obj)
{
    int off = 0;
    bool has_section = false;
    for (;;)
    {
        int ret = reader(stream, ini.buf + off, ini.length - off);
        if (ret <= 0)
        {
            break;
        }
        int len = off + ret;
        const char* next = __Ini_processRegion(ini,
                                               __region(ini.buf, len),
                                               callback,
                                               obj,
                                               &has_section);
        if (!next)
        {
            return false;
        }
        off = len - (next - ini.buf);
        if (off)
        {
            strncpy(ini.buf, next, off);
        }
    }
    /*
     * the last content will be treated as a line to proceed
     */
    if (off)
    {
        if (!__Ini_processLine(ini,
                               __region(ini.buf, off),
                               callback,
                               obj,
                               &has_section))
        {
            return ini.allow_error;
        }
    }
    return true;
}

static int32_t
__readFromStr(void* stream, char* buf, int len)
{
    Region* region = (Region*)stream;
    int total = region->len;
    int ret = total > len ? len : total;
    if (ret)
    {
        strncpy(buf, region->str, ret);
        region->str += ret;
        region->len -= ret;
    }
    return ret;
}

static int32_t
__readFromFile(void* stream, char* buf, int len)
{
    return fread(buf, 1, len, (FILE*)stream);
}

bool
Ini_parseStr(Ini ini, const char* str, IniCallback callback, void* obj)
{
    Region region = {str, strlen(str)};
    return __Ini_parseFromReader(ini, &region, __readFromStr, callback, obj);
}

bool
Ini_parse(Ini ini, const char* file_path, IniCallback callback, void* obj)
{
    FILE* fp = fopen(file_path, "r");
    if (!fp)
    {
        return false;
    }
    bool ret = __Ini_parseFromReader(ini, fp, __readFromFile, callback, obj);
    fclose(fp);
    return ret;
}

bool
Ini_parseStream(Ini ini,
                void* stream,
                IniReadFunc reader,
                IniCallback callback,
                void* obj)
{
    return __Ini_parseFromReader(ini, stream, reader, callback, obj);
}
