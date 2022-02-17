
#if defined(ATHENA_PRELOAD)
#include <athena/api/posix.h>
#else
#include <fcntl.h>
#endif
#include <cstdarg>
#include <unistd.h>

#include <experimental/filesystem>
#include <iostream>

#include <catch_config.h>
#include <test_utils.h>

#ifndef O_TMPFILE
#define __O_TMPFILE 020000000
#define O_TMPFILE (__O_TMPFILE | O_DIRECTORY)
#define O_TMPFILE_MASK (__O_TMPFILE | O_DIRECTORY | O_CREAT)
#endif
namespace fs = std::experimental::filesystem;
const uint32_t KB = 1024;
const uint32_t MB = 1024 * 1024;
namespace mimir::posix::test {
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
        size_t num_iterations = 1024 * 2;
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
}  // namespace ::posix::test
mimir::posix::test::Arguments args;
mimir::posix::test::Info info;
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
    //MPI_Init(argc, argv);
    info.write_data = gen_random(args.request_size);
    info.read_data = std::vector<char>(args.request_size, 'r');
    return 0;
}
int finalize() {
    //MPI_Finalize();
    return 0;
}

inline std::string GetFilenameFromFD(int fd) {
    const int kMaxSize = 0xFFF;
    char proclnk[kMaxSize];
    char filename[kMaxSize];
    snprintf(proclnk, kMaxSize, "/proc/self/fd/%d", fd);
    size_t r = readlink(proclnk, filename, kMaxSize);
    filename[r] = '\0';
    return filename;
}

int pretest() {
    fs::path fullpath = args.pfs;
    fullpath /= args.filename;
    info.new_file = fullpath.string() + "_new_" + std::to_string(getpid());
    info.existing_file = fullpath.string() + "_ext_" + std::to_string(getpid());
    info.new_file_cmp =
            fullpath.string() + "_new_cmp" + "_" + std::to_string(getpid());
    info.existing_file_cmp =
            fullpath.string() + "_ext_cmp" + "_" + std::to_string(getpid());
    if (fs::exists(info.new_file)) fs::remove(info.new_file);
    if (fs::exists(info.new_file_cmp)) fs::remove(info.new_file_cmp);
    if (fs::exists(info.existing_file)) fs::remove(info.existing_file);
    if (fs::exists(info.existing_file_cmp)) fs::remove(info.existing_file_cmp);
    if (!fs::exists(info.existing_file)) {
        std::string cmd = "{ tr -dc '[:alnum:]' < /dev/urandom | head -c " +
                          std::to_string(args.request_size * info.num_iterations) +
                          "; } > " + info.existing_file + " 2> /dev/null";
        int status = system(cmd.c_str());
        REQUIRE(status != -1);
        REQUIRE(fs::file_size(info.existing_file) ==
                args.request_size * info.num_iterations);
        info.total_size = fs::file_size(info.existing_file);
    }
    if (!fs::exists(info.existing_file_cmp)) {
        std::string cmd = "cp " + info.existing_file + " " + info.existing_file_cmp;
        int status = system(cmd.c_str());
        REQUIRE(status != -1);
        REQUIRE(fs::file_size(info.existing_file_cmp) ==
                args.request_size * info.num_iterations);
    }
    REQUIRE(info.total_size > 0);
    return 0;
}

int posttest(std::string new_file, std::string existing_file, bool compare_data = true) {
    if (compare_data && fs::exists(new_file) &&
        fs::exists(info.new_file_cmp)) {
        size_t size = fs::file_size(new_file);
        REQUIRE(size == fs::file_size(info.new_file_cmp));
        if (size > 0) {
            std::vector<unsigned char> d1(size, '0');
            std::vector<unsigned char> d2(size, '1');

            FILE* fh1 = fopen(new_file.c_str(), "r");
            REQUIRE(fh1 != nullptr);
            size_t read_d1 = fread(d1.data(), size, sizeof(unsigned char), fh1);
            REQUIRE(read_d1 == sizeof(unsigned char));
            int status = fclose(fh1);
            REQUIRE(status == 0);

            FILE* fh2 = fopen(info.new_file_cmp.c_str(), "r");
            REQUIRE(fh2 != nullptr);
            size_t read_d2 = fread(d2.data(), size, sizeof(unsigned char), fh2);
            REQUIRE(read_d2 == sizeof(unsigned char));
            status = fclose(fh2);
            REQUIRE(status == 0);

            size_t char_mismatch = 0;
            for (size_t pos = 0; pos < size; ++pos) {
                if (d1[pos] != d2[pos]) char_mismatch++;
            }
            REQUIRE(char_mismatch == 0);
        }
    }
    if (compare_data && fs::exists(existing_file) &&
        fs::exists(info.existing_file_cmp)) {
        size_t size = fs::file_size(existing_file);
        if (size != fs::file_size(info.existing_file_cmp)) sleep(1);
        REQUIRE(size == fs::file_size(info.existing_file_cmp));
        if (size > 0) {
            std::vector<unsigned char> d1(size, 'r');
            std::vector<unsigned char> d2(size, 'w');

            FILE* fh1 = fopen(existing_file.c_str(), "r");
            REQUIRE(fh1 != nullptr);
            size_t read_d1 = fread(d1.data(), sizeof(unsigned char), size, fh1);
            REQUIRE(read_d1 == size);
            int status = fclose(fh1);
            REQUIRE(status == 0);

            FILE* fh2 = fopen(info.existing_file_cmp.c_str(), "r");
            REQUIRE(fh2 != nullptr);
            size_t read_d2 = fread(d2.data(), sizeof(unsigned char), size, fh2);
            REQUIRE(read_d2 == size);
            status = fclose(fh2);
            REQUIRE(status == 0);
            size_t char_mismatch = 0;
            for (size_t pos = 0; pos < size; ++pos) {
                if (d1[pos] != d2[pos]) {
                    char_mismatch = pos;
                    break;
                }
            }
            REQUIRE(char_mismatch == 0);
        }
    }
    /* Clean up. */
    if (fs::exists(new_file)) fs::remove(new_file);
    if (fs::exists(existing_file)) fs::remove(existing_file);
    if (fs::exists(info.new_file_cmp)) fs::remove(info.new_file_cmp);
    if (fs::exists(info.existing_file_cmp)) fs::remove(info.existing_file_cmp);

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
    int fh_orig;
    int fh_cmp;
    int status_orig;
    size_t size_read_orig;
    size_t size_written_orig;
    void test_open(const char* path, int flags, ...) {
        int mode = 0;
        if (flags & O_CREAT || flags & O_TMPFILE) {
            va_list arg;
            va_start(arg, flags);
            mode = va_arg(arg, int);
            va_end(arg);
        }
        std::string cmp_path;
        if (strcmp(path, info.new_file.c_str()) == 0) {
            cmp_path = info.new_file_cmp;
        } else if (strcmp(path, args.pfs.c_str()) == 0) {
            cmp_path = args.pfs;
        } else {
            cmp_path = info.existing_file_cmp;
        }
        if (flags & O_CREAT || flags & O_TMPFILE) {
            fh_orig = open(path, flags, mode);
            fh_cmp = open(cmp_path.c_str(), flags, mode);
        } else {
            fh_orig = open(path, flags);
            fh_cmp = open(cmp_path.c_str(), flags);
        }
        bool is_same =
                (fh_cmp != -1 && fh_orig != -1) || (fh_cmp == -1 && fh_orig == -1);
        REQUIRE(is_same);
    }
    void test_close() {
        status_orig = close(fh_orig);
        int status = close(fh_cmp);
        REQUIRE(status == status_orig);
    }
    void test_write(const void* ptr, size_t size) {
        size_written_orig = write(fh_orig, ptr, size);
        size_t size_written = write(fh_cmp, ptr, size);
        REQUIRE(size_written == size_written_orig);
    }
    void test_read(char* ptr, size_t size) {
        size_read_orig = read(fh_orig, ptr, size);
        std::vector<unsigned char> read_data(size, 'r');
        size_t size_read = read(fh_cmp, read_data.data(), size);
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
    void test_seek(long offset, int whence) {
        status_orig = lseek(fh_orig, offset, whence);
        int status = lseek(fh_cmp, offset, whence);
        REQUIRE(status == status_orig);
    }
}  // namespace test

#include "posix_basic_test.cpp"