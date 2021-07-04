cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

project(peifeng_test)
set(CMAKE_CXX_STANDARD 14)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

find_package(Torch REQUIRED)
include_directories(${TORCH_INCLUDE_DIRS})
# project build abi must equal to torch's abi
set(CMAKE_CXX_FLAGS ${TORCH_CXX_FLAGS})

include(cmake/Dependencies.cmake)

add_subdirectory(${CMAKE_SOURCE_DIR}/stl)

add_subdirectory(${CMAKE_SOURCE_DIR}/c10)
add_subdirectory(${CMAKE_SOURCE_DIR}/aten)
add_subdirectory(${CMAKE_SOURCE_DIR}/c++)
add_subdirectory(${CMAKE_SOURCE_DIR}/example/register)
add_subdirectory(${CMAKE_SOURCE_DIR}/op_test)

include(cmake/Summary.cmake)
peifeng_print_configuration_summary()
