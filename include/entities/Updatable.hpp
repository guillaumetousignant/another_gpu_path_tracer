#ifndef AGPTRACER_ENTITIES_UPDATABLE_HPP
#define AGPTRACER_ENTITIES_UPDATABLE_HPP

#include <concepts>

namespace AGPTracer::Entities {
    /**
     * @brief The Updatable interface describes an object that can be updated.
     *
     * @tparam T Updatable type
     */
    template<typename T>
    concept Updatable = requires(T a) {
        { a.update() } -> std::convertible_to<void>;
    };
}

#endif
