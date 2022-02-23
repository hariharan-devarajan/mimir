//
// Created by hariharan on 2/21/22.
//

#ifndef MIMIR_ATHENA_CLIENT_H
#define MIMIR_ATHENA_CLIENT_H

#include <hcl/communication/rpc_lib.h>
#include <mpi.h>

#include <memory>

#include "mimir/advice/advice_handler.h"
#include "mimir/advice/job_configuration_advice.h"
#include "mimir/log/logger.h"
#include "athena/api/interceptor.h"
#include "hcl/communication/rpc_factory.h"

namespace athena {
class Client {
 private:
  static std::shared_ptr<Client> instance;

 public:
  mimir::JobConfigurationAdvice _job_configuration_advice;
  std::shared_ptr<mimir::AdviceHandler<mimir::JobConfigurationAdvice>>
      _job_handler;
  std::unordered_map<std::string, std::string> _mapped_files;
  std::unordered_map<int, uint16_t> _fd_server;
  std::shared_ptr<RPC> _rpc;
  static std::shared_ptr<Client> Instance(bool is_mpi = false) {
    auto job_conf_type =
        mimir::AdviceType(mimir::PrimaryAdviceType::JOB_CONFIGURATION,
                          mimir::OperationAdviceType::NO_OP);
    auto _job_handler =
        mimir::AdviceHandler<mimir::JobConfigurationAdvice>::Instance(
            job_conf_type);
    mimir::MimirKey job_conf_key;
    job_conf_key._id = 0;
    auto job_conf_advices = _job_handler->find_advice(job_conf_key);
    if (job_conf_advices.first) {
      if (instance == nullptr) {
        instance = std::make_shared<Client>(is_mpi);
      }
    }
    return instance;
  }
  Client(bool is_mpi)
      : _job_configuration_advice(),
        _job_handler(),
        _mapped_files(),
        _fd_server() {
    mimir::Logger::Instance("ATHENA")->log(
        mimir::LOG_INFO, "Initializing Client for MPI %d", is_mpi);
    auto job_conf_type =
        mimir::AdviceType(mimir::PrimaryAdviceType::JOB_CONFIGURATION,
                          mimir::OperationAdviceType::NO_OP);
    _job_handler =
        mimir::AdviceHandler<mimir::JobConfigurationAdvice>::Instance(
            job_conf_type);
    mimir::MimirKey job_conf_key;
    job_conf_key._id = 0;
    auto job_conf_advices = _job_handler->find_advice(job_conf_key);
    if (job_conf_advices.first) {
      _job_configuration_advice = job_conf_advices.second[0];
      int current_rank;
      if (is_mpi)
        MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
      else
        current_rank = 0;
      // node server rank
      if (is_mpi) {
        uint16_t my_server_index =
            ceil(current_rank / _job_configuration_advice._num_cores_per_node);
        HCL_CONF->IS_SERVER = false;
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
      }
    } else {
      print_backtrace();
      throw std::runtime_error(
          "[ATHENA] Job Configuration Advice not set from Athena Client.");
    }
  }

  void finalize() {  //_rpc->Stop();
  }
};
}  // namespace athena
#endif  // MIMIR_ATHENA_CLIENT_H
