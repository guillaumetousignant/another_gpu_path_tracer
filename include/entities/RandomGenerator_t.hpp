#ifndef AGPTRACER_ENTITIES_RANDOMGENERATOR_T_HPP
#define AGPTRACER_ENTITIES_RANDOMGENERATOR_T_HPP

#include <random>
#include <sycl/sycl.hpp>

namespace AGPTracer { namespace Entities {

    /**
     * @brief Holds the random generators used by the workers for each pixel
     *
     * @tparam R Random generator type
     */
    template<typename R = std::mt19937>
    class RandomGenerator_t {
        public:
            /**
             * @brief Construct a new RandomGenerator_t object with the given dimensions.
             *
             * @param size_x Horizontal number of pixels in the image.
             * @param size_y Vertical number of pixels in the image.
             */
            RandomGenerator_t(size_t size_x, size_t size_y) : size_x_(size_x), size_y_(size_y), rng_vec_(sycl::range<2>{size_x, size_y}) {
                std::random_device rdev;
                const sycl::host_accessor<R, 2, sycl::access_mode::write> host_accessor(rng_vec_, sycl::no_init);

                auto range = rng_vec_.get_range();

                for (size_t i = 0; i < rng_vec_.get_range()[0]; ++i) {
                    for (size_t j = 0; j < rng_vec_.get_range()[1]; ++j) {
                        host_accessor[sycl::id<2>{i, j}] = R(rdev());
                    }
                }
            };

            size_t size_x_; /**< @brief Horizontal number of pixels in the image.*/
            size_t size_y_; /**< @brief Vertical number of pixels in the image.*/
            sycl::buffer<R, 2> rng_vec_; /**< @brief Vector of shapes to be drawn.*/
    };
}}

#endif
