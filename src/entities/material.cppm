module;
#include <array>
#include <concepts>
#include <random>

export module material;

import ray;
import triangle;

namespace AGPTracer::Entities {
    /**
     * @brief The material interface describes how light interacts with a specific material.
     *
     * This is an interface that define the core function a material must implement.
     * A material modified a ray of light when it bounces on it. It can change the ray's
     * direction and origin, to model the light path change from the intersection. It can
     * also change the ray's colour and mask to model light absorption and emission.
     *
     * @tparam M Material type
     * @tparam T Floating point datatype to use
     */
    export template<template<typename> typename M, typename T>
    concept Material = requires(const M<T> a, std::mt19937& rng, std::uniform_real_distribution<T>& unif, std::array<T, 2> uv, const Shapes::Triangle_t<T>& hit_obj, Ray_t<T, 16>& ray) {
        { a.bounce(rng, unif, uv, hit_obj, ray) } -> std::convertible_to<void>;
    };
}
