set(exec_prefix advice)

set(MOUNT_PATH_0 /dev/shm/haridev/mimir)
set(MOUNT_SIZE_0 262144)
set(MOUNT_PATH_1 $ENV{pfs}/temp/mimir/pfs)
set(MOUNT_SIZE_1 1048576)


set(example advice_generator_test)
set(case ${exec_prefix}_${example})
add_executable(${case} ${example}.cpp ${TEST_SRC})
target_link_libraries(${case} ${TEST_LIBS} mimir -rdynamic -std=c++11 -lyaml-cpp)

set(test_name job)
add_test(NAME ${exec_prefix}_${test_name} COMMAND "${CMAKE_BINARY_DIR}/test/advice/${exec_prefix}_advice_generator_test" --durations yes --reporter compact [advice=${test_name}])
set_property(TEST ${exec_prefix}_${test_name} APPEND PROPERTY ENVIRONMENT MOUNT_PATH_0=${MOUNT_PATH_0})
set_property(TEST ${exec_prefix}_${test_name} APPEND PROPERTY ENVIRONMENT MOUNT_SIZE_0=${MOUNT_SIZE_0})
set_property(TEST ${exec_prefix}_${test_name} APPEND PROPERTY ENVIRONMENT MOUNT_PATH_1=${MOUNT_PATH_1})
set_property(TEST ${exec_prefix}_${test_name} APPEND PROPERTY ENVIRONMENT MOUNT_SIZE_1=${MOUNT_SIZE_1})
