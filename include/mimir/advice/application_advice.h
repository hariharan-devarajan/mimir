//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_APPLICATION_ADVICE_H
#define MIMIR_APPLICATION_ADVICE_H

#include <mimir/advice/advice_type.h>
#include <mimir/advice/workflow_advice.h>

namespace mimir {
class ApplicationAdvice : public WorkflowAdvice {
 private:
  using WorkflowAdvice::_num_apps;
  using WorkflowAdvice::_shared_files;
  using WorkflowAdvice::_app_mapping;

 public:
  using Advice::_type;
  using WorkflowAdvice::_application_file_dag;
  using WorkflowAdvice::_file_access_pattern;
  using WorkflowAdvice::_file_workload;
  using WorkflowAdvice::_independent_files;
  using WorkflowAdvice::_interfaces_used;
  using WorkflowAdvice::_io_size_mb;
  using WorkflowAdvice::_num_cpu_cores_used;
  using WorkflowAdvice::_num_gpus_used;
  using WorkflowAdvice::_per_io_data;
  using WorkflowAdvice::_per_io_metadata;
  using WorkflowAdvice::_runtime_minutes;
  using WorkflowAdvice::_ts_distribution;

  std::string _name;
  RankFileDAG _rank_file_dag;
  bool _is_mpi;

  ApplicationAdvice()
      : WorkflowAdvice(AdviceType(PrimaryAdviceType::JOB_APPLICATION,
                                  OperationAdviceType::NO_OP)),
        _name(),
        _rank_file_dag(), _is_mpi() {}
  ApplicationAdvice(const ApplicationAdvice& other)
      : WorkflowAdvice(other),
        _name(other._name),
        _rank_file_dag(other._rank_file_dag),
        _is_mpi(other._is_mpi){}
  ApplicationAdvice(const ApplicationAdvice&& other)
      : WorkflowAdvice(other),
        _name(other._name),
        _rank_file_dag(other._rank_file_dag),
        _is_mpi(other._is_mpi) {}
  ApplicationAdvice& operator=(const ApplicationAdvice& other) {
    WorkflowAdvice::operator=(other);
    this->_name = other._name;
    this->_rank_file_dag = other._rank_file_dag;
    this->_is_mpi = other._is_mpi;
    return *this;
  }
  bool operator<(const ApplicationAdvice& other) const {
    return WorkflowAdvice::operator<(other);
  }

  bool operator>(const ApplicationAdvice& other) const {
    return !(*this < other);
  }
  bool operator!=(const ApplicationAdvice& other) const {
    return !(other == *this);
  }
  bool operator==(const ApplicationAdvice& other) const {
    return WorkflowAdvice::operator==(other) && this->is_same(other);
  }
  bool is_same(const ApplicationAdvice& other) const {
    return WorkflowAdvice::is_same(other) && this->_name == other._name &&
           this->_rank_file_dag == other._rank_file_dag &&
           _is_mpi == other._is_mpi;
    ;
  }
};
}  // namespace mimir

namespace std {
template <>
struct hash<mimir::ApplicationAdvice> {
  size_t operator()(const mimir::ApplicationAdvice& k) const {
    return k._index;
  }
};
}  // namespace std

using json = nlohmann::json;
namespace mimir {
inline void to_json(json& j, const ApplicationAdvice& p) {
  j = json();
  to_json(j, (mimir::Advice&)p);
  j["num_cpu_cores_used"] = p._num_cpu_cores_used;
  j["num_gpus_used"] = p._num_gpus_used;
  j["application_file_dag"] = p._application_file_dag;

  j["independent_files"] = p._independent_files;
  j["io_size_mb"] = p._io_size_mb;

  j["per_io_data"] = p._per_io_data;
  j["per_io_metadata"] = p._per_io_metadata;
  j["ts_distribution"] = p._ts_distribution;
  j["interfaces_used"] = p._interfaces_used;
  j["file_access_pattern"] = p._file_access_pattern;
  j["file_workload"] = p._file_workload;
  j["runtime_minutes"] = p._runtime_minutes;
  j["name"] = p._name;
  j["rank_file_dag"] = p._rank_file_dag;
  j["is_mpi"] = p._is_mpi;
}

inline void from_json(const json& j, ApplicationAdvice& p) {
  from_json(j, (mimir::Advice&)p);
  j.at("num_cpu_cores_used").get_to(p._num_cpu_cores_used);
  j.at("num_gpus_used").get_to(p._num_gpus_used);
  j.at("application_file_dag").get_to(p._application_file_dag);
  j.at("independent_files").get_to(p._independent_files);
  j.at("io_size_mb").get_to(p._io_size_mb);
  j.at("per_io_data").get_to(p._per_io_data);
  j.at("per_io_metadata").get_to(p._per_io_metadata);
  j.at("ts_distribution").get_to(p._ts_distribution);
  j.at("interfaces_used").get_to(p._interfaces_used);
  j.at("file_access_pattern").get_to(p._file_access_pattern);
  j.at("runtime_minutes").get_to(p._runtime_minutes);
  j.at("name").get_to(p._name);
  j.at("rank_file_dag").get_to(p._rank_file_dag);
  j.at("file_workload").get_to(p._file_workload);
  j.at("is_mpi").get_to(p._is_mpi);
}
}  // namespace mimir
#endif  // MIMIR_APPLICATION_ADVICE_H
