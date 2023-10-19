#ifndef AGPTRACER_ENTITIES_UNIFORMDISTRIBUTION_T_HPP
#define AGPTRACER_ENTITIES_UNIFORMDISTRIBUTION_T_HPP

#include <random>

namespace AGPTracer::Entities {
    /**
     * @brief The uniform distribution class represent an uniform distribution of random numbers.
     *
     * @tparam T Floating point type
     */
    template<typename T = double>
    class UniformDistribution_t {
        public:
            /**
             * @brief Constructs a new UniformDistribution_t object.
             */
            constexpr UniformDistribution_t(T min, T max) : min_{min}, span_{max - min} {};

            /**
             * @brief Generates a random number according to the distribution, using the provided random number generator.
             *
             * @tparam R Random generator type
             * @param rng Random generator to use
             * @return T Generated random number
             */
            template<class R>
            auto operator()(R& rng) -> T {
                const auto number = rng();
                return (T(number - R::min()) / T(R::max() - R::min())) * span_ + min_;
            };

        private:
            T min_; /**< @brief Minimum number to generate.*/
            T span_; /**< @brief Maximum - minimum.*/
    };
}

#endif
