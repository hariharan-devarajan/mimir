//
// Created by haridev on 3/10/22.
//

#include <catch_config.h>
#include <test_utils.h>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include "mimir/advice/advice_handler.h"
#include "mimir/advice/advice_type.h"
#include "mimir/advice/file_advice.h"
#include "mimir/api/posix.h"
#include "mimir/log/logger.h"
#include <unistd.h>
#include <limits.h>
#include <thread>
namespace mimir::test {
    struct Arguments {
        std::string file = "conf.json";
        int devices = 2;
        bool debug = false;
    };
}  // namespace mimir::test

mimir::test::Arguments args;
int init(int* argc, char*** argv) {
    return 0;
}

int finalize() {
    return 0;
}

cl::Parser define_options() {
    return cl::Opt(
            args.file,
            "file")["-f"]["--file"]("Configuration file.") |
           cl::Opt(args.devices,
                   "devices")["-d"]["--devices"]("devices") |
           cl::Opt(args.debug,
                   "debug")["-d"]["--debug"]("debug");
}
std::vector<std::string> split(std::string x, char delim = ' '){
    x += delim; //includes a delimiter at the end so last word is also read

    auto set_splitted = std::unordered_set<std::string>();
    std::string temp = "";
    int count = 0;
    for (int i = 0; i < x.length(); i++) {
        if (x[i] == delim) {
            if (count > 0) set_splitted.emplace(temp);
            temp = "";
            i++;
            count ++;
        }
        temp += x[i];
    }
    auto splitted = std::vector<std::string>(set_splitted.begin(),set_splitted.end());
    return splitted;
}
/**
 * Test cases
 */

TEST_CASE("JobAdvice", "[advice=job]") {
    mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                          "Loading job configuration");
    if (fs::exists(args.file)) fs::remove(args.file);
    using namespace mimir;
    auto LSB_HOSTS = std::getenv("LSB_HOSTS");
    if (LSB_HOSTS == nullptr) {
        LSB_HOSTS = "localhost";
    }
    auto node_names = split(LSB_HOSTS);
    mimir::JobConfigurationAdvice job_conf_advice;
    job_conf_advice._job_id = 0;
    job_conf_advice._devices.clear();
    for(int i=0;i<args.devices;++i) {
        auto mount_ptr = std::getenv(("MOUNT_PATH_"+std::to_string(i)).c_str());
        auto mount_ptr_size = atol(std::getenv(("MOUNT_SIZE_"+std::to_string(i)).c_str()));
        job_conf_advice._devices.emplace_back(mount_ptr, mount_ptr_size);
    }
    const auto processor_count = 40;
    job_conf_advice._job_time_minutes = 30;
    job_conf_advice._num_cores_per_node = processor_count;
    job_conf_advice._num_gpus_per_node = 4;
    job_conf_advice._num_nodes = node_names.size();
    job_conf_advice._node_names = node_names;
    job_conf_advice._rpc_port = 8888;
    job_conf_advice._rpc_threads = 4;
    job_conf_advice._priority = 100;
    using json = nlohmann::json;
    json j;
    j["job"] = job_conf_advice;
    std::cout << j.dump(2) << std::endl;

    std::ofstream out(args.file);
    out << j;
    out.close();

    std::ifstream t(args.file);
    t.seekg(0, std::ios::end);
    size_t size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    t.close();
    json read_json = json::parse(buffer);
    mimir::JobConfigurationAdvice j_r;
    read_json["job"].get_to(j_r);
    mimir::Logger::Instance("PEGASUS_TEST")
            ->log(mimir::LOG_ERROR, "nodes %d", j_r._num_nodes);
}