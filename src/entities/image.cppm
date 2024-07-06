module;
#include <concepts>
#include <filesystem>
#include <vector>

export module image;

import resettable;
import vec3;

namespace AGPTracer::Entities {
    /**
     * @brief The ImageUpdatable interface describes an object that can be updated as in image.
     *
     * @tparam I ImageUpdatable type
     * @tparam T Floating point datatype
     */
    export template<template<typename> typename I, typename T>
    concept ImageUpdatable = requires(I<T> a) {
        { a.update() } -> std::convertible_to<void>;
    }
    &&requires(I<T> a, const std::vector<Vec3<T>>& img) {
        { a.update(img) } -> std::convertible_to<void>;
    }
    &&requires(I<T> a, const Vec3<T>& colour, size_t pos_x, size_t pos_y) {
        { a.update(colour, pos_x, pos_y) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Settable interface describes an object that can be set as in image.
     *
     * @tparam I Settable type
     * @tparam T Floating point datatype
     */
    export template<template<typename> typename I, typename T>
    concept Settable = requires(I<T> a, const std::vector<Vec3<T>>& img) {
        { a.set(img) } -> std::convertible_to<void>;
    }
    &&requires(I<T> a, const Vec3<T>& colour, size_t pos_x, size_t pos_y) {
        { a.set(colour, pos_x, pos_y) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Writeable interface describes an object that can be written as in image.
     *
     * @tparam I Writeable type
     * @tparam T Floating point datatype
     */
    export template<template<typename> typename I, typename T>
    concept Writeable = requires(I<T> a, const std::filesystem::path& filename) {
        { a.write(filename) } -> std::convertible_to<void>;
    }
    &&requires(I<T> a, const std::filesystem::path& filename, T gammaind) {
        { a.write(filename, gammaind) } -> std::convertible_to<void>;
    };

    /**
     * @brief The Shaped2D interface describes an object that has a shape in x and y.
     *
     * @tparam I Shaped2D type
     */
    export template<typename I>
    concept Shaped2D = requires(const I a) {
        { a.size_x_ } -> std::convertible_to<size_t>;
    }
    &&requires(const I a) {
        { a.size_y_ } -> std::convertible_to<size_t>;
    };

    /**
     * @brief The Image interface represents an image, which can be modified, can accumulate samples, and can be written to disk.
     *
     * This is the class that stores the results of the intersections. Generally, cameras shoot rays through a scene, and afterwards
     * add that ray's contribution to the image.
     *
     * @tparam I Image type
     * @tparam T Floating point datatype to use
     */
    export template<template<typename> typename I, typename T>
    concept Image = Resetable<I<T>> && ImageUpdatable<I, T> && Settable<I, T> && Writeable<I, T> && Shaped2D<I<T>>;
}
