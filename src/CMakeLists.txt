add_library(SYCL-TEST-LIBRARY "")

add_subdirectory(entities)

target_include_directories(SYCL-TEST-LIBRARY PUBLIC 
    $<INSTALL_INTERFACE:include/SYCL-APP/>    
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)

target_link_directories(SYCL-TEST-LIBRARY PUBLIC $ENV{SYCL_LINK_DIR})
target_link_libraries(SYCL-TEST-LIBRARY PUBLIC sycl)

if (BUILD_CUDA)
    target_compile_options(SYCL-TEST-LIBRARY PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
    target_link_options(SYCL-TEST-LIBRARY PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
else()
    target_compile_options(SYCL-TEST-LIBRARY PUBLIC -fsycl)
    target_link_options(SYCL-TEST-LIBRARY PUBLIC -fsycl)
endif()

target_compile_features(SYCL-TEST-LIBRARY PRIVATE cxx_std_20)
if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET SYCL-TEST-LIBRARY PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()

add_executable(SYCL-APP 
    main.cpp)

target_link_directories(SYCL-APP PRIVATE $ENV{SYCL_LINK_DIR})
target_link_libraries(SYCL-APP PRIVATE 
                                SYCL-TEST-LIBRARY
                                sycl)

if (BUILD_CUDA)
    target_compile_options(SYCL-APP PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
    target_link_options(SYCL-APP PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
else()
    target_compile_options(SYCL-APP PRIVATE -fsycl)
    target_link_options(SYCL-APP PRIVATE -fsycl)
endif()

target_compile_features(SYCL-APP PRIVATE cxx_std_20)
if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    set_property(TARGET SYCL-APP PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()
