//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ADVICE_H
#define MIMIR_ADVICE_H

#include <mimir/common/typedef.h>
#include <memory>

namespace mimir {
    class Advice {
    public:
        AdviceType _type;
        Advice(AdviceType type) : _type(type) {}

    };

    typedef std::shared_ptr<Advice> MimirPayload;
} // namespace mimir
#endif //MIMIR_ADVICE_H
