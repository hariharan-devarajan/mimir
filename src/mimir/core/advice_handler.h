//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_ADVICE_HANDLER_H
#define MIMIR_ADVICE_HANDLER_H

#include <memory>
#include <unordered_map>
#include <mimir/common/data_structure.h>

namespace mimir {
    class AdviceHandler {
    private:
        static std::shared_ptr<AdviceHandler> instance;
    public:
        static std::shared_ptr<AdviceHandler> Instance() {
            if (instance == nullptr) {
                instance = std::make_shared<AdviceHandler>();
            }
            return instance;
        }
        AdviceHandler(): handler_map(){}
        std::unordered_map<MimirKey, std::vector<MimirHandler>> handler_map;
    };
}


#endif //MIMIR_ADVICE_HANDLER_H
