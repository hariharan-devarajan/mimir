//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ENUMERATION_H
#define MIMIR_ENUMERATION_H
namespace mimir {
    enum InterfaceType {
        POSIX = 0,
        STDIO = 1,
        MPIIO = 2,
        HDF5 = 3
    };
    enum AccessPattern {
        SEQUENTIAL = 0,
        STRIDED = 1,
        RANDOM = 2
    };
    enum OperationType {
        DATA = 0,
        METADATA = 1
    };
    enum DataRepresentation {
        BINARY = 0,
        ND_ARRAY = 1,
        IMAGE = 2
    };
    enum Format {
        BINARY = 0,
        HDF5 = 1
    };
} // mimir
#endif //MIMIR_ENUMERATION_H
