//
// Created by haridev on 2/15/22.
//

#ifndef MIMIR_ERROR_CODE_H
#define MIMIR_ERROR_CODE_H
namespace mimir {
enum ErrorCode : MimirStatus {
  MIMIR_SUCCESS = 0,
  MIMIR_ONLY_FILE_ALLOWED = -1,
  MIMIR_ONLY_JOB_IO_ALLOWED = -2,
  MIMIR_ONLY_JOB_CONFIGURATION_ALLOWED = -3
};
}
#endif  // MIMIR_ERROR_CODE_H
