#ifndef AGPTRACER_ENTITIES_SHAPE_HPP
#define AGPTRACER_ENTITIES_SHAPE_HPP

#include "entities/Ray_t.hpp"
#include "entities/TransformMatrix_t.hpp"
#include "entities/Updatable.hpp"
#include "entities/Vec3.hpp"
#include <array>
#include <concepts>

namespace AGPTracer::Entities {
    /**
     * @brief The Intersectable interface describes an object that can be intersected by a ray.
     *
     * @tparam S Intersectable type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename S, typename T>
    concept Intersectable = requires(const S<T> a, const Entities::Ray_t<T, 16>& ray, T& t, std::array<T, 2>& uv) {
        { a.intersection(ray, t, uv) } -> std::convertible_to<bool>;
    };

    /**
     * @brief The Geometric interface describes an object that can be queried for normal, texture coordinates, and tangent at a specific point.
     *
     * @tparam S Geometric type
     * @tparam T Floating point datatype
     * @tparam T2 Time datatype
     */
    template<template<typename> typename S, typename T, typename T2>
    concept Geometric = requires(const S<T> a, T2 time, std::array<T, 2> uv) {
        { a.normal(time, uv) } -> std::convertible_to<Vec3<T>>;
    }
    &&requires(const S<T> a, T2 time, std::array<T, 2> uv, std::array<T, 2>& tuv) {
        { a.normal_uv(time, uv, tuv) } -> std::convertible_to<Vec3<T>>;
    }
    &&requires(const S<T> a, T2 time, std::array<T, 2> uv, std::array<T, 2>& tuv, Vec3<T>& tangentvec) {
        { a.normal_uv_tangent(time, uv, tuv, tangentvec) } -> std::convertible_to<Vec3<T>>;
    };

    /**
     * @brief The Coordinates interface describes an object that can be queried for minimum and maximum coordinates in space.
     *
     * @tparam S Coordinates type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename S, typename T>
    concept Coordinates = requires(const S<T> a) {
        { a.mincoord() } -> std::convertible_to<Vec3<T>>;
    }
    &&requires(const S<T> a) {
        { a.maxcoord() } -> std::convertible_to<Vec3<T>>;
    };

    /**
     * @brief The Transformable interface describes an object that can be transformed via a transformation matrix.
     *
     * @tparam S Updatable type
     */
    template<template<typename> typename S, typename T>
    concept Transformable = requires(const S<T> a) {
        { a.transformation_ } -> std::convertible_to<TransformMatrix_t<T>>;
    };

    /**
     * @brief The HasMaterial interface describes an object that has a material.
     *
     * @tparam S HasMaterial type
     */
    template<typename S>
    concept HasMaterial = requires(const S a) {
        { a.material_ } -> std::convertible_to<size_t>;
    };

    /**
     * @brief The shape interface class describes surfaces which can be intersected by a ray.
     *
     * Shapes are surfaces that can be intersected by rays and shaded. Rays bounce on them according to their material, and they can provide
     * information about an intersection, like the hit point, object coordinates hit point, surface normal, etc. Shapes are stored in a scene and
     * in an acceleration structure, and as such must provide the mincoord and maxcoord functions to be spatially sorted. The shapes can be modified
     * by applying transformations to their transformation matrix. These changes are propagated on update.
     *
     * @tparam S Shape type
     * @tparam T Floating point datatype to use
     */
    template<template<typename> typename S, typename T>
    concept Shape = Updatable<S<T>> && Intersectable<S, T> && Geometric<S, T, T> && Coordinates<S, T> && Transformable<S, T> && HasMaterial<S<T>>;
}

#endif
