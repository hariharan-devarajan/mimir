//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_FILE_ADVICE_H
#define MIMIR_FILE_ADVICE_H
class FileAdvice : public Advice {
public:
    Format _format;
    uint32 _size_mb;
    uint16 _num_files;
    uint32 _io_amount_mb;
    TransferSizeDistribution _read_distribution;
    TransferSizeDistribution _write_distribution;
    float _per_io_data, _per_io_metadata;

    FileAdvice() : Advice(PrimaryAdviceType::DATA_DATASET), _format(), _size_mb(),
                      _num_files(), _io_amount_mb(), _runtime_minutes(), _read_distribution(),
                      _write_distribution(), _per_io_data(), _per_io_metadata(){}
};
#endif //MIMIR_FILE_ADVICE_H
