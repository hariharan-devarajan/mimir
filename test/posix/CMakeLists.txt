set(PFS /home/haridev/pfs)
set(SHM /dev/shm)
set(examples posix_gcc_test)

function(gcc_base example)
    add_executable(${example} ${example}.cpp ${TEST_SRC})
    target_link_libraries(${example} ${TEST_LIBS})
endfunction()

function(gcc_athena example)
    add_executable(${example}_athena ${example}.cpp ${TEST_SRC})
    add_dependencies(${example}_athena ${PROJECT_NAME} athena)
    target_link_libraries(${example}_athena ${PROJECT_NAME} athena -ldl ${TEST_LIBS})
    set_target_properties(${example}_athena PROPERTIES COMPILE_FLAGS "-DATHENA_PRELOAD=1")


endfunction()


foreach (example ${examples})
    gcc_base(${example} "" "")
    add_test(NAME test_${example} COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_1K COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --request_size 1024 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_16K COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --request_size 16384 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_128K COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --request_size 131072 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_1M COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --request_size 1048576 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_16M COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --request_size 16777216 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})

    gcc_athena(${example} "" "")
    add_test(NAME test_${example}_athena_default COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_athena_1K COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --request_size 1024 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_athena_16K COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --request_size 16384 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_athena_128K COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --request_size 131072 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_athena_1M COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --request_size 1048576 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})
    add_test(NAME test_${example}_athena_16M COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --request_size 16777216 --durations yes --reporter compact --pfs ${PFS} --shm ${SHM})

endforeach ()