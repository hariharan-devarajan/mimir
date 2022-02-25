//
// Created by hariharan on 2/21/22.
//

#ifndef MIMIR_ATHENA_SERVER_H
#define MIMIR_ATHENA_SERVER_H

#include <mimir/advice/job_configuration_advice.h>

#include <memory>
#include <hcl/communication/rpc_lib.h>
#include "mimir/advice/advice_handler.h"
namespace athena {
class Server {
 private:
  static std::shared_ptr<Server> instance;
  mimir::JobConfigurationAdvice _job_configuration_advice;
  bool is_server;

 public:
  std::shared_ptr<RPC> _rpc;
  static std::shared_ptr<Server> Instance(bool is_mpi = false) {
    if (instance == nullptr) {
      instance = std::make_shared<Server>(is_mpi);
    }
    return instance;
  }
  Server(bool is_mpi);

  void finalize() {
    if (is_server) {
      //_rpc->Stop();
    }
  }
};
}  // namespace athena
#endif  // MIMIR_ATHENA_SERVER_H
