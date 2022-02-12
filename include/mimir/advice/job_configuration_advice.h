//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_JOB_CONFIGURATION_ADVICE_H
#define MIMIR_JOB_CONFIGURATION_ADVICE_H

#include <mimir/advice/advice.h>
#include <mimir/common/data_structure.h>

namespace mimir {
    class JobConfigurationAdvice : public Advice {
    protected:
        JobConfigurationAdvice(AdviceType type) : Advice(type) {}

    public:
        using Advice::_type;
        uint32 _num_nodes;
        uint16 _num_cores_per_node;
        uint8 _num_gpus_per_node;
        std::vector <Storage> _devices;
        uint32 _job_time_minutes;

        JobConfigurationAdvice() : Advice(PrimaryAdviceType::JOB_CONFIGURATION),
                                   _num_nodes(1), _num_cores_per_node(1),
                                   _num_gpus_per_node(0), _devices(),
                                   _job_time_minutes(30) {}

    };
}
#endif //MIMIR_JOB_CONFIGURATION_ADVICE_H
