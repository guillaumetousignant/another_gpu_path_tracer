add_library(AGPTracer INTERFACE)

add_subdirectory(cameras)
add_subdirectory(entities)
add_subdirectory(images)
add_subdirectory(materials)
add_subdirectory(mediums)
add_subdirectory(shapes)
add_subdirectory(skyboxes)

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

option(OPTIMIZE_FOR_NATIVE "Build with -march=native." OFF)
if(OPTIMIZE_FOR_NATIVE)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)

    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        target_compile_options(AGPTracer PUBLIC -march=native)
    endif()
endif()

target_compile_features(AGPTracer INTERFACE cxx_std_20)
if(USE_IPO AND NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET AGPTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()

include(GNUInstallDirs)

install(TARGETS AGPTracer
    EXPORT another_gpu_path_tracer-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET cxx_modules
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/another_gpu_path_tracer)

if(WIN32)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILES_MATCHING PATTERN "*.dll")
endif()

install(EXPORT another_gpu_path_tracer-targets
    FILE
    AGPTracerTargets.cmake
    NAMESPACE
    AGPTracer::
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/AGPTracer)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/AGPTracerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/AGPTracerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AGPTracerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AGPTracer
)

# Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AGPTracerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/AGPTracerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AGPTracer
)

export(EXPORT another_gpu_path_tracer-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/AGPTracerTargets.cmake
    NAMESPACE AGPTracer::)

# Register package in user's package registry
export(PACKAGE AGPTracer)

add_executable(another_gpu_path_tracer 
    main.cpp)

target_link_libraries(another_gpu_path_tracer PRIVATE 
                                                AGPTracer)

target_compile_features(another_gpu_path_tracer PRIVATE cxx_std_20)

if(USE_IPO AND NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET another_gpu_path_tracer PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()

if(OPTIMIZE_FOR_NATIVE)
    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        target_compile_options(another_gpu_path_tracer PUBLIC -march=native)
    endif()
endif()

install(TARGETS another_gpu_path_tracer DESTINATION ${CMAKE_INSTALL_BINDIR})

if(USE_AdaptiveCpp)
    # ACPP_TARGETS should be "generic" for single-pass compiler, "omp" for explicit openmp, "cuda:sm_61" for GTX 1070, "cuda:sm_75" for T200

    find_package(AdaptiveCpp REQUIRED)
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
