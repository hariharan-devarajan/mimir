//
// Created by hariharan on 2/23/22.
//

#include <mimir/api/application.h>
#include <mimir/api/job_configuration.h>
#include <mimir/api/mimir_interceptor.h>
#include <mimir/api/posix.h>
#include <mimir/api/workflow.h>
#include <mimir/constant.h>

namespace mimir {
bool is_mpi = false;
bool is_tracing = false;
mimir::Tracker* tracker_instance = nullptr;
mimir::Config* global_app_config = nullptr;
bool intents_loaded = false;
mimir::MimirHandler* handlers;
size_t num_handlers;
}  // namespace mimir

extern bool is_mpi() { return mimir::is_mpi; }
extern void set_mpi() { mimir::is_mpi = true; }

extern bool is_tracing() { return mimir::is_tracing; }

extern void init_mimir() {
  mimir::is_tracing = true;
  mimir::tracker_instance = new mimir::Tracker();
}
extern void finalize_mimir() {
  mimir::is_tracing = false;
  delete mimir::tracker_instance;
}

extern mimir::Tracker* MIMIR_TRACKER() { return mimir::tracker_instance; }
extern mimir::Config* MIMIR_CONFIG() { return mimir::global_app_config; }

inline std::vector<std::string> split_string(std::string x, char delim = ' ') {
  x += delim;  // includes a delimiter at the end so last word is also read
  auto set_splitted = std::unordered_set<std::string>();
  std::string temp = "";
  int count = 0;
  auto splitted = std::vector<std::string>();
  for (int i = 0; i < x.length(); i++) {
    if (x[i] == delim) {
      if (count > 0) {
        auto iter = set_splitted.find(temp);
        if (iter == set_splitted.end()) {
          set_splitted.emplace(temp);
          splitted.push_back(temp);
        }
      }
      temp = "";
      i++;
      count++;
    }
    temp += x[i];
  }
  return splitted;
}

extern MimirStatus mimir_init_config() {
  if (mimir::global_app_config == nullptr) {
    auto CONFIG = std::getenv(mimir::MIMIR_CONFIG_PATH);
    if (CONFIG != nullptr && fs::exists(CONFIG)) {
      std::ifstream input(CONFIG);
      input.seekg(0, std::ios::end);
      size_t size = input.tellg();
      std::string buffer(size, ' ');
      input.seekg(0);
      input.read(&buffer[0], size);
      input.close();
      using json = nlohmann::json;
      json read_json = json::parse(buffer);
      mimir::global_app_config = new mimir::Config();
      read_json.get_to(*mimir::global_app_config);
    } else {
      mimir::global_app_config = new mimir::Config();
      auto SHM = "/dev/shm";
      auto PFS = std::getenv("PFS_PATH");
      auto LSB_HOSTS = std::getenv("LSB_HOSTS");
      if (LSB_HOSTS == nullptr) {
        LSB_HOSTS = "localhost";
      }
      auto node_names = split_string(LSB_HOSTS);

      mimir::Logger::Instance("MIMIR")->log(
          mimir::LOG_INFO, "Loading Default job configuration");
      mimir::global_app_config->_job_config._job_id = 0;
      mimir::global_app_config->_job_config._devices.emplace_back(SHM,
                                                                  2 * 1024);
      mimir::global_app_config->_job_config._devices.emplace_back(PFS,
                                                                  64 * 1024);
      mimir::global_app_config->_job_config._job_time_minutes = 30;
      mimir::global_app_config->_job_config._num_cores_per_node = 40;
      mimir::global_app_config->_job_config._num_gpus_per_node = 0;
      mimir::global_app_config->_job_config._num_nodes = node_names.size();
      mimir::global_app_config->_job_config._node_names = node_names;
      mimir::global_app_config->_job_config._rpc_port = 8888;
      mimir::global_app_config->_job_config._rpc_threads = 1;
      mimir::global_app_config->_job_config._priority = 100;
    }
  }
  return mimir::MIMIR_SUCCESS;
}

extern MimirStatus mimir_finalize_config() {
  delete mimir::global_app_config;
  return mimir::MIMIR_SUCCESS;
}

extern MimirStatus insert_loaded_intents() {
  if (!mimir::intents_loaded && mimir::global_app_config != nullptr) {
    size_t num_jobs = 1, num_workflow = 1,
           num_apps = mimir::global_app_config->_app_repo.size(),
           num_files = mimir::global_app_config->_file_repo.size();
    mimir::num_handlers = num_jobs + num_workflow + num_apps + num_files;
    mimir::handlers = static_cast<mimir::MimirHandler*>(
        calloc(mimir::num_handlers, sizeof(mimir::MimirHandler)));
    /* Start all intents */
    size_t current_handler_index = 0;
    mimir::job_configuration_advice_begin(
        mimir::global_app_config->_job_config,
        mimir::handlers[current_handler_index++]);
    mimir::workflow_advice_begin(mimir::global_app_config->_workflow,
                                 mimir::handlers[current_handler_index++]);
    for (size_t app_index = 0; app_index < num_apps; ++app_index) {
      mimir::application_advice_begin(
          mimir::global_app_config->_app_repo[app_index],
          mimir::handlers[current_handler_index++]);
    }
    for (size_t file_index = 0; file_index < num_files; ++num_files) {
      mimir::file_advice_begin(mimir::global_app_config->_file_repo[file_index],
                               mimir::handlers[current_handler_index++]);
    }
    mimir::intents_loaded = true;
  }
}
extern MimirStatus remove_loaded_intents() {
  if (mimir::intents_loaded) {
    size_t num_jobs = 1, num_workflow = 1,
           num_apps = mimir::global_app_config->_app_repo.size(),
           num_files = mimir::global_app_config->_file_repo.size();
    assert(mimir::num_handlers ==
           (num_jobs + num_workflow + num_apps + num_files));
    /* End all intents */
    size_t current_handler_index = 0;
    mimir::job_configuration_advice_end(
        mimir::handlers[current_handler_index++]);
    mimir::workflow_advice_end(mimir::handlers[current_handler_index++]);
    for (size_t app_index = 0; app_index < num_apps; ++app_index) {
      mimir::application_advice_end(mimir::handlers[current_handler_index++]);
    }
    for (size_t file_index = 0; file_index < num_files; ++num_files) {
      mimir::file_advice_end(mimir::handlers[current_handler_index++]);
    }
    free(mimir::handlers);
    mimir::intents_loaded = false;
  }
}