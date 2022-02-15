//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_POSIX_H
#define MIMIR_POSIX_H
#include <unistd.h>
#include <cstdarg>
#include <mimir/typedef.h>
#include <mimir/common/data_structure.h>
#include <mimir/advice/advice_type.h>
#include <mimir/mimir.h>

namespace mimir {
    int file_advice_begin(const char *path, AdviceType advice, MimirPayload payload,
                          PosixMimirHandler &handler);
    int operation_advice_begin(int fd, uint32_t offset, uint32_t size, AdviceType advice,
                               MimirPayload payload, PosixMimirHandler &handler);
    int operation_advice_end(PosixMimirHandler &handler);
    int file_advice_end(PosixMimirHandler &handler);
}



#endif //MIMIR_POSIX_H
