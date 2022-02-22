//
// Created by hariharan on 2/21/22.
//

#ifndef MIMIR_ATHENA_CLIENT_H
#define MIMIR_ATHENA_CLIENT_H

#include <hcl/communication/rpc_lib.h>
#include <mpi.h>

#include <memory>

#include "mimir/advice/advice_handler.h"
#include "mimir/advice/job_configuration_advice.h"

namespace athena {
class Client {
 private:
  static std::shared_ptr<Client> instance;

 public:
  mimir::JobConfigurationAdvice job_configuration_advice;
  std::shared_ptr<mimir::AdviceHandler<mimir::JobConfigurationAdvice>>
      job_handler;
  static std::shared_ptr<Client> Instance() {
    if (instance == nullptr) {
      instance = std::make_shared<Client>();
    }
    return instance;
  }
  Client() {
    auto job_conf_type =
        mimir::AdviceType(mimir::PrimaryAdviceType::JOB_CONFIGURATION,
                          mimir::OperationAdviceType::NO_OP);
    job_handler = mimir::AdviceHandler<mimir::JobConfigurationAdvice>::Instance(
        job_conf_type);
    mimir::MimirKey job_conf_key;
    job_conf_key._id = 0;
    auto job_conf_advices = job_handler->find_advice(job_conf_key);
    if (job_conf_advices.first) {
      job_configuration_advice = job_conf_advices.second[0];
      int current_rank;
      MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
      std::shared_ptr<RPC> rpc;
      if (current_rank % job_configuration_advice._num_cores_per_node != 0) {
        // node server rank
        /** TODO: Init rpc **/
      }
    } else {
      throw std::runtime_error("[Athena] Job Configuration Advice not set.");
    }
  }

  void finalize() {}
};
}  // namespace athena
#endif  // MIMIR_ATHENA_CLIENT_H
