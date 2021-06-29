set(COMMON_LINKER_LIBS "")

# gtest
#find_package(GTest REQUIRED)
#list(APPEND COMMON_LINKER_LIBS ${GTEST_LIBRARIES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest)
list(APPEND COMMON_LINKER_LIBS gtest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/googletest/include)

# add threads
find_package(Threads REQUIRED)
list(APPEND COMMON_LINKER_LIBS ${CMAKE_THREAD_LIBS_INIT})

# add glog
include("cmake/External/glog.cmake")
include_directories(SYSTEM ${GLOG_INCLUDE_DIRS})
list(APPEND COMMON_LINKER_LIBS ${GLOG_LIBRARIES})

# add gflags
include("cmake/External/gflags.cmake")
include_directories(SYSTEM ${GFLAGS_INCLUDE_DIRS})
list(APPEND COMMON_LINKER_LIBS ${GFLAGS_LIBRARIES})

#add torch
find_package(Torch REQUIRED)
include_directories(SYSTEM ${TORCH_INCLUDE_DIRS})
list(APPEND COMMON_LINKER_LIBS ${TORCH_LIBRARIES})
