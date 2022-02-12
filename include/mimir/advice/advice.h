//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ADVICE_H
#define MIMIR_ADVICE_H

#include <mimir/common/typedef.h>

namespace mimir {
    class Advice {
    private:
        PrimaryAdviceType _type;
    protected:
        Advice(PrimaryAdviceType type) : _type(type) {}

    public:
        virtual std::pair<ReturnType, int> encode() = 0;

        virtual ReturnType decode(int &hash_value) = 0;
    };
} // namespace mimir
#endif //MIMIR_ADVICE_H
