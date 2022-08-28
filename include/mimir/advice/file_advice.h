//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_FILE_ADVICE_H
#define MIMIR_FILE_ADVICE_H
#include <mimir/advice/advice.h>
#include <mimir/common/data_structure.h>
#include <mimir/common/enumeration.h>
namespace mimir {
class FileAdvice : public Advice {
 public:
  Format _format;
  uint32_t _size_mb;
  std::string _name;
  uint32_t _io_amount_mb;
  TransferSizeDistribution _read_distribution;
  TransferSizeDistribution _write_distribution;
  int _current_device;
  int _placement_device;
  float _per_io_data, _per_io_metadata;
  bool _prefetch;
  FileSharing _file_sharing;

  FileAdvice()
      : Advice(AdviceType(PrimaryAdviceType::DATA_FILE,
                          OperationAdviceType::NO_OP)),
        _format(),
        _size_mb(),
        _name(),
        _io_amount_mb(),
        _read_distribution(),
        _write_distribution(),
        _per_io_data(),
        _per_io_metadata(),
        _current_device(0),
        _placement_device(0),
        _prefetch(false),
        _file_sharing(FileSharing::FILE_SHARING_NONE){}
  FileAdvice(const FileAdvice& other)
      : Advice(other),
        _format(other._format),
        _size_mb(other._size_mb),
        _name(other._name),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata),
        _current_device(other._current_device),
        _placement_device(other._placement_device),
        _prefetch(other._prefetch),
        _file_sharing(other._file_sharing) {}
  FileAdvice(const FileAdvice&& other)
      : Advice(other),
        _format(other._format),
        _size_mb(other._size_mb),
        _name(other._name),
        _read_distribution(other._read_distribution),
        _write_distribution(other._write_distribution),
        _per_io_data(other._per_io_data),
        _per_io_metadata(other._per_io_metadata),
        _current_device(other._current_device),
        _placement_device(other._placement_device),
        _prefetch(other._prefetch) ,
        _file_sharing(other._file_sharing) {}
  FileAdvice& operator=(const FileAdvice& other) {
    Advice::operator=(other);
    _format = other._format;
    _size_mb = other._size_mb;
    _name = other._name;
    _read_distribution = other._read_distribution;
    _write_distribution = other._write_distribution;
    _per_io_data = other._per_io_data;
    _per_io_metadata = other._per_io_metadata;
    _current_device = other._current_device;
    _placement_device = other._placement_device;
    _prefetch = other._prefetch;
    _file_sharing = other._file_sharing;
    return *this;
  }
  bool operator<(const FileAdvice& other) const {
    return Advice::operator<(other);
  }
  bool operator>(const FileAdvice& other) const { return !(*this < other); }
  bool operator==(const FileAdvice& other) const {
    return Advice::operator==(other) && this->is_same(other);
  }
  bool is_same(const FileAdvice& other) const {
    return Advice::is_same(other) && _format == other._format &&
           _size_mb == other._size_mb && _name == other._name &&
           _read_distribution == other._read_distribution &&
           _write_distribution == other._write_distribution &&
           _per_io_data == other._per_io_data &&
           _per_io_metadata == other._per_io_metadata &&
           _current_device == other._current_device &&
           _placement_device == other._placement_device &&
           _prefetch == other._prefetch &&
           _file_sharing == other._file_sharing;
  }
  bool operator!=(const FileAdvice& other) const { return !(other == *this); }
};
}  // namespace mimir

namespace std {
template <>
struct hash<mimir::FileAdvice> {
  size_t operator()(const mimir::FileAdvice& k) const { return k._index; }
};
}  // namespace std

using json = nlohmann::json;
namespace mimir {
inline void to_json(json& j, const FileAdvice& p) {
  j = json();
  to_json(j, (mimir::Advice&)p);
  j["format"] = p._format;
  j["size_mb"] = p._size_mb;
  j["name"] = p._name;

  j["read_distribution"] = p._read_distribution;
  j["write_distribution"] = p._write_distribution;

  j["per_io_data"] = p._per_io_data;
  j["per_io_metadata"] = p._per_io_metadata;
  j["current_device"] = p._current_device;
  j["placement_device"] = p._placement_device;
  j["prefetch"] = p._prefetch;
  j["file_sharing"] = p._file_sharing;
}

inline void from_json(const json& j, FileAdvice& p) {
  from_json(j, (mimir::Advice&)p);
  j.at("format").get_to(p._format);
  j.at("size_mb").get_to(p._size_mb);
  j.at("name").get_to(p._name);
  j.at("read_distribution").get_to(p._read_distribution);
  j.at("write_distribution").get_to(p._write_distribution);
  j.at("per_io_data").get_to(p._per_io_data);
  j.at("per_io_metadata").get_to(p._per_io_metadata);
  j.at("current_device").get_to(p._current_device);
  j.at("placement_device").get_to(p._placement_device);
  j.at("prefetch").get_to(p._prefetch);
  j.at("file_sharing").get_to(p._file_sharing);
}
}  // namespace mimir
#endif  // MIMIR_FILE_ADVICE_H
