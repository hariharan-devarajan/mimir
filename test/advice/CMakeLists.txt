set(exec_prefix advice)

set(PFS $ENV{pfs}/temp/mimir/pfs)
set(SHM /dev/shm/haridev/mimir)


set(example advice_generator_test)
set(case ${exec_prefix}_${example})
add_executable(${case} ${example}.cpp ${TEST_SRC})
target_link_libraries(${case} ${TEST_LIBS} mimir -rdynamic -std=c++11 -lyaml-cpp)

set(test_name job)
add_test(NAME ${exec_prefix}_${test_name} COMMAND "${CMAKE_BINARY_DIR}/test/advice/${exec_prefix}_advice_generator_test" --durations yes --reporter compact [advice=${test_name}])
set_property(TEST ${exec_prefix}_${test_name} APPEND PROPERTY ENVIRONMENT SHM_PATH=${SHM})
set_property(TEST ${exec_prefix}_${test_name} APPEND PROPERTY ENVIRONMENT PFS_PATH=${PFS})
