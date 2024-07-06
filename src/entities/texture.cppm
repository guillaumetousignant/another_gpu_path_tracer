module;
#include <array>
#include <filesystem>
#include <sycl/sycl.hpp>
#include <cmath>
#ifdef AGPTRACER_USE_JPEG
    #define cimg_use_jpeg
#endif
#ifdef AGPTRACER_USE_PNG
    #define cimg_use_png
#endif
#ifdef AGPTRACER_USE_TIFF
    #define cimg_use_tiff
#endif
#define cimg_use_tinyexr
#define cimg_use_cpp11 1
#define cimg_display 0
#include "external/CImg.h"

module texture;

import vec3;

namespace AGPTracer::Entities {
    /**
     * @brief The texture class represents an image or other 2D buffer that can be accessed using 2D coordinates.
     *
     * This is used to store image or other 2D maps, such as normal maps or emissivity maps. They can be accessed
     * in 2D coordinates, for example using texture coordinates from meshes.
     *
     * @tparam T Floating point datatype to use
     */
    export template<typename T = double>
    class Texture_t {
        public:
            /**
             * @brief Construct a new Texture_t object from an image file.
             *
             * @param filename Path to the image to be stored in the texture.
             */
            explicit Texture_t(const std::filesystem::path& filename) :
                    img_{
                        sycl::range<2>{0, 0}
            } {
                cimg_library::CImg<T> image;
                std::string extension = filename.extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (extension == ".jpeg" || extension == ".jpg") {
                    constexpr unsigned int bit_depth = 8;
                    image.load_jpeg(filename.string().c_str());
                    image /= std::pow(T{2}, bit_depth) - T{1}; // Normalizing by bit depth
                }
                else if (extension == ".png") {
                    unsigned int bit_depth = 8;
                    image.load_png(filename.string().c_str(), &bit_depth);
                    image /= std::pow(T{2}, bit_depth) - T{1}; // Normalizing by bit depth
                }
                else if (extension == ".exr") {
                    image.load_exr(filename.string().c_str());
                    image.pow(T{1} / T{2.2}); // Gamma correcting
                }
                else if (extension == ".hdr") {
                    constexpr unsigned int bit_depth = 16;
                    image.load(filename.string().c_str());
                    image /= std::pow(T{2}, bit_depth) - T{1}; // Normalizing by bit depth
                }
                else {
                    constexpr unsigned int bit_depth = 8;
                    image.load(filename.string().c_str());
                    image /= std::pow(T{2}, bit_depth) - T{1}; // Normalizing by bit depth
                }

                size_x_ = image.width();
                size_y_ = image.height();

                img_ = sycl::buffer<Vec3<T>, 2>(sycl::range<2>{size_x_, size_y_});
                const sycl::host_accessor<Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);

                const auto n = static_cast<unsigned int>(size_x_ * size_y_);

                for (size_t j = 0; j < size_y_; ++j) {
                    for (size_t i = 0; i < size_x_; ++i) {
                        accessor[sycl::id<2>{i, j}] = Vec3<T>(image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 0, n, n),
                                                            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 1, n, n),
                                                            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 2, n, n));
                    }
                }
            }

            size_t size_x_; /**< @brief Horizontal number of pixels in the texture*/
            size_t size_y_; /**< @brief Vertical number of pixels in the texture*/
            sycl::buffer<Vec3<T>, 2> img_; /**< @brief Array of 3-element pixels representing the values of all pixels of the texture.*/

            /**
             * @brief Returns the value of the texture from 2D uniform coordinates, using bicubic filtering.
             *
             * This returns the blended value of the sixteen pixels around the given point, according to their proximity.
             * It uses a Catmull-Rom Hermite spline.
             * From demofox on Shadertoy, https://www.shadertoy.com/view/MllSzX
             *
             * @param xy Array of two coordinates, [x, y], ranging from 0 to 1, starting from the bottom left of the texture.
             * @return Vec3<T> Value of the texture at the provided coordinates.
             */
            auto get(std::array<T, 2> xy) const -> Vec3<T> {
                const T x = (size_x_ - 1) * (xy[0] - std::floor(xy[0]));
                const T y = (size_y_ - 1) * (xy[1] - std::floor(xy[1]));

                const T xd = x - std::floor(x);
                const T yd = y - std::floor(y);

                const size_t x0 = static_cast<size_t>(std::max(x - T{1}, T{0})); // floor
                const size_t x1 = static_cast<size_t>(x); // floor
                const size_t x2 = static_cast<size_t>(x + T{1}); // ceil
                const size_t x3 = std::min(static_cast<size_t>(x + T{2}), size_x_ - 1); // ceil
                const size_t y0 = static_cast<size_t>(std::max(y - T{1}, T{0})); // floor
                const size_t y1 = static_cast<size_t>(y); // floor
                const size_t y2 = static_cast<size_t>(y + T{1}); // ceil
                const size_t y3 = std::min(static_cast<size_t>(y + T{2}), size_y_ - 1); // ceil

                const sycl::host_accessor<Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

                const Vec3<T> cp0x = CubicHermite(accessor[sycl::id<2>{x0, y0}], accessor[sycl::id<2>{x1, y0}], accessor[sycl::id<2>{x2, y0}], accessor[sycl::id<2>{x3, y0}], xd);
                const Vec3<T> cp1x = CubicHermite(accessor[sycl::id<2>{x0, y1}], accessor[sycl::id<2>{x1, y1}], accessor[sycl::id<2>{x2, y1}], accessor[sycl::id<2>{x3, y1}], xd);
                const Vec3<T> cp2x = CubicHermite(accessor[sycl::id<2>{x0, y2}], accessor[sycl::id<2>{x1, y2}], accessor[sycl::id<2>{x2, y2}], accessor[sycl::id<2>{x3, y2}], xd);
                const Vec3<T> cp3x = CubicHermite(accessor[sycl::id<2>{x0, y3}], accessor[sycl::id<2>{x1, y3}], accessor[sycl::id<2>{x2, y3}], accessor[sycl::id<2>{x3, y3}], xd);
                return CubicHermite(cp0x, cp1x, cp2x, cp3x, yd);
            }

            /**
             * @brief Returns the value of the texture from 2D uniform coordinates, using bicubic filtering.
             *
             * This returns the blended value of the sixteen pixels around the given point, according to their proximity.
             * It uses a Catmull-Rom Hermite spline.
             * From demofox on Shadertoy, https://www.shadertoy.com/view/MllSzX
             *
             * @param xy Array of two coordinates, [x, y], ranging from 0 to 1, starting from the bottom left of the texture.
             * @return Vec3<T> Value of the texture at the provided coordinates.
             */
            auto get_cubic(std::array<T, 2> xy) const -> Vec3<T> {
                const T x = (size_x_ - 1) * (xy[0] - std::floor(xy[0]));
                const T y = (size_y_ - 1) * (xy[1] - std::floor(xy[1]));

                const T xd = x - std::floor(x);
                const T yd = y - std::floor(y);

                const size_t x0 = static_cast<size_t>(std::max(x - T{1}, T{0})); // floor
                const size_t x1 = static_cast<size_t>(x); // floor
                const size_t x2 = static_cast<size_t>(x + T{1}); // ceil
                const size_t x3 = std::min(static_cast<size_t>(x + T{2}), size_x_ - 1); // ceil
                const size_t y0 = static_cast<size_t>(std::max(y - T{1}, T{0})); // floor
                const size_t y1 = static_cast<size_t>(y); // floor
                const size_t y2 = static_cast<size_t>(y + T{1}); // ceil
                const size_t y3 = std::min(static_cast<size_t>(y + T{2}), size_y_ - 1); // ceil

                const sycl::host_accessor<Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

                const Vec3<T> cp0x = CubicHermite(accessor[sycl::id<2>{x0, y0}], accessor[sycl::id<2>{x1, y0}], accessor[sycl::id<2>{x2, y0}], accessor[sycl::id<2>{x3, y0}], xd);
                const Vec3<T> cp1x = CubicHermite(accessor[sycl::id<2>{x0, y1}], accessor[sycl::id<2>{x1, y1}], accessor[sycl::id<2>{x2, y1}], accessor[sycl::id<2>{x3, y1}], xd);
                const Vec3<T> cp2x = CubicHermite(accessor[sycl::id<2>{x0, y2}], accessor[sycl::id<2>{x1, y2}], accessor[sycl::id<2>{x2, y2}], accessor[sycl::id<2>{x3, y2}], xd);
                const Vec3<T> cp3x = CubicHermite(accessor[sycl::id<2>{x0, y3}], accessor[sycl::id<2>{x1, y3}], accessor[sycl::id<2>{x2, y3}], accessor[sycl::id<2>{x3, y3}], xd);
                return CubicHermite(cp0x, cp1x, cp2x, cp3x, yd);
            }

            /**
             * @brief Returns the value of the texture from 2D uniform coordinates, using bilinear filtering.
             *
             * This returns the blended value of the four pixels around the given point, according to their proximity.
             *
             * @param xy Array of two coordinates, [x, y], ranging from 0 to 1, starting from the bottom left of the texture.
             * @return Vec3<T> Value of the texture at the provided coordinates.
             */
            auto get_linear(std::array<T, 2> xy) const -> Vec3<T> {
                const T x = (size_x_ - 1) * (xy[0] - std::floor(xy[0]));
                const T y = (size_y_ - 1) * (xy[1] - std::floor(xy[1]));

                const T xd = x - std::floor(x);
                const T yd = y - std::floor(y);

                const size_t xlo = static_cast<size_t>(x); // floor
                const size_t xhi = static_cast<size_t>(x + T{1}); // ceil
                const size_t ylo = static_cast<size_t>(y); // floor
                const size_t yhi = static_cast<size_t>(y + T{1}); // ceil

                const sycl::host_accessor<Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

                return accessor[sycl::id<2>{xlo, ylo}] * (T{1} - xd) * (T{1} - yd) + accessor[sycl::id<2>{xhi, ylo}] * xd * (T{1} - yd) + accessor[sycl::id<2>{xlo, yhi}] * (T{1} - xd) * yd
                    + accessor[sycl::id<2>{xhi, yhi}] * xd * yd;
            }

            /**
             * @brief Returns the value of the texture from 2D uniform coordinates, using nearest-neighbour filtering.
             *
             * This returns the value of the closest pixel to the given point.
             *
             * @param xy Array of two coordinates, [x, y], ranging from 0 to 1, starting from the bottom left of the texture.
             * @return Vec3<T> Value of the texture at the provided coordinates.
             */
            auto get_nn(std::array<T, 2> xy) const -> Vec3<T> {
                const sycl::host_accessor<Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

                return accessor[sycl::id<2>{std::lround((size_x_ - 1) * (xy[0] - std::floor(xy[0]))), std::lround((size_y_ - 1) * (xy[1] - std::floor(xy[1])))}];
            }

        private:
            /**
             * @brief Returns the cubic Hermite spline of four pixels and a distance t.
             *
             * This is the Catmull-Rom spline.
             * From demofox on Shadertoy, https://www.shadertoy.com/view/MllSzX
             *
             * @param first First pixel.
             * @param second Second pixel.
             * @param third Third pixel.
             * @param fourth Fourth pixel.
             * @param t Distance from the center.
             * @return Vec3<T> Resulting pixel.
             */
            static auto CubicHermite(Vec3<T> first, Vec3<T> second, Vec3<T> third, Vec3<T> fourth, T t) -> Vec3<T> {
                const T t2      = t * t;
                const T t3      = t * t * t;
                const Vec3<T> a = -first / T{2} + T{3} * second / T{2} - T{3} * third / T{2} + fourth / T{2};
                const Vec3<T> b = first - T{5} * second / T{2} + T{2} * third - fourth / T{2};
                const Vec3<T> c = -first / T{2} + third / T{2};

                return a * t3 + b * t2 + c * t + second;
            }
    };
}
