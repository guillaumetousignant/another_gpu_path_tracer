module;
#ifdef AGPTRACER_USE_JPEG
    #define cimg_use_jpeg
#endif
#ifdef AGPTRACER_USE_PNG
    #define cimg_use_png
#endif
#ifdef AGPTRACER_USE_TIFF
    #define cimg_use_tiff
#endif
// #define cimg_use_tinyexr // Can't put this twice for some reason, is on in Texture_t.tpp
#define cimg_use_cpp11 1
#define cimg_display 0
#include "external/CImg.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <sycl/sycl.hpp>

export module simple_image;

import vec3;

/**
 * @brief Contains different image types that can be used.
 *
 * Images describe rendering targets for cameras to use. They store an image, into which
 * a camera can accumulate samples. Some have the capability to be written to disk, or to
 * be drawn on screen.
 */
namespace AGPTracer::Images {
    /**
     * @brief The SimpleImage_t class represents an image, which can be modified, can accumulate samples, and can be written to disk.
     *
     * This is the class that stores the results of the intersections. Generally, cameras shoot rays through a scene, and afterwards
     * add that ray's contribution to the image buffer.
     *
     * @tparam T Floating point datatype to use
     */
    export template<typename T = double>
    class SimpleImage_t {
        private:
            unsigned int updates_; /**< @brief Number of times the whole image was updated. Number of samples that each pixel holds.*/
            sycl::buffer<Entities::Vec3<T>, 2> img_; /**< @brief Array of colour pixels representing the image, of size size_x_, size_y_.*/

        public:
            class Accessor_t {
                public:
                    /**
                     * @brief Construct a new Accessor_t object with the given buffer.
                     *
                     * @param cgh Device handler.
                     * @param img Buffer to access.
                     */
                    Accessor_t(sycl::handler& cgh, sycl::buffer<Entities::Vec3<T>, 2>& img) : img_(img.template get_access<sycl::access::mode::read_write>(cgh)) {}

                    /**
                     * @brief Updates a single pixel of the image, adding the contribution of the input.
                     *
                     * This doesn't increase the number of updates of the image.
                     *
                     * @param colour Colour contribution to be added to the pixel.
                     * @param pos Coordinates of the pixel to be updated.
                     */
                    auto update(const Entities::Vec3<T>& colour, sycl::id<2> pos) const -> void {
                        img_[pos] += colour;
                    }

                    /**
                     * @brief Sets the value of a single pixel of the image to the value of the input.
                     *
                     * This doesn't change the number of updates of the image.
                     *
                     * @param colour Colour contribution to be given to the pixel.
                     * @param pos Coordinates of the pixel to be set.
                     */
                    auto set(const Entities::Vec3<T>& colour, sycl::id<2> pos) const -> void {
                        img_[pos] = colour;
                    }

                private:
                    sycl::accessor<Entities::Vec3<T>, 2, sycl::access::mode::read_write> img_; /**< @brief Accessor to the image.*/
            };

            /**
             * @brief Construct a new SimpleImage_t object with the given dimensions.
             *
             * @param size_x Horizontal number of pixels in the image.
             * @param size_y Vertical number of pixels in the image.
             */
            SimpleImage_t(size_t size_x, size_t size_y) {
                const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);
                std::fill(accessor.begin(), accessor.end(), Entities::Vec3<T>());
            }

            size_t size_x_; /**< @brief Horizontal number of pixels in the image. Main axis of the layout.*/
            size_t size_y_; /**< @brief Vertical number of pixels in the image. Secondary axis of the layout.*/

            /**
             * @brief Resets the image, discarding all samples to date.
             *
             * Sets the number of updates to 0, and sets all pixels to 0, 0, 0.
             */
            auto reset() -> void {
                updates_ = 0;
                const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);
                std::fill(accessor.begin(), accessor.end(), Entities::Vec3<T>());
            }

            /**
             * @brief Updates the whole image, adding the contribution of all pixels in the input.
             *
             * This will increase the number of updates of the image by 1.
             *
             * @param img Vector of colour pixels to be added to the image.
             */
            auto update(const std::vector<Entities::Vec3<T>>& img) -> void {
                ++updates_;

                const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read_write> accessor(img_);
                std::transform(accessor.begin(), accessor.end(), img.begin(), accessor.begin(), std::plus<>());
            }

            /**
             * @brief Increments the number of updates of the image by 1.
             *
             * This increments the number of samples in the image. Useful when updating pixels individually,
             * which doesn't change the number of updates of the image.
             */
            auto update() -> void {
                ++updates_;
            }

            /**
             * @brief Updates a single pixel of the image, adding the contribution of the input.
             *
             * This doesn't increase the number of updates of the image.
             *
             * @param colour Colour contribution to be added to the pixel.
             * @param pos_x Horizontal coordinate of the pixel to be updated.
             * @param pos_y Vertical coordinate of the pixel to be updated.
             */
            auto update(const Entities::Vec3<T>& colour, size_t pos_x, size_t pos_y) -> void {
                const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read_write> accessor(img_);
                img_[sycl::id<2>{pos_x, pos_y}] += colour;
            }

            /**
             * @brief Sets the value of the whole image to the value of all pixels in the input.
             *
             * This sets the number of updates to 1.
             *
             * @param img Vector of colour pixels to be given to the image.
             */
            auto set(const std::vector<Entities::Vec3<T>>& img) -> void {
                updates_ = 1;
                const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);
                std::copy(img.begin(), img.end(), accessor.begin());
            }

            /**
             * @brief Sets the value of a single pixel of the image to the value of the input.
             *
             * This doesn't change the number of updates of the image.
             *
             * @param colour Colour contribution to be given to the pixel.
             * @param pos_x Horizontal coordinate of the pixel to be set.
             * @param pos_y Vertical coordinate of the pixel to be set.
             */
            auto set(const Entities::Vec3<T>& colour, size_t pos_x, size_t pos_y) -> void {
                const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_); // Not sure if it should be no_init. Probably not
                accessor[sycl::id<2>{pos_x, pos_y}] = colour;
            }

            /**
             * @brief Writes the image to disk using the provided filename.
             *
             * This converts the image to integer values and saves it to a 16bit png file. The image
             * is encoded with a gamma of 1.0.
             *
             * @param filename File to which the image will be saved.
             */
            auto write(const std::filesystem::path& filename) -> void {
                const T update_mult = T{1} / static_cast<T>(updates_);
                cimg_library::CImg<unsigned short> image(static_cast<unsigned int>(size_x_), static_cast<unsigned int>(size_y_), 1, 3);
                const auto n = static_cast<unsigned int>(size_x_ * size_y_);

                constexpr unsigned int bit_depth = 16;
                const T bit_multiplier           = std::pow(T{2}, bit_depth) - T{1}; // With msvc std::pow is not constexpr :(

                sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

                for (size_t j = 0; j < size_y_; ++j) {
                    for (size_t i = 0; i < size_x_; ++i) {
                        Entities::Vec3<T> colour = accessor[sycl::id<2>{i, j}] * update_mult;
                        colour.clamp(T{0}, T{1});
                        colour *= bit_multiplier;
                        image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 0, n, n) = static_cast<unsigned short>(std::lround(colour[0]));
                        image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 1, n, n) = static_cast<unsigned short>(std::lround(colour[1]));
                        image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 2, n, n) = static_cast<unsigned short>(std::lround(colour[2]));
                    }
                }

                image.save(filename.string().c_str());
            }

            /**
             * @brief Writes the image to disk using the provided filename and gamma.
             *
             * This converts the image to integer values and saves it to a 16bit png file. The image
             * is encoded with the provided gamma value, 1.0 being standard.
             *
             * @param filename File to which the image will be saved.
             * @param gammaind Gamma used to encode the image.
             */
            auto write(const std::filesystem::path& filename, T gammaind) -> void {
                const T update_mult = T{1} / static_cast<T>(updates_);
                cimg_library::CImg<unsigned short> image(static_cast<unsigned int>(size_x_), static_cast<unsigned int>(size_y_), 1, 3);
                const auto n = static_cast<unsigned int>(size_x_ * size_y_);

                constexpr unsigned int bit_depth = 16;
                const T bit_multiplier           = std::pow(T{2}, bit_depth) - T{1}; // With msvc std::pow is not constexpr :(

                sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

                for (size_t j = 0; j < size_y_; ++j) {
                    for (size_t i = 0; i < size_x_; ++i) {
                        Entities::Vec3<T> colour = accessor[sycl::id<2>{i, j}] * update_mult;
                        colour.pow_inplace(gammaind).clamp(T{0}, T{1});
                        colour *= bit_multiplier;
                        image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 0, n, n) = static_cast<unsigned short>(std::lround(colour[0]));
                        image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 1, n, n) = static_cast<unsigned short>(std::lround(colour[1]));
                        image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 2, n, n) = static_cast<unsigned short>(std::lround(colour[2]));
                    }
                }

                image.save(filename.string().c_str());
            }

            /**
             * @brief Get a Accessor_t object attached to this image
             *
             * @return Accessor_t Accessor that can be used on the device to modify the image
             */
            auto getAccessor(sycl::handler& cgh) -> Accessor_t {
                return Accessor_t(cgh, img_);
            }
    };
}
