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
        auto advices = advice_handler->resolve_conflicts(key);
        for (const auto& advice : advices) {
            switch (advice._type._secondary) {
                case mimir::OperationAdviceType::INDEPENDENT_FILE: {
                    /**
                     * place I/O in node-local BB.
                     */
                     break;
                }
                case mimir::OperationAdviceType::TMP_FILE: {
                    /** do nothing **/
                    break;
                }
                case mimir::OperationAdviceType::EVENTUAL_CONSISTENCY: {
                    /** perform async I/O **/
                    break;
                }
                case mimir::OperationAdviceType::WRITE_ONLY_FILE: {
                    /** perform async I/O **/
                    break;
                }
                case mimir::OperationAdviceType::READ_ONLY_FILE: {
                    /** perform prefetching **/
                    break;
                }
                case mimir::OperationAdviceType::INPUT_FILE: {
                    /** perform prefetching on input files **/
                    break;
                }
                case mimir::OperationAdviceType::OUTPUT_FILE: {
                    /** make sure we store on PFS **/
                    break;
                }
                case mimir::OperationAdviceType::SHARED_FILE: {
                    /** make sure we store on shared FS using shared memory **/
                    break;
                }
                case mimir::OperationAdviceType::STRONG_CONSISTENCY: {
                    /** make sure we flush I/O **/
                    break;
                }
            }
        }
    } else {
        MAP_OR_FAIL(open64);
        ret = __athena_open64(path, flags, mode);
    }
    return ret;
}

int ATHENA_POSIX_DECL(open)(const char *path, int flags, ...) {
    int ret;
    va_list arg;
    va_start(arg, flags);
    int mode = va_arg(arg, int);
    va_end(arg);
    ret = __warp_open64(path, flags, mode);
    return ret;
}

size_t ATHENA_POSIX_DECL(read)(int fd, void *buf, size_t count) {
    size_t ret;
    MAP_OR_FAIL(read);
    ret = __athena_read(fd, buf, count);
    return ret;
}

size_t ATHENA_POSIX_DECL(write)(int fd, const void *buf, size_t count) {
    size_t ret;
    MAP_OR_FAIL(write);
    ret = __athena_write(fd, buf, count);
    return ret;
}

int ATHENA_POSIX_DECL(close)(int fd) {
    int ret;
    MAP_OR_FAIL(close);
    ret = __athena_close(fd);
    return ret;
}