#include "xcl/ini/IniParse.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char* COMMENT_PREFIX = ";#";

static const char* ASSIGN_OPERATORS = "=:";

static Region __region(const char* const str, int len)
{
    Region region = {str, len};
    return region;
}

static const char* __skipLeftSpace(const char* str, int len)
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

static const char* __skipRightSpace(const char* str, int len)
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

static const char* __strQuery(const char* str, int len, char ch)
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
static const char* __strQuery2(const char* str, int len, const char* chars)
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

static bool __parseSection(const char* line, int len, IniCallback cb, void* usr,
                           bool allowInlineComment)
{
    const char* end = line + len;
    const char* start = __skipLeftSpace(line + 1, end - line - 1);
    if (!start)
    {
        return false;
    }
    const char* commentStart = __strQuery2(start, end - start, COMMENT_PREFIX);
    if (commentStart)
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
    Region emptyRegion = {NULL, 0};
    Region sectionRegion = {.str = start, .len = finish - start};
    if (cb)
    {
        cb(usr, sectionRegion, emptyRegion, emptyRegion);
    }
    return true;
}

/*
 * section is always empty in here
 */
static bool __parseSectionLine(const char* line, int len, IniCallback cb,
                               void* usr, bool allowInlineComment)
{
    const char* end = line + len;
    const char* start = __skipLeftSpace(line, end - line);
    if (!start)
    {
        return false;
    }
    const char* commentStart = __strQuery2(start, end - start, COMMENT_PREFIX);
    if (commentStart == start)
    {
        /*
         * comment line, noop
         */
        return true;
    }
    if (commentStart)
    {
        /*
         * inline comment, such as xxx;bbb
         */
        if (allowInlineComment)
        {
            /*
             * ignore inline comment
             */
            end = commentStart;
        }
        else
        {
            return false;
        }
    }
    Region sectionRegion = {NULL, 0};
    Region keyRegion = {NULL, 0};
    Region valueRegion = {NULL, 0};
    const char* sep = __strQuery2(start, end - start, ASSIGN_OPERATORS);
    if (sep)
    {
        /*
         * find key
         */
        const char* keyStart = start;
        const char* keyFinish = __skipRightSpace(keyStart, sep - keyStart) + 1;
        keyRegion = __region(keyStart, keyFinish - keyStart);
    }
    /*
     * if :or= found, value start at sep+1
     * otherwise, key is empty and treat line as value
     */
    const char* valueStart =
        sep ? __skipLeftSpace(sep + 1, end - sep - 1) : start;
    if (valueStart)
    {
        const char* valueFinish =
            __skipRightSpace(valueStart, end - valueStart) + 1;
        valueRegion = __region(valueStart, valueFinish - valueStart);
    }
    if (cb)
    {
        cb(usr, sectionRegion, keyRegion, valueRegion);
    }
    return true;
}

static bool __Ini_processLine(Ini ini, Region region, IniCallback cb, void* usr,
                              bool* hasSection)
{
    const char* line = region.str;
    if (line[0] == '[')
    {
        /*
         * find a section hint, process section
         * hasSection will be used in future calls
         */
        *hasSection =
            __parseSection(line, region.len, cb, usr, ini.allowInlineComment);
        if (!*hasSection)
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
            if (*hasSection)
            {
                bool ret = __parseSectionLine(line, region.len, cb, usr,
                                              ini.allowInlineComment);
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
static const char* __Ini_processRegion(Ini ini, Region region, IniCallback cb,
                                       void* usr, bool* hasSection)
{
    const char* buf = region.str;
    const char* bufEnd = buf + region.len;
    const char* cursor = buf;
    while (cursor < bufEnd)
    {
        const char* line = __skipLeftSpace(cursor, bufEnd - cursor);
        if (!line)
        {
            return bufEnd;
        }
        const char* lineEnd = __strQuery(line, bufEnd - line, '\n');
        if (!lineEnd)
        {
            return line;
        }
        if (!__Ini_processLine(ini, __region(line, lineEnd - line), cb, usr,
                               hasSection))
        {
            if (!ini.allowError)
            {
                return NULL;
            }
        }
        cursor = lineEnd + 1;
    }
    return bufEnd;
}

static bool __Ini_parseFromReader(Ini ini, void* stream, IniReadFunc reader,
                                  IniCallback cb, void* usr)
{
    int off = 0;
    bool hasSection = false;
    for (;;)
    {
        int ret = reader(stream, ini.buf + off, ini.length - off);
        if (ret <= 0)
        {
            break;
        }
        int len = off + ret;
        const char* next = __Ini_processRegion(ini, __region(ini.buf, len), cb,
                                               usr, &hasSection);
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
        if (!__Ini_processLine(ini, __region(ini.buf, off), cb, usr,
                               &hasSection))
        {
            return ini.allowError;
        }
    }
    return true;
}

static int32_t __readFromStr(void* stream, char* buf, int len)
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

static int32_t __readFromFile(void* stream, char* buf, int len)
{
    return fread(buf, 1, len, (FILE*)stream);
}

bool Ini_parseStr(Ini ini, const char* str, IniCallback cb, void* usr)
{
    Region region = {str, strlen(str)};
    return __Ini_parseFromReader(ini, &region, __readFromStr, cb, usr);
}

bool Ini_parse(Ini ini, const char* filePath, IniCallback cb, void* usr)
{
    FILE* fp = fopen(filePath, "r");
    if (!fp)
    {
        return false;
    }
    bool ret = __Ini_parseFromReader(ini, fp, __readFromFile, cb, usr);
    fclose(fp);
    return ret;
}

bool Ini_parseStream(Ini ini, void* stream, IniReadFunc reader, IniCallback cb,
                     void* usr)
{
    return __Ini_parseFromReader(ini, stream, reader, cb, usr);
}
