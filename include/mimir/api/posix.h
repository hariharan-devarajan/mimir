//
// Created by haridev on 2/14/22.
//

#ifndef MIMIR_POSIX_H
#define MIMIR_POSIX_H
#include <unistd.h>
#include <cstdarg>
#include <mimir/typedef.h>
#include <mimir/common/data_structure.h>
#include <mimir/core/advice.h>
#include <mimir/advice/advice_type.h>
#include <mimir/mimir.h>

namespace mimir {
    int posix_mimir_advice_begin(int fd, AdviceType advice, MimirPayload payload);
    int posix_mimir_advice_end(int fd, AdviceType advice, MimirPayload payload);

    int open_file(const char *path, int flags, int mode, AdviceType advice, MimirPayload payload);
    int open_file(const char *path, int flags, AdviceType advice, MimirPayload payload);
    int close_file(int fd, AdviceType advice, MimirPayload payload);
    size_t write_file(int fd, const void *buf, size_t count, AdviceType advice, MimirPayload payload);
    size_t read_file(int fd, void *buf, size_t count, AdviceType advice, MimirPayload payload);
}


#endif //MIMIR_POSIX_H
