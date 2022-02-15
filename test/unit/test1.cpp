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
    mimir::PosixMimirHandler file_handler[2];
    mimir::PosixMimirHandler operation_handler;
    mimir::file_advice_begin(filename, mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                                         mimir::OperationAdviceType::INDEPENDENT_FILE),
                             mimir::NO_ADVICE, file_handler[0]);
    mimir::file_advice_begin(filename, mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                                         mimir::OperationAdviceType::TMP_FILE),
                             mimir::NO_ADVICE, file_handler[1]);
    int fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG);
    if (fd > 0) {
        mimir::operation_advice_begin(fd, 0, 1024,
                                      mimir::AdviceType(mimir::PrimaryAdviceType::JOB_IO_OPERATION,
                                                       mimir::OperationAdviceType::TMP_FILE),
                                 mimir::NO_ADVICE, operation_handler);
        char *data = "hello";
        size_t count = write(fd, data, strlen(data));
        if (count != strlen(data)) {
            fprintf(stderr, "Error writing file %s\n", filename);
        }
        mimir::operation_advice_end(operation_handler);
        close(fd);
        mimir::file_advice_end(file_handler[0]);
        mimir::file_advice_end(file_handler[1]);
    }
    return 0;
}