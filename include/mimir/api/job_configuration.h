//
// Created by haridev on 2/16/22.
//

#ifndef MIMIR_API_JOB_CONFIGURATION_H
#define MIMIR_API_JOB_CONFIGURATION_H

#include <mimir/advice/job_configuration_advice.h>
#include <mimir/typedef.h>

namespace mimir {
MimirStatus job_configuration_advice_begin(JobConfigurationAdvice &payload,
                                           MimirHandler &handler);
MimirStatus job_configuration_advice_end(MimirHandler &handler);
MimirStatus free_job_configuration();
}  // namespace mimir

#endif  // MIMIR_API_JOB_CONFIGURATION_H
