//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_CORE_ADVICE_H
#define MIMIR_CORE_ADVICE_H

#include <mimir/typedef.h>
#include <mimir/common/data_structure.h>
#include <mimir/constant.h>
#include <cstdarg>
#include <mimir/advice/advice_type.h>
#include <mimir/core/advice_handler.h>

namespace mimir {
    MimirStatus advice_begin(MimirKey &key, MimirHandler &handler, AdviceType type, MimirPayload payload);
    MimirStatus advice_end(MimirKey &key, MimirHandler &handler);
    MimirStatus advice_update_key(MimirKey &old_key, MimirKey &new_key);
}

#endif //MIMIR_CORE_ADVICE_H
