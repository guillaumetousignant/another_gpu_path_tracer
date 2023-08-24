#ifndef AGPTRACER_MEDIUMS_NONABSORBER_T_HPP
#define AGPTRACER_MEDIUMS_NONABSORBER_T_HPP

#include "entities/Ray_t.hpp"

namespace AGPTracer { namespace Mediums {

    /**
     * @brief The non absorber class describes a medium that doesn't interact with light.
     *
     * This medium does nothing by itself, it is the identity medium.
     * This medium can be used to represent non interacting media like clear air or emptiness.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double>
    class NonAbsorber_t {
        public:
            /**
             * @brief Construct a new NonAbsorber_t object.
             *
             * @param ind Refractive index of the medium.
             * @param priority Priority of the medium over other mediums, used to determine which is the current medium when overlapping. Higher value means higher priority.
             */
            NonAbsorber_t(T ind, unsigned int priority);

            T ind_; /**< @brief Refractive index of the medium. Used to calculate a ray's change of direction when going from a medium to another, and reflection ratio of refractive surfaces.*/
            unsigned int priority_; /**< @brief Priority of the medium over other mediums. A ray's current medium is the one with the highest priority, and intersections with lower priority mediums
                                       are ignored when there is an overlap.*/

            /**
             * @brief Defines the interaction between a ray and the medium.
             *
             * This medium doesn't interact with light, and as such a ray is not changed here.
             *
             * @tparam N Number of mediums in the ray's medium list
             * @param ray Ray modified by the medium. Its colour and mask can be changed.
             * @return true Returns true if the ray has been scattered, meaning that its origin and/or direction has changed and the material bounce should not be performed. Never the case for a non
             * absorber.
             * @return false Returns false when the ray's path has not been changed, and it should bounce on the intersected material as planned. Always the case for a non absorber.
             */
            template<size_t N>
            auto scatter(Entities::Ray_t<T, N>& ray) -> bool;
    };
}}

#include "mediums/NonAbsorber_t.tpp"

#endif
