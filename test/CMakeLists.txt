
find_package(Catch2 REQUIRED)
#find_package(MPI REQUIRED COMPONENTS CXX)
#message(STATUS "found mpi.h at ${MPI_CXX_LIBRARIES}")
#include_directories(${MPI_CXX_INCLUDE_DIRS})
set(TEST_LIBS Catch2::Catch2 -lstdc++fs ${MPI_CXX_LIBRARIES}  -rdynamic)
#set(TEST_LIBS ${TEST_LIBS} ${MPI_CXX_LIBRARIES})
set(TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/catch_config.h ${CMAKE_CURRENT_SOURCE_DIR}/test_utils.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(posix)
add_subdirectory(stdio)
add_subdirectory(paper)
add_subdirectory(advice)

