add_library(AGPTracer "")

add_subdirectory(entities)
add_subdirectory(shapes)

target_include_directories(AGPTracer PUBLIC 
    $<INSTALL_INTERFACE:include/another_gpu_path_tracer/>    
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)

target_compile_features(AGPTracer PRIVATE cxx_std_20)
if(USE_IPO AND NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET AGPTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()

add_executable(another_gpu_path_tracer 
    main.cpp)

target_link_libraries(another_gpu_path_tracer PRIVATE 
                                                AGPTracer)

target_compile_features(another_gpu_path_tracer PRIVATE cxx_std_20)
if(USE_IPO AND NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET another_gpu_path_tracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()

if(USE_OpenSYCL)
    if(NOT DEFINED OPENSYCL_TARGETS) # "cuda:sm_61" for GTX 1070, "cuda:sm_75" for T200
        if (BUILD_CUDA)
            set(OPENSYCL_TARGETS "cuda:sm_61")
        else()
            set(OPENSYCL_TARGETS "omp")
        endif()
    endif()

    find_package(OpenSYCL REQUIRED)
    if(WIN32)
        target_compile_definitions(AGPTracer PUBLIC NOMINMAX)
        target_compile_definitions(AGPTracer PUBLIC -D_USE_MATH_DEFINES)
    endif()
    add_sycl_to_target(TARGET AGPTracer SOURCES shapes/Triangle_t.cpp entities/MeshGeometry_t.cpp entities/Ray_t.cpp entities/TransformMatrix_t.cpp)
    add_sycl_to_target(TARGET another_gpu_path_tracer SOURCES main.cpp)
endif()

if(USE_clangSYCL)
    if (BUILD_CUDA)
        target_compile_options(AGPTracer PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
        target_link_options(AGPTracer PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
    else()
        target_compile_options(AGPTracer PUBLIC -fsycl)
        target_link_options(AGPTracer PUBLIC -fsycl)
    endif()
endif()
