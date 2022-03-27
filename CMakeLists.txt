cmake_minimum_required(VERSION 3.20)
# 3.20 enables removing the boost new version warning

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

option(BUILD_CUDA "Build CUDA version of the targets." ON)
option(USE_hipSYCL "Use the hipSYCL backend." OFF)
option(USE_clangSYCL "Use the Intel clang SYCL backend." ON)
option(USE_IPO "Use interprocedural optimization." OFF)

project(SYCL_TEST VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(src)
