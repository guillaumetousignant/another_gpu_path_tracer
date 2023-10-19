#include <limits>

template<typename T>
constexpr AGPTracer::Entities::Vec3<T>::Vec3() : v_{0, 0, 0} {}

template<typename T>
constexpr AGPTracer::Entities::Vec3<T>::Vec3(T x, T y, T z) : v_{x, y, z} {}

template<typename T>
constexpr AGPTracer::Entities::Vec3<T>::Vec3(std::array<T, 3> values) : v_{values} {}

template<typename T>
constexpr AGPTracer::Entities::Vec3<T>::Vec3(T x) : v_{x, x, x} {}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator[](T2 index) -> T& {
    return v_[index];
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator[](T2 index) const -> T {
    return v_[index];
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator*(T2 scale) const -> Vec3<decltype(std::declval<T>() * std::declval<T2>())> {
    return {v_[0] * scale, v_[1] * scale, v_[2] * scale};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator*(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() * std::declval<T2>())> {
    return {v_[0] * other[0], v_[1] * other[1], v_[2] * other[2]};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator/(T2 scale) const -> Vec3<decltype(std::declval<T>() / std::declval<T2>())> {
    return {v_[0] / scale, v_[1] / scale, v_[2] / scale};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator/(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() / std::declval<T2>())> {
    return {v_[0] / other[0], v_[1] / other[1], v_[2] / other[2]};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator+(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() + std::declval<T2>())> {
    return {v_[0] + other.v_[0], v_[1] + other.v_[1], v_[2] + other.v_[2]};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator+(T2 factor) const -> Vec3<decltype(std::declval<T>() + std::declval<T2>())> {
    return {v_[0] + factor, v_[1] + factor, v_[2] + factor};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator-(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() - std::declval<T2>())> {
    return {v_[0] - other.v_[0], v_[1] - other.v_[1], v_[2] - other.v_[2]};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator-(T2 factor) const -> Vec3<decltype(std::declval<T>() - std::declval<T2>())> {
    return {v_[0] - factor, v_[1] - factor, v_[2] - factor};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::operator-() const -> Vec3<T> {
    return {-v_[0], -v_[1], -v_[2]};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::operator==(const Vec3& other) const -> bool {
    return (v_[0] == other.v_[0]) && (v_[1] == other.v_[1]) && (v_[2] == other.v_[2]);
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator*=(T2 scale) -> const Vec3<T>& {
    v_[0] *= scale;
    v_[1] *= scale;
    v_[2] *= scale;
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator*=(const Vec3<T2>& other) -> const Vec3<T>& {
    v_[0] *= other[0];
    v_[1] *= other[1];
    v_[2] *= other[2];
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator/=(T2 scale) -> const Vec3<T>& {
    v_[0] /= scale;
    v_[1] /= scale;
    v_[2] /= scale;
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator/=(const Vec3<T2>& other) -> const Vec3<T>& {
    v_[0] /= other[0];
    v_[1] /= other[1];
    v_[2] /= other[2];
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator+=(const Vec3<T2>& other) -> const Vec3<T>& {
    v_[0] += other.v_[0];
    v_[1] += other.v_[1];
    v_[2] += other.v_[2];
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator+=(T2 factor) -> const Vec3<T>& {
    v_[0] += factor;
    v_[1] += factor;
    v_[2] += factor;
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator-=(const Vec3<T2>& other) -> const Vec3<T>& {
    v_[0] -= other.v_[0];
    v_[1] -= other.v_[1];
    v_[2] -= other.v_[2];
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::operator-=(T2 factor) -> const Vec3<T>& {
    v_[0] -= factor;
    v_[1] -= factor;
    v_[2] -= factor;
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::min(const Vec3<T2>& other) -> Vec3<T>& {
    v_[0] = std::min(v_[0], other[0]);
    v_[1] = std::min(v_[1], other[1]);
    v_[2] = std::min(v_[2], other[2]);
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::min(T2 other) -> Vec3<T>& {
    v_[0] = std::min(v_[0], other);
    v_[1] = std::min(v_[1], other);
    v_[2] = std::min(v_[2], other);
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::max(const Vec3<T2>& other) -> Vec3<T>& {
    v_[0] = std::max(v_[0], other[0]);
    v_[1] = std::max(v_[1], other[1]);
    v_[2] = std::max(v_[2], other[2]);
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::max(T2 other) -> Vec3<T>& {
    v_[0] = std::max(v_[0], other);
    v_[1] = std::max(v_[1], other);
    v_[2] = std::max(v_[2], other);
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::getMin(const Vec3<T2>& other) const
    -> Vec3<typename std::remove_const<typename std::remove_reference<decltype(std::min(std::declval<T>(), std::declval<T2>()))>::type>::type> {
    return {std::min(v_[0], other[0]), std::min(v_[1], other[1]), std::min(v_[2], other[2])};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::getMin(T2 other) const -> Vec3<decltype(std::min(std::declval<T>(), std::declval<T2>()))> {
    return {std::min(v_[0], other), std::min(v_[1], other), std::min(v_[2], other)};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::getMax(const Vec3<T2>& other) const
    -> Vec3<typename std::remove_const<typename std::remove_reference<decltype(std::max(std::declval<T>(), std::declval<T2>()))>::type>::type> {
    return {std::max(v_[0], other[0]), std::max(v_[1], other[1]), std::max(v_[2], other[2])};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::getMax(T2 other) const -> Vec3<decltype(std::max(std::declval<T>(), std::declval<T2>()))> {
    return {std::max(v_[0], other), std::max(v_[1], other), std::max(v_[2], other)};
}

template<typename T>
auto AGPTracer::Entities::Vec3<T>::magnitude() const -> T { // In c++26 sqrt is constexpr
    return sycl::sqrt(v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]);
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::magnitudeSquared() const -> T {
    return v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2];
}

template<typename T>
auto AGPTracer::Entities::Vec3<T>::normalize() const -> Vec3<T> { // In c++26 sqrt is constexpr
    const T m = magnitude();
    return {v_[0] / m, v_[1] / m, v_[2] / m};
}

template<typename T>
auto AGPTracer::Entities::Vec3<T>::normalize_inplace() -> const Vec3<T>& { // In c++26 sqrt is constexpr
    const T m = magnitude();
    v_[0] /= m;
    v_[1] /= m;
    v_[2] /= m;
    return *this;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::dot(const Vec3<T2>& other) const -> decltype(std::declval<T>() * std::declval<T2>()) {
    return v_[0] * other.v_[0] + v_[1] * other.v_[1] + v_[2] * other.v_[2];
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::cross(const Vec3<T2>& other) const -> Vec3<decltype(std::declval<T>() * std::declval<T2>())> {
    return {v_[1] * other.v_[2] - v_[2] * other.v_[1], v_[2] * other.v_[0] - v_[0] * other.v_[2], v_[0] * other.v_[1] - v_[1] * other.v_[0]};
}

template<typename T>
auto AGPTracer::Entities::Vec3<T>::to_sph() -> const Vec3<T>& { // CHECK outputs nan  // In c++26 sqrt is constexpr
    // [r, theta, phi] (theta is polar angle)
    const T temp = std::atan2(v_[1], v_[0]);
    v_[0]        = magnitude();
    v_[1]        = (std::abs(v_[0]) >= std::numeric_limits<T>::min()) ? std::acos(v_[2] / v_[0]) : T{0};
    v_[2]        = temp;
    return *this;
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::to_xyz() -> const Vec3<T>& {
    const T temp  = v_[2];
    v_[2]         = v_[0] * std::cos(v_[1]);
    const T temp2 = v_[1];
    v_[1]         = v_[0] * std::sin(temp2) * std::sin(temp);
    v_[0] *= std::sin(temp2) * std::cos(temp);
    return *this;
}

template<typename T>
template<class T2, class T3, class T4>
constexpr auto AGPTracer::Entities::Vec3<T>::to_xyz_offset(const Vec3<T2>& ref1, const Vec3<T3>& ref2, const Vec3<T4>& ref3) -> const Vec3<T>& {
    const Vec3<T> temp = Vec3(v_[0] * std::sin(v_[1]) * std::cos(v_[2]), v_[0] * std::sin(v_[1]) * std::sin(v_[2]), v_[0] * std::cos(v_[1])); // CHECK could be better
    v_[0]              = ref1[0] * temp[0] + ref2[0] * temp[1] + ref3[0] * temp[2];
    v_[1]              = ref1[1] * temp[0] + ref2[1] * temp[1] + ref3[1] * temp[2];
    v_[2]              = ref1[2] * temp[0] + ref2[2] * temp[1] + ref3[2] * temp[2];
    return *this;
}

template<typename T>
auto AGPTracer::Entities::Vec3<T>::get_sph() const -> Vec3<T> { // In c++26 sqrt is constexpr
    const T r = magnitude();
    return {r, (std::abs(r) >= std::numeric_limits<T>::min()) ? std::acos(v_[2] / r) : T{0}, std::atan2(v_[1], v_[0])};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::get_xyz() const -> Vec3<T> {
    return {v_[0] * std::sin(v_[1]) * std::cos(v_[2]), v_[0] * std::sin(v_[1]) * std::sin(v_[2]), v_[0] * std::cos(v_[1])};
}

template<typename T>
template<class T2, class T3, class T4>
constexpr auto AGPTracer::Entities::Vec3<T>::get_xyz_offset(const Vec3<T2>& ref1, const Vec3<T3>& ref2, const Vec3<T4>& ref3) const
    -> Vec3<decltype(std::declval<T>() * std::declval<T2>() + std::declval<T>() * std::declval<T3>() + std::declval<T>() * std::declval<T4>())> {
    return ref1 * v_[0] * std::sin(v_[1]) * std::cos(v_[2]) + ref2 * v_[0] * std::sin(v_[1]) * std::sin(v_[2]) + ref3 * v_[0] * std::cos(v_[1]);
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::ln() const -> Vec3<decltype(std::log(std::declval<T>()))> {
    return {std::log(v_[0]), std::log(v_[1]), std::log(v_[2])};
}

template<typename T>
auto AGPTracer::Entities::Vec3<T>::sqrt() const -> Vec3<decltype(sycl::sqrt(std::declval<T>()))> { // In c++26 sqrt is constexpr
    return {sycl::sqrt(v_[0]), sycl::sqrt(v_[1]), sycl::sqrt(v_[2])};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::exp() const -> Vec3<decltype(std::exp(std::declval<T>()))> {
    return {std::exp(v_[0]), std::exp(v_[1]), std::exp(v_[2])};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::pow(T2 exp) const -> Vec3<decltype(std::pow(std::declval<T>(), std::declval<T2>()))> {
    return {std::pow(v_[0], exp), std::pow(v_[1], exp), std::pow(v_[2], exp)};
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Entities::Vec3<T>::pow_inplace(T2 exp) -> Vec3<T>& {
    v_[0] = std::pow(v_[0], exp);
    v_[1] = std::pow(v_[1], exp);
    v_[2] = std::pow(v_[2], exp);
    return *this;
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::floor() const -> Vec3<decltype(std::floor(std::declval<T>()))> {
    return {std::floor(v_[0]), std::floor(v_[1]), std::floor(v_[2])};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::ceil() const -> Vec3<decltype(std::ceil(std::declval<T>()))> {
    return {std::ceil(v_[0]), std::ceil(v_[1]), std::ceil(v_[2])};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::round_inplace() -> Vec3<T>& {
    v_[0] = std::round(v_[0]);
    v_[1] = std::round(v_[1]);
    v_[2] = std::round(v_[2]);
    return *this;
}

template<typename T>
template<class T2, class T3>
constexpr auto AGPTracer::Entities::Vec3<T>::clamp(T2 minimum, T3 maximum) -> Vec3<T>& {
    min(maximum);
    max(minimum);
    return *this;
}

template<class T>
auto operator<<(std::ostream& output, const AGPTracer::Entities::Vec3<T>& v) -> std::ostream& {
    output << '[' << v[0] << ", " << v[1] << ", " << v[2] << ']';
    return output;
}

template<class T, class T2>
constexpr auto operator*(const T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() * std::declval<T>())> {
    return {factor * v[0], factor * v[1], factor * v[2]};
}

template<class T, class T2>
constexpr auto operator/(const T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() / std::declval<T>())> {
    return {factor / v[0], factor / v[1], factor / v[2]};
}

template<class T, class T2>
constexpr auto operator+(const T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() + std::declval<T>())> {
    return {factor + v[0], factor + v[1], factor + v[2]};
}

template<class T, class T2>
constexpr auto operator-(const T2 factor, const AGPTracer::Entities::Vec3<T>& v) -> AGPTracer::Entities::Vec3<decltype(std::declval<T2>() - std::declval<T>())> {
    return {factor - v[0], factor - v[1], factor - v[2]};
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::x() const -> T {
    return v_[0];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::y() const -> T {
    return v_[1];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::z() const -> T {
    return v_[2];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::x() -> T& {
    return v_[0];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::y() -> T& {
    return v_[1];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::z() -> T& {
    return v_[2];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::r() const -> T {
    return v_[0];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::g() const -> T {
    return v_[1];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::b() const -> T {
    return v_[2];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::r() -> T& {
    return v_[0];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::g() -> T& {
    return v_[1];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::b() -> T& {
    return v_[2];
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::almost_equal(const Vec3<T>& other) const -> bool {
    constexpr int ulp = 2; // ULP
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return (std::abs(v_[0] - other.v_[0]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[0] + other.v_[0]) * ulp
            // unless the result is subnormal
            || std::abs(v_[0] - other.v_[0]) < std::numeric_limits<T>::min())

        && (std::abs(v_[1] - other.v_[1]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[1] + other.v_[1]) * ulp || std::abs(v_[1] - other.v_[1]) < std::numeric_limits<T>::min())

        && (std::abs(v_[2] - other.v_[2]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[2] + other.v_[2]) * ulp || std::abs(v_[2] - other.v_[2]) < std::numeric_limits<T>::min());
}

template<typename T>
constexpr auto AGPTracer::Entities::Vec3<T>::almost_equal(const Vec3<T>& other, const int ulp) const -> bool {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return (std::abs(v_[0] - other.v_[0]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[0] + other.v_[0]) * ulp
            // unless the result is subnormal
            || std::abs(v_[0] - other.v_[0]) < std::numeric_limits<T>::min())

        && (std::abs(v_[1] - other.v_[1]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[1] + other.v_[1]) * ulp || std::abs(v_[1] - other.v_[1]) < std::numeric_limits<T>::min())

        && (std::abs(v_[2] - other.v_[2]) <= std::numeric_limits<T>::epsilon() * std::abs(v_[2] + other.v_[2]) * ulp || std::abs(v_[2] - other.v_[2]) < std::numeric_limits<T>::min());
}
