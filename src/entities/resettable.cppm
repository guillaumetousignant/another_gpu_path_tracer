module;
#include <concepts>

export module resettable

namespace AGPTracer::Entities {
    /**
     * @brief The Resettable interface describes an object that can be reset.
     *
     * @tparam T Resettable type
     */
    export template<typename T>
    concept Resettable = requires(T a) {
        { a.reset() } -> std::convertible_to<void>;
    };
}
