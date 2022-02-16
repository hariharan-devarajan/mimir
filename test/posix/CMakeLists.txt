set(examples posix_gcc_test)

function(gcc_base example)
    add_executable(${example} ${example}.cpp ${TEST_SRC})
    target_link_libraries(${example} ${TEST_LIBS})
endfunction()

function(gcc_athena example)
    add_executable(${example}_athena ${example}.cpp ${TEST_SRC})
    add_dependencies(${example}_athena ${PROJECT_NAME} athena)
    target_link_libraries(${example}_athena ${PROJECT_NAME} athena -ldl ${TEST_LIBS})
endfunction()


foreach (example ${examples})
    gcc_base(${example})
    gcc_athena(${example})
endforeach ()