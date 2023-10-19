#ifndef AGPTRACER_SKYBOXES_SKYBOXFLAT_T_HPP
#define AGPTRACER_SKYBOXES_SKYBOXFLAT_T_HPP

#include "entities/Vec3.hpp"

namespace AGPTracer::Skyboxes {
    /**
     * @brief The skybox flat class describes a flat coloured skybox.
     *
     * The colour returned is independent of direction, and is 'background_'.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double>
    class SkyboxFlat_t {
        public:
            /**
             * @brief Construct a new SkyboxFlat_t object of the provided colour.
             *
             * @param background Colour of the skybox.
             */
            explicit SkyboxFlat_t(const AGPTracer::Entities::Vec3<T>& background);

            AGPTracer::Entities::Vec3<T> background_; /**< @brief Colour of the skybox, in all directions.*/

            auto get(const AGPTracer::Entities::Vec3<T>& xyz) const -> AGPTracer::Entities::Vec3<T>;
    };
}

#include "skyboxes/SkyboxFlat_t.tpp"

#endif
