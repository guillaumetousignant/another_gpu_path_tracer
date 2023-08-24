add_library(AGPTracer INTERFACE)

target_include_directories(AGPTracer INTERFACE 
    $<INSTALL_INTERFACE:include/another_gpu_path_tracer/>    
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)

option(USE_PNG "Use the PNG library to be able to read/write PNG images." ON)
if(USE_PNG)
    find_package(ZLIB REQUIRED)
    find_package(PNG REQUIRED)
    target_link_libraries(AGPTracer INTERFACE
        PNG::PNG)
    target_compile_definitions(AGPTracer INTERFACE AGPTRACER_USE_PNG)
    set(ZLIB_DEPENDENCY "find_dependency(ZLIB REQUIRED)")
    set(PNG_DEPENDENCY "find_dependency(PNG REQUIRED)")
endif()

option(USE_JPEG "Use the jpeg library to be able to read/write jpeg images." ON)
if(USE_JPEG)
    find_package(JPEG REQUIRED)
    target_link_libraries(AGPTracer INTERFACE
        JPEG::JPEG)
    target_compile_definitions(AGPTracer INTERFACE AGPTRACER_USE_JPEG)
    set(JPEG_DEPENDENCY "find_dependency(JPEG REQUIRED)")
endif()

option(USE_TIFF "Use the tiff library to be able to read/write tiff images." ON)
if(USE_TIFF)
    find_package(TIFF REQUIRED)
    target_link_libraries(AGPTracer INTERFACE
        TIFF::TIFF)
    target_compile_definitions(AGPTracer INTERFACE AGPTRACER_USE_TIFF)
    set(TIFF_DEPENDENCY "find_dependency(TIFF REQUIRED)")
endif()

target_compile_features(AGPTracer INTERFACE cxx_std_20)
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
        target_compile_definitions(AGPTracer INTERFACE NOMINMAX)
        target_compile_definitions(AGPTracer INTERFACE -D_USE_MATH_DEFINES)
    endif()
    add_sycl_to_target(TARGET another_gpu_path_tracer SOURCES main.cpp)
endif()

if(USE_clangSYCL)
    if (BUILD_CUDA)
        target_compile_options(AGPTracer INTERFACE -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
        target_link_options(AGPTracer INTERFACE -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
    else()
        target_compile_options(AGPTracer INTERFACE -fsycl)
        target_link_options(AGPTracer INTERFACE -fsycl)
    endif()
endif()
