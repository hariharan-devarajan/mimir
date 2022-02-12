//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_EXECUTION_ENGINE_ADVICE_H
#define MIMIR_EXECUTION_ENGINE_ADVICE_H
class ExecutionEngineAdvice : public Advice {
public:
    uint16 _cpu_cores_available;
    uint8 _gpus_available;
    uint32 _memory_available;
    TransferSizeDistribution _read_distribution;
    TransferSizeDistribution _write_distribution;
    AccessPattern _access_pattern;
    std::unordered_map<File, AdviceType> _file_advice_map;

    HLIOAdvice() : Advice(PrimaryAdviceType::SOFTWARE_EXECUTION_ENGINE), _logical_representation(),
                   _read_distribution(), _write_distribution(), _access_pattern(),
                   _file_advice_map() {}
};
#endif //MIMIR_EXECUTION_ENGINE_ADVICE_H
