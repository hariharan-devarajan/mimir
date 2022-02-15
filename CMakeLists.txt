cmake_minimum_required(VERSION 3.10)

project(mimir VERSION 1.0.0 DESCRIPTION "Mimir: Extending POSIX interface for expressing User intent in scientific workflows.")
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)
include_directories(src)
include_directories(include)

set(MIMIR_LIBRARY_SRC include/mimir/mimir.h
        include/mimir/advice/advice.h
        include/mimir/advice/advice_type.h
        include/mimir/advice/application_advice.h
        include/mimir/advice/dataset_advice.h
        include/mimir/advice/execution_engine_advice.h
        include/mimir/advice/file_advice.h
        include/mimir/advice/hlio_advice.h
        include/mimir/advice/io_phase_advice.h
        include/mimir/advice/job_configuration_advice.h
        include/mimir/advice/node_local_advice.h
        include/mimir/advice/sample_advice.h
        include/mimir/advice/shared_storage_advice.h
        include/mimir/advice/workflow_advice.h
        include/mimir/api/posix.h
        include/mimir/common/data_structure.h
        include/mimir/common/enumeration.h
        include/mimir/common/typedef.h
        include/mimir/common.h
        include/mimir/constant.h
        include/mimir/mimir.h
        include/mimir/typedef.h
        src/mimir/api/posix.cpp
        src/mimir/core/advice.h
        src/mimir/core/advice.cpp
        src/mimir/log/logger.h
        src/mimir/log/logger.cpp
        src/mimir/core/advice_handler.h
        src/mimir/core/advice_handler.cpp)

add_library(${PROJECT_NAME} SHARED ${MIMIR_LIBRARY_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER include/mimir.h)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (MIMIR_TIMER)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MIMIR_TIMER)
endif ()

if (MIMIR_TRACE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MIMIR_TRACE)
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

configure_file(mimir.pc.in mimir.pc @ONLY)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

option(BUILD_TEST "Build the unit tests" ON)
if (BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()