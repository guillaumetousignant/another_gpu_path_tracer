module;
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <sycl/sycl.hpp>
#include <utility>
#include <limits>

export module vec3;

namespace AGPTracer::Entities {

    /**
     * @brief The Vec3 class represents a 3-element vector.
     *
     * This can be used for 3D coordinates, 3D directions, 3-component colours, etc.
     * Several arithmetical operations are defined for those vectors.
     *
     * @tparam T Floating point datatype to use
     */
    export template<typename T = double>
    class Vec3 {
        public:
            std::array<T, 3> v_; /**< @brief Array of the 3 values in the vector.*/

            /**
             * @brief Construct a new Vec3 object with (0, 0, 0).
             */
            constexpr Vec3() : v_{0, 0, 0} {}

            /**
             * @brief Construct a new Vec3 object from 3 components.
             *
             * @param x First component of the vector.
             * @param y Second component of the vector.
             * @param z Third component of the vector.
             */
            constexpr Vec3(T x, T y, T z) : v_{x, y, z} {}

            /**
             * @brief Construct a new Vec3 object from an array of values.
             *
             * @param values Array containing the three values of the vector.
             */
            constexpr explicit Vec3(std::array<T, 3> values) : v_{values} {}

            /**
             * @brief Construct a new Vec3 object from one value.
             *
             * @param x Value given to the three components of the vector.
             */
            constexpr explicit Vec3(T x) : v_{x, x, x} {}

            /**
             * @brief Accesses the selected component of the vector, returning a reference.
             *
             * @tparam T2 Indexing type
             * @param index Index of the component to access.
             * @return T& Reference to the selected component.
             */
            template<class T2>
            constexpr auto operator[](T2 index) -> T& {
                return v_[index];
            }

            /**
             * @brief Returns the selected component of the vector.
             *
             * @tparam T2 Indexing type
             * @param index Index of the component to return.
             * @return T Selected component.
             */
            template<class T2>
            constexpr auto operator[](T2 index) const -> T {
                return v_[index];
            }

            /**
             * @brief Multiplies all components of the vector by a factor.
             *
             * Returns (x1*a, y1*a, z1*a).
             *
             * @tparam T2 Other type
             * @param scale Factor used to multiply all components of the vector.
             * @return Vec3 Resulting vector, (x1*a, y1*a, z1*a).
             */
            template<class T2>
            constexpr auto operator*(T2 scale) const -> Vec3<decltype(std::declval<T>() * std::declval<T2>())> {
                return {v_[0] * scale, v_[1] * scale, v_[2] * scale};
            }

            /**
             * @brief Element-wise multiplication of two vectors.
             *
             * Returns (x1*x2, y1*y2, z1*z3).
             *
             * @tparam T2 Other type
             * @param other Vector used to multiply.
             * @return Vec3 Resulting vector, (x1*x2, y1*y2, z1*z3).
             */
            template<class T2>
            constexpr auto operator*(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() * std::declval<T2>())> {
                return {v_[0] * other[0], v_[1] * other[1], v_[2] * other[2]};
            }

            /**
             * @brief Divides all components of the vector by a factor.
             *
             * Returns (x1/a, y1/a, z1/a).
             *
             * @tparam T2 Other type
             * @param scale Factor used to divide all components of the vector.
             * @return Vec3 Resulting vector, (x1/a, y1/a, z1/a).
             */
            template<class T2>
            constexpr auto operator/(T2 scale) const -> Vec3<decltype(std::declval<T>() / std::declval<T2>())> {
                return {v_[0] / scale, v_[1] / scale, v_[2] / scale};
            }

            /**
             * @brief Elements-wise division by the provided vector.
             *
             * Returns (x1/x2, y1/y2, z1/z2).
             *
             * @tparam T2 Other type
             * @param other Vector used to divide the components of this vector.
             * @return Vec3 Resulting vector, (x1/x2, y1/y2, z1/z2).
             */
            template<class T2>
            constexpr auto operator/(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() / std::declval<T2>())> {
                return {v_[0] / other[0], v_[1] / other[1], v_[2] / other[2]};
            }

            /**
             * @brief Adds two vectors.
             *
             * Returns (x1+x2, y1+y2, z1+z2).
             *
             * @tparam T2 Other type
             * @param other Vector added to this vector.
             * @return Vec3 Resulting vector, (x1+x2, y1+y2, z1+z2).
             */
            template<class T2>
            constexpr auto operator+(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() + std::declval<T2>())> {
                return {v_[0] + other.v_[0], v_[1] + other.v_[1], v_[2] + other.v_[2]};
            }

            /**
             * @brief Adds a factor to all components of the vector.
             *
             * Returns (x1+a, y1+a, z1+a).
             *
             * @tparam T2 Other type
             * @param factor Factor added to all components of the vector.
             * @return Vec3 Resulting vector, (x1+a, y1+a, z1+a).
             */
            template<class T2>
            constexpr auto operator+(T2 factor) const -> Vec3<decltype(std::declval<T>() + std::declval<T2>())> {
                return {v_[0] + factor, v_[1] + factor, v_[2] + factor};
            }

            /**
             * @brief Subtracts a vector from this vector.
             *
             * Returns (x1-x2, y1-y2, z1-z2).
             *
             * @tparam T2 Other type
             * @param other Vector to subtract from this vector.
             * @return Vec3 Resulting vector, (x1-x2, y1-y2, z1-z2).
             */
            template<class T2>
            constexpr auto operator-(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() - std::declval<T2>())> {
                return {v_[0] - other.v_[0], v_[1] - other.v_[1], v_[2] - other.v_[2]};
            }

            /**
             * @brief Subtracts a factor from all components of the vector.
             *
             * Returns (x1-a, y1-a, z1-a).
             *
             * @tparam T2 Other type
             * @param factor Factor subtracted from all components of the vector.
             * @return Vec3 Resulting vector, (x1-a, y1-a, z1-a).
             */
            template<class T2>
            constexpr auto operator-(T2 factor) const -> Vec3<decltype(std::declval<T>() - std::declval<T2>())> {
                return {v_[0] - factor, v_[1] - factor, v_[2] - factor};
            }

            /**
             * @brief Returns the vector negated.
             *
             * Returns (-x1, -y1, -z1).
             *
             * @return Vec3 Resulting vector, (-x1, -y1, -z1).
             */
            constexpr auto operator-() const -> Vec3<T> {
                return {-v_[0], -v_[1], -v_[2]};
            }

            /**
             * @brief Tests equality between two vectors.
             *
             * @param other Vector used to test equality.
             * @return true All three components of the vectors are equal.
             * @return false At least one component of the vectors is different.
             */
            constexpr auto operator==(const Vec3<T>& other) const -> bool {
                return (v_[0] == other.v_[0]) && (v_[1] == other.v_[1]) && (v_[2] == other.v_[2]);
            }

            /**
             * @brief In-place multiplies all components of the vector by a factor.
             *
             * Becomes (x1*a, y1*a, z1*a).
             *
             * @tparam T2 Other type
             * @param scale Factor used to multiply all components of the vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator*=(T2 scale) -> const Vec3<T>& {
                v_[0] *= scale;
                v_[1] *= scale;
                v_[2] *= scale;
                return *this;
            }

            /**
             * @brief In-place element-wise multiplication of the vector by another vector.
             *
             * Becomes (x1*x2, y1*y2, z1*z2).
             *
             * @tparam T2 Other type
             * @param other Vector used to multiply.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator*=(const Vec3<T2>& other) -> const Vec3<T>& {
                v_[0] *= other[0];
                v_[1] *= other[1];
                v_[2] *= other[2];
                return *this;
            }

            /**
             * @brief In-place divides all components of the vector by a factor.
             *
             * Becomes (x1/a, y1/a, z1/a).
             *
             * @tparam T2 Other type
             * @param scale Factor used to divide all components of the vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator/=(T2 scale) -> const Vec3<T>&  {
                v_[0] /= scale;
                v_[1] /= scale;
                v_[2] /= scale;
                return *this;
            }

            /**
             * @brief In-place elements-wise division by the provided vector.
             *
             * Becomes (x1/x2, y1/y2, z1/z2).
             *
             * @tparam T2 Other type
             * @param other Vector used to divide the components of this vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator/=(const Vec3<T2>& other) -> const Vec3<T>& {
                v_[0] /= other[0];
                v_[1] /= other[1];
                v_[2] /= other[2];
                return *this;
            }

            /**
             * @brief In-place addition of another vector.
             *
             * Becomes (x1+x2, y1+y2, z1+z2).
             *
             * @tparam T2 Other type
             * @param other Vector added to this vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator+=(const Vec3<T2>& other) -> const Vec3<T>& {
                v_[0] += other.v_[0];
                v_[1] += other.v_[1];
                v_[2] += other.v_[2];
                return *this;
            }

            /**
             * @brief In-place adds a factor to all components of the vector.
             *
             * Becomes (x1+a, y1+a, z1+a).
             *
             * @tparam T2 Other type
             * @param factor Factor added to all components of the vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator+=(T2 factor) -> const Vec3<T>& {
                v_[0] += factor;
                v_[1] += factor;
                v_[2] += factor;
                return *this;
            }

            /**
             * @brief In-place subtracts a vector from this vector.
             *
             * Becomes (x1-x2, y1-y2, z1-z2).
             *
             * @tparam T2 Other type
             * @param other Vector to subtract from this vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator-=(const Vec3<T2>& other) -> const Vec3<T>& {
                v_[0] -= other.v_[0];
                v_[1] -= other.v_[1];
                v_[2] -= other.v_[2];
                return *this;
            }

            /**
             * @brief In-place subtracts a factor from all components of the vector.
             *
             * Becomes (x1-a, y1-a, z1-a).
             *
             * @tparam T2 Other type
             * @param factor Factor subtracted from all components of the vector.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto operator-=(T2 factor) -> const Vec3<T>& {
                v_[0] -= factor;
                v_[1] -= factor;
                v_[2] -= factor;
                return *this;
            }

            /**
             * @brief Sets the components of the vector to the minimum of its components and the other's.
             *
             * Becomes (min(x1, x2), min(y1, y2), min(z1, z2))
             *
             * @tparam T2 Other type
             * @param other Vector to calculate minimum components with.
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto min(const Vec3<T2>& other) -> Vec3<T>& {
                v_[0] = std::min(v_[0], other[0]);
                v_[1] = std::min(v_[1], other[1]);
                v_[2] = std::min(v_[2], other[2]);
                return *this;
            }

            /**
             * @brief Sets the components of the vector to the minimum of its components and the provided factor.
             *
             * Becomes (min(x1, a), min(y1, a), min(z1, a))
             *
             * @tparam T2 Other type
             * @param other Factor to calculate minimum with.
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto min(T2 other) -> Vec3<T>& {
                v_[0] = std::min(v_[0], other);
                v_[1] = std::min(v_[1], other);
                v_[2] = std::min(v_[2], other);
                return *this;
            }

            /**
             * @brief ets the components of the vector to the maximum of its components and the other's.
             *
             * Becomes (max(x1, x2), max(y1, y2), max(z1, z2))
             *
             * @tparam T2 Other type
             * @param other Vector to calculate maximum components with.
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto max(const Vec3<T2>& other) -> Vec3<T>& {
                v_[0] = std::max(v_[0], other[0]);
                v_[1] = std::max(v_[1], other[1]);
                v_[2] = std::max(v_[2], other[2]);
                return *this;
            }

            /**
             * @brief Sets the components of the vector to the maximum of its components and the provided factor.
             *
             * Becomes (max(x1, a), max(y1, a), max(z1, a))
             *
             * @tparam T2 Other type
             * @param other Factor to calculate maximum with.
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto max(T2 other) -> Vec3<T>& {
                v_[0] = std::max(v_[0], other);
                v_[1] = std::max(v_[1], other);
                v_[2] = std::max(v_[2], other);
                return *this;
            }

            /**
             * @brief Returns a vector with the minimum components of this vector and another.
             *
             * Returns (min(x1, x2), min(y1, y2), min(z1, z2))
             *
             * @tparam T2 Other type
             * @param other Vector to calculate minimum components with.
             * @return Vec3 Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto getMin(const Vec3<T2>& other) const
                -> Vec3<typename std::remove_const<typename std::remove_reference<decltype(std::min(std::declval<T>(), std::declval<T2>()))>::type>::type> {
                return {std::min(v_[0], other[0]), std::min(v_[1], other[1]), std::min(v_[2], other[2])};
            }

            /**
             * @brief Returns a vector with the minimum components of this vector and a factor.
             *
             * Returns (min(x1, a), min(y1, a), min(z1, a))
             *
             * @tparam T2 Other type
             * @param other Factor to calculate minimum with.
             * @return Vec3 Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto getMin(T2 other) const -> Vec3<decltype(std::min(std::declval<T>(), std::declval<T2>()))>  {
                return {std::min(v_[0], other), std::min(v_[1], other), std::min(v_[2], other)};
            }

            /**
             * @brief Returns a vector with the maximum components of this vector and another.
             *
             * Returns (max(x1, x2), max(y1, y2), max(z1, z2))
             *
             * @tparam T2 Other type
             * @param other Vector to calculate maximum components with.
             * @return Vec3 Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto getMax(const Vec3<T2>& other) const
                -> Vec3<typename std::remove_const<typename std::remove_reference<decltype(std::max(std::declval<T>(), std::declval<T2>()))>::type>::type>  {
                return {std::max(v_[0], other[0]), std::max(v_[1], other[1]), std::max(v_[2], other[2])};
            }

            /**
             * @brief Returns a vector with the maximum components of this vector and a factor.
             *
             * Returns (max(x1, a), max(y1, a), max(z1, a))
             *
             * @tparam T2 Other type
             * @param other Factor to calculate maximum with.
             * @return Vec3 Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto getMax(T2 other) const -> Vec3<decltype(std::max(std::declval<T>(), std::declval<T2>()))> {
                return {std::max(v_[0], other), std::max(v_[1], other), std::max(v_[2], other)};
            }

            /**
             * @brief Returns the magnitude of the vector.
             *
             * Returns the L2 norm of the vector: sqrt(x^2 + y^2 + z^2).
             *
             * @return T Magnitude of the vector.
             */
            auto magnitude() const -> T { // In c++26 sqrt is constexpr
                return sycl::sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]);
            }

            /**
             * @brief Returns the squared magnitude of the vector.
             *
             * Returns x^2 + y^2 + z^2. Useful because it is much faster than the norm,
             * and can be used instead of it in some situations.
             *
             * @return T Squared magnitude of the norm.
             */
            constexpr auto magnitudeSquared() const -> T {
                return v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2];
            }

            /**
             * @brief Returns a the normalized vector.
             *
             * Divides all components of the vector by its magnitude.
             *
             * @return Vec3 Normalized vector.
             */
            auto normalize() const -> Vec3<T> { // In c++26 sqrt is constexpr
                const T m = magnitude();
                return {v_[0] / m, v_[1] / m, v_[2] / m};
            }

            /**
             * @brief Normalizes the vector in-place, dividing it by its norm.
             *
             * Divides all components of the vector by its magnitude.
             *
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            auto normalize_inplace() -> const Vec3<T>& { // In c++26 sqrt is constexpr
                const T m = magnitude();
                v_[0] /= m;
                v_[1] /= m;
                v_[2] /= m;
                return *this;
            }

            /**
             * @brief Computes the dot product of this vector and another.
             *
             * Returns v1.v2
             *
             * @tparam T2 Other type
             * @param other Vector to dot with this one.
             * @return T Dot product of the two vectors.
             */
            template<class T2>
            constexpr auto dot(const Vec3<T2>& other) const -> decltype(std::declval<T>() * std::declval<T2>()) {
                return v_[0] * other.v_[0] + v_[1] * other.v_[1] + v_[2] * other.v_[2];
            }

            /**
             * @brief Computes the cross product of this vector and another.
             *
             * Returns v1 x v2.
             *
             * @tparam T2 Other type
             * @tparam T2 Other type
             * @param other Vector to cross with this one.
             * @return Vec3 Cross product of the two vectors.
             */
            template<class T2>
            constexpr auto cross(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() * std::declval<T2>())>  {
                return {v_[1] * other.v_[2] - v_[2] * other.v_[1], v_[2] * other.v_[0] - v_[0] * other.v_[2], v_[0] * other.v_[1] - v_[1] * other.v_[0]};
            }

            /**
             * @brief Changes the vector in-place to spherical coordinates.
             *
             * Assumes the vector is in cartesian coordinates.
             *
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            auto to_sph() -> const Vec3<T>& { // CHECK outputs nan  // In c++26 sqrt is constexpr
                // [r, theta, phi] (theta is polar angle)
                const T temp = std::atan2(v_[1], v_[0]);
                v_[0]        = magnitude();
                v_[1]        = (std::abs(v_[0]) >= std::numeric_limits<T>::min()) ? std::acos(v_[2] / v_[0]) : T{0};
                v_[2]        = temp;
                return *this;
            }

            /**
             * @brief Changes the vector in-place to cartesian coordinates.
             *
             * Assumes the vector is in spherical coordinates.
             *
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            constexpr auto to_xyz() -> const Vec3<T>& {
                const T temp  = v_[2];
                v_[2]         = v_[0] * std::cos(v_[1]);
                const T temp2 = v_[1];
                v_[1]         = v_[0] * std::sin(temp2) * std::sin(temp);
                v_[0] *= std::sin(temp2) * std::cos(temp);
                return *this;
            }

            /**
             * @brief Changes the vector in-place to cartesian coordinates with arbitrary axises.
             *
             * Assumes the vector is in spherical coordinates.
             *
             * @tparam T2 Type for x reference
             * @tparam T3 Type for y reference
             * @tparam T4 Type for z reference
             * @param ref1 Axis used for x.
             * @param ref2 Axis used for y.
             * @param ref3 Axis used for z.
             * @return const Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2, class T3, class T4>
            constexpr auto to_xyz_offset(const Vec3<T2>& ref1, const Vec3<T3>& ref2, const Vec3<T4>& ref3) -> const Vec3<T>&  {
                const Vec3<T> temp = Vec3(v_[0] * std::sin(v_[1]) * std::cos(v_[2]), v_[0] * std::sin(v_[1]) * std::sin(v_[2]), v_[0] * std::cos(v_[1])); // CHECK could be better
                v_[0]              = ref1[0] * temp[0] + ref2[0] * temp[1] + ref3[0] * temp[2];
                v_[1]              = ref1[1] * temp[0] + ref2[1] * temp[1] + ref3[1] * temp[2];
                v_[2]              = ref1[2] * temp[0] + ref2[2] * temp[1] + ref3[2] * temp[2];
                return *this;
            }

            /**
             * @brief Returns the vector in spherical coordinates.
             *
             * Assumes the vector is in cartesian coordinates.
             *
             * @return Vec3 Spherical coordinates of the vector.
             */
            auto get_sph() const -> Vec3<T> { // In c++26 sqrt is constexpr
                const T r = magnitude();
                return {r, (std::abs(r) >= std::numeric_limits<T>::min()) ? std::acos(v_[2] / r) : T{0}, std::atan2(v_[1], v_[0])};
            }

            /**
             * @brief Returns the vector in cartesian coordinates.
             *
             * Assumes the vector is in spherical coordinates.
             *
             * @return Vec3 Cartesian coordinates of the vector.
             */
            constexpr auto get_xyz() const -> Vec3<T> {
                return {v_[0] * std::sin(v_[1]) * std::cos(v_[2]), v_[0] * std::sin(v_[1]) * std::sin(v_[2]), v_[0] * std::cos(v_[1])};
            }

            /**
             * @brief Returns the vector in cartesian coordinates with arbitrary axises.
             *
             * Assumes the vector is in spherical coordinates.
             *
             * @tparam T2 Type for x reference
             * @tparam T3 Type for y reference
             * @tparam T4 Type for z reference
             * @param ref1 Axis used for x.
             * @param ref2 Axis used for y.
             * @param ref3 Axis used for z.
             * @return Vec3 Cartesian coordinates of the vector.
             */
            template<class T2, class T3, class T4>
            constexpr auto get_xyz_offset(const Vec3<T2>& ref1, const Vec3<T3>& ref2, const Vec3<T4>& ref3) const
                -> Vec3<decltype(std::declval<T>() * std::declval<T2>() + std::declval<T>() * std::declval<T3>() + std::declval<T>() * std::declval<T4>())> {
                return ref1 * v_[0] * std::sin(v_[1]) * std::cos(v_[2]) + ref2 * v_[0] * std::sin(v_[1]) * std::sin(v_[2]) + ref3 * v_[0] * std::cos(v_[1]);
            }

            /**
             * @brief Returns a vector of the natural logarithm of all components of the vector.
             *
             * Returns (ln(x), ln(y), ln(z))
             *
             * @return Vec3 Vector made of the natural logarithm of all components of the vector.
             */
            constexpr auto ln() const -> Vec3<decltype(std::log(std::declval<T>()))> {
                return {std::log(v_[0]), std::log(v_[1]), std::log(v_[2])};
            }

            /**
             * @brief Returns a vector of the square root of all components of the vector.
             *
             * Returns (sqrt(x), sqrt(y), sqrt(z))
             *
             * @return Vec3 Vector made of the square root of all components of the vector.
             */
            auto sqrt() const -> Vec3<decltype(sycl::sqrt(std::declval<T>()))> { // In c++26 sqrt is constexpr
                return {sycl::sqrt(v_[0]), sycl::sqrt(v_[1]), sycl::sqrt(v_[2])};
            }

            /**
             * @brief Returns a vector of the exponential of all components of the vector.
             *
             * Returns (e^x, e^y, e^z).
             *
             * @return Vec3 Vector made of the exponential of all components of the vector.
             */
            constexpr auto exp() const -> Vec3<decltype(std::exp(std::declval<T>()))> {
                return {std::exp(v_[0]), std::exp(v_[1]), std::exp(v_[2])};
            }

            /**
             * @brief Returns a vector of the components of the vector to the specified power.
             *
             * Returns (x^a, y^a, z^a).
             *
             * @tparam T2 Other type
             * @param exp Power to be applied to all components.
             * @return Vec3 Vector made of the components of the vector to the specified power.
             */
            template<class T2>
            constexpr auto pow(T2 exp) const -> Vec3<decltype(std::pow(std::declval<T>(), std::declval<T2>()))> {
                return {std::pow(v_[0], exp), std::pow(v_[1], exp), std::pow(v_[2], exp)};
            }

            /**
             * @brief In-place raise the components of the vector to the specified power.
             *
             * Becomes (x^a, y^a, z^a).
             *
             * @tparam T2 Other type
             * @param exp Power to be applied to all components.
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2>
            constexpr auto pow_inplace(T2 exp) -> Vec3<T>& {
                v_[0] = std::pow(v_[0], exp);
                v_[1] = std::pow(v_[1], exp);
                v_[2] = std::pow(v_[2], exp);
                return *this;
            }

            /**
             * @brief Returns a vector of the components of the vector rounded down.
             *
             * Returns (floor(x), floor(y), floor(z))
             *
             * @return Vec3 Vector made of the components of the vector rounded down.
             */
            constexpr auto floor() const -> Vec3<decltype(std::floor(std::declval<T>()))> {
                return {std::floor(v_[0]), std::floor(v_[1]), std::floor(v_[2])};
            }

            /**
             * @brief Returns a vector of the components of the vector rounded up.
             *
             * Returns (ceil(x), ceil(y), ceil(z))
             *
             * @return Vec3 Vector made of the components of the vector rounded up.
             */
            constexpr auto ceil() const -> Vec3<decltype(std::ceil(std::declval<T>()))> {
                return {std::ceil(v_[0]), std::ceil(v_[1]), std::ceil(v_[2])};
            }

            /**
             * @brief In-place rounds the components to the nearest integer value.
             *
             * Becomes (round(x), round(y), round(z))
             *
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            constexpr auto round_inplace() -> Vec3<T>& {
                v_[0] = std::round(v_[0]);
                v_[1] = std::round(v_[1]);
                v_[2] = std::round(v_[2]);
                return *this;
            }

            /**
             * @brief In-place limits the components of the vector to a minimum and maximum value.
             *
             * @tparam T2 Type for minimum
             * @tparam T3 Type for maximum
             * @param minimum Minimum value of the components.
             * @param maximum Maximum value of the components.
             * @return Vec3& Reference to the vector, used to chain operations.
             */
            template<class T2, class T3>
            constexpr auto clamp(T2 minimum, T3 maximum) -> Vec3<T>& {
                min(maximum);
                max(minimum);
                return *this;
            }

            /**
             * @brief Returns the x component of the vector
             *
             * @return T x component of the vector.
             */
            constexpr auto x() const -> T {
                return v_[0];
            }

            /**
             * @brief Returns the y component of the vector
             *
             * @return T y component of the vector.
             */
            constexpr auto y() const -> T {
                return v_[1];
            }

            /**
             * @brief Returns the z component of the vector
             *
             * @return T z component of the vector.
             */
            constexpr auto z() const -> T {
                return v_[2];
            }

            /**
             * @brief Returns a reference to the x component of the vector
             *
             * @return T Reference to the x component of the vector.
             */
            constexpr auto x() -> T& {
                return v_[0];
            }

            /**
             * @brief Returns a reference to the y component of the vector
             *
             * @return T Reference to the y component of the vector.
             */
            constexpr auto y() -> T& {
                return v_[1];
            }

            /**
             * @brief Returns a reference to the z component of the vector
             *
             * @return T Reference to the z component of the vector.
             */
            constexpr auto z() -> T& {
                return v_[2];
            }

            /**
             * @brief Returns the red component of the vector
             *
             * @return T red component of the vector.
             */
            constexpr auto r() const -> T  {
                return v_[0];
            }

            /**
             * @brief Returns the green component of the vector
             *
             * @return T green component of the vector.
             */
            constexpr auto g() const -> T  {
                return v_[1];
            }

            /**
             * @brief Returns the blue component of the vector
             *
             * @return T blue component of the vector.
             */
            constexpr auto b() const -> T {
                return v_[2];
            }

            /**
             * @brief Returns a reference to the red component of the vector
             *
             * @return T Reference to the red component of the vector.
             */
            constexpr auto r() -> T& {
                return v_[0];
            }

            /**
             * @brief Returns a reference to the green component of the vector
             *
             * @return T Reference to the green component of the vector.
             */
            constexpr auto g() -> T& {
                return v_[1];
            }

            /**
             * @brief Returns a reference to the blue component of the vector
             *
             * @return T Reference to the blue component of the vector.
             */
            constexpr auto b() -> T& {
                return v_[2];
            }

            /**
             * @brief Tests approximate equality between two vectors.
             *
             * Tests equality to two significant digits.
             *
             * @param other Vector used to test approximate equality.
             * @return true All two components of the vectors are approximately equal.
             * @return false At least one component of the vectors is not approximately equal.
             */
            constexpr auto almost_equal(const Vec3<T>& other) const -> bool {
                constexpr int ulp = 2; // ULP
                // the machine epsilon has to be scaled to the magnitude of the values used
                // and multiplied by the desired precision in ULPs (units in the last place)
                return (std::abs(v_[0] - other.v_[0]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[0] + other.v_[0]) * ulp
                        // unless the result is subnormal
                        || std::abs(v_[0] - other.v_[0]) < std::numeric_limits<T>::min())

                    && (std::abs(v_[1] - other.v_[1]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[1] + other.v_[1]) * ulp || std::abs(v_[1] - other.v_[1]) < std::numeric_limits<T>::min())

                    && (std::abs(v_[2] - other.v_[2]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[2] + other.v_[2]) * ulp || std::abs(v_[2] - other.v_[2]) < std::numeric_limits<T>::min());
            }

            /**
             * @brief Tests approximate equality between two vectors to a certain precision.
             *
             * @param other Vector used to test approximate equality.
             * @param ulp Precision of the equality, higher is higher acceptable difference.
             * @return true All two components of the vectors are approximately equal.
             * @return false At least one component of the vectors is not approximately equal.
             */
            constexpr auto almost_equal(const Vec3<T>& other, int ulp) const -> bool  {
                // the machine epsilon has to be scaled to the magnitude of the values used
                // and multiplied by the desired precision in ULPs (units in the last place)
                return (std::abs(v_[0] - other.v_[0]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[0] + other.v_[0]) * ulp
                        // unless the result is subnormal
                        || std::abs(v_[0] - other.v_[0]) < std::numeric_limits<T>::min())

                    && (std::abs(v_[1] - other.v_[1]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[1] + other.v_[1]) * ulp || std::abs(v_[1] - other.v_[1]) < std::numeric_limits<T>::min())

                    && (std::abs(v_[2] - other.v_[2]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[2] + other.v_[2]) * ulp || std::abs(v_[2] - other.v_[2]) < std::numeric_limits<T>::min());
            }
    };
}

/**
 * @brief Formats a vector to be displayed.
 *
 * @tparam T Vector type
 * @param output Output stream.
 * @param v Vector to be displayed.
 * @return std::ostream& Output stream.
 */
template<class T>
auto operator<<(std::ostream& output, const AGPTracer::Entities::Vec3<T>& v) -> std::ostream& {
    output << '[' << v[0] << ", " << v[1] << ", " << v[2] << ']';
    return output;
}

/**
 * @brief Multiplies a factor with a vector.
 *
 * Returns (a*x, a*y, a*z).
 *
 * @tparam T Vector type
 * @tparam T2 Other type
 * @param factor Factor multiplying the vector.
 * @param v Vector to be multiplied.
 * @return AGPTracer::Entities::Vec3 Resulting Vector, (a*x, a*y, a*z).
 */
template<class T, class T2>
constexpr auto operator*(T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() * std::declval<T>())> {
    return {factor * v[0], factor * v[1], factor * v[2]};
}

/**
 * @brief Divides a factor with a vector.
 *
 * Returns (a/x, a/y, a/z).
 *
 * @tparam T Vector type
 * @tparam T2 Other type
 * @param factor Factor divided by the vector.
 * @param v Vector to be divided by.
 * @return AGPTracer::Entities::Vec3 Resulting Vector, (a/x, a/y, a/z).
 */
template<class T, class T2>
constexpr auto operator/(T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() / std::declval<T>())> {
    return {factor / v[0], factor / v[1], factor / v[2]};
}

/**
 * @brief Adds a factor to a vector.
 *
 * Returns (a+x, a+y, a+z).
 *
 * @tparam T Vector type
 * @tparam T2 Other type
 * @param factor Factor added to the vector.
 * @param v Vector to be added.
 * @return AGPTracer::Entities::Vec3 Resulting Vector, (a+x, a+y, a+z).
 */
template<class T, class T2>
constexpr auto operator+(T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() + std::declval<T>())> {
    return {factor + v[0], factor + v[1], factor + v[2]};
}

/**
 * @brief Subtracts a vector from a factor.
 *
 * Returns (a-x, a-y, a-z).
 *
 * @tparam T Vector type
 * @tparam T2 Other type
 * @param factor Factor subtracted by the vector.
 * @param v Vector to be subtracted.
 * @return AGPTracer::Entities::Vec3 Resulting Vector, (a-x, a-y, a-z).
 */
template<class T, class T2>
constexpr auto operator-(T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() - std::declval<T>())> {
    return {factor - v[0], factor - v[1], factor - v[2]};
}
