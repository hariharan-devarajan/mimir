//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_FILE_ADVICE_H
#define MIMIR_FILE_ADVICE_H
#include <mimir/advice/advice.h>
namespace mimir {
class FileAdvice : public Advice {
public:
    Format _format;
    uint32_t _size_mb;
    uint16_t _num_files;
    uint32_t _io_amount_mb;
    TransferSizeDistribution _read_distribution;
    TransferSizeDistribution _write_distribution;
    float _per_io_data, _per_io_metadata;

    FileAdvice() : Advice(AdviceType(PrimaryAdviceType::DATA_DATASET,
                                     OperationAdviceType::NO_OP)), _format(), _size_mb(),
                      _num_files(), _io_amount_mb(), _read_distribution(),
                      _write_distribution(), _per_io_data(), _per_io_metadata(){}
};
}
#endif //MIMIR_FILE_ADVICE_H
