//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_EXECUTION_ENGINE_ADVICE_H
#define MIMIR_EXECUTION_ENGINE_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
class ExecutionEngineAdvice : public Advice {
 public:
  uint16_t _cpu_cores_available;
  uint8_t _gpus_available;
  uint32_t _memory_available;
  TransferSizeDistribution _read_distribution;
  TransferSizeDistribution _write_distribution;
  AccessPattern _access_pattern;
  std::unordered_map<File, AdviceType> _file_advice_map;

  ExecutionEngineAdvice()
      : Advice(AdviceType(PrimaryAdviceType::SOFTWARE_EXECUTION_ENGINE,
                          OperationAdviceType::NO_OP)),
        _cpu_cores_available(),
        _gpus_available(),
        _memory_available(),
        _read_distribution(),
        _write_distribution(),
        _access_pattern(),
        _file_advice_map() {}
  ExecutionEngineAdvice(const ExecutionEngineAdvice& other)
      : Advice(other),
        _cpu_cores_available(other._cpu_cores_available),
        _gpus_available(other._gpus_available),
        _memory_available(other._memory_available),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _access_pattern(other._access_pattern),
        _file_advice_map(other._file_advice_map) {}
  ExecutionEngineAdvice(const ExecutionEngineAdvice&& other)
      : Advice(other),
        _cpu_cores_available(other._cpu_cores_available),
        _gpus_available(other._gpus_available),
        _memory_available(other._memory_available),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _access_pattern(other._access_pattern),
        _file_advice_map(other._file_advice_map) {}
  ExecutionEngineAdvice& operator=(const ExecutionEngineAdvice& other) {
    Advice::operator=(other);
    _cpu_cores_available = other._cpu_cores_available;
    _gpus_available = other._gpus_available;
    _memory_available = other._memory_available;
    _read_distribution = other._read_distribution;
    _write_distribution = other._write_distribution;
    _access_pattern = other._access_pattern;
    _file_advice_map = other._file_advice_map;
    return *this;
  }
  bool operator<(const ExecutionEngineAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const ExecutionEngineAdvice& other) const {
    return !(*this < other);
  }
};
}  // namespace mimir
#endif  // MIMIR_EXECUTION_ENGINE_ADVICE_H
