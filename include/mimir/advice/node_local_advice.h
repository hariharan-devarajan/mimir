//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_NODE_LOCAL_ADVICE_H
#define MIMIR_NODE_LOCAL_ADVICE_H
class NodeLocalAdvice : public Advice {
public:
    uint8 _controller_parallelism;
    uint32 _capacity_mb;
    TransferSizeDistribution _write_performance_mbps;
    TransferSizeDistribution _read_performance_mbps;
    Storage storage;

    NodeLocalAdvice() : Advice(PrimaryAdviceType::SOFTWARE_NODE_LOCAL_FS), _controller_parallelism(),
                        _capacity_mb(), _write_performance_mbps(), _read_performance_mbps(),
                        storage() {}
};
#endif //MIMIR_NODE_LOCAL_ADVICE_H
