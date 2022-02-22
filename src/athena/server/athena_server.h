//
// Created by hariharan on 2/21/22.
//

#ifndef MIMIR_ATHENA_SERVER_H
#define MIMIR_ATHENA_SERVER_H

#include <mimir/advice/job_configuration_advice.h>

#include <memory>
namespace athena {
class Server {
 private:
  static std::shared_ptr<Server> instance;
  mimir::JobConfigurationAdvice job_configuration_advice;

 public:
  static std::shared_ptr<Server> Instance() {
    if (instance == nullptr) {
      instance = std::make_shared<Server>();
    }
    return instance;
  }
  Server();

  void finalize() {}
};
}  // namespace athena
#endif  // MIMIR_ATHENA_SERVER_H
