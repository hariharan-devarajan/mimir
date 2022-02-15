//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_HLIO_ADVICE_H
#define MIMIR_HLIO_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
    class HLIOAdvice : public Advice {
    public:
        DataRepresentation _logical_representation;
        TransferSizeDistribution _read_distribution;
        TransferSizeDistribution _write_distribution;
        AccessPattern _access_pattern;
        std::unordered_map<File, AdviceType> _file_advice_map;

        HLIOAdvice() : Advice(AdviceType(PrimaryAdviceType::SOFTWARE_HLIO_LIB,
                                         OperationAdviceType::NO_OP)), _logical_representation(),
                       _read_distribution(), _write_distribution(), _access_pattern(),
                       _file_advice_map() {}
    };
}
#endif //MIMIR_HLIO_ADVICE_H
