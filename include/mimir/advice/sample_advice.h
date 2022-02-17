//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_SAMPLE_ADVICE_H
#define MIMIR_SAMPLE_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
class SampleAdvice : public Advice {
 public:
  uint32_t _index;
  uint32_t _size_mb;
  DataRepresentation _representation;

  SampleAdvice()
      : Advice(AdviceType(PrimaryAdviceType::DATA_SAMPLE,
                          OperationAdviceType::NO_OP)),
        _size_mb(),
        _representation() {}
  SampleAdvice(const SampleAdvice& other)
      : Advice(other),
        _size_mb(other._size_mb),
        _representation(other._representation) {}
  SampleAdvice(const SampleAdvice&& other)
      : Advice(other),
        _size_mb(other._size_mb),
        _representation(other._representation) {}
  SampleAdvice& operator=(const SampleAdvice& other) {
    Advice::operator=(other);
    _size_mb = other._size_mb;
    _representation = other._representation;
    return *this;
  }
  bool operator<(const SampleAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const SampleAdvice& other) const { return !(*this < other); }
};
}  // namespace mimir
#endif  // MIMIR_SAMPLE_ADVICE_H
