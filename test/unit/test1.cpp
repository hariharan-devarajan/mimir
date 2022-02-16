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
    mimir::MimirHandler file_handler[2];
    mimir::MimirHandler operation_handler;
    mimir::FileAdvice file_advice[2];
    file_advice[0]._name = filename;
    file_advice[0]._type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                             mimir::OperationAdviceType::INDEPENDENT_FILE);
    file_advice[1]._name = filename;
    file_advice[1]._type = mimir::AdviceType(mimir::PrimaryAdviceType::DATA_FILE,
                                             mimir::OperationAdviceType::TMP_FILE);






    mimir::file_advice_begin(file_advice[0], file_handler[0]);
    mimir::file_advice_begin(file_advice[1], file_handler[1]);
    int fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG);
    if (fd > 0) {
        mimir::POSIXFileOperationAdvice operation_advice;
        operation_advice._fd = fd;
        operation_advice._type = mimir::AdviceType(mimir::PrimaryAdviceType::JOB_IO_OPERATION,
                                                   mimir::OperationAdviceType::TMP_FILE);
        operation_advice._offset = 0;
        operation_advice._size = 1024;
        operation_advice._interface_used = mimir::InterfaceType::POSIX;
        operation_advice._priority=10;
        mimir::operation_advice_begin(operation_advice, operation_handler);
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