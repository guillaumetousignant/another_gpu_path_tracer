module;
#include <concepts>

export module updatable;

namespace AGPTracer::Entities {
    /**
     * @brief The Updatable interface describes an object that can be updated.
     *
     * @tparam T Updatable type
     */
    export template<typename T>
    concept Updatable = requires(T a) {
        { a.update() } -> std::convertible_to<void>;
    };
}
