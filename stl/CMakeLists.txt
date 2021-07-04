include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE STL_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
add_library(stl SHARED ${STL_SRCS})
list(APPEND COMMON_LINKER_LIBS stl)
