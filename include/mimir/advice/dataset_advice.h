//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_DATASET_ADVICE_H
#define MIMIR_DATASET_ADVICE_H
class DatasetAdvice : public Advice {
public:
    Format _format;
    uint32 _size_mb;
    uint16 _num_files;
    uint32 _io_amount_mb;
    float _runtime_minutes;
    TransferSizeDistribution _read_distribution;
    TransferSizeDistribution _write_distribution;
    std::unordered_map<File, uint32> file_size_distribution;
    float _per_io_data, _per_io_metadata;

    DatasetAdvice() : Advice(PrimaryAdviceType::DATA_DATASET), _format(), _size_mb(),
                      _num_files(), _io_amount_mb(), _runtime_minutes(), _read_distribution(),
                      _write_distribution(), file_size_distribution(), _per_io_data(), _per_io_metadata(){}
};
#endif //MIMIR_DATASET_ADVICE_H
