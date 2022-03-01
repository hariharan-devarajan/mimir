
#if defined(ATHENA_PRELOAD)
#include <athena/api/stdio.h>
#else
#include <stdio.h>
#endif
#include <catch_config.h>
#include <test_utils.h>
#include <unistd.h>

#include <cstdarg>
#include <experimental/filesystem>
#include <iostream>

#ifndef O_TMPFILE
#define __O_TMPFILE 020000000
#define O_TMPFILE (__O_TMPFILE | O_DIRECTORY)
#define O_TMPFILE_MASK (__O_TMPFILE | O_DIRECTORY | O_CREAT)
#endif
namespace fs = std::experimental::filesystem;
namespace mimir::stdio::test {
struct Arguments {
  std::string filename = "test.dat";
  std::string pfs = "/home/haridev/pfs";
  std::string shm = "/dev/shm/haridev";
  size_t request_size = 65536;
};
struct Info {
  int rank = 0;
  int comm_size = 1;
  std::vector<char> write_data;
  std::vector<char> read_data;
  std::string new_file;
  std::string existing_file;
  std::string new_file_cmp;
  std::string existing_file_cmp;
  size_t num_iterations = 8;
  unsigned int offset_seed = 1;
  unsigned int rs_seed = 1;
  unsigned int temporal_interval_seed = 5;
  size_t total_size;
  size_t stride_size = 1024;
  unsigned int temporal_interval_ms = 1;
  size_t small_min = 1, small_max = 4 * 1024;
  size_t medium_min = 4 * 1024 + 1, medium_max = 256 * 1024;
  size_t large_min = 256 * 1024 + 1, large_max = 3 * 1024 * 1024;
};
}  // namespace mimir::stdio::test
mimir::stdio::test::Arguments args;
mimir::stdio::test::Info info;
std::vector<char> gen_random(const int len) {
  auto tmp_s = std::vector<char>(len);
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  srand(100);
  for (int i = 0; i < len; ++i)
    tmp_s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  return tmp_s;
}
int init(int* argc, char*** argv) {
  // MPI_Init(argc, argv);
  return 0;
}
int finalize() {
  // MPI_Finalize();
  return 0;
}

int pretest() {
  fs::create_directories(args.pfs);
  info.write_data = std::vector<char>(args.request_size, 'w');
  info.read_data = std::vector<char>(args.request_size, 'r');
  fs::path fullpath = args.pfs;
  fullpath /= args.filename;
  info.new_file = fullpath.string() + "_new_" + std::to_string(getpid());
  info.existing_file = fullpath.string() + "_ext_" + std::to_string(getpid());
  info.new_file_cmp =
      fullpath.string() + "_new_cmp" + "_" + std::to_string(getpid());
  info.existing_file_cmp =
      fullpath.string() + "_ext_cmp" + "_" + std::to_string(getpid());
  return 0;
}

int posttest(std::string new_file, std::string existing_file,
             bool compare_data = true) {
  return 0;
}

cl::Parser define_options() {
  return cl::Opt(args.filename, "filename")["-f"]["--filename"](
             "Filename used for performing I/O") |
         cl::Opt(args.pfs, "pfs")["-p"]["--pfs"](
             "Directory used for performing I/O (default pfs)") |
         cl::Opt(args.shm, "shm")["-s"]["--shm"](
             "Directory used for performing I/O (default shm)") |
         cl::Opt(args.request_size, "request_size")["-s"]["--request_size"](
             "Request size used for performing I/O");
}

namespace test {
FILE* fh_orig;
FILE* fh_cmp;
int status_orig;
size_t size_read_orig;
size_t size_written_orig;
void test_fopen(const char* path, const char* mode) {
  std::string cmp_path;
  if (strcmp(path, info.new_file.c_str()) == 0) {
    cmp_path = info.new_file_cmp;
  } else {
    cmp_path = info.existing_file_cmp;
  }
  fh_orig = fopen(path, mode);
  fh_cmp = fopen(cmp_path.c_str(), mode);
  bool is_same = (fh_cmp != nullptr && fh_orig != nullptr) ||
                 (fh_cmp == nullptr && fh_orig == nullptr);
  REQUIRE(is_same);
}
void test_fclose() {
  status_orig = fclose(fh_orig);
  int status = fclose(fh_cmp);
  REQUIRE(status == status_orig);
}
void test_fwrite(const void* ptr, size_t size) {
  size_written_orig = fwrite(ptr, sizeof(char), size, fh_orig);
  size_t size_written = fwrite(ptr, sizeof(char), size, fh_cmp);
  REQUIRE(size_written == size_written_orig);
}
void test_fread(char* ptr, size_t size) {
  size_read_orig = fread(ptr, sizeof(char), size, fh_orig);
  std::vector<unsigned char> read_data(size, 'r');
  size_t size_read = fread(read_data.data(), sizeof(char), size, fh_cmp);
  REQUIRE(size_read == size_read_orig);
  if (size_read > 0) {
    size_t unmatching_chars = 0;
    for (size_t i = 0; i < size; ++i) {
      if (read_data[i] != ptr[i]) {
        unmatching_chars = i;
        break;
      }
    }
    REQUIRE(unmatching_chars == 0);
  }
}
void test_fseek(long offset, int whence) {
  status_orig = fseek(fh_orig, offset, whence);
  int status = fseek(fh_cmp, offset, whence);
  REQUIRE(status == status_orig);
}
}  // namespace test

#include "stdio_basic_test.cpp"