//
// Created by haridev on 2/14/22.
//

#include <mimir/api/posix.h>
#include <mimir/constant.h>
#include <mimir/log/logger.h>
#include <athena/mimir_adapter/advice/advice_handler.h>
#include <athena/api/interceptor.h>
#include <athena/api/posix.h>

namespace mimir {
    int file_advice_begin(const char *path, AdviceType advice,
                          MimirPayload payload, PosixMimirHandler &handler) {
        MimirKey key;
        std::hash<std::string> hash_str;
        key._id = hash_str(std::string(path));
        handler._filename = std::string(path);
        handler._offset = 0;
        handler._size = 0;
        handler._payload = payload;
        handler._type = advice;
        handler._index++;
        AdviceHandler::Instance()->save_advice(key, handler);
        return MIMIR_SUCCESS;
    }

    int operation_advice_begin(int fd, uint32_t offset, uint32_t size,
                               AdviceType advice, MimirPayload payload, PosixMimirHandler &handler) {
        MimirKey key;
        size_t hash_val = std::hash<int>()(fd);
        hash_val ^= std::hash<uint32_t>()(offset);
        hash_val ^= std::hash<uint32_t>()(size);
        key._id = hash_val;
        handler._fd = fd;
        handler._offset = offset;
        handler._size = size;
        handler._payload = payload;
        handler._type = advice;
        handler._index++;
        AdviceHandler::Instance()->save_advice(key, handler);
        return MIMIR_SUCCESS;

    }

    int operation_advice_end(PosixMimirHandler &handler) {
        MimirKey key;
        size_t hash_val = std::hash<int>()(handler._fd);
        hash_val ^= std::hash<uint32_t>()(handler._offset);
        hash_val ^= std::hash<uint32_t>()(handler._size);
        key._id = hash_val;
        AdviceHandler::Instance()->remove_advice(key);
        return MIMIR_SUCCESS;

    }

    int file_advice_end(PosixMimirHandler &handler) {
        MimirKey key;
        size_t hash_val = std::hash<std::string>()(handler._filename);
        key._id = hash_val;
        AdviceHandler::Instance()->remove_advice(key);
        return MIMIR_SUCCESS;
    }
} // mimir

int ATHENA_POSIX_DECL(open64)(const char *path, int flags, ...) {

}

int ATHENA_POSIX_DECL(open)(const char *path, int flags, ...) {

}

size_t ATHENA_POSIX_DECL(read)(int fd, void *buf, size_t count) {

}

size_t ATHENA_POSIX_DECL(write)(int fd, const void *buf, size_t count) {

}

int ATHENA_POSIX_DECL(close)(int fd) {

}