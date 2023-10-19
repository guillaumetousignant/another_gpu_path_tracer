#ifndef AGPTRACER_ENTITIES_RAY_T_HPP
#define AGPTRACER_ENTITIES_RAY_T_HPP

#include "entities/MediumList_t.hpp"
#include "entities/Vec3.hpp"

namespace AGPTracer::Entities {
    /**
     * @brief The ray class contains colour data and can be intersected with geometry.
     *
     * This is the main class of the program. It describes a ray with a position and a direction.
     * It also has colour data. It can be intersected with shapes or any intersectable object.
     * It holds a list a mediums to figure out in what material it is.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double, size_t N = 16>
    class Ray_t {
        public:
            /**
             * @brief Construct a new Ray_t object.
             *
             * @param[in] origin Initial origin of the ray.
             * @param[in] direction Initial direction of the ray.
             * @param[in] colour Initial colour of the ray. Usually [0 0 0] and is increased by hitting emissive objects.
             * @param[in] mask Initial part of the ray not yet absorbed. Is multiplied with the contribution of a light source to increment colour. Usually [1 1 1] and is decreased by being absorbed
             * by materials.
             * @param[in] medium_list Initial list of materials through which the ray is travelling. Should have at least two copies of an "outside" medium not assigned to any object (issue #25).
             */
            constexpr Ray_t(const Vec3<T>& origin, const Vec3<T>& direction, const Vec3<T>& colour, const Vec3<T>& mask, MediumList_t<N> medium_list) :
                    origin_(origin), direction_(direction), colour_(colour), mask_(mask), dist_(0), medium_list_(std::move(medium_list)), time_(1){};

            /**
             * @brief Construct a new Ray_t object with a  time.
             *
             * @param[in] origin Initial origin of the ray.
             * @param[in] direction Initial direction of the ray.
             * @param[in] colour Initial colour of the ray. Usually [0 0 0] and is increased by hitting emissive objects.
             * @param[in] mask Initial part of the ray not yet absorbed. Is multiplied with the contribution of a light source to increment colour. Usually [1 1 1] and is decreased by being absorbed
             * by materials.
             * @param[in] medium_list Initial list of materials through which the ray is travelling. Should have at least two copies of an "outside" medium not assigned to any object (issue #25).
             * @param[in] time Time at which the ray is emitted. From 0 for exposure start to 1 for exposure end.
             */
            constexpr Ray_t(const Vec3<T>& origin, const Vec3<T>& direction, const Vec3<T>& colour, const Vec3<T>& mask, MediumList_t<N> medium_list, T time) :
                    origin_(origin), direction_(direction), colour_(colour), mask_(mask), dist_(0), medium_list_(std::move(medium_list)), time_(time){};

            Vec3<T> origin_; /**< @brief Origin of the ray. Changed by materials on bounce.*/
            Vec3<T> direction_; /**< @brief Direction of the ray. Changed by materials on bounce.*/
            Vec3<T> colour_; /**< @brief Colour accumulated by the ray. Changed by emissive materials. Starts at [0 0 0].*/
            Vec3<T> mask_; /**< @brief Part of the ray not yet absorbed by materials. Multiplies the emission of materials to set colour. Starts at [1 1 1], the colour can't be changed once the
                                   mask reaches 0.*/
            T dist_; /**< @brief Distance traveled by the ray since last bounce.*/
            MediumList_t<N> medium_list_; /**< @brief List of materials in which the ray travels. The first one is the current one.*/
            T time_; /**< @brief Time of emission of the ray, relative to exposure time. 0 for start of exposure to 1 for end.*/
    };
}

#endif
