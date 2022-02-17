//
// Created by haridev on 2/15/22.
//

#ifndef MIMIR_FILE_OPERATION_ADVICE_H
#define MIMIR_FILE_OPERATION_ADVICE_H

#include "advice_type.h"
#include "mimir/common/enumeration.h"

namespace mimir {
class FileOperationAdvice : public Advice {
 public:
  using Advice::_type;
  uint32_t _offset;
  uint32_t _size;
  InterfaceType _interface_used;
  uint32_t _runtime_minutes;

  FileOperationAdvice()
      : Advice(AdviceType(PrimaryAdviceType::JOB_IO_OPERATION,
                          OperationAdviceType::NO_OP)),
        _offset(),
        _size(),
        _interface_used(),
        _runtime_minutes() {}
  FileOperationAdvice(const FileOperationAdvice& other)
      : Advice(other),
        _offset(other._offset),
        _size(other._size),
        _interface_used(other._interface_used),
        _runtime_minutes(other._runtime_minutes) {}
  FileOperationAdvice(const FileOperationAdvice&& other)
      : Advice(other),
        _offset(other._offset),
        _size(other._size),
        _interface_used(other._interface_used),
        _runtime_minutes(other._runtime_minutes) {}
  FileOperationAdvice& operator=(const FileOperationAdvice& other) {
    Advice::operator=(other);
    _offset = other._offset;
    _size = other._size;
    _interface_used = other._interface_used;
    _runtime_minutes = other._runtime_minutes;
    return *this;
  }
  bool operator<(const FileOperationAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const FileOperationAdvice& other) const {
    return !(*this < other);
  }
};
}  // namespace mimir

namespace std {
template <>
struct hash<mimir::FileOperationAdvice> {
  size_t operator()(const mimir::FileOperationAdvice& k) const {
    return k._index;
  }
};
}  // namespace std
#endif  // MIMIR_FILE_OPERATION_ADVICE_H
