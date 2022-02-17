//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_HLIO_ADVICE_H
#define MIMIR_HLIO_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
class HLIOAdvice : public Advice {
 public:
  DataRepresentation _logical_representation;
  TransferSizeDistribution _read_distribution;
  TransferSizeDistribution _write_distribution;
  AccessPattern _access_pattern;
  std::unordered_map<File, AdviceType> _file_advice_map;
  std::string _name;

  HLIOAdvice()
      : Advice(AdviceType(PrimaryAdviceType::SOFTWARE_HLIO_LIB,
                          OperationAdviceType::NO_OP)),
        _logical_representation(),
        _name(),
        _read_distribution(),
        _write_distribution(),
        _access_pattern(),
        _file_advice_map() {}
  HLIOAdvice(const HLIOAdvice& other)
      : Advice(other),
        _logical_representation(other._logical_representation),
        _name(other._name),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _access_pattern(other._access_pattern),
        _file_advice_map(other._file_advice_map) {}
  HLIOAdvice(const HLIOAdvice&& other)
      : Advice(other),
        _logical_representation(other._logical_representation),
        _name(other._name),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _access_pattern(other._access_pattern),
        _file_advice_map(other._file_advice_map) {}
  HLIOAdvice& operator=(const HLIOAdvice& other) {
    Advice::operator=(other);
    _logical_representation = other._logical_representation;
    _name = other._name;
    _access_pattern = other._access_pattern;
    _read_distribution = other._read_distribution;
    _write_distribution = other._write_distribution;
    _file_advice_map = other._file_advice_map;
    return *this;
  }

  bool operator<(const HLIOAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const HLIOAdvice& other) const { return !(*this < other); }
};
}  // namespace mimir
#endif  // MIMIR_HLIO_ADVICE_H
