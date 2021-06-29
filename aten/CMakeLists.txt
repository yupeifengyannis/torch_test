set(ATEN_TEST_LINKER_LIBS ${COMMON_LINKER_LIBS})
list(APPEND ATEN_TEST_LINKER_LIBS torch_cpu)
file(GLOB_RECURSE test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)
add_executable(atentest.bin ${test_srcs} gtest_main.cpp)
target_link_libraries(atentest.bin ${ATEN_TEST_LINKER_LIBS})

