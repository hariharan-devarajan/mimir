//
// Created by hariharan on 2/21/22.
//

#include "athena_server.h"

#include <athena/server/posix_io.h>
#include <hcl/common/macros.h>
#include <hcl/communication/rpc_lib.h>
#include <mimir/advice/advice_handler.h>
#include <mimir/advice/advice_type.h>
#include <mimir/advice/job_configuration_advice.h>
#include <mpi.h>

#include <cmath>
#include <thallium/serialization/stl/string.hpp>

std::shared_ptr<athena::Server> athena::Server::instance = nullptr;
namespace athena {
THALLIUM_DEFINE(posix_open, (filename, mode, flags), std::string &filename,
                int mode, int flags);
THALLIUM_DEFINE(posix_close, (fd), int fd);
THALLIUM_DEFINE(posix_lseek, (fd, offset, whence), int fd, off_t offset,
                int whence);
THALLIUM_DEFINE(posix_write, (fd, buf, count), int fd, std::string &buf,
                int count);
THALLIUM_DEFINE(posix_read, (fd, count), int fd, int count);
}  // namespace athena
athena::Server::Server() {
  auto job_conf_type =
      mimir::AdviceType(mimir::PrimaryAdviceType::JOB_CONFIGURATION,
                        mimir::OperationAdviceType::NO_OP);
  auto job_conf_advice_handler =
      mimir::AdviceHandler<mimir::JobConfigurationAdvice>::Instance(
          job_conf_type);
  mimir::MimirKey job_conf_key;
  job_conf_key._id = 0;
  auto job_conf_advices = job_conf_advice_handler->find_advice(job_conf_key);
  if (job_conf_advices.first) {
    job_configuration_advice = job_conf_advices.second[0];
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    /* FIXME: std::shared_ptr<athena::ThalliumRPC> rpc;*/
    if (current_rank % job_configuration_advice._num_cores_per_node == 0) {
      // node server rank
      uint16_t my_server_index =
          ceil(current_rank / job_configuration_advice._num_cores_per_node);
      /*FIXME: auto rpc = athena::ThalliumRPC::Instance(
          true, job_configuration_advice._node_names,
          job_configuration_advice._network_protocol,
          job_configuration_advice._rpc_port, my_server_index,
          job_configuration_advice._rpc_threads);
      std::function<void(const thallium::request &, std::string &, int, int)>
          funcOpen = std::bind(&athena::Thalliumposix_open,
                               std::placeholders::_1, std::placeholders::_2,
                               std::placeholders::_3, std::placeholders::_4);
      std::function<void(const thallium::request &, int)> funcClose =
          std::bind(&athena::Thalliumposix_close, std::placeholders::_1,
                    std::placeholders::_2);
      std::function<void(const thallium::request &, int, int, int)> funcSeek =
          std::bind(&athena::Thalliumposix_lseek, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3,
                    std::placeholders::_4);
      std::function<void(const thallium::request &, int, int)> funcRead =
          std::bind(&athena::Thalliumposix_read, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3);
      std::function<void(const thallium::request &, int, std::string &, int)>
          funcWrite = std::bind(&athena::Thalliumposix_write,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3, std::placeholders::_4);
      rpc->bind("athena::posix::open", funcOpen);
      rpc->bind("athena::posix::close", funcClose);
      rpc->bind("athena::posix::lseek", funcSeek);
      rpc->bind("athena::posix::write", funcWrite);
      rpc->bind("athena::posix::read", funcRead);*/
    }
  } else {
    throw std::runtime_error("[Athena] Job Configuration Advice not set.");
  }
}
