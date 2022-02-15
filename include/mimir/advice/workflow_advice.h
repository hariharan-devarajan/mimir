//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_WORKFLOW_ADVICE_H
#define MIMIR_WORKFLOW_ADVICE_H

#include <unordered_map>
#include <mimir/common/enumeration.h>
#include <mimir/advice/advice.h>

namespace mimir {
    class WorkflowAdvice : public Advice {
    public:
        using Advice::_type;
        uint16_t _num_cpu_cores_used;
        uint8_t _num_gpus_used;
        uint32_t _num_apps;
        ApplicationFileDAG _application_file_dag;
        std::vector<File> _independent_files;
        std::unordered_map<File, std::vector<Application>> _shared_files;
        uint32_t _io_size_mb;
        float _per_io_data, _per_io_metadata;
        TransferSizeDistribution _ts_distribution;
        std::vector<InterfaceType> _interfaces_used;
        std::unordered_map<File, AccessPattern> _file_access_pattern;

        uint32_t _runtime_minutes;

        WorkflowAdvice(AdviceType type) : Advice(type), _num_cpu_cores_used(1),
                                          _num_gpus_used(0), _num_apps(1), _application_file_dag(),
                                          _independent_files(),
                                          _shared_files(), _io_size_mb(), _per_io_data(), _per_io_metadata(),
                                          _ts_distribution(),
                                          _interfaces_used(), _file_access_pattern(), _runtime_minutes() {}

        WorkflowAdvice() : WorkflowAdvice(AdviceType(PrimaryAdviceType::JOB_WORKFLOW,
                                                     OperationAdviceType::NO_OP)) {}
    };
} //mimir
#endif //MIMIR_WORKFLOW_ADVICE_H
