//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ADVICE_TYPE_H
#define MIMIR_ADVICE_TYPE_H
namespace mimir {
    enum class PrimaryAdviceType: uint16 {
        JOB_CONFIGURATION = 0,
        JOB_WORKFLOW = 1,
        JOB_APPLICATION = 2,
        JOB_IO_PHASE = 3,
        JOB_IO_OPERATION = 4,
        SOFTWARE_HLIO_LIB = 5,
        SOFTWARE_EXECUTION_ENGINE = 6,
        SOFTWARE_MIDDLEWARE_LIBRARY = 7,
        SOFTWARE_NODE_LOCAL_FS = 8,
        SOFTWARE_SHARED_FS = 9,
        DATA_DATASET = 10,
        DATA_FILE = 11,
        DATA_SAMPLE = 12
    }

    enum class OperationAdviceType: uint16 {
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

    union AdviceType {
        struct PrimaryAdviceType primary;
        struct OperationAdviceType secondary;
    };
}

#endif //MIMIR_ADVICE_TYPE_H
