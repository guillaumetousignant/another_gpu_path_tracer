#ifndef AGPTRACER_ENTITIES_RESETABLE_HPP
#define AGPTRACER_ENTITIES_RESETABLE_HPP

#include <concepts>

namespace AGPTracer::Entities {
    /**
     * @brief The Resetable interface describes an object that can be reset.
     *
     * @tparam T Resetable type
     */
    template<typename T>
    concept Resetable = requires(T a) {
        { a.reset() } -> std::convertible_to<void>;
    };
}

#endif
