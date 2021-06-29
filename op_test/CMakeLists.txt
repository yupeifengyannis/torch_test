set(OP_TEST_LINKER_LIBS ${COMMON_LINKER_LIBS})
file(GLOB_RECURSE test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)
add_executable(op_test.bin ${test_srcs} gtest_main.cpp)
target_link_libraries(op_test.bin ${OP_TEST_LINKER_LIBS})
