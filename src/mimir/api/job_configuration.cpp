//
// Created by haridev on 2/16/22.
//

#include <mimir/api/job_configuration.h>
#include <mimir/common/error_code.h>
#include <athena/mimir_adapter/advice/advice_handler.h>

namespace mimir {
    MimirStatus job_configuration_advice_begin(JobConfigurationAdvice &advice, MimirHandler &handler) {
        if (handler._type._primary != PrimaryAdviceType::JOB_CONFIGURATION) {
            return MIMIR_ONLY_JOB_CONFIGURATION_ALLOWED;
        }
        MimirKey key;
        std::hash<uint32_t> hash_str;
        key._id = hash_str(advice._job_id);
        AdviceHandler<JobConfigurationAdvice>::Instance(advice._type)->save_advice(key, advice);
        handler._type = advice._type;
        handler._id = key._id;
        return MIMIR_SUCCESS;
    }
    MimirStatus job_configuration_advice_end(MimirHandler &handler);
}
