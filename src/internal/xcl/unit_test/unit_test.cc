//
// Created by 徐琰 on 2022/2/8.
//

#include <dirent.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <xcl/unit_test/unit_test.h>

#include <cstring>
#include <string>
#include <vector>
void
UnitTest::Add(void (*func)()) {
  auto test_node = new TestNode;
  test_node->test_func = func;
  test_node->next = nullptr;
  tail_->next = test_node;
  tail_ = test_node;
}
void
UnitTest::Test() const {
  auto cur_node = head_.next;
  while (cur_node) {
	cur_node->test_func();
	cur_node = cur_node->next;
  }
}
UnitTest::~UnitTest() {
  auto cur_node = head_.next;
  TestNode *next_node;
  while (cur_node) {
	next_node = cur_node->next;
	delete cur_node;
	cur_node = next_node;
  }
}

UnitTest *UnitTest::unit_test_ = new UnitTest();

UnitTest &
UnitTest::Instance() {
  return *unit_test_;
}
void
UnitTest::TestFiles(const char *test_dir, unsigned n, ...) {
  va_list libraries;
  va_start(libraries, n);
  class SubScanner {
	void *library_ = nullptr;
	std::string command_;
	std::vector<std::string> files_;

   public:
	explicit SubScanner(unsigned n, va_list libraries) {
	  command_.assign("g++ -Wall -std=c++17 -dynamiclib -o libtest_tmp.dylib ");
	  for (auto i = 0; i < n; i++) {
		command_.append(va_arg(libraries, const char *));
		command_.append(1, ' ');
	  }
	}

	~SubScanner() {
	  if (library_) {
		::dlclose(library_);
		::remove("./libtest_tmp.dylib");
	  }
	}

   private:
	void
	CollectFiles(const char *dir_path) {
	  struct dirent *file_name;
	  DIR *dir = ::opendir(dir_path);
	  if (!dir) { return; }
	  while ((file_name = readdir(dir))) {
		if (strcmp(file_name->d_name, ".") == 0
			|| strcmp(file_name->d_name, "..") == 0) {
		  continue;
		}
		auto name = file_name->d_name;
		if (file_name->d_type == DT_DIR) {
		  std::string sub_dir = dir_path;
		  sub_dir.append(1, '/');
		  sub_dir.append(name);
		  CollectFiles(sub_dir.c_str());
		  continue;
		}
		if (strncmp(name, "test_", 5) != 0) { continue; }
		auto format = name;
		while (*format != '.') { ++format; }
		const char *source_formats[5] = {".cc", ".cpp", ".c", ".cxx", ".cu"};
		bool is_source_file = false;
		for (auto &source_format : source_formats) {
		  if (strcmp(format, source_format) == 0) {
			is_source_file = true;
			break;
		  }
		}
		if (!is_source_file) { continue; }
		std::string path = dir_path;
		path.append(1, '/');
		path.append(name);
		files_.emplace_back(path);
	  }
	}

	static char *
	SkipSpace(char *cursor) {
	  while (isspace(*cursor)) { ++cursor; }
	  return cursor;
	}

	static char *
	Match(char *cursor, bool *found) {
	  *found = false;
	  if (strncmp(cursor, "void", 4) == 0) {
		cursor += 4;
		if (isspace(*cursor)) {
		  cursor = SkipSpace(cursor);
		  if (strncmp(cursor, "Test", 4) == 0) {
			*found = true;
			return cursor;
		  }
		}
	  }
	  return cursor + 1;
	}

	void
	ProcessOneFile(const std::string &file) {
	  char file_buffer[1000];
	  char func_name[1000];
	  for (;;) {
		auto fp = ::fopen(file.c_str(), "r");
		if (!fp) { return; }
		auto ret = ::fread(file_buffer, 1, sizeof(file_buffer), fp);
		if (ret <= 0) { return; }
		char *cursor = file_buffer;
		while (cursor < file_buffer + ret) {
		  bool found;
		  cursor = Match(cursor, &found);
		  if (!found) { continue; }
		  cursor = SkipSpace(cursor);
		  auto old_cursor = cursor;
		  while (*cursor != '(') { ++cursor; }
		  auto func_name_len = cursor - old_cursor;
		  strncpy(func_name, old_cursor, func_name_len);
		  func_name[func_name_len] = 0;
		  auto func = ::dlsym(library_, func_name);
		  if (func && !(::dlerror())) {
			UnitTest::Instance().Add(reinterpret_cast<void (*)()>(func));
		  }
		}
		if (ret < sizeof(file_buffer)) { break; }
	  }
	}

   public:
	void
	Scan(const char *dir_path) {
	  struct stat s {};
	  lstat(dir_path, &s);
	  if (!S_ISDIR(s.st_mode)) { return; }
	  CollectFiles(dir_path);
	  for (auto &file : files_) {
		command_.append(file);
		command_.append(1, ' ');
	  }
	  if (system(command_.c_str()) == 0) {
		library_ = ::dlopen("./libtest_tmp.dylib", RTLD_LAZY);
	  }
	  if (!library_) { return; }
	  for (auto &file : files_) { ProcessOneFile(file); }
	}
  };
  SubScanner sub_scanner(n, libraries);
  va_end(libraries);
  sub_scanner.Scan(test_dir);
  Test();
}
