// #include "entities/Material_t.h"
#include <algorithm>
#include <cmath>
#include <limits>

template<typename T>
AGPTracer::Shapes::Triangle_t<T>::Triangle_t(size_t material,
                                             Entities::TransformMatrix_t<T> transform_matrix,
                                             std::array<AGPTracer::Entities::Vec3<T>, 3> points, // In c++26 sqrt is constexpr
                                             const std::optional<std::array<AGPTracer::Entities::Vec3<T>, 3>> normals,
                                             const std::optional<std::array<T, 6>> texcoord) :
        material_(material), transformation_(std::move(transform_matrix)), points_orig_{points} {

    const AGPTracer::Entities::Vec3<T> nor = (points_orig_[1] - points_orig_[0]).cross(points_orig_[2] - points_orig_[0]).normalize_inplace();
    normals_orig_                          = normals.value_or(std::array<AGPTracer::Entities::Vec3<T>, 3>{nor, nor, nor});

    texture_coordinates_ = texcoord.value_or(std::array<T, 6>{0, 1, 0, 0, 1, 0});

    points_ = {transformation_.multVec(points_orig_[0]), transformation_.multVec(points_orig_[1]), transformation_.multVec(points_orig_[2])};

    normals_ = {transformation_.multDir(normals_orig_[0]), transformation_.multDir(normals_orig_[1]), transformation_.multDir(normals_orig_[2])}; // was transformation_ before

    v0v1_ = points_[1] - points_[0];
    v0v2_ = points_[2] - points_[0];

    const std::array<T, 2> tuv0v1 = {texture_coordinates_[2] - texture_coordinates_[0], texture_coordinates_[3] - texture_coordinates_[1]};
    const std::array<T, 2> tuv0v2 = {texture_coordinates_[4] - texture_coordinates_[0], texture_coordinates_[5] - texture_coordinates_[1]};

    if (std::abs(tuv0v1[0] * tuv0v2[1] - tuv0v1[1] * tuv0v2[0]) >= std::numeric_limits<T>::min()) {
        const T invdet = 1.0 / (tuv0v1[0] * tuv0v2[1] - tuv0v1[1] * tuv0v2[0]);
        tuv_to_world_  = {invdet * -tuv0v2[0], invdet * tuv0v1[0]};
    }
    else {
        tuv_to_world_ = {1.0, 0.0};
    }
    tangent_vec_ = v0v1_ * tuv_to_world_[0] + v0v2_ * tuv_to_world_[1];
}

template<typename T>
auto AGPTracer::Shapes::Triangle_t<T>::update() -> void { // In c++26 sqrt is constexpr
    points_  = {transformation_.multVec(points_orig_[0]), transformation_.multVec(points_orig_[1]), transformation_.multVec(points_orig_[2])};
    normals_ = {transformation_.multDir(normals_orig_[0]), transformation_.multDir(normals_orig_[1]), transformation_.multDir(normals_orig_[2])};

    v0v1_ = points_[1] - points_[0];
    v0v2_ = points_[2] - points_[0];

    tangent_vec_ = v0v1_ * tuv_to_world_[0] + v0v2_ * tuv_to_world_[1];
}

template<typename T>
template<size_t N>
constexpr auto AGPTracer::Shapes::Triangle_t<T>::intersection(const AGPTracer::Entities::Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> bool {
    const AGPTracer::Entities::Vec3<T> pvec = ray.direction_.cross(v0v2_);
    const T det                             = v0v1_.dot(pvec);

    if (std::abs(det) < std::numeric_limits<T>::min()) {
        return false;
    }

    const T invdet                          = 1.0 / det;
    const AGPTracer::Entities::Vec3<T> tvec = ray.origin_ - points_[0];
    const T u                               = tvec.dot(pvec) * invdet;
    uv[0]                                   = u;

    if ((u < 0.0) || (u > 1.0)) {
        return false;
    }

    const AGPTracer::Entities::Vec3<T> qvec = tvec.cross(v0v1_);
    const T v                               = ray.direction_.dot(qvec) * invdet;
    uv[1]                                   = v;

    if ((v < 0.0) || ((u + v) > 1.0)) {
        return false;
    }

    t = v0v2_.dot(qvec) * invdet;

    if (t < 0.0) {
        return false;
    }

    return true;
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Shapes::Triangle_t<T>::normal(T2 time, std::array<T, 2> uv) const -> AGPTracer::Entities::Vec3<T> {
    const AGPTracer::Entities::Vec3<T> distance = AGPTracer::Entities::Vec3<T>(1.0 - uv[0] - uv[1], uv[0], uv[1]);
    return {distance[0] * normals_[0][0] + distance[1] * normals_[1][0] + distance[2] * normals_[2][0],
            distance[0] * normals_[0][1] + distance[1] * normals_[1][1] + distance[2] * normals_[2][1],
            distance[0] * normals_[0][2] + distance[1] * normals_[1][2] + distance[2] * normals_[2][2]};
    // Matrix multiplication, optimise.
}

template<typename T>
template<class T2>
constexpr auto AGPTracer::Shapes::Triangle_t<T>::normal_uv(T2 time, std::array<T, 2> uv, std::array<T, 2>& tuv) const -> AGPTracer::Entities::Vec3<T> {
    const AGPTracer::Entities::Vec3<T> distance = AGPTracer::Entities::Vec3<T>(1.0 - uv[0] - uv[1], uv[0], uv[1]);
    // Matrix multiplication, optimise.
    tuv = {distance[0] * texture_coordinates_[0] + distance[1] * texture_coordinates_[2] + distance[2] * texture_coordinates_[4],
           distance[0] * texture_coordinates_[1] + distance[1] * texture_coordinates_[3] + distance[2] * texture_coordinates_[5]};
    return {distance[0] * normals_[0][0] + distance[1] * normals_[1][0] + distance[2] * normals_[2][0],
            distance[0] * normals_[0][1] + distance[1] * normals_[1][1] + distance[2] * normals_[2][1],
            distance[0] * normals_[0][2] + distance[1] * normals_[1][2] + distance[2] * normals_[2][2]};
}

template<typename T>
template<class T2>
auto AGPTracer::Shapes::Triangle_t<T>::normal_uv_tangent(T2 time, std::array<T, 2> uv, std::array<T, 2>& tuv, AGPTracer::Entities::Vec3<T>& tangentvec) const // In c++26 sqrt is constexpr
    -> AGPTracer::Entities::Vec3<T> {
    const AGPTracer::Entities::Vec3<T> distance = AGPTracer::Entities::Vec3<T>(1.0 - uv[0] - uv[1], uv[0], uv[1]);
    // Matrix multiplication, optimise.
    tuv = {distance[0] * texture_coordinates_[0] + distance[1] * texture_coordinates_[2] + distance[2] * texture_coordinates_[4],
           distance[0] * texture_coordinates_[1] + distance[1] * texture_coordinates_[3] + distance[2] * texture_coordinates_[5]};

    const AGPTracer::Entities::Vec3<T> normalvec = AGPTracer::Entities::Vec3<T>(distance[0] * normals_[0][0] + distance[1] * normals_[1][0] + distance[2] * normals_[2][0],
                                                                                distance[0] * normals_[0][1] + distance[1] * normals_[1][1] + distance[2] * normals_[2][1],
                                                                                distance[0] * normals_[0][2] + distance[1] * normals_[1][2] + distance[2] * normals_[2][2]);

    tangentvec = tangent_vec_.cross(normalvec).normalize_inplace();
    return normalvec;
}

template<typename T>
template<class T2>
auto AGPTracer::Shapes::Triangle_t<T>::normal_face(T2 time) const -> AGPTracer::Entities::Vec3<T> { // In c++26 sqrt is constexpr
    return v0v1_.cross(v0v2_).normalize_inplace();
}

template<typename T>
constexpr auto AGPTracer::Shapes::Triangle_t<T>::mincoord() const -> AGPTracer::Entities::Vec3<T> {
    return points_[0].getMin(points_[1]).min(points_[2]);
}

template<typename T>
constexpr auto AGPTracer::Shapes::Triangle_t<T>::maxcoord() const -> AGPTracer::Entities::Vec3<T> {
    return points_[0].getMax(points_[1]).max(points_[2]);
}
