//
// Created by haridev on 2/14/22.
//

#include <athena/api/interceptor.h>
#include <athena/api/posix.h>
#include <dlfcn.h>
#include <mimir/advice/advice_handler.h>
#include <mimir/api/posix.h>
#include <mimir/common/error_code.h>
#include <mimir/constant.h>
#include <mimir/macro.h>
#include <mpi.h>

MimirStatus file_prefetch(mimir::FileAdvice &advice) {}
namespace mimir {

MimirStatus file_advice_begin(FileAdvice &advice, MimirHandler &handler) {
  if (advice._type._primary != PrimaryAdviceType::DATA_FILE) {
    return MIMIR_ONLY_FILE_ALLOWED;
  }

  mimir::MimirKey key;
  std::hash<std::string> hash_str;
  key._id = hash_str(advice._name);
  mimir::MimirKey job_key;
  job_key._id = 0;
  if (advice._prefetch) {
    typedef MimirStatus (*real_t_file_prefetch_)(FileAdvice &);

    if ((advice._type._secondary == OperationAdviceType::INPUT_FILE ||
         advice._type._secondary == OperationAdviceType::READ_ONLY_FILE)) {
      auto ld_so = std::getenv("LD_PRELOAD");
      auto handle = dlopen(ld_so, RTLD_GLOBAL | RTLD_LAZY);
      real_t_file_prefetch_ derived_file_prefetch_ =
          (real_t_file_prefetch_)dlsym(handle, "file_prefetch");
      if (derived_file_prefetch_ == NULL) {
        file_prefetch(advice);
      } else {
        derived_file_prefetch_(advice);
      }
    }
  }
  AdviceHandler<FileAdvice>::Instance(advice._type)->save_advice(key, advice);
  handler._type = advice._type;
  handler._key_id = key._id;
  handler._advice_index = advice._index;
  MIMIR_TRACKER()->track(advice._name);
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
  handler._key_id = key._id;
  handler._advice_index = payload._index;
  handler._type = payload._type;
  return MIMIR_SUCCESS;
}

MimirStatus operation_advice_end(MimirHandler &handler) {
  MimirKey key;
  key._id = handler._key_id;
  AdviceHandler<FileOperationAdvice>::Instance(handler._type)
      ->remove_advice(key, handler._advice_index);
  return MIMIR_SUCCESS;
}

MimirStatus file_advice_end(MimirHandler &handler) {
  MimirKey key;
  key._id = handler._key_id;
  auto advice = AdviceHandler<FileAdvice>::Instance(handler._type)
                    ->remove_advice(key, handler._advice_index);
  MIMIR_TRACKER()->remove(advice._name);
  return MIMIR_SUCCESS;
}

MimirStatus free_files() {
  auto ptr = AdviceHandler<FileAdvice>::Instance(
      {mimir::PrimaryAdviceType::DATA_FILE, mimir::OperationAdviceType::NO_OP});
  ptr->clear();
  ptr.reset();
  return MIMIR_SUCCESS;
}
}  // namespace mimir