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

 public:
  std::shared_ptr<RPC> rpc;
  static std::shared_ptr<Server> Instance(bool is_mpi = false) {
    auto job_conf_type =
        mimir::AdviceType(mimir::PrimaryAdviceType::JOB_CONFIGURATION,
                          mimir::OperationAdviceType::NO_OP);
    auto _job_handler =
        mimir::AdviceHandler<mimir::JobConfigurationAdvice>::Instance(
            job_conf_type);
    mimir::MimirKey job_conf_key;
    job_conf_key._id = 0;
    auto job_conf_advices = _job_handler->find_advice(job_conf_key);
    if (job_conf_advices.first) {
      if (instance == nullptr) {
        instance = std::make_shared<Server>(is_mpi);
      }
    }
    return instance;
  }
  Server(bool is_mpi);

  void finalize() {}
};
}  // namespace athena
#endif  // MIMIR_ATHENA_SERVER_H
