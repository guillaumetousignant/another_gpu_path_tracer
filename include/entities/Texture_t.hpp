#ifndef AGPTRACER_ENTITIES_TEXTURE_T_HPP
#define AGPTRACER_ENTITIES_TEXTURE_T_HPP

#include "entities/Vec3.hpp"
#include <array>
#include <filesystem>
#include <sycl/sycl.hpp>

namespace AGPTracer { namespace Entities {

    /**
     * @brief The texture class represents an image or other 2D buffer that can be accessed using 2D coordinates.
     *
     * This is used to store image or other 2D maps, such as normal maps or emissivity maps. They can be accessed
     * in 2D coordinates, for example using texture coordinates from meshes.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double>
    class Texture_t {
        public:
            /**
             * @brief Construct a new Texture_t object from an image file.
             *
             * @param filename Path to the image to be stored in the texture.
             */
            explicit Texture_t(const std::filesystem::path& filename);

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
            auto get(std::array<T, 2> xy) const -> Vec3<T>;

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
            auto get_cubic(std::array<T, 2> xy) const -> Vec3<T>;

            /**
             * @brief Returns the value of the texture from 2D uniform coordinates, using bilinear filtering.
             *
             * This returns the blended value of the four pixels around the given point, according to their proximity.
             *
             * @param xy Array of two coordinates, [x, y], ranging from 0 to 1, starting from the bottom left of the texture.
             * @return Vec3<T> Value of the texture at the provided coordinates.
             */
            auto get_linear(std::array<T, 2> xy) const -> Vec3<T>;

            /**
             * @brief Returns the value of the texture from 2D uniform coordinates, using nearest-neighbour filtering.
             *
             * This returns the value of the closest pixel to the given point.
             *
             * @param xy Array of two coordinates, [x, y], ranging from 0 to 1, starting from the bottom left of the texture.
             * @return Vec3<T> Value of the texture at the provided coordinates.
             */
            auto get_nn(std::array<T, 2> xy) const -> Vec3<T>;

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
            static auto CubicHermite(Vec3<T> first, Vec3<T> second, Vec3<T> third, Vec3<T> fourth, T t) -> Vec3<T>;
    };
}}

#include "entities/Texture_t.tpp"

#endif
