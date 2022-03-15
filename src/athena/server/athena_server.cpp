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

#include "hcl/communication/rpc_factory.h"
#include "mimir/api/mimir_interceptor.h"
#include "mimir/log/logger.h"

std::shared_ptr<athena::Server> athena::Server::instance = nullptr;
namespace athena {}  // namespace athena
athena::Server::Server(bool is_mpi) : is_server(false) {
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                          "Constructing athena::Server");
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
    _job_configuration_advice = job_conf_advices.second.begin()->second;
  } else {
    _job_configuration_advice = load_job_details();
    job_conf_advice_handler->save_advice(job_conf_key,
                                         _job_configuration_advice);
  }
  int current_rank;
  if (is_mpi)
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
  else
    current_rank = 0;
  if (current_rank % _job_configuration_advice._num_cores_per_node == 0) {
    // node server rank
    uint16_t my_server_index =
        floor(current_rank / _job_configuration_advice._num_cores_per_node);
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);
    char* conf_hostname = _job_configuration_advice._node_names[my_server_index].data();
    if(strcmp(conf_hostname,hostname) != 0) {
      mimir::Logger::Instance("ATHENA")->log(
          mimir::LOG_ERROR, "Mismatch hostname %s and conf %s for index %d", hostname, conf_hostname, my_server_index);
    }
    //assert();
    mimir::Logger::Instance("ATHENA")->log(mimir::LOG_WARN,
                                            "current_rank %d, _num_cores_per_node %d", current_rank, _job_configuration_advice._num_cores_per_node);

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
    bind_posix_calls();

    int pid = getpid();
    mimir::Logger::Instance("ATHENA")->log(
        mimir::LOG_WARN, "Started the rpc server %s on rank %d pid %d with index %d", _job_configuration_advice._node_names[my_server_index].c_str(), current_rank, pid, my_server_index);
  }
}
bool athena::Server::bind_posix_calls() {
  std::function<int(DATA, int, int)> funcOpen =
      std::bind(&athena::posix_open, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
  std::function<int(int)> funcClose =
      std::bind(&athena::posix_close, std::placeholders::_1);
  std::function<off_t(int, int, int)> funcSeek =
      std::bind(&athena::posix_lseek, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
  std::function<DATA(int, size_t)> funcRead = std::bind(
      &athena::posix_read, std::placeholders::_1, std::placeholders::_2);
  std::function<ssize_t(int, DATA, size_t)> funcWrite =
      std::bind(&athena::posix_write, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);

  std::function<bool(DATA)> funcPrefetch =
      std::bind(&athena::posix_prefetch, std::placeholders::_1);
  _rpc->bind("athena::posix::open", funcOpen);
  _rpc->bind("athena::posix::close", funcClose);
  _rpc->bind("athena::posix::lseek", funcSeek);
  _rpc->bind("athena::posix::write", funcWrite);
  _rpc->bind("athena::posix::read", funcRead);
  _rpc->bind("athena::posix::prefetch", funcPrefetch);
  return true;
}
