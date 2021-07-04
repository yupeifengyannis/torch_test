set(CXX_TEST_LINKER_LIBS ${COMMON_LINKER_LIBS})
file(GLOB_RECURSE test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)
add_executable(test.bin ${test_srcs} gtest_main.cpp)
target_link_libraries(test.bin ${CXX_TEST_LINKER_LIBS})

