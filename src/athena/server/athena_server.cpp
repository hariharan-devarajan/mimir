//
// Created by hariharan on 2/21/22.
//

#include "athena_server.h"
#include "hcl/communication/rpc_factory.h"

#include <athena/server/posix_io.h>

#include <hcl/communication/rpc_lib.h>
#include <mimir/advice/advice_handler.h>
#include <mimir/advice/advice_type.h>
#include <mimir/advice/job_configuration_advice.h>
#include <mpi.h>

#include <cmath>
#include <thallium.hpp>
#include <hcl/common/macros.h>
#include <thallium/serialization/stl/string.hpp>

std::shared_ptr<athena::Server> athena::Server::instance = nullptr;
namespace athena {
THALLIUM_DEFINE(posix_open, (filename, mode, flags), std::vector<char> filename,
                int mode, int flags);
THALLIUM_DEFINE(posix_close, (fd), int fd);
THALLIUM_DEFINE(posix_lseek, (fd, offset, whence), int fd, off_t offset,
                int whence);
THALLIUM_DEFINE(posix_write, (fd, buf, count), int fd, std::vector<char> &buf,
                size_t count);
THALLIUM_DEFINE(posix_read, (fd, count), int fd, size_t count);
}  // namespace athena
athena::Server::Server(bool is_mpi) : is_server(false) {
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
    _job_configuration_advice = job_conf_advices.second[0];
    int current_rank;
    if (is_mpi)
      MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    else
      current_rank = 0;
    if (current_rank % _job_configuration_advice._num_cores_per_node == 0) {
      // node server rank
      uint16_t my_server_index =
          ceil(current_rank / _job_configuration_advice._num_cores_per_node);
      is_server = true;
      HCL_CONF->IS_SERVER = is_server;
      HCL_CONF->MY_SERVER = my_server_index;
      HCL_CONF->NUM_SERVERS = _job_configuration_advice._num_nodes;
      HCL_CONF->SERVER_ON_NODE = true;
      for (auto &&server_name : _job_configuration_advice._node_names) {
        HCL_CONF->SERVER_LIST.emplace_back(server_name);
      }
      HCL_CONF->RPC_PORT = _job_configuration_advice._rpc_port;
      HCL_CONF->RPC_THREADS = _job_configuration_advice._rpc_threads;

      _rpc = hcl::Singleton<RPCFactory>::GetInstance()->GetRPC(
          _job_configuration_advice._rpc_port);
      std::function<void(const thallium::request &, std::vector<char> &, int,
                         int)>
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
      std::function<void(const thallium::request &, int, size_t)> funcRead =
          std::bind(&athena::Thalliumposix_read, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3);
      std::function<void(const thallium::request &, int, std::vector<char> &,
                         size_t)>
          funcWrite = std::bind(&athena::Thalliumposix_write,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3, std::placeholders::_4);
      _rpc->bind("athena::posix::open", funcOpen);
      _rpc->bind("athena::posix::close", funcClose);
      _rpc->bind("athena::posix::lseek", funcSeek);
      _rpc->bind("athena::posix::write", funcWrite);
      _rpc->bind("athena::posix::read", funcRead);
    }
  } else {
    throw std::runtime_error(
        "[ATHENA] Job Configuration Advice not set  from Athena Server.");
  }
}
