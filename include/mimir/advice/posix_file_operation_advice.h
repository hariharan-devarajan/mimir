//
// Created by haridev on 2/15/22.
//

#ifndef MIMIR_POSIX_FILE_OPERATION_ADVICE_H
#define MIMIR_POSIX_FILE_OPERATION_ADVICE_H

#include "advice.h"
#include "file_operation_advice.h"

namespace mimir {
class POSIXFileOperationAdvice : public FileOperationAdvice {
 public:
  using FileOperationAdvice::_interface_used;
  using FileOperationAdvice::_offset;
  using FileOperationAdvice::_runtime_minutes;
  using FileOperationAdvice::_size;
  using FileOperationAdvice::_type;
  int _fd;
  POSIXFileOperationAdvice() : FileOperationAdvice(), _fd() {}
  POSIXFileOperationAdvice(const POSIXFileOperationAdvice& other)
      : FileOperationAdvice(other), _fd(other._fd) {}
  POSIXFileOperationAdvice(const POSIXFileOperationAdvice&& other)
      : FileOperationAdvice(other), _fd(other._fd) {}
  POSIXFileOperationAdvice& operator=(const POSIXFileOperationAdvice& other) {
    FileOperationAdvice::operator=(other);
    _fd = other._fd;
    return *this;
  }
  bool operator<(const POSIXFileOperationAdvice& other) const {
    return FileOperationAdvice::operator<(other);
  }
  bool operator>(const POSIXFileOperationAdvice& other) const {
    return !(*this < other);
  }
};
}  // namespace mimir

namespace std {
template <>
struct hash<mimir::POSIXFileOperationAdvice> {
  size_t operator()(const mimir::POSIXFileOperationAdvice& k) const {
    return k._index;
  }
};
}  // namespace std
#endif  // MIMIR_POSIX_FILE_OPERATION_ADVICE_H
