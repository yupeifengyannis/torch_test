set(C10_TEST_LINKER_LIBS ${COMMON_LINKER_LIBS})
include_directories(${CMAKE_CURRENT_DIR})
file(GLOB_RECURSE test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)
add_executable(c10test.bin ${test_srcs} gtest_main.cpp)
target_link_libraries(c10test.bin ${C10_TEST_LINKER_LIBS})

