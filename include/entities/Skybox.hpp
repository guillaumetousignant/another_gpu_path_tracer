#ifndef AGPTRACER_ENTITIES_SKYBOX_HPP
#define AGPTRACER_ENTITIES_SKYBOX_HPP

#include "entities/Vec3.hpp"
#include <concepts>

namespace AGPTracer::Entities {
    /**
     * @brief Skyboxes represent the infinitely far background of a scene. Define how the ray gets coloured when it doesn't intersect geometry.
     *
     * Skyboxes return the colour of the background based on ray direction only. Using only the direction places the skybox infinitely far.
     * The colour returned is used as an emissive colour, and thus lights up the scene as a colour source.
     *
     * @tparam K Skybox type
     * @tparam T Floating point datatype to use
     */
    template<template<typename> typename K, typename T>
    concept Skybox = requires(const K<T> a, const Vec3<T>& v) {
        { a.get(v) } -> std::convertible_to<Vec3<T>>;
    };
}

#endif
