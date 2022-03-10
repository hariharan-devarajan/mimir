set(exec_prefix paper)

set(PFS $ENV{pfs}/temp/mimir/pfs)
set(SHM /dev/shm/haridev/mimir)


set(example mimir_test)
set(case ${exec_prefix}_${example})
add_executable(${case} ${example}.cpp ${TEST_SRC})
target_link_libraries(${case} ${TEST_LIBS} mimir -rdynamic)

set(test_name anatomy)
set(iteration 1000000)
add_test(NAME ${exec_prefix}_${test_name}_${iteration} COMMAND "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])

set(test_name performance)
set(iteration 10000)
set(ranks 160 320 640 1280)
foreach (rank ${ranks})
    add_test(NAME ${exec_prefix}_${test_name}_${iteration}_${rank} COMMAND ${CMAKE_PREFIX_PATH}/bin/mpirun -n ${rank} "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])
endforeach ()

set(test_name footprint)
set(iteration 1000000)
add_test(NAME ${exec_prefix}_${test_name}_${iteration} COMMAND "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])
set_property(TEST ${exec_prefix}_${test_name}_${iteration} APPEND PROPERTY ENVIRONMENT SHM_PATH=${SHM})
set_property(TEST ${exec_prefix}_${test_name}_${iteration} APPEND PROPERTY ENVIRONMENT PFS_PATH=${PFS})

set(test_name optimization)
set(iterations 1 10 100 1000 10000 100000)
foreach (iteration ${iterations})
    add_test(NAME ${exec_prefix}_${test_name}_${iteration}_ COMMAND ${MPIEXEC_EXECUTABLE} -n 4 "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_ APPEND PROPERTY ENVIRONMENT SHM_PATH=${SHM})
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_ APPEND PROPERTY ENVIRONMENT PFS_PATH=${PFS})

    add_test(NAME ${exec_prefix}_${test_name}_${iteration}_athena COMMAND ${MPIEXEC_EXECUTABLE} -n 4 "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libathena.so)
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT SHM_PATH=${SHM})
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT PFS_PATH=${PFS})
endforeach ()