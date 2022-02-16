//
// Created by haridev on 2/14/22.
//

#include <mimir/api/posix.h>
#include <mimir/constant.h>
#include <mimir/log/logger.h>
#include <athena/api/interceptor.h>
#include <athena/api/posix.h>
#include <athena/mimir_adapter/advice/advice_handler.h>
#include <mimir/common/error_code.h>

namespace mimir {
    int file_advice_begin(FileAdvice &advice, MimirHandler &handler) {
        if (handler._type._primary != PrimaryAdviceType::DATA_FILE) {
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

    int operation_advice_begin(POSIXFileOperationAdvice &payload, MimirHandler &handler) {
        MimirKey key;
        size_t hash_val = std::hash<int>()(payload._fd);
        hash_val ^= std::hash<uint32_t>()(payload._offset);
        hash_val ^= std::hash<uint32_t>()(payload._size);
        key._id = hash_val;
        AdviceHandler<POSIXFileOperationAdvice>::Instance(payload._type)->save_advice(key, payload);
        handler._id = hash_val;
        handler._type = payload._type;
        return MIMIR_SUCCESS;

    }

    int operation_advice_end(MimirHandler &handler) {
        MimirKey key;
        key._id = handler._id;
        AdviceHandler<FileOperationAdvice>::Instance(handler._type)->remove_advice(key);
        return MIMIR_SUCCESS;

    }

    int file_advice_end(MimirHandler &handler) {
        MimirKey key;
        key._id = handler._id;
        AdviceHandler<FileAdvice>::Instance(handler._type)->remove_advice(key);
        return MIMIR_SUCCESS;
    }
} // mimir

int ATHENA_POSIX_DECL(open64)(const char *path, int flags, ...) {
    int ret;
    va_list arg;
    va_start(arg, flags);
    int mode = va_arg(arg, int);
    va_end(arg);
    auto type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                  mimir::OperationAdviceType::NO_OP);
    auto advice_handler = mimir::AdviceHandler<mimir::FileAdvice>::Instance(type);
    mimir::MimirKey key;
    std::hash<std::string> hash_str;
    key._id = hash_str(std::string(path));
    if (advice_handler->is_advice_present(key)) {

    } else {
        MAP_OR_FAIL(open64);
        ret = __athena_open64(path, flags, mode);
    }
    return ret;
}

int ATHENA_POSIX_DECL(open)(const char *path, int flags, ...) {
    int ret;
    return ret;
}

size_t ATHENA_POSIX_DECL(read)(int fd, void *buf, size_t count) {
    size_t ret;
    return ret;
}

size_t ATHENA_POSIX_DECL(write)(int fd, const void *buf, size_t count) {
    size_t ret;
    return ret;
}

int ATHENA_POSIX_DECL(close)(int fd) {
    int ret;
    return ret;
}