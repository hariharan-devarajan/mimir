//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_DATASET_ADVICE_H
#define MIMIR_DATASET_ADVICE_H

#include <mimir/advice/advice.h>
namespace mimir {
class DatasetAdvice : public Advice {
 public:
  Format _format;
  uint32_t _size_mb;
  uint16_t _num_files;
  uint32_t _io_amount_mb;
  float _runtime_minutes;
  TransferSizeDistribution _read_distribution;
  TransferSizeDistribution _write_distribution;
  std::unordered_map<File, uint32_t> _file_size_distribution;
  float _per_io_data, _per_io_metadata;

  DatasetAdvice()
      : Advice(AdviceType(PrimaryAdviceType::DATA_DATASET,
                          OperationAdviceType::NO_OP)),
        _format(),
        _size_mb(),
        _num_files(),
        _io_amount_mb(),
        _runtime_minutes(),
        _read_distribution(),
        _write_distribution(),
        _file_size_distribution(),
        _per_io_data(),
        _per_io_metadata() {}

  DatasetAdvice(const DatasetAdvice& other)
      : Advice(other),
        _format(other._format),
        _size_mb(other._size_mb),
        _num_files(other._num_files),
        _io_amount_mb(other._io_amount_mb),
        _runtime_minutes(other._runtime_minutes),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _file_size_distribution(other._file_size_distribution),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata) {}
  DatasetAdvice(const DatasetAdvice&& other)
      : Advice(other),
        _format(other._format),
        _size_mb(other._size_mb),
        _num_files(other._num_files),
        _io_amount_mb(other._io_amount_mb),
        _runtime_minutes(other._runtime_minutes),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _file_size_distribution(other._file_size_distribution),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata) {}
  DatasetAdvice& operator=(const DatasetAdvice& other) {
    Advice::operator=(other);
    _format = other._format;
    _size_mb = other._size_mb;
    _num_files = other._num_files;
    _io_amount_mb = other._io_amount_mb;
    _runtime_minutes = other._runtime_minutes;
    _read_distribution = other._read_distribution;
    _write_distribution = other._write_distribution;
    _file_size_distribution = other._file_size_distribution;
    _per_io_data = other._per_io_data;
    _per_io_metadata = other._per_io_metadata;
    return *this;
  }
  bool operator<(const DatasetAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const DatasetAdvice& other) const { return !(*this < other); }
};
}  // namespace mimir
#endif  // MIMIR_DATASET_ADVICE_H
