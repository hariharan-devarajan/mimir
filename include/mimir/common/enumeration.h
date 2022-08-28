//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_ENUMERATION_H
#define MIMIR_ENUMERATION_H
namespace mimir {
enum InterfaceType { POSIX = 0, STDIO = 1, MPIIO = 2, HDF5 = 3 };
enum AccessPattern { SEQUENTIAL = 0, STRIDED = 1, RANDOM = 2 };
enum OperationType { DATA = 0, METADATA = 1 };
enum DataRepresentation { DATA_BINARY = 0, DATA_ND_ARRAY = 1, DATA_IMAGE = 2 };
enum Format { FORMAT_BINARY = 0, FORMAT_HDF5 = 1 };
enum FileSharing { FILE_SHARING_NONE = 0, FILE_PER_PROCESS = 1, FILE_SHARED = 2 };
}  // namespace mimir
#endif  // MIMIR_ENUMERATION_H
