#include "shapes/Triangle_t.hpp"
// #include "entities/Material_t.h"
#include <algorithm>
#include <cmath>
#include <limits>

using AGPTracer::Entities::Vec3;

AGPTracer::Shapes::Triangle_t::Triangle_t(/*AGPTracer::Entities::Material_t *material,*/ std::array<Vec3<double>, 3> points,
                                          const std::optional<std::array<Vec3<double>, 3>> normals,
                                          const std::optional<std::array<double, 6>> texcoord) :
        points_orig_{points} {

    const Vec3<double> nor = (points_orig_[1] - points_orig_[0]).cross(points_orig_[2] - points_orig_[0]).normalize_inplace();
    normals_orig_          = normals.value_or(std::array<Vec3<double>, 3>{nor, nor, nor});

    texture_coordinates_ = texcoord.value_or(std::array<double, 6>{0, 1, 0, 0, 1, 0});

    points_ = {transformation_.multVec(points_orig_[0]), transformation_.multVec(points_orig_[1]), transformation_.multVec(points_orig_[2])};

    normals_ = {transformation_.multDir(normals_orig_[0]), transformation_.multDir(normals_orig_[1]), transformation_.multDir(normals_orig_[2])}; // was transformation_ before

    v0v1_ = points_[1] - points_[0];
    v0v2_ = points_[2] - points_[0];

    const std::array<double, 2> tuv0v1 = {texture_coordinates_[2] - texture_coordinates_[0], texture_coordinates_[3] - texture_coordinates_[1]};
    const std::array<double, 2> tuv0v2 = {texture_coordinates_[4] - texture_coordinates_[0], texture_coordinates_[5] - texture_coordinates_[1]};

    if (std::abs(tuv0v1[0] * tuv0v2[1] - tuv0v1[1] * tuv0v2[0]) >= std::numeric_limits<double>::min()) {
        const double invdet = 1.0 / (tuv0v1[0] * tuv0v2[1] - tuv0v1[1] * tuv0v2[0]);
        tuv_to_world_       = {invdet * -tuv0v2[0], invdet * tuv0v1[0]};
    }
    else {
        tuv_to_world_ = {1.0, 0.0};
    }
    tangent_vec_ = v0v1_ * tuv_to_world_[0] + v0v2_ * tuv_to_world_[1];
}

auto AGPTracer::Shapes::Triangle_t::update() -> void {
    points_ = {transformation_.multVec(points_orig_[0]), transformation_.multVec(points_orig_[1]), transformation_.multVec(points_orig_[2])};

    normals_ = {transformation_.multDir(normals_orig_[0]), transformation_.multDir(normals_orig_[1]), transformation_.multDir(normals_orig_[2])};

    v0v1_ = points_[1] - points_[0];
    v0v2_ = points_[2] - points_[0];

    tangent_vec_ = v0v1_ * tuv_to_world_[0] + v0v2_ * tuv_to_world_[1];
}

auto AGPTracer::Shapes::Triangle_t::intersection(const AGPTracer::Entities::Ray_t& ray, double& t, std::array<double, 2>& uv) const -> bool {
    const Vec3<double> pvec = ray.direction_.cross(v0v2_);
    const double det        = v0v1_.dot(pvec);

    if (std::abs(det) < std::numeric_limits<double>::min()) {
        return false;
    }

    const double invdet     = 1.0 / det;
    const Vec3<double> tvec = ray.origin_ - points_[0];
    const double u          = tvec.dot(pvec) * invdet;
    uv[0]                   = u;

    if ((u < 0.0) || (u > 1.0)) {
        return false;
    }

    const Vec3<double> qvec = tvec.cross(v0v1_);
    const double v          = ray.direction_.dot(qvec) * invdet;
    uv[1]                   = v;

    if ((v < 0.0) || ((u + v) > 1.0)) {
        return false;
    }

    t = v0v2_.dot(qvec) * invdet;

    if (t < 0.0) {
        return false;
    }

    return true;
}

auto AGPTracer::Shapes::Triangle_t::normaluv(double time, std::array<double, 2> uv, std::array<double, 2>& tuv) const -> Vec3<double> {
    const Vec3<double> distance = Vec3<double>(1.0 - uv[0] - uv[1], uv[0], uv[1]);
    // Matrix multiplication, optimise.
    tuv = {distance[0] * texture_coordinates_[0] + distance[1] * texture_coordinates_[2] + distance[2] * texture_coordinates_[4],
           distance[0] * texture_coordinates_[1] + distance[1] * texture_coordinates_[3] + distance[2] * texture_coordinates_[5]};
    return {distance[0] * normals_[0][0] + distance[1] * normals_[1][0] + distance[2] * normals_[2][0],
            distance[0] * normals_[0][1] + distance[1] * normals_[1][1] + distance[2] * normals_[2][1],
            distance[0] * normals_[0][2] + distance[1] * normals_[1][2] + distance[2] * normals_[2][2]};
}

auto AGPTracer::Shapes::Triangle_t::normal(double time, std::array<double, 2> uv) const -> Vec3<double> {
    const Vec3<double> distance = Vec3<double>(1.0 - uv[0] - uv[1], uv[0], uv[1]);
    return {distance[0] * normals_[0][0] + distance[1] * normals_[1][0] + distance[2] * normals_[2][0],
            distance[0] * normals_[0][1] + distance[1] * normals_[1][1] + distance[2] * normals_[2][1],
            distance[0] * normals_[0][2] + distance[1] * normals_[1][2] + distance[2] * normals_[2][2]};
    // Matrix multiplication, optimise.
}

auto AGPTracer::Shapes::Triangle_t::normal_uv_tangent(double time, std::array<double, 2> uv, std::array<double, 2>& tuv, Vec3<double>& tangentvec) const -> Vec3<double> {
    const Vec3<double> distance = Vec3<double>(1.0 - uv[0] - uv[1], uv[0], uv[1]);
    // Matrix multiplication, optimise.
    tuv = {distance[0] * texture_coordinates_[0] + distance[1] * texture_coordinates_[2] + distance[2] * texture_coordinates_[4],
           distance[0] * texture_coordinates_[1] + distance[1] * texture_coordinates_[3] + distance[2] * texture_coordinates_[5]};

    const Vec3<double> normalvec = Vec3<double>(distance[0] * normals_[0][0] + distance[1] * normals_[1][0] + distance[2] * normals_[2][0],
                                                distance[0] * normals_[0][1] + distance[1] * normals_[1][1] + distance[2] * normals_[2][1],
                                                distance[0] * normals_[0][2] + distance[1] * normals_[1][2] + distance[2] * normals_[2][2]);

    tangentvec = tangent_vec_.cross(normalvec).normalize_inplace();
    return normalvec;
}

auto AGPTracer::Shapes::Triangle_t::normal_face(double time) const -> Vec3<double> {
    return v0v1_.cross(v0v2_).normalize_inplace();
}

auto AGPTracer::Shapes::Triangle_t::mincoord() const -> Vec3<double> {
    return points_[0].getMin(points_[1]).min(points_[2]);
}

auto AGPTracer::Shapes::Triangle_t::maxcoord() const -> Vec3<double> {
    return points_[0].getMax(points_[1]).max(points_[2]);
}