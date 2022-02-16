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
#include <mimir/advice/posix_file_operation_advice.h>

namespace mimir {
    MimirStatus file_advice_begin(FileAdvice &payload, MimirHandler &handler);
    MimirStatus operation_advice_begin(POSIXFileOperationAdvice &payload, MimirHandler &handler);
    MimirStatus operation_advice_end(MimirHandler &handler);
    MimirStatus file_advice_end(MimirHandler &handler);
}



#endif //MIMIR_POSIX_H
