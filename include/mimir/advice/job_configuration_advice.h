//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_JOB_CONFIGURATION_ADVICE_H
#define MIMIR_JOB_CONFIGURATION_ADVICE_H

#include <mimir/advice/advice.h>
#include <mimir/common/data_structure.h>

namespace mimir {
class JobConfigurationAdvice : public Advice {
 public:
  uint32_t _job_id;
  uint32_t _num_nodes;
  uint16_t _num_cores_per_node;
  uint8_t _num_gpus_per_node;
  std::vector<Storage> _devices;
  uint32_t _job_time_minutes;
  std::vector<std::string> _node_names;
  std::string _network_protocol;
  uint16_t _rpc_port;
  uint16_t _rpc_threads;

  JobConfigurationAdvice()
      : Advice(AdviceType(PrimaryAdviceType::JOB_CONFIGURATION,
                          OperationAdviceType::NO_OP)),
        _job_id(0),
        _num_nodes(1),
        _num_cores_per_node(1),
        _num_gpus_per_node(0),
        _devices(),
        _job_time_minutes(30),
        _node_names({"localhost"}),
        _network_protocol("tcp"),
        _rpc_port(8888),
        _rpc_threads(1) {}

  JobConfigurationAdvice(const JobConfigurationAdvice& other)
      : Advice(other),
        _num_nodes(other._num_nodes),
        _job_id(other._job_id),
        _num_cores_per_node(other._num_cores_per_node),
        _num_gpus_per_node(other._num_gpus_per_node),
        _devices(other._devices),
        _job_time_minutes(other._job_time_minutes),
        _node_names(other._node_names),
        _network_protocol(other._network_protocol),
        _rpc_port(other._rpc_port),
        _rpc_threads(other._rpc_threads) {}
  JobConfigurationAdvice(const JobConfigurationAdvice&& other)
      : Advice(other),
        _num_nodes(other._num_nodes),
        _job_id(other._job_id),
        _num_cores_per_node(other._num_cores_per_node),
        _num_gpus_per_node(other._num_gpus_per_node),
        _devices(other._devices),
        _job_time_minutes(other._job_time_minutes),
        _node_names(other._node_names),
        _network_protocol(other._network_protocol),
        _rpc_port(other._rpc_port),
        _rpc_threads(other._rpc_threads) {}
  JobConfigurationAdvice& operator=(const JobConfigurationAdvice& other) {
    Advice::operator=(other);
    _num_nodes = other._num_nodes;
    _job_id = other._job_id;
    _num_cores_per_node = other._num_cores_per_node;
    _num_gpus_per_node = other._num_gpus_per_node;
    _devices = other._devices;
    _job_time_minutes = other._job_time_minutes;
    _node_names = other._node_names;
    _network_protocol = other._network_protocol;
    _rpc_port = other._rpc_port;
    _rpc_threads = other._rpc_threads;
    return *this;
  }
  bool operator<(const JobConfigurationAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const JobConfigurationAdvice& other) const {
    return !(*this < other);
  }
  bool operator==(const JobConfigurationAdvice& other) const {
    return Advice::operator==(other) && _num_nodes == other._num_nodes &&
           _job_id == other._job_id &&
           _num_cores_per_node == other._num_cores_per_node &&
           _num_gpus_per_node == other._num_gpus_per_node &&
           _devices == other._devices &&
           _job_time_minutes == other._job_time_minutes &&
           _node_names == other._node_names &&
           _network_protocol == other._network_protocol &&
           _rpc_port == other._rpc_port && _rpc_threads == other._rpc_threads;
  }
};
}  // namespace mimir
namespace std {
template <>
struct hash<mimir::JobConfigurationAdvice> {
  size_t operator()(const mimir::JobConfigurationAdvice& k) const {
    return k._index;
  }
};
}  // namespace std
#endif  // MIMIR_JOB_CONFIGURATION_ADVICE_H
