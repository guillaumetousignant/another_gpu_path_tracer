#include "entities/RandomGenerator_t.hpp"
#include <cmath>
#include <numbers>

template<typename T>
AGPTracer::Materials::Diffuse_t<T>::Diffuse_t(AGPTracer::Entities::Vec3<T> emission, AGPTracer::Entities::Vec3<T> colour, T roughness) :
        emission_(emission), colour_(colour), roughness_(roughness), unif_(std::uniform_real_distribution<T>(0, 1)) {}

template<typename T>
template<class R, template<typename> typename S, size_t N>
requires AGPTracer::Entities::Shape<S, T> auto AGPTracer::Materials::Diffuse_t<T>::bounce(R& random_generator, std::array<T, 2> uv, const S<T>& hit_obj, AGPTracer::Entities::Ray_t<T, N>& ray)
    -> void {
    AGPTracer::Entities::Vec3<T> normal = hit_obj.normal(ray.time_, uv);

    const T rand1  = unif_(random_generator()) * T{2} * std::numbers::pi_v<T>;
    const T rand2  = unif_(random_generator());
    const T rand2s = sqrt(rand2);

    if (normal.dot(ray.direction_) > T{0}) {
        normal = -normal;
    }

    const AGPTracer::Entities::Vec3<T> axis = std::abs(normal[0]) > T{0.1} ? AGPTracer::Entities::Vec3<T>(T{0}, T{1}, T{0}) : AGPTracer::Entities::Vec3<T>(T{1}, T{0}, T{0});

    const AGPTracer::Entities::Vec3<T> u = axis.cross(normal).normalize_inplace();
    const AGPTracer::Entities::Vec3<T> v = normal.cross(u).normalize_inplace(); // wasn't normalized before

    const AGPTracer::Entities::Vec3<T> newdir = (u * cos(rand1) * rand2s + v * sin(rand1) * rand2s + normal * sqrt(T{1} - rand2)).normalize_inplace();

    ray.origin_ += ray.direction_ * ray.dist_ + normal * T{0.00001}; //*ray.dist_; // Made EPSILON relative, check // well guess what wasn't a good idea
    ray.direction_ = newdir;

    ray.colour_ += ray.mask_ * emission_;
    ray.mask_ *= colour_ * pow(newdir.dot(normal), roughness_);
}
