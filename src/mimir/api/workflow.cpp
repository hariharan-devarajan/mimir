//
// Created by haridev on 2/16/22.
//

#include <mimir/advice/advice_handler.h>
#include <mimir/api/workflow.h>
#include <mimir/common/error_code.h>

namespace mimir {
MimirStatus workflow_advice_begin(WorkflowAdvice &advice,
                                  MimirHandler &handler) {
  if (advice._type._primary != PrimaryAdviceType::JOB_WORKFLOW) {
    return MIMIR_ONLY_WORKFLOW_ALLOWED;
  }
  mimir::MimirKey key;
  key._id = 0;
  AdviceHandler<WorkflowAdvice>::Instance(advice._type)
      ->save_advice(key, advice);
  handler._type = advice._type;
  handler._key_id = key._id;
  handler._advice_index = advice._index;
  return MIMIR_SUCCESS;
}
MimirStatus workflow_advice_end(MimirHandler &handler) {
  mimir::MimirKey key;
  key._id = 0;
  AdviceHandler<WorkflowAdvice>::Instance(handler._type)
      ->remove_advice(key, handler._advice_index);
  return MIMIR_SUCCESS;
}

MimirStatus free_workflow() {
  auto ptr = AdviceHandler<WorkflowAdvice>::Instance(
      {mimir::PrimaryAdviceType::JOB_WORKFLOW,
       mimir::OperationAdviceType::NO_OP});
  ptr->clear();
  ptr.reset();
  return MIMIR_SUCCESS;
}
}  // namespace mimir
