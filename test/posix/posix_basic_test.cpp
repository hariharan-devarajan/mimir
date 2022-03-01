//
// Created by haridev on 2/16/22.
//

#include "mimir/common/data_structure.h"
#include "mimir/advice/job_configuration_advice.h"
#include "mimir/api/job_configuration.h"
#include "mimir/advice/file_advice.h"
#include "mimir/api/posix.h"
TEST_CASE("BatchedWriteSequential",
          "[process=" + std::to_string(info.comm_size) +
              "]"
              "[operation=batched_write]"
              "[request_size=type-fixed]"
              "[repetition=" +
              std::to_string(info.num_iterations) +
              "]"
              "[pattern=sequential][file=1]") {
  pretest();
#ifdef ATHENA_PRELOAD
  using namespace mimir;
  MimirHandler job_configuration_handler;
  JobConfigurationAdvice job_conf_advice;
  job_conf_advice._job_id = 0;
  job_conf_advice._devices.emplace_back(args.shm, 16);
  job_conf_advice._devices.emplace_back(args.pfs, 128);
  job_conf_advice._job_time_minutes = 30;
  job_conf_advice._num_cores_per_node = 8;
  job_conf_advice._num_gpus_per_node = 0;
  job_conf_advice._num_nodes = 1;
  job_conf_advice._priority = 100;
  job_configuration_advice_begin(job_conf_advice, job_configuration_handler);

  MimirHandler file_handler;
  FileAdvice file_advice;
  file_advice._type._secondary = OperationAdviceType::INDEPENDENT_FILE;
  file_advice._per_io_data = info.num_iterations / (info.num_iterations + 2);
  file_advice._per_io_metadata = 2 / (info.num_iterations + 2);
  file_advice._size_mb = args.request_size * info.num_iterations / MB;
  file_advice._current_device = 1;

  if (args.request_size >= 0 && args.request_size < 4 * KB)
    file_advice._write_distribution._0_4kb = 1.0;
  else if (args.request_size >= 4 * KB && args.request_size < 64 * KB)
    file_advice._write_distribution._4_64kb = 1.0;
  if (args.request_size >= 64 * KB && args.request_size < 1 * MB)
    file_advice._write_distribution._64kb_1mb = 1.0;
  if (args.request_size >= 1 * MB && args.request_size < 16 * MB)
    file_advice._write_distribution._1mb_16mb = 1.0;
  if (args.request_size >= 16 * MB) file_advice._write_distribution._16mb = 1.0;

  file_advice._io_amount_mb = args.request_size * info.num_iterations / MB;
  file_advice._format = Format::FORMAT_BINARY;
  file_advice._priority = 100;

#endif
  std::string new_file = info.new_file, existing_file = info.existing_file;
  SECTION("write to new file always at start") {
#ifdef ATHENA_PRELOAD
    file_advice._name = info.new_file;
    file_advice_begin(file_advice, file_handler);
#endif
    test::test_open(info.new_file.c_str(), O_WRONLY | O_CREAT,
                    S_IRWXU | S_IRWXG | S_IRWXO);
    REQUIRE(test::fh_orig != -1);

    for (size_t i = 0; i < info.num_iterations; ++i) {
      test::test_seek(0, SEEK_SET);
      REQUIRE(test::status_orig == 0);
      test::test_write(info.write_data.data(), args.request_size);
      REQUIRE(test::size_written_orig == args.request_size);
      fsync(test::fh_orig);
    }
    new_file = test::GetFilenameFromFD(test::fh_orig);
    printf("I/O performed on file %s\n", new_file.c_str());
    test::test_close();
    REQUIRE(test::status_orig == 0);
    REQUIRE(fs::file_size(new_file) == args.request_size);
  }

  SECTION("write to new file") {
#ifdef ATHENA_PRELOAD
    file_advice._name = info.new_file;
    file_advice_begin(file_advice, file_handler);
#endif
    test::test_open(info.new_file.c_str(), O_WRONLY | O_CREAT, 0600);
    REQUIRE(test::fh_orig != -1);

    for (size_t i = 0; i < info.num_iterations; ++i) {
      test::test_write(info.write_data.data(), args.request_size);
      REQUIRE(test::size_written_orig == args.request_size);
      fsync(test::fh_orig);
    }
    new_file = test::GetFilenameFromFD(test::fh_orig);
    printf("I/O performed on file %s\n", new_file.c_str());
    test::test_close();
    REQUIRE(test::status_orig == 0);
    REQUIRE(fs::file_size(new_file) == args.request_size * info.num_iterations);
  }

#if defined(ATHENA_PRELOAD)
  file_advice_end(file_handler);
#endif
  posttest(new_file, existing_file, false);
}