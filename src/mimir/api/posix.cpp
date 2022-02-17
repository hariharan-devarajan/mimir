//
// Created by haridev on 2/14/22.
//

#include <athena/api/interceptor.h>
#include <athena/api/posix.h>
#include <mimir/advice/advice_handler.h>
#include <mimir/api/posix.h>
#include <mimir/common/error_code.h>
#include <mimir/constant.h>
#include <mimir/log/logger.h>

namespace mimir {
MimirStatus file_advice_begin(FileAdvice &advice, MimirHandler &handler) {
  if (advice._type._primary != PrimaryAdviceType::DATA_FILE) {
    return MIMIR_ONLY_FILE_ALLOWED;
  }
  MimirKey key;
  std::hash<std::string> hash_str;
  key._id = hash_str(advice._name);
  AdviceHandler<FileAdvice>::Instance(advice._type)->save_advice(key, advice);
  handler._type = advice._type;
  handler._id = key._id;
  return MIMIR_SUCCESS;
}

MimirStatus operation_advice_begin(POSIXFileOperationAdvice &payload,
                                   MimirHandler &handler) {
  if (handler._type._primary != PrimaryAdviceType::JOB_IO_OPERATION) {
    return MIMIR_ONLY_JOB_IO_ALLOWED;
  }
  MimirKey key;
  size_t hash_val = std::hash<int>()(payload._fd);
  hash_val ^= std::hash<uint32_t>()(payload._offset);
  hash_val ^= std::hash<uint32_t>()(payload._size);
  key._id = hash_val;
  AdviceHandler<POSIXFileOperationAdvice>::Instance(payload._type)
      ->save_advice(key, payload);
  handler._id = hash_val;
  handler._type = payload._type;
  return MIMIR_SUCCESS;
}

MimirStatus operation_advice_end(MimirHandler &handler) {
  MimirKey key;
  key._id = handler._id;
  AdviceHandler<FileOperationAdvice>::Instance(handler._type)
      ->remove_advice(key);
  return MIMIR_SUCCESS;
}

MimirStatus file_advice_end(MimirHandler &handler) {
  MimirKey key;
  key._id = handler._id;
  AdviceHandler<FileAdvice>::Instance(handler._type)->remove_advice(key);
  return MIMIR_SUCCESS;
}
}  // namespace mimir