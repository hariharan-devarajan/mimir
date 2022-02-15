//
// Created by haridev on 2/14/22.
//

#include <athena/mimir_adapter/advice/advice_handler.h>

std::shared_ptr<mimir::AdviceHandler> mimir::AdviceHandler::instance = nullptr;

MimirStatus mimir::AdviceHandler::remove_advice(mimir::MimirKey &key) {
    auto iter = handler_map.erase(key);
    return MIMIR_SUCCESS;
}

MimirStatus mimir::AdviceHandler::save_advice(mimir::MimirKey &key, mimir::PosixMimirHandler &handler) {
    auto iter = handler_map.find(key);
    std::vector<MimirHandler> val;
    if (iter == handler_map.end()) {
        val = std::vector<MimirHandler>();
    } else {
        val = iter->second;
        handler_map.erase(iter);
    }
    val.push_back(handler);
    handler_map.emplace(key,val);
    return MIMIR_SUCCESS;
}
