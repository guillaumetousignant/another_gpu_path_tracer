module;
#include <random>
#include <cmath>
#include <numbers>

export module diffuse_material;

import ray;
#include "entities/Ray_t.hpp"
#include "entities/Shape.hpp"
#include "entities/Vec3.hpp"
#include "entities/RandomGenerator_t.hpp"

/**
 * @brief Contains different material types that can be used.
 *
 * Materials describe how shapes interact with light when they are intersected by a ray.
 * They can change the ray's colour and mask values, and the ray's origin and direction.
 */
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
    export template<typename T = double>
    class DiffuseMaterial_t {
        public:
            /**
             * @brief Construct a new DiffuseMaterial_t object with an emissive and reflective colour, and a roughness.
             *
             * @param emission Colour emitted by the material when a ray bounces on it.
             * @param colour Colour reflected by the material when a ray bounces on it.
             * @param roughness Attenuation of the colour as the angle between the incident ray and the surface normal increase. 0 to 1, 0 being very rough (no attenuation) and 1 being an ideal
             * diffusely reflective surface, obeying Lambert's cosine law.
             */
            DiffuseMaterial_t(AGPTracer::Entities::Vec3<T> emission, AGPTracer::Entities::Vec3<T> colour, T roughness) : emission_(emission), colour_(colour), roughness_(roughness) {}

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
            requires Entities::Shape<S, T> auto bounce(R& rng, U<T>& unif, std::array<T, 2> uv, const S<T>& hit_obj, AGPTracer::Entities::Ray_t<T, N>& ray) const -> void {
                AGPTracer::Entities::Vec3<T> normal = hit_obj.normal(ray.time_, uv);

                const T rand1  = unif(rng) * T{2} * std::numbers::pi_v<T>;
                const T rand2  = unif(rng);
                const T rand2s = sycl::sqrt(rand2);

                if (normal.dot(ray.direction_) > T{0}) {
                    normal = -normal;
                }

                const AGPTracer::Entities::Vec3<T> axis = std::abs(normal[0]) > T{0.1} ? AGPTracer::Entities::Vec3<T>(T{0}, T{1}, T{0}) : AGPTracer::Entities::Vec3<T>(T{1}, T{0}, T{0});

                const AGPTracer::Entities::Vec3<T> u = axis.cross(normal).normalize_inplace();
                const AGPTracer::Entities::Vec3<T> v = normal.cross(u).normalize_inplace(); // wasn't normalized before

                const AGPTracer::Entities::Vec3<T> newdir = (u * sycl::cos(rand1) * rand2s + v * sycl::sin(rand1) * rand2s + normal * sycl::sqrt(T{1} - rand2)).normalize_inplace();

                ray.origin_ += ray.direction_ * ray.dist_ + normal * T{0.00001}; // *ray.dist_; // Made EPSILON relative, check // well guess what wasn't a good idea
                ray.direction_ = newdir;

                ray.colour_ += ray.mask_ * emission_;
                ray.mask_ *= colour_ * sycl::pow(newdir.dot(normal), roughness_);
            }
    };
}
