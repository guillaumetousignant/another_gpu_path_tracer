target_sources(AGPTracer
    PUBLIC
    FILE_SET cxx_modules
    BASE_DIRS ${CMAKE_SOURCE_DIR}/src
    TYPE CXX_MODULES
    FILES
        spherical_camera.cppm
)
