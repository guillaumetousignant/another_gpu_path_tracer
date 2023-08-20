#ifndef AGPTRACER_ENTITIES_RANDOMGENERATOR_T_HPP
#define AGPTRACER_ENTITIES_RANDOMGENERATOR_T_HPP

#include <CL/sycl.hpp>
#include <random>

namespace AGPTracer { namespace Entities {

    /**
     * @brief Holds the random generators used by the workers for each pixel
     *
     * @tparam T Random generator type
     */
    template<typename T = std::mt19937>
    class RandomGenerator_t {
        public:
            /**
             * @brief Construct a new RandomGenerator_t object with the given dimensions.
             *
             * @param size_x Horizontal number of pixels in the image.
             * @param size_y Vertical number of pixels in the image.
             */
            RandomGenerator_t(size_t size_x, size_t size_y);

            size_t size_x_; /**< @brief Horizontal number of pixels in the image.*/
            size_t size_y_; /**< @brief Vertical number of pixels in the image.*/
            cl::sycl::buffer<T, 2> rng_vec_; /**< @brief Vector of shapes to be drawn.*/
    };
}}

#include "entities/RandomGenerator_t.tpp"

#endif
