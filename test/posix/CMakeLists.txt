set(examples posix_gcc_test)

function(gcc_base example)
    add_executable(${example} ${example}.cpp ${TEST_SRC})
    target_link_libraries(${example} ${TEST_LIBS})
    add_test(NAME Test${example} COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}" --durations yes --reporter compact)
endfunction()

function(gcc_athena example)
    add_executable(${example}_athena ${example}.cpp ${TEST_SRC})
    add_dependencies(${example}_athena ${PROJECT_NAME} athena)
    target_link_libraries(${example}_athena ${PROJECT_NAME} athena -ldl ${TEST_LIBS})
    set_target_properties(${example}_athena PROPERTIES COMPILE_FLAGS "-DATHENA_PRELOAD=1")

    add_test(NAME test_${example}_athena COMMAND "${CMAKE_BINARY_DIR}/test/posix/${example}_athena" --durations yes --reporter compact)
endfunction()


foreach (example ${examples})
    gcc_base(${example})
    gcc_athena(${example})
endforeach ()