#ifndef AGPTRACER_ENTITIES_MEDIUM_HPP
#define AGPTRACER_ENTITIES_MEDIUM_HPP

#include "entities/Ray_t.hpp"
#include "entities/Translucent.hpp"
#include <concepts>
#include <random>

namespace AGPTracer::Entities {
    /**
     * @brief The Scattering interface describes an object that can scatter a ray.
     *
     * @tparam D Scattering type
     */
    template<template<typename> typename D, typename T>
    concept Scattering = requires(const D<T> a, std::mt19937& rng, std::uniform_real_distribution<T>& unif, Ray_t<T, 16>& ray) {
        { a.scatter(rng, unif, ray) } -> std::convertible_to<bool>;
    };

    /**
     * @brief The medium interface describes a medium in which a ray of light can travel, like water, glass or air.
     *
     * A medium is like a volumetric material. It can modify a ray of light depending on the distance the ray has passed
     * through it, contrary to a material which modifies a ray in a punctual manner. It has a refractive index to calculate
     * how the light changes direction when passing from a medium to another, and the reflection ratio of refractive surfaces.
     * It has a priority to permit the use of overlapping mediums. A material which passes a ray through a surface, such as
     * the surface of a glass ball, should add a medium to a ray's medium list to indicate that it is now travelling through it.
     * A ray can have multiple mediums in its list, the one with the highest priority is the current medium. Intersections with
     * lower priority mediums are ignored.
     *
     * @tparam D Medium type
     * @tparam T Floating point datatype to use
     */
    template<template<typename> typename D, typename T>
    concept Medium = Scattering<D, T> && Translucent<D, T>;
}

#endif
