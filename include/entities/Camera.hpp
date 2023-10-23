#ifndef AGPTRACER_ENTITIES_CAMERA_HPP
#define AGPTRACER_ENTITIES_CAMERA_HPP

#include "entities/Image.hpp"
#include "entities/RandomGenerator_t.hpp"
#include "entities/Resetable.hpp"
#include "entities/Scene_t.hpp"
#include "entities/Skybox.hpp"
#include "entities/Updatable.hpp"
#include "shapes/Triangle_t.hpp"
#include <array>
#include <concepts>
#include <filesystem>
#include <random>


namespace AGPTracer::Entities {
    /**
     * @brief The Raytrace interface describes an object that can cast rays into a scene.
     *
     * @tparam C Raytrace type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename C, typename T>
    concept Raytrace = requires(C<T> a, sycl::queue& queue, RandomGenerator_t<T, std::mt19937>& rng, std::uniform_real_distribution<T>& unif, Scene_t<T, Shapes::Triangle_t<T>>& scene) {
        { a.raytrace(queue, rng, unif, scene) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Accumulate interface describes an object that can accumulate samples in image.
     *
     * @tparam C Accumulate type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename C, typename T>
    concept Accumulate =
        requires(C<T> a, sycl::queue& queue, RandomGenerator_t<T, std::mt19937>& rng, std::uniform_real_distribution<T>& unif, const Scene_t<T, Shapes::Triangle_t<T>>& scene, unsigned int n_iter) {
        { a.accumulate(queue, rng, unif, scene, n_iter) } -> std::convertible_to<void>;
    }
    &&requires(C<T> a, sycl::queue& queue, RandomGenerator_t<T, std::mt19937>& rng, std::uniform_real_distribution<T>& unif, const Scene_t<T, Shapes::Triangle_t<T>>& scene) {
        { a.accumulate(queue, rng, unif, scene) } -> std::convertible_to<void>;
    }
    &&requires(C<T> a,
               sycl::queue& queue,
               RandomGenerator_t<T, std::mt19937>& rng,
               std::uniform_real_distribution<T>& unif,
               const Scene_t<T, Shapes::Triangle_t<T>>& scene,
               unsigned int n_iter,
               unsigned int interval) {
        { a.accumulateWrite(queue, rng, unif, scene, n_iter, interval) } -> std::convertible_to<void>;
    }
    &&requires(C<T> a, sycl::queue& queue, RandomGenerator_t<T, std::mt19937>& rng, std::uniform_real_distribution<T>& unif, const Scene_t<T, Shapes::Triangle_t<T>>& scene, unsigned int interval) {
        { a.accumulateWrite(queue, rng, unif, scene, interval) } -> std::convertible_to<void>;
    }
    &&requires(C<T> a, sycl::queue& queue, RandomGenerator_t<T, std::mt19937>& rng, std::uniform_real_distribution<T>& unif, const Scene_t<T, Shapes::Triangle_t<T>>& scene) {
        { a.accumulateWrite(queue, rng, unif, scene) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Focus interface describes an object that can be focussed on a point.
     *
     * @tparam C Focus type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename C, typename T>
    concept Focus = requires(C<T> a, T focus_distance) {
        { a.focus(focus_distance) } -> std::convertible_to<void>;
    }
    &&requires(C<T> a, sycl::queue& queue, const Scene_t<T, Shapes::Triangle_t<T>>& scene, std::array<T, 2> position) {
        { a.autoFocus(queue, scene, position) } -> std::convertible_to<void>;
    };

    /**
     * @brief The SetUp interface describes an object that can cast rays into a scene.
     *
     * @tparam C SetUp type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename C, typename T>
    concept SetUp = requires(C<T> a, Vec3<T> new_up) {
        { a.setUp(new_up) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Zoom interface describes an object that can cast rays into a scene.
     *
     * @tparam C Zoom type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename C, typename T>
    concept Zoom = requires(C<T> a, T factor) {
        { a.zoom(factor) } -> std::convertible_to<void>;
    }
    &&requires(C<T> a, std::array<T, 2> fov) {
        { a.zoom(fov) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Write interface describes an object that can write its result to disk.
     *
     * @tparam C Write type
     * @tparam T Floating point datatype
     */
    template<typename C>
    concept Write = requires(C a) {
        { a.write() } -> std::convertible_to<void>;
    }
    &&requires(C a, const std::filesystem::path& filename) {
        { a.write(filename) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Camera interface creates rays and casts them into the scene and stores the result in an image buffer.
     *
     * This is an interface that defines what all cameras must implement. It should handle
     * the creation of rays according to a projection, and cast them in a scene. It should then
     * store the colour of the ray in an image buffer. The interface has no image buffer, as
     * the needed image buffer(s) vary by configuration, or are even not needed for some specific
     * situations.
     *
     * @tparam C Camera type
     * @tparam T Floating point datatype to use
     */
    template<template<typename> typename C, typename T>
    concept Camera = Raytrace<C, T> && Accumulate<C, T> && Focus<C, T> && SetUp<C, T> && Zoom<C, T> && Write<C<T>> && Resetable<C<T>>;
}

#endif
