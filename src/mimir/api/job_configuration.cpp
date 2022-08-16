//
// Created by haridev on 2/16/22.
//

#include <mimir/advice/advice_handler.h>
#include <mimir/api/job_configuration.h>
#include <mimir/common/error_code.h>

namespace mimir {
MimirStatus job_configuration_advice_begin(JobConfigurationAdvice &advice,
                                           MimirHandler &handler) {
  if (advice._type._primary != PrimaryAdviceType::JOB_CONFIGURATION) {
    return MIMIR_ONLY_JOB_CONFIGURATION_ALLOWED;
  }
  mimir::MimirKey key;
  key._id = 0;
  AdviceHandler<JobConfigurationAdvice>::Instance(advice._type)
      ->save_advice(key, advice);
  handler._type = advice._type;
  handler._key_id = key._id;
  handler._advice_index = advice._index;
  return MIMIR_SUCCESS;
}
MimirStatus job_configuration_advice_end(MimirHandler &handler) {
  mimir::MimirKey key;
  key._id = 0;
  AdviceHandler<JobConfigurationAdvice>::Instance(handler._type)
      ->remove_advice(key, handler._advice_index);
  return MIMIR_SUCCESS;
}
MimirStatus free_job_configuration() {
  auto ptr = AdviceHandler<JobConfigurationAdvice>::Instance(
      {mimir::PrimaryAdviceType::JOB_CONFIGURATION,
       mimir::OperationAdviceType::NO_OP});
  ptr->clear();
  ptr.reset();
  return MIMIR_SUCCESS;
}
}  // namespace mimir
