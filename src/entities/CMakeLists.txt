target_sources(AGPTracer
    PUBLIC
    FILE_SET cxx_modules
    BASE_DIRS ${CMAKE_SOURCE_DIR}/src
    TYPE CXX_MODULES
    FILES
        camera.cppm
        image.cppm
        material.cppm
        medium.cppm
        medium_list.cppm
        mesh_geometry.cppm
        random_generator.cppm
        random_number_generator.cppm
        ray.cppm
        resettable.cppm
        scene.cppm
        shape.cppm
        skybox.cppm
        texture.cppm
        transformation_matrix.cppm
        translucent.cppm
        uniform_distribution.cppm
        updatable.cppm
        vec3.cppm
)
