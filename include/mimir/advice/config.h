//
// Created by hariharan on 8/10/22.
//

#ifndef MIMIR_CONFIG_H
#define MIMIR_CONFIG_H

#include <mimir/advice/application_advice.h>
#include <mimir/advice/file_advice.h>
#include <mimir/advice/job_configuration_advice.h>

namespace mimir {
struct Config {
  int _current_process_index;
  JobConfigurationAdvice _job_config;
  WorkflowAdvice _workflow;
  std::vector<ApplicationAdvice> _app_repo;
  std::vector<FileAdvice> _file_repo;
  Config() : _job_config(), _workflow(), _app_repo(), _file_repo() {}
  Config(const Config& other)
      : _job_config(other._job_config),
        _workflow(other._workflow),
        _app_repo(other._app_repo),
        _file_repo(other._file_repo) {}
  Config(const Config&& other)
      : _job_config(other._job_config),
        _workflow(other._workflow),
        _app_repo(other._app_repo),
        _file_repo(other._file_repo) {}
  bool operator!=(const Config& other) const { return !(other == *this); }
  bool operator==(const Config& other) const {
    if (_job_config != other._job_config) return false;
    if (_workflow != other._workflow) return false;
    if (_app_repo.size() != other._app_repo.size()) return false;
    for (int i = 0; i < _app_repo.size(); ++i) {
      if (_app_repo[i] != other._app_repo[i]) return false;
    }
    if (_file_repo.size() != other._file_repo.size()) return false;
    for (int i = 0; i < _file_repo.size(); ++i) {
      if (_file_repo[i] != other._file_repo[i]) return false;
    }
    return true;
  }
  bool is_same(const Config& other) const {
    if (!_job_config.is_same(other._job_config)) return false;
    if (!_workflow.is_same(other._workflow)) return false;
    if (_app_repo.size() != other._app_repo.size()) return false;
    for (int i = 0; i < _app_repo.size(); ++i) {
      if (!_app_repo[i].is_same(other._app_repo[i])) return false;
    }
    if (_file_repo.size() != other._file_repo.size()) return false;
    for (int i = 0; i < _file_repo.size(); ++i) {
      if (!_file_repo[i].is_same(other._file_repo[i])) return false;
    }
    return true;
  }
  Config& operator=(const Config& other) {
    _job_config = other._job_config;
    _workflow = other._workflow;
    _app_repo = other._app_repo;
    _file_repo = other._file_repo;
    return *this;
  }
};
}  // namespace mimir

using json = nlohmann::json;
namespace mimir {
inline void to_json(json& j, const Config& p) {
  j = json();
  j["job_config"] = p._job_config;
  j["workflow"] = p._workflow;
  j["app_repo"] = p._app_repo;
  j["file_repo"] = p._file_repo;
}

inline void from_json(const json& j, Config& p) {
  j.at("job_config").get_to(p._job_config);
  j.at("workflow").get_to(p._workflow);
  j.at("app_repo").get_to(p._app_repo);
  j.at("file_repo").get_to(p._file_repo);
}
}  // namespace mimir

#endif  // MIMIR_CONFIG_H
