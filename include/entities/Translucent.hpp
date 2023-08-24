#ifndef AGPTRACER_ENTITIES_TRANSLUCENT_HPP
#define AGPTRACER_ENTITIES_TRANSLUCENT_HPP

#include <concepts>

namespace AGPTracer { namespace Entities {
    /**
     * @brief The Translucent interface describes an object through which light can pass.
     *
     * @tparam D Translucent type
     * @tparam T Floating point datatype
     */
    template<template<typename> typename D, typename T>
    concept Translucent = requires(const D<T> a) {
        { a.ind_ } -> std::convertible_to<T>;
    }
    &&requires(const D<T> a) {
        { a.priority_ } -> std::convertible_to<unsigned int>;
    };
}}

#endif
