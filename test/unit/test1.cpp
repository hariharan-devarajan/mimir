//
// Created by haridev on 2/11/22.
//
#include <stdio.h>
#include <mimir/api/posix.h>
#include <fcntl.h>
#include <mimir/advice/advice_type.h>
#include <cstring>

int main(int argc, char *argv[]) {

    char* filename = "./test.dat";
    int fd = mimir::open_file(filename,
                              O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG,
                              mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                                mimir::OperationAdviceType::INDEPENDENT_FILE),
                              mimir::NO_ADVICE);
    if (fd > 0) {
        mimir::posix_mimir_advice_begin(fd, mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                                       mimir::OperationAdviceType::TMP_FILE),
                                        mimir::NO_ADVICE);
        char *data = "hello";
        ssize_t status = mimir::write_file(fd,
                                           data, strlen(data),
                                           mimir::AdviceType(mimir::PrimaryAdviceType::JOB_IO_OPERATION,
                                                             mimir::OperationAdviceType::WRITE_ONLY_FILE),
                                           mimir::NO_ADVICE);
        if (status != strlen(data)) {
            fprintf(stderr, "Error writing file %s\n", filename);
        }
        mimir::posix_mimir_advice_end(fd, mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                                              mimir::OperationAdviceType::TMP_FILE),
                                      mimir::NO_ADVICE);
        mimir::close_file(fd,
                          mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                               mimir::OperationAdviceType::NO_OP),
                          mimir::NO_ADVICE);
    }
    return 0;
}