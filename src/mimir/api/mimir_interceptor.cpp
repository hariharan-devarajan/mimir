//
// Created by hariharan on 2/23/22.
//

#include <mimir/api/application.h>
#include <mimir/api/job_configuration.h>
#include <mimir/api/mimir_interceptor.h>
#include <mimir/api/posix.h>
#include <mimir/api/workflow.h>
#include <mimir/constant.h>
#include <mpi.h>

namespace mimir {
bool is_mpi = false;
bool is_tracing = false;
mimir::Tracker* tracker_instance = nullptr;
mimir::Config* global_app_config = nullptr;
bool config_loaded_file = false;
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
  MIMIR_LOGINFO( "split_string: start","");
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
  MIMIR_LOGINFO("split_string with num_nodes %ld: end", splitted.size());
  return splitted;
}

extern MimirStatus mimir_init_config(bool is_mpi) {
  MIMIR_LOGINFO("Loading job configuration: start","");
  if (mimir::global_app_config == nullptr) {
    auto CONFIG = std::getenv(mimir::MIMIR_CONFIG_PATH);
    if (CONFIG != nullptr && fs::exists(CONFIG)) {
      MIMIR_LOGINFO("Loading job configuration from config: start","");
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
      mimir::config_loaded_file = true;
      std::string sp;
      std::ifstream("/proc/self/cmdline") >> sp;
      std::replace( sp.begin(), sp.end() - 1, '\000', ' ');
      if (is_mpi) {
        int rank, comm_size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
        sp = std::to_string(comm_size) + " " + sp;
      }
      mimir::global_app_config->_current_process_index = -1;
      for(auto element:mimir::global_app_config->_workflow._app_mapping) {
        if (strcmp(element.first.c_str(),sp.c_str()) == 0) {
          mimir::global_app_config->_current_process_index = element.second;
          break;
        }
      }
      if(mimir::global_app_config->_current_process_index == -1){
        MIMIR_LOGERROR("app hash not matching","");
      }
      MIMIR_LOGINFO("Loading job configuration from config: end","");
    } else {
      MIMIR_LOGERROR("Environment variable %s not present or path "
          "does not exist. Config not loaded.", mimir::MIMIR_CONFIG_PATH);
    }
  }
  MIMIR_LOGINFO("Loading job configuration: finished","");
  return mimir::MIMIR_SUCCESS;
}

extern MimirStatus mimir_finalize_config() {
  MIMIR_LOGINFO("mimir_finalize_config: start","");
  delete mimir::global_app_config;
  MIMIR_LOGINFO("mimir_finalize_config: finished","");
  return mimir::MIMIR_SUCCESS;
}

extern MimirStatus insert_loaded_intents() {
  MIMIR_LOGINFO("insert_loaded_intents: start","");
  if (mimir::config_loaded_file && !mimir::intents_loaded &&
      mimir::global_app_config != nullptr) {
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
    for (size_t file_index = 0; file_index < num_files; ++file_index) {
      mimir::file_advice_begin(mimir::global_app_config->_file_repo[file_index],
                               mimir::handlers[current_handler_index++]);
    }
    mimir::intents_loaded = true;
  }
}
extern MimirStatus remove_loaded_intents() {
  MIMIR_LOGINFO("remove_loaded_intents: start","");
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
    for (size_t file_index = 0; file_index < num_files; ++file_index) {
      mimir::file_advice_end(mimir::handlers[current_handler_index++]);
    }
    mimir::free_job_configuration();
    mimir::free_workflow();
    mimir::free_applications();
    mimir::free_files();
    free(mimir::handlers);
    mimir::intents_loaded = false;
  }
  MIMIR_LOGINFO("remove_loaded_intents: end","");
}