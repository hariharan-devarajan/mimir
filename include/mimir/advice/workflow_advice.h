//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_WORKFLOW_ADVICE_H
#define MIMIR_WORKFLOW_ADVICE_H

#include <mimir/advice/advice.h>
#include <mimir/common/enumeration.h>

#include <unordered_map>

namespace mimir {
inline unsigned oat_hash(void const *key, int len) {
  auto const *p = static_cast<unsigned char const *>(key);
  unsigned h = 0;
  for (int i = 0; i < len; i++) {
    h += p[i];
    h += (h << 10);
    h ^= (h >> 6);
  }
  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);

  return h;
}
class WorkflowAdvice : public Advice {
 public:
  using Advice::_type;
  uint16_t _num_cpu_cores_used;
  uint8_t _num_gpus_used;
  uint32_t _num_apps;
  ApplicationFileDAG _application_file_dag;
  std::vector<FileIndex> _independent_files;
  std::unordered_map<FileIndex, std::vector<ApplicationIndex>> _shared_files;
  uint32_t _io_size_mb;
  float _per_io_data, _per_io_metadata;
  TransferSizeDistribution _ts_distribution;
  std::unordered_map<FileIndex, std::unordered_set<InterfaceType>> _interfaces_used;
  std::unordered_map<FileIndex, AccessPattern> _file_access_pattern;
  std::unordered_map<FileIndex, WorkloadType> _file_workload;
  std::unordered_map<std::string, ApplicationIndex> _app_mapping;

  uint32_t _runtime_minutes;

  WorkflowAdvice(AdviceType type)
      : Advice(type),
        _num_cpu_cores_used(1),
        _num_gpus_used(0),
        _num_apps(1),
        _application_file_dag(),
        _independent_files(),
        _shared_files(),
        _io_size_mb(),
        _per_io_data(),
        _per_io_metadata(),
        _ts_distribution(),
        _interfaces_used(),
        _file_access_pattern(),
        _runtime_minutes(),
        _file_workload(),
        _app_mapping(){}

  WorkflowAdvice()
      : WorkflowAdvice(AdviceType(PrimaryAdviceType::JOB_WORKFLOW,
                                  OperationAdviceType::NO_OP)) {}
  WorkflowAdvice(const WorkflowAdvice& other)
      : Advice(other),
        _num_cpu_cores_used(other._num_cpu_cores_used),
        _num_gpus_used(other._num_gpus_used),
        _num_apps(other._num_apps),
        _application_file_dag(other._application_file_dag),
        _independent_files(other._independent_files),
        _shared_files(other._shared_files),
        _io_size_mb(other._io_size_mb),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata),
        _ts_distribution(other._ts_distribution),
        _interfaces_used(other._interfaces_used),
        _file_access_pattern(other._file_access_pattern),
        _runtime_minutes(other._runtime_minutes),
        _file_workload(other._file_workload),
        _app_mapping(other._app_mapping) {}
  WorkflowAdvice(const WorkflowAdvice&& other)
      : Advice(other),
        _num_cpu_cores_used(other._num_cpu_cores_used),
        _num_gpus_used(other._num_gpus_used),
        _num_apps(other._num_apps),
        _application_file_dag(other._application_file_dag),
        _independent_files(other._independent_files),
        _shared_files(other._shared_files),
        _io_size_mb(other._io_size_mb),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata),
        _ts_distribution(other._ts_distribution),
        _interfaces_used(other._interfaces_used),
        _file_access_pattern(other._file_access_pattern),
        _runtime_minutes(other._runtime_minutes),
        _file_workload(other._file_workload) ,
        _app_mapping(other._app_mapping)  {}
  WorkflowAdvice& operator=(const WorkflowAdvice& other) {
    Advice::operator=(other);
    _num_cpu_cores_used = other._num_cpu_cores_used;
    _num_gpus_used = other._num_gpus_used;
    _num_apps = other._num_apps;
    _application_file_dag = other._application_file_dag;
    _independent_files = other._independent_files;
    _shared_files = other._shared_files;
    _io_size_mb = other._io_size_mb;
    _per_io_data = other._per_io_data;
    _per_io_metadata = other._per_io_metadata;
    _ts_distribution = other._ts_distribution;
    _interfaces_used = other._interfaces_used;
    _file_access_pattern = other._file_access_pattern;
    _runtime_minutes = other._runtime_minutes;
    _file_workload = other._file_workload;
    _app_mapping = other._app_mapping;
    return *this;
  }

  bool operator!=(const WorkflowAdvice& other) const {
    return !(other == *this);
  }
  bool operator<(const WorkflowAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const WorkflowAdvice& other) const { return !(*this < other); }
  bool operator==(const WorkflowAdvice& other) const {
    return Advice::operator==(other) && this->is_same(other);
  }
  bool is_same(const WorkflowAdvice& other) const {
    if (!Advice::is_same(other)) return false;
    if (!(this->_num_cpu_cores_used == other._num_cpu_cores_used &&
          this->_num_gpus_used == other._num_gpus_used &&
          this->_num_apps == other._num_apps &&
          this->_application_file_dag == other._application_file_dag &&
          this->_io_size_mb == other._io_size_mb &&
          this->_per_io_data == other._per_io_data &&
          this->_per_io_metadata == other._per_io_metadata &&
          this->_ts_distribution == other._ts_distribution &&
          this->_runtime_minutes == other._runtime_minutes))
      return false;

    if (_independent_files.size() != other._independent_files.size())
      return false;
    for (int i = 0; i < _independent_files.size(); ++i) {
      if (_independent_files[i] != other._independent_files[i]) return false;
    }
    if (_shared_files.size() != other._shared_files.size()) return false;
    for (auto element : _shared_files) {
      auto other_iter = other._shared_files.find(element.first);
      if (other_iter == other._shared_files.end()) return false;
      if (other_iter->second.size() != element.second.size()) return false;
      for (int i = 0; i < element.second.size(); ++i)
        if (element.second[i] != other_iter->second[i]) return false;
    }

    if (_interfaces_used.size() != other._interfaces_used.size()) return false;
    for (auto element : _interfaces_used) {
      auto other_iter = other._interfaces_used.find(element.first);
      if (other_iter == other._interfaces_used.end()) return false;
      if (other_iter->second.size() != element.second.size()) return false;
      for (auto inter_element : element.second) {
        auto other_iter_2 = other_iter->second.find(inter_element);
        if (other_iter_2 == other_iter->second.end()) return false;
      }
    }
    if (_file_access_pattern.size() != other._file_access_pattern.size())
      return false;
    for (auto element : _file_access_pattern) {
      auto other_iter = other._file_access_pattern.find(element.first);
      if (other_iter == other._file_access_pattern.end()) return false;
      if (other_iter->second != element.second) return false;
    }
    if (_file_workload.size() != other._file_workload.size())
      return false;
    for (auto element : _file_workload) {
      auto other_iter = other._file_workload.find(element.first);
      if (other_iter == other._file_workload.end()) return false;
      if (other_iter->second != element.second) return false;
    }
    if (_app_mapping.size() != other._app_mapping.size())
       return false;
    for (auto element : _app_mapping) {
      auto other_iter = other._app_mapping.find(element.first);
      if (other_iter == other._app_mapping.end()) return false;
      if (other_iter->second != element.second) return false;
    }
    return true;
  }
};
}  // namespace mimir

namespace std {
template <>
struct hash<mimir::WorkflowAdvice> {
  size_t operator()(const mimir::WorkflowAdvice& k) const { return k._index; }
};
}  // namespace std

using json = nlohmann::json;
namespace mimir {
inline void to_json(json& j, const WorkflowAdvice& p) {
  j = json();
  to_json(j, (mimir::Advice&)p);
  j["num_cpu_cores_used"] = p._num_cpu_cores_used;
  j["num_gpus_used"] = p._num_gpus_used;
  j["num_apps"] = p._num_apps;
  j["application_file_dag"] = p._application_file_dag;

  j["independent_files"] = p._independent_files;
  j["shared_files"] = p._shared_files;
  j["io_size_mb"] = p._io_size_mb;

  j["per_io_data"] = p._per_io_data;
  j["per_io_metadata"] = p._per_io_metadata;
  j["ts_distribution"] = p._ts_distribution;
  j["interfaces_used"] = p._interfaces_used;
  j["file_access_pattern"] = p._file_access_pattern;
  j["file_workload"] = p._file_workload;
  j["app_mapping"] = p._app_mapping;
  j["runtime_minutes"] = p._runtime_minutes;
}

inline void from_json(const json& j, WorkflowAdvice& p) {
  from_json(j, (mimir::Advice&)p);
  j.at("num_cpu_cores_used").get_to(p._num_cpu_cores_used);
  j.at("num_gpus_used").get_to(p._num_gpus_used);
  j.at("num_apps").get_to(p._num_apps);
  j.at("application_file_dag").get_to(p._application_file_dag);
  j.at("independent_files").get_to(p._independent_files);
  j.at("shared_files").get_to(p._shared_files);
  j.at("io_size_mb").get_to(p._io_size_mb);
  j.at("per_io_data").get_to(p._per_io_data);
  j.at("per_io_metadata").get_to(p._per_io_metadata);
  j.at("ts_distribution").get_to(p._ts_distribution);

  j.at("interfaces_used").get_to(p._interfaces_used);
  j.at("file_access_pattern").get_to(p._file_access_pattern);
  j.at("file_workload").get_to(p._file_workload);
  j.at("app_mapping").get_to(p._app_mapping);
  j.at("runtime_minutes").get_to(p._runtime_minutes);
}
}  // namespace mimir

#endif  // MIMIR_WORKFLOW_ADVICE_H
