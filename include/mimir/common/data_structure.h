//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_DATA_STRUCTURE_H
#define MIMIR_DATA_STRUCTURE_H

#include <assert.h>
#include <mimir/advice/advice.h>
#include <mimir/advice/advice_type.h>
#include <stdint-gcc.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "typedef.h"

namespace mimir {
struct Storage {
  std::string _mount_point;
  uint32_t _capacity_mb;
  uint32_t _used_capacity_mb;
  bool _is_shared;
  Storage() : _mount_point(), _capacity_mb(0), _used_capacity_mb(0), _is_shared() {}

  Storage(std::string mount_point, uint32_t capacity_mb, bool is_shared)
      : _mount_point(std::move(mount_point)),
        _capacity_mb(capacity_mb),
        _used_capacity_mb(0),
        _is_shared(is_shared){}
  Storage(const Storage& other)
      : _mount_point(other._mount_point),
        _capacity_mb(other._capacity_mb),
        _used_capacity_mb(other._used_capacity_mb),
        _is_shared(other._is_shared){}
  Storage(const Storage&& other)
      : _mount_point(other._mount_point),
        _capacity_mb(other._capacity_mb),
        _used_capacity_mb(other._used_capacity_mb),
        _is_shared(other._is_shared) {}
  Storage& operator=(const Storage& other) {
    _mount_point = other._mount_point;
    _capacity_mb = other._capacity_mb;
    _used_capacity_mb = other._used_capacity_mb;
    _is_shared = other._is_shared;
    return *this;
  }
  bool operator==(const Storage& other) const {
    return _mount_point == other._mount_point &&
           _capacity_mb == other._capacity_mb;
  }
};

struct Node {
  uint32_t _unique_hash;
  Node() : _unique_hash() {}
  Node(uint32_t unique_hash) : _unique_hash(unique_hash) {}
  Node(const Node& other) : _unique_hash(other._unique_hash) {}
  Node(const Node&& other) : _unique_hash(other._unique_hash) {}
  Node& operator=(const Node& other) {
    _unique_hash = other._unique_hash;
    return *this;
  }
  bool operator==(const Node& other) const {
    return _unique_hash == other._unique_hash;
  }
};

struct Application : public Node {
  std::string _name;
  uint32_t _argument_hash;
  Application() : Node(), _name(), _argument_hash() {}
  Application(uint32_t unique_hash, std::string name, uint32_t argument_hash)
      : Node(unique_hash), _name(name), _argument_hash(argument_hash) {}
  Application(const Application& other)
      : Node(other), _name(other._name), _argument_hash(other._argument_hash) {}
  Application(const Application&& other)
      : Node(other), _name(other._name), _argument_hash(other._argument_hash) {}
  Application& operator=(const Application& other) {
    Node::operator=(other);
    _name = other._name;
    _argument_hash = other._argument_hash;
    return *this;
  }
  bool operator==(const Application& other) const {
    return Node::operator==(other) && _name == other._name &&
           _argument_hash == other._argument_hash;
  }
};

struct File : public Node {
  std::string _name;
  uint32_t _full_path_hash;
  File() : Node(), _name(), _full_path_hash() {}
  File(uint32_t unique_hash, std::string name, uint32_t full_path_hash)
      : Node(unique_hash), _name(name), _full_path_hash(full_path_hash) {}
  File(const File& other)
      : Node(other),
        _name(other._name),
        _full_path_hash(other._full_path_hash) {}
  File(const File&& other)
      : Node(other),
        _name(other._name),
        _full_path_hash(other._full_path_hash) {}
  File& operator=(const File& other) {
    Node::operator=(other);
    _name = other._name;
    _full_path_hash = other._full_path_hash;
    return *this;
  }
  bool operator==(const File& other) const {
    return Node::operator==(other) && _name == other._name &&
           _full_path_hash == other._full_path_hash;
  }
};

template <typename SOURCE, typename DESTINATION>
struct Edge {
  SOURCE source;
  DESTINATION destination;
  Edge() : source(), destination() {}
  Edge(SOURCE _source, DESTINATION _destination)
      : source(_source), destination(_destination) {}
  Edge(const Edge<SOURCE, DESTINATION>& other)
      : source(other.source), destination(other.destination) {}
  Edge(const Edge<SOURCE, DESTINATION>&& other)
      : source(other.source), destination(other.destination) {}
  Edge& operator=(const Edge<SOURCE, DESTINATION>& other) {
    source = other.source;
    destination = other.destination;
    return *this;
  }
  bool operator==(const Edge<SOURCE, DESTINATION>& other) const {
    return source == other.source && destination == other.destination;
  }
  bool operator!=(const Edge<SOURCE, DESTINATION>& other) const {
    return !(other == *this);
  }
};

struct ApplicationFileDAG {
  std::unordered_set<ApplicationIndex> applications;
  std::unordered_set<FileIndex> files;
  std::vector<Edge<ApplicationIndex, FileIndex>> edges;
  bool operator==(const ApplicationFileDAG& other) const {
    if (applications.size() != other.applications.size()) return false;
    for (auto item : applications) {
      auto iter = other.applications.find(item);
      if (iter == other.applications.end()) return false;
    }
    if (files.size() != other.files.size()) return false;
    for (auto item : files) {
      auto iter = other.files.find(item);
      if (iter == other.files.end()) return false;
    }
    if (edges.size() != other.edges.size()) return false;
    for (int i = 0; i < edges.size(); ++i)
      if (edges[i] != other.edges[i]) return false;
    return true;
  }
  bool operator!=(const ApplicationFileDAG& other) const {
    return !(other == *this);
  }
};

struct RankFileDAG {
  std::unordered_set<RankIndex> ranks;
  std::unordered_set<FileIndex> files;
  std::vector<Edge<RankIndex, FileIndex>> edges;
  bool operator==(const RankFileDAG& other) const {
    if (ranks.size() != other.ranks.size()) return false;
    for (auto item : ranks) {
      auto iter = other.ranks.find(item);
      if (iter == other.ranks.end()) return false;
    }
    if (files.size() != other.files.size()) return false;
    for (auto item : files) {
      auto iter = other.files.find(item);
      if (iter == other.files.end()) return false;
    }

    if (edges.size() != other.edges.size()) return false;
    for (int i = 0; i < edges.size(); ++i)
      if (edges[i] != other.edges[i]) return false;
    return true;
  }
  bool operator!=(const RankFileDAG& other) const { return !(other == *this); }
};

struct TransferSizeDistribution {
  float _0_4kb;
  float _4_64kb;
  float _64kb_1mb;
  float _1mb_16mb;
  float _16mb;
  TransferSizeDistribution(float ts_0_4kb, float ts_4_64kb, float ts_64kb_1mb,
                           float ts_1mb_16mb, float ts_16mb)
      : _0_4kb(ts_0_4kb),
        _4_64kb(ts_4_64kb),
        _64kb_1mb(ts_64kb_1mb),
        _1mb_16mb(ts_1mb_16mb),
        _16mb(ts_16mb) {
    assert(ts_0_4kb + ts_4_64kb + ts_64kb_1mb + ts_1mb_16mb + ts_16mb <= 1.0 &&
           ts_0_4kb + ts_4_64kb + ts_64kb_1mb + ts_1mb_16mb + ts_16mb >= 0);
  }
  TransferSizeDistribution()
      : TransferSizeDistribution(0.0, 0.0, 0.0, 0.0, 0.0) {}
  bool operator==(const TransferSizeDistribution& other) const {
    return _0_4kb == other._0_4kb && _4_64kb == other._4_64kb &&
           _64kb_1mb == other._64kb_1mb && _1mb_16mb == other._1mb_16mb &&
           _16mb == other._16mb;
  }
  TransferSizeDistribution(const TransferSizeDistribution& other)
      : _0_4kb(other._0_4kb),
        _4_64kb(other._4_64kb),
        _64kb_1mb(other._64kb_1mb),
        _1mb_16mb(other._1mb_16mb),
        _16mb(other._16mb) {}
  TransferSizeDistribution(const TransferSizeDistribution&& other)
      : _0_4kb(other._0_4kb),
        _4_64kb(other._4_64kb),
        _64kb_1mb(other._64kb_1mb),
        _1mb_16mb(other._1mb_16mb),
        _16mb(other._16mb) {}
  TransferSizeDistribution& operator=(const TransferSizeDistribution& other) {
    _0_4kb = other._0_4kb;
    _4_64kb = other._4_64kb;
    _64kb_1mb = other._64kb_1mb;
    _1mb_16mb = other._1mb_16mb;
    _16mb = other._16mb;
    return *this;
  }
};
struct MimirKey {
  size_t _id;
  MimirKey() : _id() {}
  MimirKey(size_t id) : _id(id) {}
  MimirKey(const MimirKey& key) : _id(key._id) {}
  MimirKey(const MimirKey&& key) : _id(key._id) {}

  bool operator==(const MimirKey& other) const { return _id == other._id; }
};

struct MimirHandler {
 public:
  size_t _key_id;
  size_t _advice_index;
  AdviceType _type;
  MimirHandler()
      : _key_id(),
        _advice_index(),
        _type(PrimaryAdviceType::ADVICE_NONE, OperationAdviceType::NO_OP) {}
  MimirHandler(const MimirHandler& other)
      : _type(other._type),
        _key_id(other._key_id),
        _advice_index(other._advice_index) {}
  MimirHandler(const MimirHandler&& other)
      : _type(other._type),
        _key_id(other._key_id),
        _advice_index(other._advice_index) {}
  bool operator==(const MimirHandler& other) const {
    return _type == other._type && _key_id == other._key_id &&
           _advice_index == other._advice_index;
  }
  MimirHandler& operator=(const MimirHandler& other) {
    _type = other._type;
    _key_id = other._key_id;
    _advice_index = other._advice_index;
    return *this;
  }
};

}  // namespace mimir
namespace std {
template <>
struct hash<mimir::File> {
  size_t operator()(const mimir::File& k) const {
    size_t hash_val = hash<uint32_t>()(k._full_path_hash);
    hash_val ^= hash<uint32_t>()(k._unique_hash);
    hash_val ^= hash<std::string>()(k._name);
    return hash_val;
  }
};

template <>
struct hash<mimir::MimirKey> {
  size_t operator()(const mimir::MimirKey& k) const { return k._id; }
};

template <>
struct hash<mimir::Storage> {
  size_t operator()(const mimir::Storage& k) const {
    size_t hash_val = hash<std::string>()(k._mount_point);
    return hash_val;
  }
};

}  // namespace std

inline std::ostream& operator<<(std::ostream& os, mimir::MimirKey const& m) {
  return os << "{TYPE:MimirKey,"
            << "id:" << m._id << "}";
}
using json = nlohmann::json;

namespace mimir {
inline void to_json(json& j, const mimir::Storage& p) {
  j["mount_point"] = p._mount_point;
  j["capacity_mb"] = p._capacity_mb;
  j["used_capacity_mb"] = p._used_capacity_mb;
  j["is_shared"] = p._is_shared;
}

inline void from_json(const json& j, mimir::Storage& p) {
  j.at("mount_point").get_to(p._mount_point);
  j.at("capacity_mb").get_to(p._capacity_mb);
  j.at("used_capacity_mb").get_to(p._used_capacity_mb);
  j.at("is_shared").get_to(p._is_shared);
}
inline void to_json(json& j, const TransferSizeDistribution& p) {
  float a[5] = {p._0_4kb, p._4_64kb, p._64kb_1mb, p._1mb_16mb, p._16mb};
  j = a;
}

inline void from_json(const json& j, TransferSizeDistribution& p) {
  float a[5];
  j.get_to(a);
  p._0_4kb = a[0];
  p._4_64kb = a[1];
  p._64kb_1mb = a[2];
  p._1mb_16mb = a[3];
  p._16mb = a[4];
}
inline void to_json(json& j, const mimir::Edge<Index, Index>& p) {
  uint32_t a[2] = {p.source, p.destination};
  j = a;
}

inline void from_json(const json& j, mimir::Edge<Index, Index>& p) {
  uint32_t a[2];
  j.get_to(a);
  p.source = a[0];
  p.destination = a[1];
}

inline void to_json(json& j, const mimir::ApplicationFileDAG& p) {
  j["applications"] = p.applications;
  j["files"] = p.files;
  j["edges"] = p.edges;
}

inline void from_json(const json& j, mimir::ApplicationFileDAG& p) {
  j.at("applications").get_to(p.applications);
  j.at("files").get_to(p.files);
  j.at("edges").get_to(p.edges);
}

inline void to_json(json& j, const mimir::RankFileDAG& p) {
  j["ranks"] = p.ranks;
  j["files"] = p.files;
  j["edges"] = p.edges;
}

inline void from_json(const json& j, mimir::RankFileDAG& p) {
  j.at("ranks").get_to(p.ranks);
  j.at("files").get_to(p.files);
  j.at("edges").get_to(p.edges);
}

}  // namespace mimir

#endif  // MIMIR_DATA_STRUCTURE_H
