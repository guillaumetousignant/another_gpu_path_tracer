#ifndef AGPTRACER_ENTITIES_RANDOMGENERATOR_T_HPP
#define AGPTRACER_ENTITIES_RANDOMGENERATOR_T_HPP

#include <random>
#include <sycl/sycl.hpp>

namespace AGPTracer { namespace Entities {

    /**
     * @brief Holds the random generators used by the workers for each pixel
     *
     * @tparam T Floating point type
     * @tparam R Random generator type
     * @tparam U Random distribution type to use
     */
    template<typename T, typename R = std::mt19937, template<typename> typename U = std::uniform_real_distribution>
    class RandomGenerator_t {
        public:
            class Accessor_t {
                public:
                    /**
                     * @brief Construct a new Accessor_t object with the given buffers.
                     *
                     * @param cgh Device handler.
                     * @param rng Random number generator buffer to access.
                     * @param unif Uniform distribution buffer to access.
                     */
                    Accessor_t(sycl::handler& cgh, sycl::buffer<R, 2>& rng, sycl::buffer<U<T>, 2>& unif) :
                            rng_(rng.template get_access<sycl::access::mode::read_write>(cgh)), unif_(unif.template get_access<sycl::access::mode::read_write>(cgh)){};

                    sycl::accessor<R, 2, sycl::access::mode::read_write> rng_; /**< @brief Accessor to the random number generators.*/
                    sycl::accessor<U<T>, 2, sycl::access::mode::read_write> unif_; /**< @brief Accessor to the uniform distributions.*/
            };

            /**
             * @brief Construct a new RandomGenerator_t object with the given dimensions.
             *
             * @param size_x Horizontal number of pixels in the image.
             * @param size_y Vertical number of pixels in the image.
             */
            RandomGenerator_t(size_t size_x, size_t size_y) : size_x_(size_x), size_y_(size_y), rng_(sycl::range<2>{size_x, size_y}), unif_(sycl::range<2>{size_x, size_y}) {
                std::random_device rdev;
                const sycl::host_accessor<R, 2, sycl::access_mode::write> rng_accessor(rng_, sycl::no_init);
                const sycl::host_accessor<U<T>, 2, sycl::access_mode::write> unif_accessor(unif_, sycl::no_init);

                auto range = rng_.get_range();

                for (size_t i = 0; i < rng_.get_range()[0]; ++i) {
                    for (size_t j = 0; j < rng_.get_range()[1]; ++j) {
                        rng_accessor[sycl::id<2>{i, j}]  = R(rdev());
                        unif_accessor[sycl::id<2>{i, j}] = U<T>(0, 1);
                    }
                }
            };

            /**
             * @brief Construct a new RandomGenerator_t object on the device with the given dimensions.
             *
             * @param queue Device queue to use to populate the vectors.
             * @param size_x Horizontal number of pixels in the image.
             * @param size_y Vertical number of pixels in the image.
             */
            RandomGenerator_t(sycl::queue& queue, size_t size_x, size_t size_y) : size_x_(size_x), size_y_(size_y), rng_(sycl::range<2>{size_x, size_y}), unif_(sycl::range<2>{size_x, size_y}) {
                std::random_device rdev;
                const auto seed = rdev();

                // Size of index space for kernel
                const sycl::range<2> num_work_items{size_x_, size_y_};

                // Submitting command group(work) to queue
                queue.submit([&](sycl::handler& cgh) {
                    // Getting read write access to the buffer on a device
                    sycl::accessor rng_accessor{rng_, cgh, sycl::write_only, sycl::no_init};
                    sycl::accessor unif_accessor{unif_, cgh, sycl::write_only, sycl::no_init};

                    // Executing kernel
                    cgh.parallel_for<class RandomGeneratorConstructor>(num_work_items, [=](sycl::item<2> WIitem) {
                        rng_accessor[WIitem]  = R(seed + WIitem.get_linear_id());
                        unif_accessor[WIitem] = U<T>(0, 1);
                    });
                });
            };

            size_t size_x_; /**< @brief Horizontal number of pixels in the image.*/
            size_t size_y_; /**< @brief Vertical number of pixels in the image.*/
            sycl::buffer<R, 2> rng_; /**< @brief Vector of shapes to be drawn.*/
            sycl::buffer<U<T>, 2> unif_; /**< @brief Uniform random distribution used for generating random numbers.*/

            /**
             * @brief Get a Accessor_t object attached to this random generator
             *
             * @param cgh Device context to use to get access.
             * @return Accessor_t Accessor that can be used on the device to query the random generator
             */
            auto getAccessor(sycl::handler& cgh) -> Accessor_t {
                return Accessor_t(cgh, rng_, unif_);
            };
    };
}}

#endif
