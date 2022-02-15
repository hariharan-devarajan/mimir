//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_SHARED_STORAGE_ADVICE_H
#define MIMIR_SHARED_STORAGE_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
    class SharedStorageAdvice : public Advice {
    public:
        uint8_t _server_parallelism;
        uint32_t _capacity_mb;
        TransferSizeDistribution _write_performance_mbps;
        TransferSizeDistribution _read_performance_mbps;
        Storage storage;

        SharedStorageAdvice() : Advice(AdviceType(PrimaryAdviceType::SOFTWARE_SHARED_FS,
                                                  OperationAdviceType::NO_OP)), _server_parallelism(),
                                _capacity_mb(), _write_performance_mbps(), _read_performance_mbps(),
                                storage() {}
    };
}
#endif //MIMIR_SHARED_STORAGE_ADVICE_H
