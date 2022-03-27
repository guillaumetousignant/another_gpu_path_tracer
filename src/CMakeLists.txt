add_library(AGPTracer "")

add_subdirectory(entities)

target_include_directories(AGPTracer PUBLIC 
    $<INSTALL_INTERFACE:include/SYCL-APP/>    
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)

if(USE_hipSYCL)
    if (BUILD_CUDA)
        if(NOT DEFINED HIPSYCL_TARGETS)
            set(HIPSYCL_TARGETS "cuda:sm_61")
        endif()
    endif()
    find_package(hipSYCL REQUIRED)
    add_sycl_to_target(TARGET AGPTracer)
endif()

if(USE_clangSYCL)
    target_link_directories(AGPTracer PUBLIC $ENV{SYCL_LINK_DIR})
    target_link_libraries(AGPTracer PUBLIC sycl)

    if (BUILD_CUDA)
        target_compile_options(AGPTracer PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
        target_link_options(AGPTracer PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
    else()
        target_compile_options(AGPTracer PUBLIC -fsycl)
        target_link_options(AGPTracer PUBLIC -fsycl)
    endif()
endif()

target_compile_features(AGPTracer PRIVATE cxx_std_20)
if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET AGPTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()

add_executable(another_gpu_path_tracer 
    main.cpp)

target_link_libraries(another_gpu_path_tracer PRIVATE 
                                                AGPTracer)

target_compile_features(another_gpu_path_tracer PRIVATE cxx_std_20)
if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET another_gpu_path_tracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()
