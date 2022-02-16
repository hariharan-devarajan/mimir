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
    std::string _name;
    uint32_t _io_amount_mb;
    TransferSizeDistribution _read_distribution;
    TransferSizeDistribution _write_distribution;
    float _per_io_data, _per_io_metadata;

    FileAdvice() : Advice(AdviceType(PrimaryAdviceType::DATA_DATASET,
                                     OperationAdviceType::NO_OP)), _format(), _size_mb(),
                   _name(), _io_amount_mb(), _read_distribution(),
                      _write_distribution(), _per_io_data(), _per_io_metadata(){}
    FileAdvice(const FileAdvice& other): Advice(other), _format(other._format),
                                         _size_mb(other._size_mb), _name(other._name),
                                         _read_distribution(other._read_distribution),
                                         _write_distribution(other._write_distribution), _per_io_data(other._per_io_data),
                                         _per_io_metadata(other._per_io_metadata) {}
    FileAdvice(const FileAdvice&& other): Advice(other), _format(other._format),
                                          _size_mb(other._size_mb), _name(other._name),
                                          _read_distribution(other._read_distribution),
                                          _write_distribution(other._write_distribution), _per_io_data(other._per_io_data),
                                          _per_io_metadata(other._per_io_metadata) {}
    FileAdvice& operator=(const FileAdvice& other) {
        Advice::operator=(other);
        _format = other._format;
        _size_mb = other._size_mb;
        _name = other._name;
        _read_distribution = other._read_distribution;
        _write_distribution = other._write_distribution;
        _per_io_data = other._per_io_data;
        _per_io_metadata = other._per_io_metadata;
        return *this;
    }
    bool operator<(const FileAdvice& other) const {
        return Advice::operator<(other);
    }
    bool operator>(const FileAdvice& other) const {
        return !(*this < other);
    }
};
}

namespace std {
    template<>
    struct hash<mimir::FileAdvice> {
        size_t operator()(const mimir::FileAdvice &k) const {
            return k._index;
        }
    };
}
#endif //MIMIR_FILE_ADVICE_H
