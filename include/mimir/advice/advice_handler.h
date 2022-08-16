//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_ADVICE_HANDLER_H
#define MIMIR_ADVICE_HANDLER_H

#include <mimir/advice/middleware_advice.h>
#include <mimir/common/data_structure.h>
#include <mimir/common/error_code.h>
#include <mimir/constant.h>
#include <mimir/typedef.h>

#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "mimir/common/debug.h"

namespace mimir {
template <typename ADVICE>
class AdviceHandler {
 protected:
  static std::shared_ptr<AdviceHandler<ADVICE>> _instance;
  typedef std::map<size_t, ADVICE, std::greater<size_t>> AdviceMap;
  std::unordered_map<MimirKey, AdviceMap> _advice;

  std::unordered_map<MimirKey, std::unordered_set<ADVICE>> _conflicts;
  std::shared_mutex _advice_mutex;

 public:
  void clear() {
    for (auto item : _conflicts) {
      item.second.clear();
    }
    _conflicts.clear();
    for (auto item : _advice) {
      item.second.clear();
    }
    _advice.clear();
    _instance.reset();
  }
  AdviceHandler() : _conflicts(), _advice() {}

  static std::shared_ptr<AdviceHandler<ADVICE>> Instance(AdviceType type) {
    assert(type._primary == ADVICE()._type._primary);
    if (_instance == nullptr) {
      _instance = std::make_shared<AdviceHandler<ADVICE>>();
    }
    return _instance;
  }

  std::set<ADVICE, std::greater<ADVICE>> resolve_conflicts(MimirKey &key) {
    std::shared_lock guard(_advice_mutex);
    auto trace = mimir::AutoTrace("mimir::resolve_conflicts", key);
    auto added_handlers = std::set<ADVICE, std::greater<ADVICE>>();
    auto iter = _advice.find(key);
    if (iter != _advice.end()) {
      for (const auto &advice : iter->second) {
        auto conflict_iter = _conflicts.find(MimirKey(advice.second._index));
        bool found_conflict = false;
        if (conflict_iter != _conflicts.end()) {
          for (const auto &added_handler : added_handlers) {
            auto conflict_val_iter = conflict_iter->second.find(added_handler);
            if (conflict_val_iter != conflict_iter->second.end()) {
              found_conflict = true;
              break;
            }
          }
        }
        if (!found_conflict) {
          added_handlers.emplace(advice.second);
        }
      }
    }
    return added_handlers;
  }

  MimirStatus save_advice(MimirKey &key, ADVICE advice) {
    auto trace = mimir::AutoTrace("mimir::save_advice", key, advice);
    std::unique_lock guard(_advice_mutex);
    auto iter = _advice.find(key);
    AdviceMap val;
    if (iter == _advice.end()) {
      val = AdviceMap();
    } else {
      val = iter->second;
      iter->second.erase(advice._index);
      _advice.erase(iter);
    }
    val.emplace(advice._index, advice);
    _advice.emplace(key, val);
    return MIMIR_SUCCESS;
  }

  ADVICE remove_advice(MimirKey &key, size_t index) {
    auto trace = mimir::AutoTrace("mimir::remove_advice", key);
    std::unique_lock guard(_advice_mutex);
    auto iter = _advice.find(key);
    if (iter != _advice.end()) {
      auto specific_advice_iter = iter->second.find(index);
      if (specific_advice_iter != iter->second.end()) {
        iter->second.erase(index);
        if (iter->second.empty()) {
          _advice.erase(key);
        }
        return specific_advice_iter->second;
      }
    }
    return ADVICE();
  }

  MimirStatus is_advice_present(MimirKey &key) {
    auto trace = mimir::AutoTrace("mimir::is_advice_present", key);
    std::shared_lock guard(_advice_mutex);
    return _advice.find(key) != _advice.end();
  }

  std::pair<bool, AdviceMap> find_advice(MimirKey &key) {
    auto trace = mimir::AutoTrace("mimir::find_advice", key);
    std::shared_lock guard(_advice_mutex);
    auto iter = _advice.find(key);
    if (iter == _advice.end()) {
      return std::make_pair(false, AdviceMap());
    }
    return std::pair<bool, AdviceMap>(true, iter->second);
  }

  MimirStatus add_conflicts(MimirKey &key, ADVICE &advice) {
    auto trace = mimir::AutoTrace("mimir::add_conflicts", key, advice);
    auto conflict_iter = _conflicts.find(key);
    std::unordered_set<ADVICE> val;
    if (conflict_iter == _conflicts.end()) {
      val = std::unordered_set<ADVICE>();
    } else {
      val = conflict_iter->second;
      _conflicts.erase(key);
    }
    val.emplace(advice);
    _conflicts.emplace(key, val);
    return MIMIR_SUCCESS;
  }
};
template <typename ADVICE>
std::shared_ptr<AdviceHandler<ADVICE>> mimir::AdviceHandler<ADVICE>::_instance =
    nullptr;

}  // namespace mimir

#endif  // MIMIR_ADVICE_HANDLER_H
