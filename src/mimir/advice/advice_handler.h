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
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "mimir/common/debug.h"

namespace mimir {
template <typename ADVICE>
class AdviceHandler {
 protected:
  static std::unordered_map<PrimaryAdviceType,
                            std::shared_ptr<AdviceHandler<ADVICE>>>
      instance_map;
  typedef std::map<size_t, ADVICE, std::greater<size_t>> AdviceMap;
  std::unordered_map<MimirKey, AdviceMap> _advice;

  std::unordered_map<MimirKey, std::unordered_set<ADVICE>> _conflicts;

 public:
  AdviceHandler() : _conflicts(), _advice() {}

  static std::shared_ptr<AdviceHandler<ADVICE>> Instance(AdviceType type) {
    auto iter = instance_map.find(type._primary);
    if (iter != instance_map.end()) {
      return iter->second;
    } else {
      auto instance = std::make_shared<AdviceHandler<ADVICE>>();
      instance_map.emplace(type._primary, instance);
      return instance;
    }
  }

  std::set<ADVICE, std::greater<ADVICE>> resolve_conflicts(MimirKey &key) {
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

  MimirStatus remove_advice(MimirKey &key, size_t index) {
    auto trace = mimir::AutoTrace("mimir::remove_advice", key);
    auto iter = _advice.find(key);
    if (iter != _advice.end()) {
      iter->second.erase(index);
      if (iter->second.empty()) {
        _advice.erase(key);
      }
    }
    return MIMIR_SUCCESS;
  }

  MimirStatus is_advice_present(MimirKey &key) {
    auto trace = mimir::AutoTrace("mimir::is_advice_present", key);
    return _advice.find(key) != _advice.end();
  }

  std::pair<bool, AdviceMap> find_advice(MimirKey &key) {
    auto trace = mimir::AutoTrace("mimir::find_advice", key);
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
std::unordered_map<mimir::PrimaryAdviceType,
                   std::shared_ptr<mimir::AdviceHandler<ADVICE>>>
    mimir::AdviceHandler<ADVICE>::instance_map = std::unordered_map<
        PrimaryAdviceType, std::shared_ptr<AdviceHandler<ADVICE>>>();
}  // namespace mimir

#endif  // MIMIR_ADVICE_HANDLER_H
