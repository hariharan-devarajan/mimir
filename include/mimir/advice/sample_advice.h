//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_SAMPLE_ADVICE_H
#define MIMIR_SAMPLE_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
    class SampleAdvice : public Advice {
    public:
        uint32_t _size_mb;
        DataRepresentation _representation;

        SampleAdvice() : Advice(AdviceType(PrimaryAdviceType::DATA_SAMPLE,
                                         OperationAdviceType::NO_OP)), _size_mb(), _representation() {}
    };
}
#endif //MIMIR_SAMPLE_ADVICE_H
