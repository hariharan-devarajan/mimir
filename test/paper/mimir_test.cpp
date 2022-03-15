//
// Created by hariharan on 3/8/22.
//
#include <catch_config.h>
#include <fcntl.h>
#include <mpi.h>
#include <test_utils.h>

#include <experimental/filesystem>
#include <mimir/api/mimir_interceptor.h>

#include "mimir/advice/advice_handler.h"
#include "mimir/advice/advice_type.h"
#include "mimir/advice/file_advice.h"
#include "mimir/api/posix.h"
#include "mimir/log/logger.h"
namespace fs = std::experimental::filesystem;
namespace mimir::test {
struct Info {
  int rank;
  int comm_size;
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
  //  fprintf(stdout, "Initializing MPI\n");
  MPI_Init(argc, argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &info.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &info.comm_size);
  if (args.debug && info.rank == 0) {
    printf("%d ready for attach\n", info.comm_size);
    fflush(stdout);
    getchar();
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return 0;
}

int finalize() {
  MPI_Finalize();
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

TEST_CASE("Anatomy",
          "[test=anatomy]"
          "[iteration=" +
              std::to_string(args.num_operations) + "]") {
  Timer init, save, find, remove, add_conflicts, resolve;
  auto file_type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                     mimir::OperationAdviceType::NO_OP);
  for (int i = 0; i < args.num_operations; ++i) {
    init.resumeTime();
    auto file_advice_handler =
        mimir::AdviceHandler<mimir::FileAdvice>::Instance(file_type);
    init.pauseTime();
  }
  mimir::MimirHandler file_handler;
  mimir::FileAdvice file_advice;
  file_advice._type._secondary = mimir::OperationAdviceType::INDEPENDENT_FILE;

  file_advice._per_io_data = args.num_operations / (args.num_operations + 2);
  file_advice._per_io_metadata = 2 / (args.num_operations + 2);
  file_advice._size_mb = 1024 * args.num_operations / MB;
  file_advice._current_device = 1;
  file_advice._write_distribution._0_4kb = 1.0;
  file_advice._io_amount_mb = 1024 * args.num_operations / MB;
  file_advice._format = mimir::Format::FORMAT_BINARY;
  file_advice._priority = 100;
  file_advice._name = "test.dat";

  auto advice_handler =
      mimir::AdviceHandler<mimir::FileAdvice>::Instance(file_advice._type);
  for (int i = 0; i < args.num_operations; ++i) {
    mimir::MimirKey key;
    key._id = file_advice._index;
    save.resumeTime();

    advice_handler->save_advice(key, file_advice);
    save.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    mimir::MimirKey key;
    key._id = file_advice._index;
    find.resumeTime();
    advice_handler->find_advice(key);
    find.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    auto conflict_key = mimir::MimirKey(file_advice._index);
    add_conflicts.resumeTime();
    advice_handler->add_conflicts(conflict_key, file_advice);
    add_conflicts.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    auto conflict_key = mimir::MimirKey(file_advice._index);
    resolve.resumeTime();
    advice_handler->resolve_conflicts(conflict_key);
    resolve.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    mimir::MimirKey key;
    key._id = file_advice._index;
    remove.resumeTime();
    advice_handler->remove_advice(key, file_advice._index);
    remove.pauseTime();
  }
  double total_init = 0.0, total_save = 0.0, total_find = 0.0,
         total_remove = 0.0, total_conflicts = 0.0, total_resolve = 0.0;
  double init_time = init.getElapsedTime(), save_time = save.getElapsedTime(),
         find_time = find.getElapsedTime(),
         remove_time = remove.getElapsedTime(),
         add_conflicts_time = add_conflicts.getElapsedTime(),
         resolve_time = resolve.getElapsedTime();
  MPI_Reduce(&init_time, &total_init, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&save_time, &total_save, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&find_time, &total_find, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&remove_time, &total_remove, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&add_conflicts_time, &total_conflicts, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&resolve_time, &total_resolve, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  if (info.rank == 0) {
    double total_time = total_init + total_save + total_find + total_conflicts +
                        total_resolve + total_remove;
    fprintf(stdout,
            "Timing:\n"
            "init,save,find,add_conflicts,resolve_conflicts,remove\n"
            "%f,%f,%f,%f,%f,%f\n",
            total_init / total_time, total_save / total_time,
            total_find / total_time, total_conflicts / total_time,
            total_resolve / total_time, total_remove / total_time);
  }
}

TEST_CASE("Performance",
          "[test=performance]"
          "[iteration=" +
              std::to_string(args.num_operations) + "]") {
  Timer init, save, find, remove, add_conflicts, resolve;
  auto file_type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                     mimir::OperationAdviceType::NO_OP);
  for (int i = 0; i < args.num_operations; ++i) {
    init.resumeTime();
    auto file_advice_handler =
        mimir::AdviceHandler<mimir::FileAdvice>::Instance(file_type);
    init.pauseTime();
  }
  mimir::MimirHandler file_handler;
  mimir::FileAdvice file_advice;
  file_advice._type._secondary = mimir::OperationAdviceType::INDEPENDENT_FILE;

  file_advice._per_io_data = args.num_operations / (args.num_operations + 2);
  file_advice._per_io_metadata = 2 / (args.num_operations + 2);
  file_advice._size_mb = 1024 * args.num_operations / MB;
  file_advice._current_device = 1;
  file_advice._write_distribution._0_4kb = 1.0;
  file_advice._io_amount_mb = 1024 * args.num_operations / MB;
  file_advice._format = mimir::Format::FORMAT_BINARY;
  file_advice._priority = 100;
  file_advice._name = "test.dat";

  auto advice_handler =
      mimir::AdviceHandler<mimir::FileAdvice>::Instance(file_advice._type);
  for (int i = 0; i < args.num_operations; ++i) {
    mimir::MimirKey key;
    key._id = file_advice._index;
    save.resumeTime();

    advice_handler->save_advice(key, file_advice);
    save.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    mimir::MimirKey key;
    key._id = file_advice._index;
    find.resumeTime();
    advice_handler->find_advice(key);
    find.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    auto conflict_key = mimir::MimirKey(file_advice._index);
    add_conflicts.resumeTime();
    advice_handler->add_conflicts(conflict_key, file_advice);
    add_conflicts.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    auto conflict_key = mimir::MimirKey(file_advice._index);
    resolve.resumeTime();
    advice_handler->resolve_conflicts(conflict_key);
    resolve.pauseTime();
  }

  for (int i = 0; i < args.num_operations; ++i) {
    mimir::MimirKey key;
    key._id = file_advice._index;
    remove.resumeTime();
    advice_handler->remove_advice(key, file_advice._index);
    remove.pauseTime();
  }
  double total_init = 0.0, total_save = 0.0, total_find = 0.0,
         total_remove = 0.0, total_conflicts = 0.0, total_resolve = 0.0;
  double init_time = init.getElapsedTime(), save_time = save.getElapsedTime(),
         find_time = find.getElapsedTime(),
         remove_time = remove.getElapsedTime(),
         add_conflicts_time = add_conflicts.getElapsedTime(),
         resolve_time = resolve.getElapsedTime();
  MPI_Reduce(&init_time, &total_init, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&save_time, &total_save, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&find_time, &total_find, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&remove_time, &total_remove, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&add_conflicts_time, &total_conflicts, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&resolve_time, &total_resolve, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  if (info.rank == 0) {
    fprintf(
        stdout,
        "Timing:\n"
        "init,save,find,add_conflicts,resolve_conflicts,remove\n"
        "%f,%f,%f,%f,%f,%f\n",
        args.num_operations * info.comm_size * info.comm_size / total_init,
        args.num_operations * info.comm_size * info.comm_size / total_save,
        args.num_operations * info.comm_size * info.comm_size / total_find,
        args.num_operations * info.comm_size * info.comm_size / total_conflicts,
        args.num_operations * info.comm_size * info.comm_size / total_resolve,
        args.num_operations * info.comm_size * info.comm_size / total_remove);
  }
}

TEST_CASE("footprint",
          "[test=footprint]"
          "[iteration=" +
              std::to_string(args.num_operations) + "]") {
  size_t base_file_advice_size = 0, base_job_advice_size = 0,
         base_app_advice_size = 0;

  mimir::FileAdvice file_advice;
  file_advice._type._secondary = mimir::OperationAdviceType::INDEPENDENT_FILE;
  file_advice._per_io_data = args.num_operations / (args.num_operations + 2);
  file_advice._per_io_metadata = 2 / (args.num_operations + 2);
  file_advice._size_mb = 1024 * args.num_operations / MB;
  file_advice._current_device = 1;
  file_advice._write_distribution._0_4kb = 1.0;
  file_advice._io_amount_mb = 1024 * args.num_operations / MB;
  file_advice._format = mimir::Format::FORMAT_BINARY;
  file_advice._priority = 100;
  base_file_advice_size =
      sizeof(file_advice._type) + sizeof(file_advice._per_io_data) +
      sizeof(file_advice._per_io_metadata) + sizeof(file_advice._size_mb) +
      sizeof(file_advice._current_device) +
      sizeof(file_advice._write_distribution) +
      sizeof(file_advice._io_amount_mb) + sizeof(file_advice._format) +
      sizeof(file_advice._priority);

  auto SHM = std::getenv("SHM_PATH");
  auto PFS = std::getenv("PFS_PATH");
  mimir::JobConfigurationAdvice job_conf_advice;
  job_conf_advice._job_id = 0;
  job_conf_advice._devices.emplace_back(SHM, 16);
  job_conf_advice._devices.emplace_back(PFS, 128);
  job_conf_advice._job_time_minutes = 30;
  job_conf_advice._num_cores_per_node = 2;
  job_conf_advice._num_gpus_per_node = 0;
  job_conf_advice._num_nodes = 2;
  job_conf_advice._node_names = {"localhost", "localhost"};
  job_conf_advice._rpc_port = 8888;
  job_conf_advice._rpc_threads = 1;
  job_conf_advice._priority = 100;
  base_job_advice_size =
      sizeof(job_conf_advice._type) + sizeof(job_conf_advice._job_id) +
      job_conf_advice._devices[0]._mount_point.size() +
      sizeof(job_conf_advice._devices[0]._used_capacity_mb) +
      sizeof(job_conf_advice._devices[0]._capacity_mb) +
      job_conf_advice._devices[1]._mount_point.size() +
      sizeof(job_conf_advice._devices[1]._used_capacity_mb) +
      sizeof(job_conf_advice._devices[1]._capacity_mb) +
      sizeof(job_conf_advice._job_time_minutes) +
      sizeof(job_conf_advice._num_cores_per_node) +
      sizeof(job_conf_advice._num_gpus_per_node) +
      sizeof(job_conf_advice._num_nodes) +
      job_conf_advice._node_names[0].size() +
      job_conf_advice._node_names[1].size() +
      sizeof(job_conf_advice._rpc_port) + sizeof(job_conf_advice._rpc_threads) +
      sizeof(job_conf_advice._priority);

  mimir::ApplicationAdvice app_advice;
  app_advice._interfaces_used.push_back(mimir::InterfaceType::POSIX);
  app_advice._io_size_mb = 1024 * args.num_operations / MB;
  app_advice._num_apps = args.num_operations;
  app_advice._num_cpu_cores_used =
      job_conf_advice._num_cores_per_node * job_conf_advice._num_nodes;
  app_advice._num_gpus_used =
      job_conf_advice._num_gpus_per_node * job_conf_advice._num_nodes;
  app_advice._per_io_data = file_advice._per_io_data;
  app_advice._per_io_metadata = file_advice._per_io_metadata;
  app_advice._runtime_minutes = job_conf_advice._job_time_minutes;
  app_advice._ts_distribution = file_advice._write_distribution;
  app_advice._priority = 100;

  base_app_advice_size =
      sizeof(app_advice._type) + sizeof(app_advice._interfaces_used[0]) +
      sizeof(app_advice._io_size_mb) + sizeof(app_advice._num_apps) +
      sizeof(app_advice._num_cpu_cores_used) +
      sizeof(app_advice._num_gpus_used) + sizeof(app_advice._per_io_data) +
      sizeof(app_advice._per_io_metadata) +
      sizeof(app_advice._runtime_minutes) +
      sizeof(app_advice._ts_distribution) + sizeof(app_advice._priority);

  size_t file_advice_size = 0, job_advice_size = 0, app_advice_size = 0;
  for (uint32_t i = 0; i < args.num_operations; ++i) {
    file_advice._name = "file_" + std::to_string(i);
    file_advice_size += base_file_advice_size + file_advice._name.size();

    mimir::Application app = {static_cast<uint32_t>(args.num_operations + i),
                              "app_" + std::to_string(i),
                              static_cast<uint32_t>(args.num_operations + i)};
    auto app_size = sizeof(app._unique_hash) + app._name.size() +
                    sizeof(app._argument_hash);

    mimir::File file = {i, file_advice._name, i};
    auto file_size = sizeof(file._unique_hash) + file._name.size() +
                     sizeof(file._full_path_hash);

    app_advice._application_file_dag.applications =
        std::vector<mimir::Application>();
    app_advice._application_file_dag.applications.push_back(app);
    app_advice._application_file_dag.files = std::vector<mimir::File>();
    app_advice._application_file_dag.files.push_back(file);
    app_advice._application_file_dag.edges =
        std::vector<mimir::Edge<mimir::Node, mimir::Node>>();
    app_advice._application_file_dag.edges.push_back({app, file});
    app_advice._file_access_pattern =
        std::unordered_map<mimir::File, mimir::AccessPattern>();
    app_advice._file_access_pattern.emplace(file,
                                            mimir::AccessPattern::SEQUENTIAL);
    app_advice._independent_files = std::vector<mimir::File>();
    app_advice._independent_files.push_back(file);
    app_advice._shared_files =
        std::unordered_map<mimir::File, std::vector<mimir::Application>>();
    app_advice._shared_files.emplace(
        file, app_advice._application_file_dag.applications);
    app_advice._rank_file_dag.ranks = std::vector<mimir::Rank>();
    app_advice._rank_file_dag.ranks.push_back(0);
    app_advice._rank_file_dag.files = app_advice._application_file_dag.files;
    app_advice._rank_file_dag.edges =
        std::vector<mimir::Edge<mimir::Node, mimir::Node>>();
    app_advice._rank_file_dag.edges.push_back({0, file});
    app_advice_size += base_app_advice_size + (2 * (app_size + file_size)) +
                       (file_size + sizeof(mimir::AccessPattern::SEQUENTIAL)) +
                       file_size + (file_size + app_size) +
                       (file_size + sizeof(mimir::Rank));

    job_advice_size += base_job_advice_size;
  }

  printf("%zu,%zu,%zu\n", app_advice_size, file_advice_size, job_advice_size);
}

TEST_CASE("hostfile",
          "[test=hostfile]"
          "[ranks_per_node=" +
          std::to_string(args.num_operations) + "]") {
  auto ranks_per_node = args.num_operations;
  auto LSB_HOSTS = std::getenv("LSB_HOSTS");
  if (LSB_HOSTS == nullptr) {
    LSB_HOSTS = "localhost";
  }
  auto node_names = split_string(LSB_HOSTS);
  auto num_nodes = node_names.size();
  std::string filename = "hostfile_" + std::to_string(ranks_per_node) + "_" + std::to_string(num_nodes);
  std::ofstream outfile (filename.c_str());
  for(auto node_name:node_names) {
    auto data = node_name+"  slots="+std::to_string(ranks_per_node)+"\n";
    outfile.write(data.data(), data.size());
  }
  outfile.close();
}


TEST_CASE("optimization",
          "[test=optimization]"
          "[iteration=" +
              std::to_string(args.num_operations) + "]") {
  int my_rank=-1, comm_size, world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  auto num_ranks_per_node = atoi(std::getenv("MPI_PROCS_PER_NODE"));
  bool is_client = world_rank % num_ranks_per_node != 0;
  MPI_Comm client_comm;
  MPI_Comm_split(MPI_COMM_WORLD, is_client, world_rank, &client_comm);
  int request_size = 4 * 1024;
  MPI_Comm_size(client_comm, &comm_size);
  if (!is_client) {
    comm_size = world_size - comm_size;
  }
  auto PFS = std::string(std::getenv("PFS_PATH")) + "/" + std::to_string(comm_size);
  auto SHM = std::string(std::getenv("SHM_PATH")) + "/" + std::to_string(comm_size);
  auto read_file = fs::path(PFS) / "test_read_" + ".dat";
  auto write_ind_file = fs::path(PFS) / "test_write_ind_" + ".dat";
  auto write_shared_file = fs::path(PFS) / "test_write_shared_" + ".dat";
  int num_producers = comm_size / 2;
  if (num_producers == 0) num_producers = 1;
  bool is_producer = false;
  if (is_client) {
    MPI_Comm_rank(client_comm, &my_rank);
    read_file = fs::path(PFS) / "test_read_" + std::to_string(my_rank) + "_" +
                std::to_string(comm_size) + ".dat";
    std::string cmd_clean =
        "rm -rf " + std::string(PFS) + "/* " + std::string(SHM) + "/* ";
    system(cmd_clean.c_str());

    std::string cmd_mkdir =
        "mkdir -p " + std::string(PFS) + " " + std::string(SHM) + " ";
    system(cmd_mkdir.c_str());
    MPI_Barrier(client_comm);
    mimir::Logger::Instance("PEGASUS_TEST")
        ->log(mimir::LOG_INFO, "Cleaned PFS %s and SHM %s", PFS.c_str(), SHM.c_str());
    write_ind_file = fs::path(PFS) / "test_write_ind_" +
                          std::to_string(my_rank) + "_" +
                          std::to_string(comm_size) + ".dat";
    if (my_rank < num_producers) {
      is_producer = true;
    }
    int producer_rank_file = my_rank % num_producers;
    write_shared_file = fs::path(PFS) / "test_write_shared_" +
                             std::to_string(producer_rank_file) + "_" +
                             std::to_string(num_producers) + ".dat";


    std::string cmd = "{ tr -dc '[:alnum:]' < /dev/urandom | head -c " +
                      std::to_string(request_size * args.num_operations) +
                      "; } > " + read_file.c_str() + " ";
    int status = system(cmd.c_str());
    if (fs::exists(read_file.c_str())) {
      mimir::Logger::Instance("PEGASUS_TEST")
          ->log(mimir::LOG_INFO, "written file %s", read_file.c_str());
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  Timer preload_timer, read_only_timer, write_i_timer, read_i_timer, write_s_timer, read_s_timer;

    mimir::FileAdvice read_file_advice;
    read_file_advice._type._secondary =
        mimir::OperationAdviceType::READ_ONLY_FILE;
    read_file_advice._per_io_data =
        args.num_operations / (args.num_operations + 2);
    read_file_advice._per_io_metadata = 2 / (args.num_operations + 2);
    read_file_advice._size_mb = 1024 * args.num_operations / MB;
    read_file_advice._current_device = 1;
    read_file_advice._write_distribution._64kb_1mb = 1.0;
    read_file_advice._io_amount_mb = request_size * args.num_operations / MB;
    read_file_advice._format = mimir::Format::FORMAT_BINARY;
    read_file_advice._priority = 100;

    mimir::FileAdvice write_file_ind_advice;
    write_file_ind_advice._type._secondary =
        mimir::OperationAdviceType::INDEPENDENT_FILE;
    write_file_ind_advice._per_io_data =
        args.num_operations / (args.num_operations + 2);
    write_file_ind_advice._per_io_metadata = 2 / (args.num_operations + 2);
    write_file_ind_advice._size_mb = 1024 * args.num_operations / MB;
    write_file_ind_advice._current_device = 1;
    write_file_ind_advice._write_distribution._64kb_1mb = 1.0;
    write_file_ind_advice._io_amount_mb = request_size * args.num_operations / MB;
    write_file_ind_advice._format = mimir::Format::FORMAT_BINARY;
    write_file_ind_advice._priority = 100;
    write_file_ind_advice._name = write_ind_file.string();

    mimir::FileAdvice write_file_shared_advice;
    write_file_shared_advice._type._secondary =
        mimir::OperationAdviceType::SHARED_FILE;
    write_file_shared_advice._per_io_data =
        args.num_operations / (args.num_operations + 2);
    write_file_shared_advice._per_io_metadata = 2 / (args.num_operations + 2);
    write_file_shared_advice._size_mb = 1024 * args.num_operations / MB;
    write_file_shared_advice._current_device = 1;
    write_file_shared_advice._write_distribution._64kb_1mb = 1.0;
    write_file_shared_advice._io_amount_mb =
        request_size * args.num_operations / MB;
    write_file_shared_advice._format = mimir::Format::FORMAT_BINARY;
    write_file_shared_advice._priority = 100;

    mimir::MimirHandler read_file_handler, write_ind_handler,
        write_shared_handler;

    for (int i = 0; i < comm_size; ++i) {
      if (i != my_rank) {
        auto read_file = fs::path(PFS) / "test_read_" + std::to_string(i) + "_" +
                         std::to_string(comm_size) + ".dat";
        read_file_advice._name = read_file.string();
        mimir::file_advice_begin(read_file_advice, read_file_handler);
      }
    }

    for (int i = 0; i < num_producers; ++i) {
      auto write_shared_file = fs::path(PFS) / "test_write_shared_" +
                               std::to_string(i) + "_" +
                               std::to_string(num_producers) + ".dat";
      write_file_shared_advice._name = write_shared_file.string();
      mimir::file_advice_begin(write_file_shared_advice, write_shared_handler);
    }


  MPI_Barrier(MPI_COMM_WORLD);
  if (is_client) {
    read_file_advice._name = read_file.string();
    write_file_shared_advice._name = write_shared_file.string();
    mimir::file_advice_begin(write_file_ind_advice, write_ind_handler);
    MPI_Barrier(client_comm);
    if (my_rank == 0)
      mimir::Logger::Instance("PEGASUS_TEST")
          ->log(mimir::LOG_ERROR, "--------------------");
    MPI_Barrier(client_comm);

    /**
     * Preload data
     */
    preload_timer.resumeTime();
    read_file_advice._prefetch = true;
    mimir::file_advice_begin(read_file_advice, read_file_handler);
    preload_timer.pauseTime();
    MPI_Barrier(client_comm);
    if (my_rank == 0)
      mimir::Logger::Instance("PEGASUS_TEST")
          ->log(mimir::LOG_ERROR, "Prefetch Done --------------------");
    MPI_Barrier(client_comm);
    /**
     * Read Only file data
     */
    {
      auto read_data = std::vector<char>(request_size, 'r');
      int read_fd = open(read_file.c_str(), O_RDONLY);
      REQUIRE(read_fd != -1);
      for (int i = 0; i < args.num_operations; ++i) {
        read_only_timer.resumeTime();
        ssize_t bytes_read = read(read_fd, read_data.data(), request_size);
        read_only_timer.pauseTime();
        REQUIRE(bytes_read == request_size);
      }
      int close_status = close(read_fd);
      REQUIRE(close_status == 0);
    }
    MPI_Barrier(client_comm);
    if (my_rank == 0)
      mimir::Logger::Instance("PEGASUS_TEST")
          ->log(mimir::LOG_ERROR, "Read Done --------------------");
    MPI_Barrier(client_comm);
    /**
     * Write Individual
     */
    {
      {
        auto write_data = std::vector<char>(request_size, 'w');
        int write_fd = open(write_ind_file.c_str(), O_WRONLY | O_CREAT,
                            S_IRWXU | S_IRWXG | S_IRWXO);
        REQUIRE(write_fd != -1);
        for (int i = 0; i < args.num_operations; ++i) {
          write_i_timer.resumeTime();
          ssize_t bytes_written = write(write_fd, write_data.data(), request_size);
          write_i_timer.pauseTime();
          REQUIRE(bytes_written == request_size);
        }
        int close_status = close(write_fd);
        REQUIRE(close_status == 0);
      }
      {
        auto read_data = std::vector<char>(request_size, 'r');
        int read_fd = open(write_ind_file.c_str(), O_RDONLY | O_DIRECT | O_SYNC);
        REQUIRE(read_fd != -1);
        for (int i = 0; i < args.num_operations; ++i) {
          read_i_timer.resumeTime();
          ssize_t bytes_read = read(read_fd, read_data.data(), request_size);
          read_i_timer.pauseTime();
          REQUIRE(bytes_read == request_size);
        }
        int close_status = close(read_fd);
        REQUIRE(close_status == 0);
      }
    }

    MPI_Barrier(client_comm);
    if (my_rank == 0)
      mimir::Logger::Instance("PEGASUS_TEST")
          ->log(mimir::LOG_ERROR, "Write Individual Done --------------------");
    MPI_Barrier(client_comm);
    /**
     * Write Shared
     */
    {
      if (is_producer) {
        auto write_data = std::vector<char>(request_size, 'w');
        int write_fd = open(write_shared_file.c_str(), O_WRONLY | O_CREAT | O_DIRECT | O_SYNC,
                            S_IRWXU | S_IRWXG | S_IRWXO);
        REQUIRE(write_fd != -1);
        for (int i = 0; i < args.num_operations; ++i) {
          write_s_timer.resumeTime();
          ssize_t bytes_read = write(write_fd, write_data.data(), request_size);
          write_s_timer.pauseTime();
          REQUIRE(bytes_read == request_size);
        }
        int close_status = close(write_fd);
        REQUIRE(close_status == 0);
      }
      MPI_Barrier(client_comm);
      if (!is_producer) {
        auto read_data = std::vector<char>(request_size, 'r');
        int read_fd = open(write_shared_file.c_str(), O_RDONLY);
        REQUIRE(read_fd != -1);
        for (int i = 0; i < args.num_operations; ++i) {
          read_s_timer.resumeTime();
          ssize_t bytes_read = read(read_fd, read_data.data(), request_size);
          read_s_timer.pauseTime();
          REQUIRE(bytes_read == request_size);
        }
        int close_status = close(read_fd);
        REQUIRE(close_status == 0);
      }
      MPI_Barrier(client_comm);
    }

    MPI_Barrier(client_comm);
    if (my_rank == 0)
      mimir::Logger::Instance("PEGASUS_TEST")
          ->log(mimir::LOG_ERROR, "Write Shared Done --------------------");
    MPI_Barrier(client_comm);
    double total_preload = 0.0, total_read_only = 0.0,
        total_write_i = 0.0, total_read_i = 0.0,
        total_write_s = 0.0, total_read_s = 0.0;
    double preload_time = preload_timer.getElapsedTime(),
        write_i_time = write_i_timer.getElapsedTime(),
        read_i_time = read_i_timer.getElapsedTime(),
        write_s_time = write_s_timer.getElapsedTime(),
        read_s_time = read_s_timer.getElapsedTime(),
        read_only_time = read_only_timer.getElapsedTime();


    MPI_Reduce(&preload_time, &total_preload, 1, MPI_DOUBLE, MPI_SUM, 0, client_comm);
    MPI_Reduce(&read_only_time, &total_read_only, 1, MPI_DOUBLE, MPI_SUM, 0, client_comm);
    MPI_Reduce(&write_i_time, &total_write_i, 1, MPI_DOUBLE, MPI_SUM, 0, client_comm);
    MPI_Reduce(&read_i_time, &total_read_i, 1, MPI_DOUBLE, MPI_SUM, 0, client_comm);
    MPI_Reduce(&write_s_time, &total_write_s, 1, MPI_DOUBLE, MPI_SUM, 0, client_comm);
    MPI_Reduce(&read_s_time, &total_read_s, 1, MPI_DOUBLE, MPI_SUM, 0, client_comm);
    if (my_rank == 0) {
      ////"Timing,iter,procs,preload,read_only,write_i,read_i,write_s,read_s\n"
      fprintf(stdout,
              "Timing,%d,%d,%f,%f,%f,%f,%f,%f\n",
              args.num_operations, comm_size,
              total_preload / comm_size, total_read_only / comm_size,
              total_write_i / comm_size, total_read_i / comm_size,
              total_write_s * 2 / comm_size, total_read_s * 2 / comm_size);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  int local = 0, total_local = 0;
  int remote = 0, total_remote = 0;
  auto tracker = MIMIR_TRACKER();
  if (tracker != nullptr) {
    local = tracker->local.load();
    remote = tracker->remote.load();
  }

  MPI_Reduce(&local, &total_local, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&remote, &total_remote, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    ////"Timing,local,remote\n"
    fprintf(stdout,
            "Timing,%d,%d\n",
            total_local, total_remote);
  }
}