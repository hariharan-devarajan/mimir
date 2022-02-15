//
// Created by haridev on 2/14/22.
//

#include <mimir/core/advice.h>

namespace mimir {
    MimirStatus advice_begin(MimirKey &key, MimirHandler &handler, AdviceType type, MimirPayload payload) {
        handler._index++;
        handler._type = type;
        handler._payload = payload;
        auto iter = AdviceHandler::Instance()->handler_map.find(key);
        std::vector<MimirHandler> val;
        if (iter == AdviceHandler::Instance()->handler_map.end()) {
            val = std::vector<MimirHandler>();
        } else {
            val = iter->second;
            AdviceHandler::Instance()->handler_map.erase(iter);
        }
        val.push_back(handler);
        AdviceHandler::Instance()->handler_map.emplace(key,val);
        return MIMIR_SUCCESS;
    }
    MimirStatus advice_end(MimirKey &key, MimirHandler &handler) {
        return MIMIR_SUCCESS;
    }
    MimirStatus advice_update_key(MimirKey &old_key, MimirKey &new_key) {
        return MIMIR_SUCCESS;
    }
}