//
// Created by haridev on 3/10/22.
//

#include <catch_config.h>
#include <test_utils.h>
#include <experimental/filesystem>
#include "mimir/advice/advice_handler.h"
#include "mimir/advice/advice_type.h"
#include "mimir/advice/file_advice.h"
#include "mimir/api/posix.h"
#include "mimir/log/logger.h"
#include <unistd.h>
#include <limits.h>
namespace fs = std::experimental::filesystem;
namespace mimir::test {
    struct Info {
    };
    struct Arguments {
        size_t num_operations = 100;
        size_t request_size = 65536;
        bool debug = false;
    };
}  // namespace mimir::test

mimir::test::Arguments args;
mimir::test::Info info;
int init(int* argc, char*** argv) {
    return 0;
}

int finalize() {
    return 0;
}

cl::Parser define_options() {
    return cl::Opt(
            args.num_operations,
            "num_operations")["-n"]["--num_operations"]("# of operations.") |
           cl::Opt(args.request_size,
                   "request_size")["-r"]["--request_size"]("request_size") |
           cl::Opt(args.debug,
                   "debug")["-d"]["--debug"]("debug");
}

/**
 * Test cases
 */

TEST_CASE("JobAdvice",
"[advice=job]"
"[iteration=" +
std::to_string(args.num_operations) + "]") {
    mimir::Logger::Instance("MIMIR")->log(mimir::LOG_INFO,
                                          "Loading job configuration");
    using namespace mimir;
    auto SHM = std::getenv("SHM_PATH");
    auto PFS = std::getenv("PFS_PATH");

    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);
    mimir::JobConfigurationAdvice job_conf_advice;
    job_conf_advice._job_id = 0;
    job_conf_advice._devices.emplace_back(SHM, 1024);
    job_conf_advice._devices.emplace_back(PFS, 128);
    job_conf_advice._job_time_minutes = 30;
    job_conf_advice._num_cores_per_node = 40;
    job_conf_advice._num_gpus_per_node = 0;
    job_conf_advice._num_nodes = 1;
    job_conf_advice._node_names.clear();
    job_conf_advice._node_names.push_back(hostname);
    job_conf_advice._rpc_port = 8888;
    job_conf_advice._rpc_threads = 1;
    job_conf_advice._priority = 100;
    using json = nlohmann::json;
    json j;
    j["job"] = job_conf_advice;
    std::cout << j.dump(2) << std::endl;

    std::ofstream out("config.json");
    out << j;
    out.close();

    std::ifstream t("config.json");
    t.seekg(0, std::ios::end);
    size_t size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    json read_json = json::parse(buffer);
    mimir::JobConfigurationAdvice j_r;
    read_json["job"].get_to(j_r);
    mimir::Logger::Instance("PEGASUS_TEST")
            ->log(mimir::LOG_ERROR, "nodes %d", j_r._num_nodes);

    fs::remove("config.json");

}