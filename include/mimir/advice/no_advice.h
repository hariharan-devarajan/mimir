//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_NO_ADVICE_H
#define MIMIR_NO_ADVICE_H

#include <mimir/advice/advice.h>

namespace mimir {
    class NoAdvice: public Advice {
    public:
        NoAdvice() : Advice(AdviceType(PrimaryAdviceType::ADVICE_NONE,
                                       OperationAdviceType::NO_OP)) {}
    };
    const std::shared_ptr<mimir::NoAdvice> NO_ADVICE;
} // namespace mimir
#endif //MIMIR_NO_ADVICE_H
