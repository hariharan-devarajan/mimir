//
// Created by haridev on 2/16/22.
//

#include <mimir/advice/advice_handler.h>
#include <mimir/api/application.h>
#include <mimir/common/error_code.h>

namespace mimir {
MimirStatus application_advice_begin(ApplicationAdvice &advice,
                                     MimirHandler &handler) {
  if (advice._type._primary != PrimaryAdviceType::JOB_APPLICATION) {
    return MIMIR_ONLY_APPLICATION_ALLOWED;
  }
  mimir::MimirKey key;
  key._id = 0;
  AdviceHandler<ApplicationAdvice>::Instance(advice._type)
      ->save_advice(key, advice);
  handler._type = advice._type;
  handler._key_id = key._id;
  handler._advice_index = advice._index;
  return MIMIR_SUCCESS;
}
MimirStatus application_advice_end(MimirHandler &handler) {
  mimir::MimirKey key;
  key._id = 0;
  AdviceHandler<ApplicationAdvice>::Instance(handler._type)
      ->remove_advice(key, handler._advice_index);
  return MIMIR_SUCCESS;
}

MimirStatus free_applications() {
  auto ptr = AdviceHandler<ApplicationAdvice>::Instance(
      {mimir::PrimaryAdviceType::JOB_APPLICATION,
       mimir::OperationAdviceType::NO_OP});
  ptr->clear();
  ptr.reset();
  return MIMIR_SUCCESS;
}
}  // namespace mimir
