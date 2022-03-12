//
// Created by hariharan on 2/21/22.
//

#ifndef MIMIR_ATHENA_CLIENT_H
#define MIMIR_ATHENA_CLIENT_H

#include <hcl/communication/rpc_lib.h>
#include <mpi.h>

#include <memory>
#include <mutex>
#include <shared_mutex>

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
  std::unordered_map<std::string, std::string> _mapped_files;
  std::unordered_map<INTERFACE_IDENTIFIER, uint16_t> _id_server_map;
  std::shared_mutex _mapped_files_mutex, _id_server_map_mutex;
 public:
  mimir::JobConfigurationAdvice _job_configuration_advice;
  std::shared_ptr<mimir::AdviceHandler<mimir::JobConfigurationAdvice>>
      _job_handler;
  std::shared_ptr<RPC> _rpc;

  std::pair<bool, uint16_t> id_server_map_find(INTERFACE_IDENTIFIER id) {
    std::shared_lock guard(_id_server_map_mutex);
    auto ret = std::pair<bool, uint16_t>();
    ret.first = false;
    auto iter = _id_server_map.find(id);
    if (iter != _id_server_map.end()) {
        ret.first = true;
        ret.second = iter->second;
    }
    return ret;
  }
  bool id_server_map_emplace(INTERFACE_IDENTIFIER id, uint16_t server_index) {
    std::unique_lock guard(_id_server_map_mutex);
      auto iter = _id_server_map.find(id);
      if (iter != _id_server_map.end()) {
          _id_server_map.erase(id);
      }
      _id_server_map.emplace(id, server_index);
      return true;
  }
  bool id_server_map_erase(INTERFACE_IDENTIFIER id) {
    std::unique_lock guard(_id_server_map_mutex);
      _id_server_map.erase(id);
      return true;
  }

  std::pair<bool, std::string> mapped_files_find(std::string file_key) {
    std::shared_lock guard(_mapped_files_mutex);
    auto ret = std::pair<bool, std::string>();
    ret.first = false;
    auto iter = _mapped_files.find(file_key);
    if (iter != _mapped_files.end()) {
        ret.first = true;
        ret.second = iter->second;
    }
    return ret;
  }
  bool mapped_files_emplace(std::string file_key, std::string file) {
    std::unique_lock guard(_mapped_files_mutex);
      auto iter = _mapped_files.find(file_key);
      if (iter != _mapped_files.end()) {
          _mapped_files.erase(file_key);
      }
      _mapped_files.emplace(file_key, file);
      return true;
  }
  bool mapped_files_erase(std::string file_key) {
    std::unique_lock guard(_mapped_files_mutex);
      _mapped_files.erase(file_key);
      return true;
  }
};
}  // namespace athena
#endif  // MIMIR_ATHENA_CLIENT_H
