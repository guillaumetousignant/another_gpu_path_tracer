include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

add_executable(unit_tests 
    example_test.cpp)
target_link_libraries(unit_tests PRIVATE 
    AGPTracer
    Catch2::Catch2WithMain)
target_compile_features(unit_tests PRIVATE cxx_std_20)

catch_discover_tests(unit_tests)
