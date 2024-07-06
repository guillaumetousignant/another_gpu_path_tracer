module;

export module flat_skybox;

import vec3;

/**
 * @brief Contains different skybox types that can be used.
 *
 * Skyboxes describe how the light get coloured when they miss geometry. It is the
 * background of scenes. They output the background colour depending on a ray's
 * direction. Several types exist, some taking their colour from a texture, a
 * flat colour, light sources, or a combination of these.
 */
namespace AGPTracer::Skyboxes {
    /**
     * @brief The skybox flat class describes a flat coloured skybox.
     *
     * The colour returned is independent of direction, and is 'background_'.
     *
     * @tparam T Floating point datatype to use
     */
    export template<typename T = double>
    class SkyboxFlat_t {
        public:
            /**
             * @brief Construct a new SkyboxFlat_t object of the provided colour.
             *
             * @param background Colour of the skybox.
             */
            explicit SkyboxFlat_t(const AGPTracer::Entities::Vec3<T>& background) : background_(background) {}

            AGPTracer::Entities::Vec3<T> background_; /**< @brief Colour of the skybox, in all directions.*/

            auto get(const AGPTracer::Entities::Vec3<T>& xyz) const -> AGPTracer::Entities::Vec3<T> {
                return background_;
            }
    };
}
