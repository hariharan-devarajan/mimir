//
// Created by haridev on 2/16/22.
//

#ifndef MIMIR_API_WORKFLOW_H
#define MIMIR_API_WORKFLOW_H

#include <mimir/advice/workflow_advice.h>
#include <mimir/typedef.h>

namespace mimir {
MimirStatus workflow_advice_begin(WorkflowAdvice &payload,
                                  MimirHandler &handler);
MimirStatus workflow_advice_end(MimirHandler &handler);
MimirStatus free_workflow();
}  // namespace mimir

#endif  // MIMIR_API_WORKFLOW_H
