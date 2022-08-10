cmake_minimum_required(VERSION 3.10)

project(mimir VERSION 1.0.0 DESCRIPTION "Mimir: Extending POSIX interface for expressing User intent in scientific workflows.")
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_definitions(-w)
option(ENABLE_ATHENA "Build Athena" OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -rdynamic")
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
        include/mimir/advice/sample_advice.h
        include/mimir/advice/workflow_advice.h
        include/mimir/api/posix.h
        src/mimir/api/posix.cpp
        include/mimir/common/data_structure.h
        include/mimir/common/enumeration.h
        include/mimir/common/typedef.h
        include/mimir/common.h
        include/mimir/constant.h
        include/mimir/mimir.h
        include/mimir/typedef.h
        include/mimir/log/logger.h
        src/mimir/log/logger.cpp
        include/mimir/common/error_code.h
        include/mimir/advice/middleware_advice.h
        include/mimir/advice/file_operation_advice.h
        include/mimir/advice/posix_file_operation_advice.h
        src/mimir/api/job_configuration.cpp
        include/mimir/api/job_configuration.h
        src/mimir/advice/advice.cpp
        src/mimir/advice/advice_handler.h
        include/mimir/api/mimir_interceptor.h src/mimir/api/mimir_interceptor.cpp include/mimir/common/debug.h src/mimir/common/debug.cpp include/mimir/advice/config.h)

add_library(${PROJECT_NAME} SHARED ${MIMIR_LIBRARY_SRC})
if (ENABLE_ATHENA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHCL_ENABLE_RPCLIB")
    message("HCL_ENABLE_RPCLIB: ${HCL_ENABLE_RPCLIB}")


    set(ATHENA_LIBRARY_SRC src/athena/mimir_adapter/api/posix.cpp
            include/athena/api/posix.h src/athena/api/mpi.cpp include/athena/api/mpi.h src/athena/server/athena_server.cpp src/athena/server/athena_server.h src/athena/client/athena_client.h src/athena/server/posix_io.cpp src/athena/server/posix_io.h src/athena/api/interceptor.cpp src/athena/mimir_adapter/api/stdio.cpp include/athena/api/stdio.h src/athena/client/posix_athena_client.cpp src/athena/client/posix_athena_client.h)

    add_library(athena SHARED ${ATHENA_LIBRARY_SRC})
    add_subdirectory(external/hcl)
    include_directories(external/hcl/include)
endif ()
find_package(nlohmann_json 3.10.5 REQUIRED)

if (nlohmann_json_FOUND)
    message(STATUS "found nlohmann_json")
    target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
endif ()
find_package(MPI REQUIRED COMPONENTS CXX)
if (MPI_FOUND)
    message(STATUS "found mpi.h at ${MPI_CXX_INCLUDE_DIRS}")
    include_directories(${MPI_CXX_INCLUDE_DIRS})
endif ()
if (ENABLE_ATHENA)
    find_package(rpclib REQUIRED)
    if (rpclib_FOUND)
        message(STATUS "found rpclib ${rpclib_ROOT_DIR}")
    endif ()
    find_package(Boost 1.69 REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${CMAKE_PREFIX_PATH}/include)
    target_link_libraries(athena ${PROJECT_NAME} -lstdc++fs -ldl -fPIC -rdynamic rpclib::rpc ${MPI_CXX_LIBRARIES} hcl ${Boost_LIBRARIES})
    target_compile_definitions(athena PUBLIC ATHENA_PRELOAD)
endif ()
find_package(Threads)
target_link_libraries(${PROJECT_NAME} -ldl ${MPI_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER include/mimir.h)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (MIMIR_TIMER)
    message(STATUS "MIMIR_TIMER ${MIMIR_TIMER}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE MIMIR_TIMER)
    target_compile_definitions(athena PRIVATE MIMIR_TIMER)
endif ()

if (MIMIR_TRACE)
    message(STATUS "MIMIR_TRACE ${MIMIR_TRACE}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE MIMIR_TRACE)
    target_compile_definitions(athena PRIVATE MIMIR_TRACE)
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