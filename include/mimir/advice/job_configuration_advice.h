//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_JOB_CONFIGURATION_ADVICE_H
#define MIMIR_JOB_CONFIGURATION_ADVICE_H

#include <mimir/common/data_structure.h>
#include <mimir/advice/advice.h>

namespace mimir {
    class JobConfigurationAdvice : public Advice {
    public:
        JobConfigurationAdvice(AdviceType type) : Advice(type) {}
        using Advice::_type;
        uint32_t _num_nodes;
        uint16_t _num_cores_per_node;
        uint8_t _num_gpus_per_node;
        std::vector <Storage> _devices;
        uint32_t _job_time_minutes;

        JobConfigurationAdvice() : Advice(AdviceType(PrimaryAdviceType::JOB_CONFIGURATION,
                                                     OperationAdviceType::NO_OP)),
                                   _num_nodes(1), _num_cores_per_node(1),
                                   _num_gpus_per_node(0), _devices(),
                                   _job_time_minutes(30) {}

    };
}
#endif //MIMIR_JOB_CONFIGURATION_ADVICE_H
