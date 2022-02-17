//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_ADVICE_HANDLER_H
#define MIMIR_ADVICE_HANDLER_H

#include <memory>
#include <unordered_map>
#include <mimir/common/data_structure.h>
#include <mimir/constant.h>
#include <mimir/typedef.h>
#include <unordered_set>
#include <set>
#include <mimir/common/error_code.h>
#include <mimir/advice/middleware_advice.h>

namespace mimir {
    template <typename ADVICE>
    class AdviceHandler {
    protected:
        static std::unordered_map<PrimaryAdviceType, std::shared_ptr<AdviceHandler<ADVICE>>> instance_map;

        std::unordered_map<MimirKey, std::set<ADVICE, std::greater<ADVICE>>> _advice;

        std::unordered_map<ADVICE, std::unordered_set<ADVICE>> _conflicts;
    public:

        AdviceHandler(): _conflicts(), _advice() {}

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
            auto added_handlers = std::set<ADVICE, std::greater<ADVICE>>();
            auto iter = _advice.find(key);
            if (iter != _advice.end()) {
                for (const auto & advice:iter->second) {
                    auto conflict_iter = _conflicts.find(advice);
                    bool found_conflict = false;
                    if (conflict_iter != _conflicts.end()) {
                        for (const auto & added_handler: added_handlers) {
                            auto conflict_val_iter = conflict_iter->second.find(added_handler);
                            if (conflict_val_iter != conflict_iter->second.end()) {
                                found_conflict = true;
                                break;
                            }
                        }
                    }
                    if (!found_conflict) {
                        added_handlers.emplace(advice);
                    }
                }
            }
            return added_handlers;
        }

        MimirStatus save_advice(MimirKey &key, ADVICE advice) {
            auto iter = _advice.find(key);
            std::set<ADVICE, std::greater<ADVICE>> val;
            if (iter == _advice.end()) {
                val = std::set<ADVICE, std::greater<ADVICE>>();
            } else {
                val = iter->second;
                iter->second.erase(advice);
                _advice.erase(iter);
            }
            val.emplace(advice);
            _advice.emplace(key, val);
            return MIMIR_SUCCESS;
        }

        MimirStatus remove_advice(MimirKey &key) {
            auto iter = _advice.erase(key);
            return MIMIR_SUCCESS;
        }

        MimirStatus is_advice_present(MimirKey &key) {
            return _advice.find(key) != _advice.end();
        }

        std::pair<bool, std::vector<ADVICE>> find_advice(MimirKey &key) {
            auto iter = _advice.find(key);
            if (iter == _advice.end()) {
                return std::make_pair(false, std::vector<ADVICE>());
            }
            auto advices =  std::vector<ADVICE>();
            for (auto advice:iter->second) {
                advices.push_back(advice);
            }
            return std::pair<bool,std::vector<ADVICE>>(true, advices);
        }

        MimirStatus add_conflicts(ADVICE &advice) {
            _conflicts.emplace(advice);
            return MIMIR_SUCCESS;
        }

    };
    template <typename ADVICE>
    std::unordered_map<mimir::PrimaryAdviceType, std::shared_ptr<mimir::AdviceHandler<ADVICE>>> mimir::AdviceHandler<ADVICE>::instance_map =
            std::unordered_map<PrimaryAdviceType, std::shared_ptr<AdviceHandler<ADVICE>>>();
}


#endif //MIMIR_ADVICE_HANDLER_H
