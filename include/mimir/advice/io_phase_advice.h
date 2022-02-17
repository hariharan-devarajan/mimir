//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_IO_PHASE_ADVICE_H
#define MIMIR_IO_PHASE_ADVICE_H
namespace mimir {
class IOPhaseAdvice : public Advice {
 public:
  using Advice::_type;
  std::vector<File> _independent_files;
  std::unordered_map<File, std::vector<Application>> _shared_files;
  uint32_t _io_size_mb;
  float _per_io_data, _per_io_metadata;
  TransferSizeDistribution _ts_distribution;
  std::vector<InterfaceType> _interfaces_used;
  std::unordered_map<File, AccessPattern> _file_access_pattern;
  uint32_t _runtime_minutes;

  IOPhaseAdvice()
      : Advice(AdviceType(PrimaryAdviceType::JOB_IO_PHASE,
                          OperationAdviceType::NO_OP)),
        _independent_files(),
        _shared_files(),
        _io_size_mb(),
        _per_io_data(),
        _per_io_metadata(),
        _ts_distribution(),
        _interfaces_used(),
        _file_access_pattern(),
        _runtime_minutes() {}
  IOPhaseAdvice(const IOPhaseAdvice& other)
      : Advice(other),
        _independent_files(other._independent_files),
        _shared_files(other._shared_files),
        _io_size_mb(other._io_size_mb),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata),
        _ts_distribution(other._ts_distribution),
        _interfaces_used(other._interfaces_used),
        _file_access_pattern(other._file_access_pattern),
        _runtime_minutes(other._runtime_minutes) {}
  IOPhaseAdvice(const IOPhaseAdvice&& other)
      : Advice(other),
        _independent_files(other._independent_files),
        _shared_files(other._shared_files),
        _io_size_mb(other._io_size_mb),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata),
        _ts_distribution(other._ts_distribution),
        _interfaces_used(other._interfaces_used),
        _file_access_pattern(other._file_access_pattern),
        _runtime_minutes(other._runtime_minutes) {}
  IOPhaseAdvice& operator=(const IOPhaseAdvice& other) {
    Advice::operator=(other);
    _independent_files = other._independent_files;
    _shared_files = other._shared_files;
    _io_size_mb = other._io_size_mb;
    _per_io_data = other._per_io_data;
    _per_io_metadata = other._per_io_metadata;
    _ts_distribution = other._ts_distribution;
    _interfaces_used = other._interfaces_used;
    _file_access_pattern = other._file_access_pattern;
    _runtime_minutes = other._runtime_minutes;
    return *this;
  }
  bool operator<(const IOPhaseAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const IOPhaseAdvice& other) const { return !(*this < other); }
};
}  // namespace mimir
#endif  // MIMIR_IO_PHASE_ADVICE_H
