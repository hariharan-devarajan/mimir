//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_APPLICATION_ADVICE_H
#define MIMIR_APPLICATION_ADVICE_H

#include <mimir/advice/advice_type.h>
#include <mimir/advice/workflow_advice.h>

namespace mimir {
    class ApplicationAdvice : public WorkflowAdvice {
    public:
        using Advice::_type;
        using WorkflowAdvice::_num_cpu_cores_used;
        using WorkflowAdvice::_num_gpus_used;
        using WorkflowAdvice::_num_apps;
        using WorkflowAdvice::_application_file_dag;
        using WorkflowAdvice::_independent_files;
        using WorkflowAdvice::_shared_files;
        using WorkflowAdvice::_io_size_mb;
        using WorkflowAdvice::_per_io_data;
        using WorkflowAdvice::_per_io_metadata;
        using WorkflowAdvice::_ts_distribution;
        using WorkflowAdvice::_interfaces_used;
        using WorkflowAdvice::_runtime_minutes;
        using WorkflowAdvice::_file_access_pattern;

        std::string _name;
        RankFileDAG _rank_file_dag;

        ApplicationAdvice() : WorkflowAdvice(AdviceType(PrimaryAdviceType::JOB_WORKFLOW,
                                                        OperationAdviceType::NO_OP)),
                                                        _name(), _rank_file_dag() {}
        ApplicationAdvice(const ApplicationAdvice& other):WorkflowAdvice(other),
                                                          _name(), _rank_file_dag() {}
        ApplicationAdvice(const ApplicationAdvice&& other):WorkflowAdvice(other),
                                                           _name(), _rank_file_dag() {}
        ApplicationAdvice& operator=(const ApplicationAdvice& other) {
            WorkflowAdvice::operator=(other);
            this->_name = other._name;
            this->_rank_file_dag = other._rank_file_dag;
            return *this;
        }
        bool operator<(const ApplicationAdvice& other) const {
            return WorkflowAdvice::operator<(other);
        }

        bool operator>(const ApplicationAdvice& other) const {
            return !(*this < other);
        }
    };
} //mimir
#endif //MIMIR_APPLICATION_ADVICE_H
