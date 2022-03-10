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
template <typename INTERFACE_IDENTIFIER>
class Client {
 private:
  static std::shared_ptr<Client> instance;

 protected:
  Client(bool is_mpi)
      : _job_configuration_advice(),
        _job_handler(),
        _mapped_files(),
        _id_server_map() {
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
      _job_configuration_advice = job_conf_advices.second.begin()->second;
    } else {
      _job_configuration_advice = load_job_details();
      _job_handler->save_advice(job_conf_key, _job_configuration_advice);
    }
    int current_rank;
    if (is_mpi)
      MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    else
      current_rank = 0;
    // node server rank
    uint16_t my_server_index =
        floor(current_rank / _job_configuration_advice._num_cores_per_node);
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

 public:
  mimir::JobConfigurationAdvice _job_configuration_advice;
  std::shared_ptr<mimir::AdviceHandler<mimir::JobConfigurationAdvice>>
      _job_handler;
  std::unordered_map<std::string, std::string> _mapped_files;
  std::unordered_map<INTERFACE_IDENTIFIER, uint16_t> _id_server_map;
  std::shared_ptr<RPC> _rpc;

  void finalize() {  //_rpc->Stop();
  }
};
}  // namespace athena
#endif  // MIMIR_ATHENA_CLIENT_H
