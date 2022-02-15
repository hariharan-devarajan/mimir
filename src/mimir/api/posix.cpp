//
// Created by haridev on 2/14/22.
//

#include <mimir/api/posix.h>
#include <mimir/constant.h>
#include <mimir/log/logger.h>
#include <fcntl.h>

int mimir::posix_mimir_advice_begin(int fd, AdviceType advice, MimirPayload payload) {
    if (advice._primary > 0) {

    }
}

int mimir::posix_mimir_advice_end(int fd, AdviceType advice, MimirPayload payload) {

}

int mimir::open_file(const char *path, int flags, int mode, AdviceType advice, MimirPayload payload) {
    int ret = -1;
    if (advice._primary > 0) {
        MimirHandler handler;
        std::hash<std::string> hash_string;
        auto hash_val = hash_string(std::string(path));
        POSIXMimirKey key;
        key._fd = hash_val;
        MimirStatus status = advice_begin(key, handler, advice, payload);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        ret = open(path, flags, mode);
        POSIXMimirKey new_key;
        new_key._fd = ret;
        status = advice_update_key(key, new_key);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        status = advice_end(key, handler);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot end advice for type %d", advice);
        }
    } else {
        ret = open(path, flags);
    }
    return ret;
}

int mimir::open_file(const char *path, int flags, AdviceType advice, MimirPayload payload) {
    int ret = -1;
    if (advice._primary > 0) {
        MimirHandler handler;
        std::hash<std::string> hash_string;
        auto hash_val = hash_string(std::string(path));
        POSIXMimirKey key;
        key._fd = hash_val;
        MimirStatus status = advice_begin(key, handler, advice, payload);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        ret = open(path, flags);
        POSIXMimirKey new_key;
        new_key._fd = ret;
        status = advice_update_key(key, new_key);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        status = advice_end(key, handler);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot end advice for type %d", advice);
        }
    } else {
        ret = open(path, flags);
    }
    return ret;
}

int mimir::close_file(int fd, AdviceType advice, MimirPayload payload) {
    int ret;
    if (advice._primary > 0) {
        POSIXMimirKey key;
        key._fd = fd;
        MimirHandler handler;
        MimirStatus status = advice_begin(key, handler, advice, payload);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        ret = close(fd);
        status = advice_end(key, handler);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot end advice for type %d", advice);
        }
    } else {
        ret = close(fd);
    }
    return ret;
}

size_t mimir::write_file(int fd, const void *buf, size_t count, AdviceType advice, MimirPayload payload) {
    ssize_t ret;
    if (advice._primary > 0) {
        MimirHandler handler;
        POSIXMimirKey key;
        key._fd = fd;
        MimirStatus status = advice_begin(key, handler, advice, payload);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        ret = write(fd, buf, count);
        status = advice_end(key, handler);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot end advice for type %d", advice);
        }
    } else {
        ret = write(fd, buf, count);
    }
    return ret;
}

size_t mimir::read_file(int fd, void *buf, size_t count, AdviceType advice, MimirPayload payload) {
    ssize_t ret;
    if (advice._primary > 0) {
        MimirHandler handler;
        POSIXMimirKey key;
        key._fd = fd;
        MimirStatus status = advice_begin(key, handler, advice, payload);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot start advice for type %d", advice);
        }
        ret = read(fd, buf, count);
        status = advice_end(key, handler);
        if (status != MIMIR_SUCCESS) {
            Logger::Instance()->log(LoggerType::LOG_WARN, "Cannot end advice for type %d", advice);
        }
    } else {
        ret = read(fd, buf, count);
    }
    return ret;
}

