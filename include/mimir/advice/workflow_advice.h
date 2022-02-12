//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_WORKFLOW_ADVICE_H
#define MIMIR_WORKFLOW_ADVICE_H
namespace mimir {
    class WorkflowAdvice : public Advice {
    public:
        using Advice::_type;
        uint16 _num_cpu_cores_used;
        uint8 _num_gpus_used;
        uint32 _num_apps;
        ApplicationFileDAG _application_file_dag;
        std::vector <File> _independent_files;
        std::unordered_map <File, std::vector<Application>> _shared_files;
        uint32 _io_size_mb;
        float _per_io_data, _per_io_metadata;
        TransferSizeDistribution _ts_distribution;
        std::vector <InterfaceType> _interfaces_used;
        std::unordered_map <File, AccessPattern> _file_access_pattern;

        uint32 _runtime_minutes;


        WorkflowAdvice() : Advice(PrimaryAdviceType::JOB_WORKFLOW), _num_cpu_cores_used(1),
                           _num_gpus_used(0), _num_apps(1), _application_file_dag() {}
    }
} //mimir
#endif //MIMIR_WORKFLOW_ADVICE_H
