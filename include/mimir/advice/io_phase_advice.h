//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_IO_PHASE_ADVICE_H
#define MIMIR_IO_PHASE_ADVICE_H
namespace mimir {
    class IOPhaseAdvice : public Advice {
    public:
        using Advice::_type;
        std::vector <File> _independent_files;
        std::unordered_map <File, std::vector<Application>> _shared_files;
        uint32 _io_size_mb;
        float _per_io_data, _per_io_metadata;
        TransferSizeDistribution _ts_distribution;
        std::vector <InterfaceType> _interfaces_used;
        std::unordered_map <File, AccessPattern> _file_access_pattern;
        uint32 _runtime_minutes;

        IOPhaseAdvice() : Advice(PrimaryAdviceType::JOB_IO_PHASE), _independent_files(),
                          _shared_files(), _io_size_mb(), _per_io_data(), _per_io_metadata(),
                          _ts_distribution(), _interfaces_used(), _file_access_pattern(),
                          _runtime_minutes() {}
    }
} //mimir
#endif //MIMIR_IO_PHASE_ADVICE_H
