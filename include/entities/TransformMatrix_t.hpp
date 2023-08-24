#ifndef AGPTRACER_ENTITIES_TRANSFORMMATRIX_T_HPP
#define AGPTRACER_ENTITIES_TRANSFORMMATRIX_T_HPP

#include "entities/Vec3.hpp"
#include <array>
#include <sycl/sycl.hpp>

namespace AGPTracer { namespace Entities {

    /**
     * @brief The transformation matrix class represents a 4x4 3D transformation matrix, used to transform other objects in 3D space.
     *
     * The transformation matrix can be rotated, scaled, translated and other operations. Other objects, such as shapes and rays,
     * then use the transformation matrix to modify themselves. All objects using transformation matrices can therefore be modified
     * in a single unified way, without modifying the underlying objects. This enables basic instancing.
     *
     * @tparam T Floating point datatype to use
     */
    template<typename T = double>
    class TransformMatrix_t {
        public:
            /**
             * @brief Construct a new TransformMatrix_t object as the identity transformation matrix, which does not modify objects.
             */
            constexpr TransformMatrix_t();

            /**
             * @brief Construct a new TransformMatrix_t object from the 16 values of its 4x4 matrix.
             *
             * Elements are given row by row. The identity matrix has ones on its diagonal.
             *
             * @param i0 Element at position (1, 1)
             * @param i1 Element at position (1, 2)
             * @param i2 Element at position (1, 3)
             * @param i3 Element at position (1, 4)
             * @param i4 Element at position (2, 1)
             * @param i5 Element at position (2, 2)
             * @param i6 Element at position (2, 3)
             * @param i7 Element at position (2, 4)
             * @param i8 Element at position (3, 1)
             * @param i9 Element at position (3, 2)
             * @param i10 Element at position (3, 3)
             * @param i11 Element at position (3, 4)
             * @param i12 Element at position (4, 1)
             * @param i13 Element at position (4, 2)
             * @param i14 Element at position (4, 3)
             * @param i15 Element at position (4, 4)
             */
            constexpr TransformMatrix_t(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7, T i8, T i9, T i10, T i11, T i12, T i13, T i14, T i15);

            /**
             * @brief Construct a new TransformMatrix_t object from an array of the 16 values of its 4x4 matrix.
             *
             * @param values Array containing the elements of the matrix, ordered by rows.
             */
            constexpr TransformMatrix_t(std::array<T, 16> values);

            std::array<T, 16> matrix_; /**< @brief Array of the 16 values in the 4x4 matrix.*/
            std::array<T, 16> matrix_inverse_; /**< @brief Transposed inverted matrix, used to transform directions.*/

            /**
             * @brief Rotates the matrix around the x axis by a specified angle in radians.
             *
             * This means that if an object it at (0, 1, 0) and is rotated π rad, it will be at (0, -1, 0) and face the opposite direction.
             *
             * @tparam T2 Other type
             * @param angle Angle in radians to rotate around the x axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto rotateXAxis(T2 angle) -> TransformMatrix_t<T>&;

            /**
             * @brief Rotates the matrix around the x axis by a specified angle in radians.
             *
             * This means that if an object it at (1, 0, 0) and is rotated π rad, it will be at (-1, 0, 0) and face the opposite direction.
             *
             * @tparam T2 Other type
             * @param angle Angle in radians to rotate around the y axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto rotateYAxis(T2 angle) -> TransformMatrix_t<T>&;

            /**
             * @brief Rotates the matrix around the z axis by a specified angle in radians.
             *
             * This means that if an object it at (1, 0, 0) and is rotated π rad, it will be at (-1, 0, 0) and face the opposite direction.
             *
             * @tparam T2 Other type
             * @param angle Angle in radians to rotate around the z axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto rotateZAxis(T2 angle) -> TransformMatrix_t<T>&;

            /**
             * @brief Rotates the matrix on itself around the x direction.
             *
             * The object won't move but will face the opposite direction around x.
             *
             * @tparam T2 Other type
             * @param angle Angle in radians to rotate on itself around the x direction.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto rotateX(T2 angle) -> TransformMatrix_t<T>&;

            /**
             * @brief Rotates the matrix on itself around the y direction.
             *
             * The object won't move but will face the opposite direction around y.
             *
             * @tparam T2 Other type
             * @param angle Angle in radians to rotate on itself around the y direction.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto rotateY(T2 angle) -> TransformMatrix_t<T>&;

            /**
             * @brief Rotates the matrix on itself around the z direction.
             *
             * The object won't move but will face the opposite direction around z.
             *
             * @tparam T2 Other type
             * @param angle Angle in radians to rotate on itself around the z direction.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto rotateZ(T2 angle) -> TransformMatrix_t<T>&;

            /**
             * @brief Rotates the matrix around an arbitrary axis by a specified angle in radians.
             *
             * This rotates the matrix around an arbitrary axis passing through (0, 0, 0).
             * This moves the object.
             *
             * @tparam T2 Other type
             * @tparam T3 Other vector type
             * @param vec Axis around which the matrix will be rotated. Passes through (0, 0, 0).
             * @param angle Angle in radians to rotate around the axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2, class T3>
            auto rotateAxis(const Vec3<T3>& vec, T2 angle) -> TransformMatrix_t<T>&; // In c++26 sqrt is constexpr

            /**
             * @brief Rotates the matrix on itself around an arbitrary axis by a specified angle in radians.
             *
             * This rotates the matrix around an arbitrary axis passing through the matrix's center.
             * This doesn't move the object.
             *
             * @tparam T2 Other type
             * @tparam T3 Other vector type
             * @param vec Axis around which the matrix will be rotated on itself.
             * @param angle Angle in radians to rotate on itself around the axis' direction.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2, class T3>
            auto rotate(const Vec3<T3>& vec, T2 angle) -> TransformMatrix_t<T>&; // In c++26 sqrt is constexpr

            /**
             * @brief Translates the matrix in 3D space.
             *
             * @tparam T2 Other type
             * @param vec Vector by which to translate the matrix.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto translate(const Vec3<T2>& vec) -> TransformMatrix_t<T>&;

            /**
             * @brief Scales the matrix from (0, 0, 0) by a three-component vector.
             *
             * This means that if an object is at (1, 0, 1) and is scaled by (2, 2, 1) it will
             * be placed at (2, 0, 1) and be twice bigger except for the z direction.
             *
             * @tparam T2 Other type
             * @param vec Vector whose three components are the scale factors for the three axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto scaleAxis(const Vec3<T2>& vec) -> TransformMatrix_t<T>&;

            /**
             * @brief Scales the matrix from (0, 0, 0) by a factor.
             *
             * This means that if an object is at (1, 0, 1) and is scaled by 2 it will
             * be placed at (2, 0, 2) and be twice bigger.
             *
             * @tparam T2 Other type
             * @param fac Factor by which the matrix will be scaled from (0, 0, 0).
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto scaleAxis(T2 fac) -> TransformMatrix_t<T>&;

            /**
             * @brief Scales the matrix inplace by a three-component vector.
             *
             * This won't move the object.
             *
             * @tparam T2 Other type
             * @param vec Vector whose three components are the scale factors for the three axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto scale(const Vec3<T2>& vec) -> TransformMatrix_t<T>&;

            /**
             * @brief Scales the matrix inplace by a factor.
             *
             * This won't move the object.
             *
             * @tparam T2 Other type
             * @param fac Scale factors for the three axis.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto scale(T2 fac) -> TransformMatrix_t<T>&;

            /**
             * @brief Reflects the matrix. Not implemented yet.
             *
             * Not implemented.
             *
             * @tparam T2 Other type
             * @param vec Vector around which to reflect.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto reflect(const Vec3<T2>& vec) -> TransformMatrix_t<T>&;

            /**
             * @brief Shears the matrix. Not implemented yet.
             *
             * Not implemented.
             *
             * @tparam T2 Other type
             * @param vec Vector along which to shear.
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            template<class T2>
            constexpr auto shear(const Vec3<T2>& vec) -> TransformMatrix_t<T>&;

            /**
             * @brief Transposes the matrix.
             *
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            constexpr auto transpose() -> TransformMatrix_t<T>&;

            /**
             * @brief Inverts the matrix.
             *
             * See https://graphics.stanford.edu/~mdfisher/Code/Engine/Matrix4.cpp.html
             *
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            constexpr auto invert() -> TransformMatrix_t<T>&;

            /**
             * @brief Negates the matrix.
             *
             * @return TransformMatrix_t<T>& Reference to this matrix, used to chain operations.
             */
            constexpr auto neg() -> TransformMatrix_t<T>&;

            /**
             * @brief Transforms a point with the matrix, moving it around according to the operations made on the matrix.
             *
             * @tparam T2 Other type
             * @param vec Point to transform.
             * @return Vec3 Transformed point.
             */
            template<class T2>
            constexpr auto multVec(const Vec3<T2>& vec) const -> Vec3<T2>;

            /**
             * @brief Transforms a direction, rotating it according to the operations made on the matrix.
             *
             * @tparam T2 Other type
             * @param vec Direction to transform.
             * @return Vec3<T2> Transformed direction.
             */
            template<class T2>
            auto multDir(const Vec3<T2>& vec) const -> Vec3<T2>; // In c++26 sqrt is constexpr

            /**
             * @brief Get the maximum scale of all three axis.
             *
             * @return T Maximum scale of the matrix.
             */
            auto getScale() const -> T; // In c++26 sqrt is constexpr

        private:
            /**
             * @brief Builds the transposed inverse matrix, to transform directions.
             *
             * This should be called after each transformation to ensure the inverse is up to date.
             */
            constexpr auto buildInverse() -> void;
    };
}}

#include "entities/TransformMatrix_t.tpp"

#endif