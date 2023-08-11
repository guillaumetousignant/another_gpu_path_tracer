#ifndef AGPTRACER_SHAPES_TRIANGLE_T_H
#define AGPTRACER_SHAPES_TRIANGLE_T_H

#include "entities/Ray_t.h"
#include "entities/TransformMatrix_t.h"
#include "entities/Vec3.h"
#include <array>
#include <CL/sycl.hpp>
#include <optional>

namespace AGPTracer { namespace Entities {
    // class Material_t;
}}

namespace AGPTracer { namespace Shapes {
    /**
     * @brief The triangle class defines a triangle shape that can be intersected by rays.
     *
     * A triangle is defined by three points, in counter-clockwise order. Its transformation matrix is used
     * to modify those points.
     */
    class Triangle_t {
        public:
            /**
             * @brief Construct a new Triangle_t object with a transformation matrix and material from three points.
             *
             * @param material Material of the triangle. Material that will be bounced on at intersection.
             * @param points Array of three points, in counter-clockwise order, defining the triangle.
             * @param normals Array of three normals, in counter-clockwise order, at the three points of the triangle.
             * @param texcoord Array of three texture coordinates with two components, in counter-clockwise order, at the three points of the triangle. [x0, y0, x1, y1, x2, y2]
             */
            Triangle_t(/*Material_t *material,*/ std::array<AGPTracer::Entities::Vec3<double>, 3> points,
                       const std::optional<std::array<AGPTracer::Entities::Vec3<double>, 3>> normals,
                       const std::optional<std::array<double, 6>> texcoord);

            // Material_t *material_; /**< @brief Material of which the shape is made of.*/
            AGPTracer::Entities::TransformMatrix_t transformation_; /**< @brief Transformation matrix used to modify the position and other transformations of the shape.*/
            std::array<AGPTracer::Entities::Vec3<double>, 3>
                points_orig_; /**< @brief Array of the three un-transformed points of the triangle, in counter-clockwise order. Transformed by the transform matrix on update to give points.*/
            std::array<AGPTracer::Entities::Vec3<double>, 3>
                normals_orig_; /**< @brief Array of the three un-transformed normals of the triangle, in counter-clockwise order. Transformed by the transform matrix on update to give normals.*/
            std::array<double, 6> texture_coordinates_; /**< @brief Array of the three texture coordinates with two components of the triangle, in counter-clockwise order. Transformed by the transform
                                                           matrix on update to give texture coordinates. [x0, y0, x1, y1, x2, y2]*/
            std::array<AGPTracer::Entities::Vec3<double>, 3> points_; /**< @brief Array of the three points of the triangle, in counter-clockwise order.*/
            std::array<AGPTracer::Entities::Vec3<double>, 3> normals_; /**< @brief Array of the three  normals of the triangle, in counter-clockwise order.*/
            AGPTracer::Entities::Vec3<double> v0v1_; /**< @brief Cached vector from point 0 to point 1. Used for intersection.*/
            AGPTracer::Entities::Vec3<double> v0v2_; /**< @brief Cached vector from point 0 to point 2. Used for intersection.*/
            std::array<double, 2> tuv_to_world_; /**< @brief Matrix to change referential from texture coordinate space to world space. Used to compute tangent vector.*/
            AGPTracer::Entities::Vec3<double> tangent_vec_; /**< @brief Tangent vector of the triangle in world space. Points to positive u in texture coordinates. Used for normal mapping.*/

            SYCL_EXTERNAL
            /**
             * @brief Updates the triangle's points from its transformation matrix.
             *
             * The points are created from the transformation matrix and the original points, stored in normals_orig_.
             */
            auto update() -> void;

            SYCL_EXTERNAL
            /**
             * @brief Intersects a ray with the triangle, and stores information about the intersection.
             *
             * This function returns wether a ray intersected the triangle or not, according to its direction and origin.
             * The intersection point, in object coordinates, is stored in uv, and the distance from the ray origin is stored
             * in t. uv is in barycentric coordinates, minus w [u, v].
             *
             * @param ray Ray to be tested for intersection with the triangle.
             * @param[out] t Distance at which the intersection ocurred, from ray origin. Undefined if not intersected.
             * @param[out] uv Coordinates in object space of the intersection. Undefined if not intersected. The coordinates are in barycentric coordinates, minus w [u, v].
             * @return true The ray intersected the triangle, t and uv are defined.
             * @return false The ray doesn't intersect the triangle, t and uv are undefined.
             */
            auto intersection(const AGPTracer::Entities::Ray_t& ray, double& t, std::array<double, 2>& uv) const -> bool;

            SYCL_EXTERNAL
            /**
             * @brief Returns the surface normal and texture coordinates at a point in object coordinates.
             *
             * This is used to find the surface normal on ray bounce. Used by materials to determine ray colour.
             * The texture coordinates is also returned, used by materials to fetch a colour in a texture.
             * The time parameter is for motionblur shapes, where time is used to interpolate. Not used here.
             * The object coordinates are in barycentric coordinates (minus w) [u, v].
             *
             * @param[in] time Time at which we want the normal and texture coordinates. Not used here.
             * @param[in] uv Object coordinates at which we want to find the normal and texture coordinates. The coordinates are in barycentric coordinates, minus w [u, v].
             * @param[out] tuv Texture coordinates at the specified coordinates.
             * @return AGPTracer::Entities::Vec3<double> Normal vector at the specified coordinates.
             */
            auto normaluv(double time, std::array<double, 2> uv, std::array<double, 2>& tuv) const -> AGPTracer::Entities::Vec3<double>;

            SYCL_EXTERNAL
            /**
             * @brief Returns the surface normal at a point in object coordinates.
             *
             * This is used to find the surface normal on ray bounce. Used by materials to determine ray colour.
             * The time parameter is for motionblur shapes, where time is used to interpolate. Not used here.
             * The object coordinates are in barycentric coordinates (minus w) [u, v].
             *
             * @param[in] time Time at which we want the normal. Used when motion blur is used. Not used here.
             * @param[in] uv Object coordinates at which we want to find the normal. The coordinates are in barycentric coordinates, minus w [u, v].
             * @return AGPTracer::Entities::Vec3<double> Normal vector at the specified coordinates.
             */
            auto normal(double time, std::array<double, 2> uv) const -> AGPTracer::Entities::Vec3<double>;

            SYCL_EXTERNAL
            /**
             * @brief Returns the surface normal, texture coordinates and tangent vector at a point in object coordinates.
             *
             * This is used to find the surface normal on ray bounce. Used by materials to determine ray colour.
             * The texture coordinates is also returned, used by materials to fetch a colour in a texture. The tangent
             * vector is used by materials for normal mapping, as the normals returned by those textures are in object
             * coordinates. The time parameter is for motionblur shapes, where time is used to interpolate. Not used here.
             * The object coordinates are in barycentric coordinates (minus w) [u, v].
             *
             * @param[in] time Time at which we want the normal, tangent and texture coordinates. Used when motion blur is used. Not used here.
             * @param[in] uv Object coordinates at which we want to find the normal, texture coordinates and tangent vector. The coordinates are in barycentric coordinates, minus w [u, v].
             * @param[out] tuv Texture coordinates at the specified coordinates.
             * @param[out] tangentvec Tangent vector at the specified coordinates.
             * @return AGPTracer::Entities::Vec3<double> Normal vector at the specified coordinates.
             */
            auto normal_uv_tangent(double time, std::array<double, 2> uv, std::array<double, 2>& tuv, AGPTracer::Entities::Vec3<double>& tangentvec) const -> AGPTracer::Entities::Vec3<double>;

            SYCL_EXTERNAL
            /**
             * @brief Returns the geometric surface normal of the triangle, not the interpolated one from vertex normals.
             *
             * Not used anywhere usually, used to debug normal interpolation.
             *
             * @param time Time at which we want the normal. Used when motion blur is used. Not used here.
             * @return AGPTracer::Entities::Vec3<double> Normal vector of the triangle.
             */
            auto normal_face(double time) const -> AGPTracer::Entities::Vec3<double>;

            SYCL_EXTERNAL
            /**
             * @brief Minimum coordinates of an axis-aligned bounding box around the triangle.
             *
             * This is used by acceleration structures to spatially sort shapes. Returns the minimum of all
             * three points for all axes.
             *
             * @return AGPTracer::Entities::Vec3<double> Minimum coordinates of an axis-aligned bounding box around the triangle.
             */
            auto mincoord() const -> AGPTracer::Entities::Vec3<double>;

            SYCL_EXTERNAL
            /**
             * @brief Maximum coordinates of an axis-aligned bounding box around the triangle.
             *
             * This is used by acceleration structures to spatially sort shapes. Returns the minimum of all
             * three points for all axes.
             *
             * @return AGPTracer::Entities::Vec3<double> Maximum coordinates of an axis-aligned bounding box around the triangle.
             */
            auto maxcoord() const -> AGPTracer::Entities::Vec3<double>;
    };
}}
#endif