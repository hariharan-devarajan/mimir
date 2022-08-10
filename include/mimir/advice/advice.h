//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ADVICE_H
#define MIMIR_ADVICE_H

#include <mimir/advice/advice_type.h>
#include <mimir/common/data_structure.h>
#include <mimir/common/typedef.h>

#include <memory>

namespace mimir {
class Advice {
 private:
  static uint32_t __index;

 public:
  uint32_t _index;
  uint32_t _priority;
  AdviceType _type;
  Advice()
      : _type(AdviceType(PrimaryAdviceType::ADVICE_NONE,
                         OperationAdviceType::NO_OP)),
        _index(__index),
        _priority(0) {
    __index++;
  }
  Advice(AdviceType type) : _type(type), _index(__index), _priority(0) {
    __index++;
  }
  Advice(const Advice& other)
      : _type(other._type), _index(other._index), _priority(other._priority) {}
  Advice(const Advice&& other)
      : _type(other._type), _priority(other._priority) {}
  Advice& operator=(const Advice& other) {
    _type = other._type;
    _index = other._index;
    _priority = other._priority;
    return *this;
  }

  bool operator==(const Advice& other) const {
    return _index == other._index == other._index && this->is_same(other);
  }
  bool is_same(const Advice& other) const {
    return _type == other._type && _priority == other._priority;
  }

  bool operator<(const Advice& other) const {
    if (_priority < other._priority) return true;
    if (_priority == other._priority) return _index < other._index;
    return false;
  }

  bool operator>(const Advice& other) const { return !(*this < other); }
};

typedef std::shared_ptr<Advice> MimirPayload;
}  // namespace mimir

namespace std {
template <>
struct hash<mimir::Advice> {
  size_t operator()(const mimir::Advice& k) const { return k._index; }
};
}  // namespace std

inline std::ostream& operator<<(std::ostream& os, mimir::Advice const& m) {
  return os << "{TYPE:Advice,"
            << "_index:" << m._index << ","
            << "_priority:" << m._priority << ","
            << "_type:" << m._type << "}";
}
using json = nlohmann::json;
namespace mimir {
inline void to_json(json& j, const Advice& p) {
  j["index"] = p._index;
  j["priority"] = p._priority;
  j["type"] = p._type;
}

inline void from_json(const json& j, Advice& p) {
  j.at("index").get_to(p._index);
  j.at("priority").get_to(p._priority);
  j.at("type").get_to(p._type);
}
}  // namespace mimir
#endif  // MIMIR_ADVICE_H
