module;
#include <random>

export module non_absorber_medium;

import ray;

/**
 * @brief Contains different medium types that can be used.
 *
 * Mediums describe how light interacts when being inside a medium. They can change the
 * ray's colour and mask relatively to the distance traveled through the medium. They
 * can also intersect the ray themselves, changing the ray's colour and mask, and its
 * origin and direction. In that case, it reports that it has intersected the ray so
 * the shape intersection can be ignored.
 */
namespace AGPTracer::Mediums {
    /**
     * @brief The non absorber class describes a medium that doesn't interact with light.
     *
     * This medium does nothing by itself, it is the identity medium.
     * This medium can be used to represent non interacting media like clear air or emptiness.
     *
     * @tparam T Floating point datatype to use
     */
    export template<typename T = double>
    class NonAbsorberMedium_t {
        public:
            /**
             * @brief Construct a new NonAbsorberMedium_t object.
             *
             * @param ind Refractive index of the medium.
             * @param priority Priority of the medium over other mediums, used to determine which is the current medium when overlapping. Higher value means higher priority.
             */
            NonAbsorberMedium_t(T ind, unsigned int priority) : ind_(ind), priority_(priority) {}

            T ind_; /**< @brief Refractive index of the medium. Used to calculate a ray's change of direction when going from a medium to another, and reflection ratio of refractive surfaces.*/
            unsigned int priority_; /**< @brief Priority of the medium over other mediums. A ray's current medium is the one with the highest priority, and intersections with lower priority mediums
                                       are ignored when there is an overlap.*/

            /**
             * @brief Defines the interaction between a ray and the medium.
             *
             * This medium doesn't interact with light, and as such a ray is not changed here.
             *
             * @tparam R Random generator type to use
             * @tparam U Random distribution type to use
             * @tparam N Number of mediums in the ray's medium list
             * @param rng Random generator used to get random numbers
             * @param unif Uniform distribution used to get random numbers
             * @param ray Ray modified by the medium. Its colour and mask can be changed.
             * @return true Returns true if the ray has been scattered, meaning that its origin and/or direction has changed and the material bounce should not be performed. Never the case for a non
             * absorber.
             * @return false Returns false when the ray's path has not been changed, and it should bounce on the intersected material as planned. Always the case for a non absorber.
             */
            template<class R, template<typename> typename U, size_t N>
            auto scatter(R& rng, U<T>& unif, Entities::Ray_t<T, N>& ray) const -> bool {
                return false;
            }
    };
}
