#ifndef AGPTRACER_MATERIALS_DIFFUSE_T_HPP
#define AGPTRACER_MATERIALS_DIFFUSE_T_HPP

#include "entities/Ray_t.hpp"
#include "entities/Shape.hpp"
#include "entities/Vec3.hpp"
#include <random>

namespace AGPTracer::Materials {

    /**
     * @brief The diffuse class describes a material that reflects rays in random directions, to model diffuse reflection.
     *
     * This material has an emissive and reflective colour, applied to rays on bounce.
     * The rays are reflected in a random direction in the hemisphere over the hit point.
     * This models diffuse reflection, which reflect light in all directions.
     * This material represents uniformly and finely rough surfaces, such as matte paint,
     * chalk, and non-shiny plastic.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double>
    class Diffuse_t {
        public:
            /**
             * @brief Construct a new Diffuse_t object with an emissive and reflective colour, and a roughness.
             *
             * @param emission Colour emitted by the material when a ray bounces on it.
             * @param colour Colour reflected by the material when a ray bounces on it.
             * @param roughness Attenuation of the colour as the angle between the incident ray and the surface normal increase. 0 to 1, 0 being very rough (no attenuation) and 1 being an ideal
             * diffusely reflective surface, obeying Lambert's cosine law.
             */
            Diffuse_t(AGPTracer::Entities::Vec3<T> emission, AGPTracer::Entities::Vec3<T> colour, T roughness);

            AGPTracer::Entities::Vec3<T> emission_; /**< @brief Colour emitted by the material at each bounce.*/
            AGPTracer::Entities::Vec3<T> colour_; /**< @brief Colour reflected by the material at each bounce.*/
            T roughness_; /**< @brief Attenuation of the colour as the angle between the incident ray and the surface normal increase. 0 to 1, 0 being very rough (no attenuation) and 1 being an
                                  ideal diffusely reflective surface, obeying Lambert's cosine law.*/

            /**
             * @brief Bounces a ray of light on the material.
             *
             * The ray's mask is attenuated with the material's colour to model part of the light being absorbed.
             * Then, the material's emissive colour is multiplied with the ray's mask and added to the ray's colour
             * to model light being emitted by the material.
             * The ray's origin is set to the hit point, and its direction is randomly selected within the hemisphere
             * above the hit point to model diffuse reflection.
             *
             * @tparam R Random generator type to use
             * @tparam U Random distribution type to use
             * @tparam S Shape that was intersected
             * @tparam N Number of mediums in the ray's medium list
             * @param rng Random generator used to get random numbers
             * @param unif Uniform distribution used to get random numbers
             * @param uv Object space coordinates of the hit point. Used to query the shape for values at coordinates on it. Two components, u, and v, that can change meaning depending on the shape.
             * @param hit_obj Pointer to the shape that was hit by the ray.
             * @param ray Ray that has intersected the shape.
             */
            template<class R, template<typename> typename U, template<typename> typename S, size_t N>
            requires Entities::Shape<S, T> auto bounce(R& rng, U<T>& unif, std::array<T, 2> uv, const S<T>& hit_obj, AGPTracer::Entities::Ray_t<T, N>& ray) const -> void;
    };
}

#include "materials/Diffuse_t.tpp"

#endif
