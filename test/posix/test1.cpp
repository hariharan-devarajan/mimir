//
// Created by haridev on 2/11/22.
//
#include <fcntl.h>
#include <math.h>
#include <mimir/api/posix.h>
#include <stdio.h>

#include <cstring>

int main(int argc, char *argv[]) {
  using namespace mimir;
  const char *filename = "./test.dat";
  const uint32_t MB = 1024 * 1024;
  uint32_t size_of_file_mb = 8;
  uint32_t transfer_size_mb = 1;
  /**
   * Job configuration
   */
  MimirHandler job_configuration_handler;
  JobConfigurationAdvice job_conf_advice;
  job_conf_advice._job_id = 0;
  job_conf_advice._devices.emplace_back("/dev/shm/haridev", 16);
  job_conf_advice._devices.emplace_back("/home/haridev/pfs", 128);
  job_conf_advice._job_time_minutes = 30;
  job_conf_advice._num_cores_per_node = 8;
  job_conf_advice._num_gpus_per_node = 0;
  job_conf_advice._num_nodes = 1;
  job_conf_advice._priority = 100;
  printf("index of job_configuration is %d\n", job_conf_advice._index);
  /**
   * File configuration
   */
  MimirHandler file_handler;
  FileAdvice file_advice;
  file_advice._name = filename;
  file_advice._type._secondary = OperationAdviceType::INDEPENDENT_FILE;
  file_advice._per_io_data = 1 / 3.0;
  file_advice._per_io_metadata = 2 / 3.0;
  file_advice._size_mb = size_of_file_mb;
  file_advice._write_distribution._1mb_16mb = 1.0;
  file_advice._io_amount_mb = size_of_file_mb;
  file_advice._format = Format::FORMAT_BINARY;
  file_advice._priority = 100;
  file_advice_begin(file_advice, file_handler);
  int fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG);
  if (fd > 0) {
    char *data = (char *)malloc(transfer_size_mb * MB);
    memset(data, '#', transfer_size_mb * MB);
    int iterations = ceil((1.0 * size_of_file_mb) / transfer_size_mb);
    for (int i = 0; i < iterations; ++i) {
      size_t count = write(fd, data, strlen(data));
      if (count != strlen(data)) {
        fprintf(stderr, "Error writing file %s for operation index %d\n",
                filename, i);
      } else {
        fprintf(stdout, "Successfully written file %s for operation index %d\n",
                filename, i);
      }
    }
    close(fd);
    mimir::file_advice_end(file_handler);
  }
  return 0;
}