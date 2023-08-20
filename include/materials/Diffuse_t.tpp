#include "entities/RandomGenerator_t.hpp"
#include <cmath>
#include <numbers>

template<typename T>
AGPTracer::Materials::Diffuse_t<T>::Diffuse_t(AGPTracer::Entities::Vec3<T> emission, AGPTracer::Entities::Vec3<T> colour, T roughness) :
        emission_(emission), colour_(colour), roughness_(roughness), unif_(std::uniform_real_distribution<T>(0, 1)) {}

template<typename T>
template<class S>
auto APTracer::Materials::Diffuse_t<T>::bounce(std::array<T, 2> uv, const S& hit_obj, AGPTracer::Entities::Ray_t<T>& ray) -> void {
    AGPTracer::Entities::Vec3<T> normal = hit_obj->normal(ray.time_, uv);

    const T rand1  = unif_(APTracer::Entities::rng()) * 2.0 * std::numbers::pi_v<T>;
    const T rand2  = unif_(APTracer::Entities::rng());
    const T rand2s = sqrt(rand2);

    if (normal.dot(ray.direction_) > 0.0) {
        normal *= -1.0;
    }

    const AGPTracer::Entities::Vec3<T> axis = std::abs(normal[0]) > 0.1 ? AGPTracer::Entities::Vec3<T>(0.0, 1.0, 0.0) : AGPTracer::Entities::Vec3<T>(1.0, 0.0, 0.0);

    const AGPTracer::Entities::Vec3<T> u = axis.cross(normal).normalize_inplace();
    const AGPTracer::Entities::Vec3<T> v = normal.cross(u).normalize_inplace(); // wasn't normalized before

    const AGPTracer::Entities::Vec3<T> newdir = (u * cos(rand1) * rand2s + v * sin(rand1) * rand2s + normal * sqrt(1.0 - rand2)).normalize_inplace();

    ray.origin_ += ray.direction_ * ray.dist_ + normal * 0.00001; //*ray.dist_; // Made EPSILON relative, check // well guess what wasn't a good idea
    ray.direction_ = newdir;

    ray.colour_ += ray.mask_ * emission_;
    ray.mask_ *= colour_ * pow(newdir.dot(normal), roughness_);
}
