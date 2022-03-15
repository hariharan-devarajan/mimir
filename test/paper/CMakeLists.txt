set(exec_prefix paper)

set(PFS $ENV{pfs}/temp/mimir/pfs)
set(SHM /dev/shm/mimir)


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

set(MPI_PROCS_PER_NODE 40)
set(MPI_NUM_NODES 4)
math(EXPR MPI_PROCS ${MPI_PROCS_PER_NODE}*${MPI_NUM_NODES})
set(MPI_HOSTFILE ${CMAKE_BINARY_DIR}/test/paper/hostfile_${MPI_PROCS_PER_NODE}_${MPI_NUM_NODES})
set(MPI_ARGS -n ${MPI_PROCS} --hostfile ${MPI_HOSTFILE} -x CONFIG_PATH -x SHM_PATH -x PFS_PATH -x MPI_PROCS_PER_NODE)

add_test(NAME ${exec_prefix}_hostfile COMMAND "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${MPI_PROCS_PER_NODE} [test=hostfile])


set(test_name optimization)
set(iterations 1 10 100 1000 10000 100000)
foreach (iteration ${iterations})
    add_test(NAME ${exec_prefix}_${test_name}_${iteration}_ COMMAND ${MPIEXEC_EXECUTABLE} ${MPI_ARGS}  "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_ APPEND PROPERTY ENVIRONMENT SHM_PATH=${SHM})
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_ APPEND PROPERTY ENVIRONMENT PFS_PATH=${PFS})
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_ APPEND PROPERTY ENVIRONMENT MPI_PROCS_PER_NODE=${MPI_PROCS_PER_NODE})

    add_test(NAME ${exec_prefix}_${test_name}_${iteration}_athena COMMAND ${MPIEXEC_EXECUTABLE} ${MPI_ARGS} "${CMAKE_BINARY_DIR}/test/paper/${exec_prefix}_mimir_test" --durations yes --reporter compact --num_operations ${iteration} [test=${test_name}])
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libathena.so)
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT SHM_PATH=${SHM})
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT PFS_PATH=${PFS})
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT CONFIG_PATH=${CMAKE_CURRENT_SOURCE_DIR}/conf.json)
    set_property(TEST ${exec_prefix}_${test_name}_${iteration}_athena APPEND PROPERTY ENVIRONMENT MPI_PROCS_PER_NODE=${MPI_PROCS_PER_NODE})
endforeach ()