//
// Created by hariharan on 2/21/22.
//

#ifndef MIMIR_ATHENA_SERVER_H
#define MIMIR_ATHENA_SERVER_H

#include <mimir/advice/job_configuration_advice.h>

#include <memory>
#include <hcl/communication/rpc_lib.h>
#include "mimir/advice/advice_handler.h"
#include "mimir/api/mimir_interceptor.h"
namespace athena {
class Server {
 private:
  mimir::JobConfigurationAdvice _job_configuration_advice;
  bool is_server;
  static std::shared_ptr<Server> instance;

 public:
  Server(bool is_mpi);
  std::shared_ptr<RPC> _rpc;
  static std::shared_ptr<Server> Instance() {
    if (instance == nullptr) {
      instance = std::make_shared<Server>(is_mpi());
    }
    return instance;
  }
  void finalize() {
    if (is_server) {
      //_rpc->Stop();
    }
  }
  bool bind_posix_calls();
};
}  // namespace athena
#endif  // MIMIR_ATHENA_SERVER_H
