//
// Created by haridev on 2/15/22.
//

#ifndef MIMIR_MIDDLEWARE_ADVICE_H
#define MIMIR_MIDDLEWARE_ADVICE_H

#include <mimir/common/enumeration.h>
#include <mimir/advice/advice.h>
#include <mimir/common/data_structure.h>
#include <mimir/advice/advice_type.h>

namespace mimir {
class MiddlewareAdvice : public Advice {
 public:
  using Advice::_type;
  uint16_t _num_cpu_cores_used;
  uint8_t _num_gpus_used;
  TransferSizeDistribution _ts_distribution;
  std::unordered_map<File, AccessPattern> _file_access_pattern;
  std::vector<InterfaceType> _interfaces_used;

  MiddlewareAdvice(AdviceType type)
      : Advice(type),
        _num_cpu_cores_used(1),
        _num_gpus_used(0),
        _ts_distribution(),
        _interfaces_used(),
        _file_access_pattern() {}

  MiddlewareAdvice()
      : Advice(AdviceType(PrimaryAdviceType::SOFTWARE_MIDDLEWARE_LIBRARY,
                          OperationAdviceType::NO_OP)),
        _num_cpu_cores_used(1),
        _num_gpus_used(0),
        _ts_distribution(),
        _interfaces_used(),
        _file_access_pattern() {}
  MiddlewareAdvice(const MiddlewareAdvice& other)
      : Advice(other),
        _num_cpu_cores_used(other._num_cpu_cores_used),
        _num_gpus_used(other._num_gpus_used),
        _ts_distribution(other._ts_distribution),
        _interfaces_used(other._interfaces_used),
        _file_access_pattern(other._file_access_pattern) {}
  MiddlewareAdvice(const MiddlewareAdvice&& other)
      : Advice(other),
        _num_cpu_cores_used(other._num_cpu_cores_used),
        _num_gpus_used(other._num_gpus_used),
        _ts_distribution(other._ts_distribution),
        _interfaces_used(other._interfaces_used),
        _file_access_pattern(other._file_access_pattern) {}
  MiddlewareAdvice& operator=(const MiddlewareAdvice& other) {
    Advice::operator=(other);
    _num_cpu_cores_used = other._num_cpu_cores_used;
    _num_gpus_used = other._num_gpus_used;
    _ts_distribution = other._ts_distribution;
    _interfaces_used = other._interfaces_used;
    _file_access_pattern = other._file_access_pattern;
    return *this;
  }

  bool operator<(const MiddlewareAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const MiddlewareAdvice& other) const {
    return !(*this < other);
  }
};
}  // namespace mimir
#endif  // MIMIR_MIDDLEWARE_ADVICE_H
