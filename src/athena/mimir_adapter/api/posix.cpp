//
// Created by haridev on 2/14/22.
//

#include <athena/api/posix.h>
#include <mimir/constant.h>
#include <mimir/log/logger.h>
#include <athena/api/interceptor.h>
#include <athena/api/posix.h>
#include <mimir/advice/advice_handler.h>
#include <mimir/common/error_code.h>
#include <mimir/advice/file_advice.h>

int ATHENA_DECL(open64)(const char *path, int flags, ...) {
    int ret;
    va_list arg;
    va_start(arg, flags);
    int mode = va_arg(arg, int);
    va_end(arg);
    auto file_type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                  mimir::OperationAdviceType::NO_OP);
    auto job_conf_type = mimir::AdviceType(mimir::PrimaryAdviceType::JOB_CONFIGURATION,
                                       mimir::OperationAdviceType::NO_OP);
    auto file_advice_handler = mimir::AdviceHandler<mimir::FileAdvice>::Instance(file_type);
    auto job_conf_advice_handler = mimir::AdviceHandler<mimir::JobConfigurationAdvice>::Instance(job_conf_type);
    mimir::MimirKey file_key;
    std::hash<std::string> hash_str;
    file_key._id = hash_str(std::string(path));
    mimir::MimirKey job_conf_key;
    job_conf_key._id = 0;
    auto job_conf_advices = job_conf_advice_handler->find_advice(job_conf_key);

    if (file_advice_handler->is_advice_present(file_key)) {
        auto advices = file_advice_handler->resolve_conflicts(file_key);
        std::string filename(path);
        for (const auto& advice : advices) {
            switch (advice._type._secondary) {
                case mimir::OperationAdviceType::INDEPENDENT_FILE: {
                    /**
                     * place I/O in node-local BB.
                     */
                     if (job_conf_advices.first) {
                         auto job_conf_advice = job_conf_advices.second[0];
                         auto num_devices = job_conf_advice._devices.size();
                         if (num_devices > 1) {
                             bool updated = false;
                             auto updated_devices = std::vector<mimir::Storage>();
                             mimir::Storage device;
                             for (int i = 0; i < num_devices; ++i) {
                                 device = job_conf_advice._devices[i];
                                 if (!updated && (device._capacity_mb - device._used_capacity_mb - advice._size_mb) > 0) {

                                     filename.replace(0, advice._device._mount_point.size(),
                                                      device._mount_point);
                                     device._used_capacity_mb = device._used_capacity_mb + advice._size_mb;
                                     updated = true;

                                 }
                                 updated_devices.push_back(device);
                             }
                             if (updated) {
                                 job_conf_advice._devices = updated_devices;
                                 job_conf_advice_handler->save_advice(job_conf_key, job_conf_advice);
                             }

                         }
                     }
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
        MAP_OR_FAIL(open64);
        if (flags & O_DIRECT) {
            flags ^= O_DIRECT;
        }
        ret = real_open64_(filename.c_str(), flags, mode);
    } else {
        MAP_OR_FAIL(open64);
        ret = real_open64_(path, flags, mode);
    }
    return ret;
}

int ATHENA_DECL(open)(const char *path, int flags, ...) {
    int ret;
    va_list arg;
    va_start(arg, flags);
    int mode = va_arg(arg, int);
    va_end(arg);
    ret = open64(path, flags, mode);
    return ret;
}

ssize_t ATHENA_DECL(read)(int fd, void *buf, size_t count) {
    size_t ret;
    MAP_OR_FAIL(read);
    ret = real_read_(fd, buf, count);
    return ret;
}

ssize_t ATHENA_DECL(write)(int fd, const void *buf, size_t count) {
    size_t ret;
    MAP_OR_FAIL(write);
    ret = real_write_(fd, buf, count);
    return ret;
}

int ATHENA_DECL(close)(int fd) {
    int ret;
    MAP_OR_FAIL(close);
    ret = real_close_(fd);
    return ret;
}

off64_t ATHENA_DECL(lseek64)(int fd, off64_t offset, int whence) {
    off64_t ret;
    MAP_OR_FAIL(lseek64);
    ret = real_lseek64_(fd, offset, whence);
    return ret;
}

off_t ATHENA_DECL(lseek)(int fd, off_t offset, int whence) {
    off_t ret;
    MAP_OR_FAIL(lseek);
    ret = real_lseek_(fd, offset, whence);
    return ret;
}