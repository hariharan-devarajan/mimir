cmake_minimum_required(VERSION 3.10)

project(mimir)# VERSION 1.0.0 DESCRIPTION "Mimir: Extending POSIX interface for expressing User intent in scientific workflows.")
#------------------------------------------------------------------------------
# Version information
#------------------------------------------------------------------------------
set(MIMIR_VERSION_MAJOR "0")
set(MIMIR_VERSION_MINOR "0")
set(MIMIR_VERSION_PATCH "1")
set(MIMIR_PACKAGE ${PROJECT_NAME})
set(MIMIR_PACKAGE_NAME ${PROJECT_NAME})
set(MIMIR_PACKAGE_VERSION "${MIMIR_VERSION_MAJOR}.${MIMIR_VERSION_MINOR}.${MIMIR_VERSION_PATCH}")
set(MIMIR_PACKAGE_VERSION_MAJOR "${MIMIR_VERSION_MAJOR}.${MIMIR_VERSION_MINOR}")
set(MIMIR_PACKAGE_VERSION_MINOR "${MIMIR_VERSION_PATCH}")
set(MIMIR_PACKAGE_STRING "${MIMIR_PACKAGE_NAME} ${MIMIR_PACKAGE_VERSION}")
set(MIMIR_PACKAGE_TARNAME "${MIMIR_PACKAGE}")
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_definitions(-w)
option(ENABLE_ATHENA "Build Athena" OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -rdynamic")
set(MIMIR_PRIVATE_INCLUDE)
set(MIMIR_PUBLIC_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/mimir.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/no_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/advice_type.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/application_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/dataset_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/execution_engine_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/file_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/hlio_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/io_phase_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/job_configuration_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/sample_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/workflow_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/api/posix.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/api/workflow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/api/application.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/common/data_structure.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/common/enumeration.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/common/typedef.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/constant.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/mimir.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/typedef.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/macro.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/common/error_code.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/middleware_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/file_operation_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/posix_file_operation_advice.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/api/job_configuration.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/advice_handler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/api/mimir_interceptor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/common/debug.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mimir/advice/config.h)
set(MIMIR_PUBLIC_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/api/posix.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/api/application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/api/workflow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/api/job_configuration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/advice/advice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/common/debug.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimir/api/mimir_interceptor.cpp)

add_library(${PROJECT_NAME} SHARED)# ${MIMIR_LIBRARY_SRC})


target_sources(${PROJECT_NAME}
        PRIVATE
        ${MIMIR_PUBLIC_SRC}
        )
target_include_directories(${PROJECT_NAME}
        PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

#------------------------------------------------------------------------------
# Setup install and output Directories
#------------------------------------------------------------------------------
if (NOT MIMIR_INSTALL_BIN_DIR)
    set(MIMIR_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
    message(STATUS ${MIMIR_INSTALL_BIN_DIR})
endif ()
if (NOT MIMIR_INSTALL_LIB_DIR)
    set(MIMIR_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
if (NOT MIMIR_INSTALL_INCLUDE_DIR)
    set(MIMIR_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
endif ()
if (NOT MIMIR_INSTALL_DATA_DIR)
    set(MIMIR_INSTALL_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share)
endif ()

if (NOT CMAKE_INSTALL_RPATH)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()

#------------------------------------------------------------------------------
if (NOT MIMIR_EXTERNALLY_CONFIGURED)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
            )

    set(CMAKE_INCLUDE_OUTPUT_DIRECTORY
            ${CMAKE_BINARY_DIR}/include CACHE PATH "Store the headers."
            )
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
            ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single Directory for all Libraries"
            )
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
            ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single Directory for all static libraries."
            )
endif ()

#------------------------------------------------------------------------------
# Disallow in-source build
#------------------------------------------------------------------------------
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR
            "${PROJECT_NAME} requires an out of source Build. "
            "Please create a separate binary directory and run CMake there.")
endif ()

#------------------------------------------------------------------------------
# Set a default build type if none was specified
#------------------------------------------------------------------------------
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
            "MinSizeRel" "RelWithDebInfo")
endif ()

if (NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif ()

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects.
#-----------------------------------------------------------------------------
if (NOT MIMIR_EXPORTED_TARGETS)
    set(MIMIR_EXPORTED_TARGETS "cpp-logger-targets")
endif ()

set(MIMIR_LIBTYPE SHARED)
set_target_properties(${libtarget}
        PROPERTIES
        DEBUG_OUTPUT_NAME ${LIB_DEBUG_NAME}
        RELEASE_OUTPUT_NAME ${LIB_RELEASE_NAME}
        MINSIZEREL_OUTPUT_NAME ${LIB_RELEASE_NAME}
        RELWITHDEBINFO_OUTPUT_NAME ${LIB_RELEASE_NAME}
        VERSION ${LIB_VERSION}
        SOVERSION ${LIB_VERSION}
        )

include(GNUInstallDirs)
# the variant with PUBLIC_HEADER property unfortunately does not preserve the folder structure
#set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
# so instead we iterate through public headers and install them "manually"
foreach (header ${MIMIR_PUBLIC_INCLUDE})
    file(RELATIVE_PATH header_file_path "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}" "${header}")
    get_filename_component(header_directory_path "${header_file_path}" DIRECTORY)
    install(
            FILES ${header}
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${header_directory_path}"
    )
    file(COPY ${header}
            DESTINATION "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${header_directory_path}")
endforeach ()
#-----------------------------------------------------------------------------
# Configure the config.cmake file for the build directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/CMake/${PROJECT_NAME}-config.cmake.build.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake @ONLY
)
install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake
        DESTINATION
        ${MIMIR_INSTALL_LIB_DIR}/cmake/${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#-----------------------------------------------------------------------------
# Configure the ${PROJECT_NAME}-config-version .cmake file for the install directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/CMake/${PROJECT_NAME}-config-version.cmake.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake @ONLY
)

install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake
        DESTINATION
        ${MIMIR_INSTALL_LIB_DIR}/cmake/${PROJECT_NAME}
)

install(
        TARGETS
        ${PROJECT_NAME}
        EXPORT
        ${MIMIR_EXPORTED_TARGETS}
        LIBRARY DESTINATION ${MIMIR_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${MIMIR_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${MIMIR_INSTALL_BIN_DIR}
)
export(EXPORT ${MIMIR_EXPORTED_TARGETS}
        FILE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake")
install(EXPORT
        ${MIMIR_EXPORTED_TARGETS}
        DESTINATION
        ${MIMIR_INSTALL_LIB_DIR}/cmake/${PROJECT_NAME}
        FILE
        ${MIMIR_EXPORTED_TARGETS}.cmake
        )
if (ENABLE_ATHENA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHCL_ENABLE_RPCLIB")
    message("HCL_ENABLE_RPCLIB: ${HCL_ENABLE_RPCLIB}")


    set(ATHENA_LIBRARY_SRC src/athena/mimir_adapter/api/posix.cpp
            include/athena/api/posix.h src/athena/api/mpi.cpp include/athena/api/mpi.h src/athena/server/athena_server.cpp src/athena/server/athena_server.h src/athena/client/athena_client.h src/athena/server/posix_io.cpp src/athena/server/posix_io.h src/athena/api/interceptor.cpp src/athena/mimir_adapter/api/stdio.cpp include/athena/api/stdio.h src/athena/client/posix_athena_client.cpp src/athena/client/posix_athena_client.h)

    add_library(athena SHARED ${ATHENA_LIBRARY_SRC})
    add_subdirectory(external/hcl)
    include_directories(external/hcl/include)
endif ()

find_package(cpp-logger REQUIRED)
if (${CPP_LOGGER_FOUND})
    message(STATUS "[CPP_LOGGER] found cpp-logger at ${CPP_LOGGER_INCLUDE_DIRS}")
    include_directories(${CPP_LOGGER_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${CPP_LOGGER_LIBRARIES})
else ()
    message(FATAL_ERROR "-- [CPP_LOGGER] is needed for ${PROJECT_NAME} build")
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


option(MIMIR_BUILD_TEST "Build the unit tests" OFF)
if (MIMIR_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()