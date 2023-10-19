#ifndef AGPTRACER_ENTITIES_RANDOMNUMBERGENERATOR_T_HPP
#define AGPTRACER_ENTITIES_RANDOMNUMBERGENERATOR_T_HPP

#include <limits>

namespace AGPTracer::Entities {
    /**
     * @brief The random number generator class represent an engine to generate random numbers.
     *
     * @tparam T Integral point type
     */
    template<typename T = std::size_t>
    class RandomNumberGenerator_t {
        public:
            /**
             * @brief Constructs a new RandomNumberGenerator_t object.
             */
            constexpr RandomNumberGenerator_t(T seed) : state_{seed} {};

            /**
             * @brief Generates a random number.
             *
             * @return T Generated random number
             */
            constexpr auto operator()() -> T {
                return state_++;
            };

            constexpr static auto max() -> T {
                return std::numeric_limits<T>::max();
            };

            constexpr static auto min() -> T {
                return std::numeric_limits<T>::lowest();
            };

        private:
            T state_; /**< @brief Internal state of the generator.*/
    };
}

#endif
