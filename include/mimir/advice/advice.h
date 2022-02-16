//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ADVICE_H
#define MIMIR_ADVICE_H

#include <mimir/common/typedef.h>
#include <memory>

namespace mimir {
    class Advice {
    private:
        static uint32_t __index;
    public:
        uint32_t _index;
        uint32_t _priority;
        AdviceType _type;
        Advice() : _type(AdviceType(PrimaryAdviceType::ADVICE_NONE,
                                     OperationAdviceType::NO_OP)),
                    _index(__index), _priority(0) {
            __index++;
        }
        Advice(AdviceType type) : _type(type), _index(__index), _priority(0) {
            __index++;
        }
        Advice(const Advice& other):_type(other._type),_index(other._index),_priority(other._priority) {}
        Advice(const Advice&& other):_type(other._type),_priority(other._priority) {}
        Advice& operator=(const Advice& other) {
            _type = other._type;
            _index = other._index;
            _priority = other._priority;
            return *this;
        }

        bool operator==(const Advice& other) const {
            return _type == other._type  &&
                   _index == other._index && _priority == other._priority;
        }

        bool operator<(const Advice& other) const {
            if (_priority < other._priority) return true;
            if (_priority == other._priority) return _index < other._index;
            return false;
        }

        bool operator>(const Advice& other) const {
            return !(*this < other);
        }

    };
    uint32_t Advice::__index = 0;

    typedef std::shared_ptr<Advice> MimirPayload;
} // namespace mimir


namespace std {
    template<>
    struct hash<mimir::Advice> {
        size_t operator()(const mimir::Advice &k) const {
            return k._index;
        }
    };
}
#endif //MIMIR_ADVICE_H
