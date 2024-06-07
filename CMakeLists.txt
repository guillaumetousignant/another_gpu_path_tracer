cmake_minimum_required(VERSION 3.18)
# 3.20 enables removing the boost new version warning
# 3.18 because that's the version on my raspberry pi

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

option(BUILD_CUDA "Build CUDA version of the targets." ON)
option(USE_AdaptiveCpp "Use the AdaptiveCpp backend." OFF)
option(USE_clangSYCL "Use the Intel clang SYCL backend." OFF)
option(USE_IPO "Use interprocedural optimization." OFF)

project(SYCL_TEST VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(src)

option(BUILD_DOC "Build documentation." OFF)
if (BUILD_DOC)
    add_subdirectory(docs)
endif()

option(BUILD_TESTING "Build all tests." OFF)
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
