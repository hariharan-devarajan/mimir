//
// Created by haridev on 2/16/22.
//

#include <mimir/api/job_configuration.h>
#include <mimir/common/error_code.h>
#include <mimir/advice/advice_handler.h>

namespace mimir {
    MimirStatus job_configuration_advice_begin(JobConfigurationAdvice &advice, MimirHandler &handler) {
        if (advice._type._primary != PrimaryAdviceType::JOB_CONFIGURATION) {
            return MIMIR_ONLY_JOB_CONFIGURATION_ALLOWED;
        }
        mimir::MimirKey key;
        key._id = 0;
        AdviceHandler<JobConfigurationAdvice>::Instance(advice._type)->save_advice(key, advice);
        handler._type = advice._type;
        handler._id = key._id;
        return MIMIR_SUCCESS;
    }
    MimirStatus job_configuration_advice_end(MimirHandler &handler) {
        mimir::MimirKey key;
        key._id = 0;
        AdviceHandler<JobConfigurationAdvice>::Instance(handler._type)->remove_advice(key);
        return MIMIR_SUCCESS;
    }
}
