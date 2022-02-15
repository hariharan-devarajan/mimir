//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ADVICE_TYPE_H
#define MIMIR_ADVICE_TYPE_H

#include <stdint-gcc.h>

namespace mimir {
    enum PrimaryAdviceType: uint16_t {
        ADVICE_NONE = 0,
        JOB_CONFIGURATION = 1,
        JOB_WORKFLOW = 2,
        JOB_APPLICATION = 3,
        JOB_IO_PHASE = 4,
        JOB_IO_OPERATION = 5,
        SOFTWARE_HLIO_LIB = 6,
        SOFTWARE_EXECUTION_ENGINE = 7,
        SOFTWARE_MIDDLEWARE_LIBRARY = 8,
        SOFTWARE_NODE_LOCAL_FS = 9,
        SOFTWARE_SHARED_FS = 10,
        DATA_DATASET = 11,
        DATA_FILE = 12,
        DATA_SAMPLE = 13
    };

    enum OperationAdviceType: uint16_t {
        NO_OP = 0,
        TMP_FILE = 1,
        INDEPENDENT_FILE = 2,
        SHARED_FILE = 3,
        INPUT_FILE = 4,
        OUTPUT_FILE = 5,
        IO_PHASE_START = 6,
        IO_PHASE_END = 7,
        FILE_OPEN_COUNT = 8,
        WRITE_ONLY_FILE = 9,
        READ_ONLY_FILE = 10,
        EVENTUAL_CONSISTENCY = 11,
        STRONG_CONSISTENCY = 12
    };

    typedef union AdviceType{
        PrimaryAdviceType _primary;
        OperationAdviceType _secondary;
        AdviceType(PrimaryAdviceType primary, OperationAdviceType secondary) {
            _primary = primary;
            _secondary = secondary;
        }
    } AdviceType;

}



#endif //MIMIR_ADVICE_TYPE_H
