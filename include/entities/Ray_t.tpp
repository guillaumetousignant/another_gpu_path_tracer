// #include "entities/Material_t.h"
// #include "entities/Medium_t.h"
// #include "entities/Scene_t.h"
// #include "entities/Skybox_t.h"
// #include "entities/Shape_t.h"

// using AGPTracer::Entities::Medium_t;
// using AGPTracer::Entities::Skybox_t;
// using AGPTracer::Entities::Scene_t;
// using AGPTracer::Entities::Shape_t;

template<typename T>
constexpr AGPTracer::Entities::Ray_t<T>::Ray_t(const AGPTracer::Entities::Vec3<T>& origin,
                                               const AGPTracer::Entities::Vec3<T>& direction,
                                               const AGPTracer::Entities::Vec3<T>& colour,
                                               const AGPTracer::Entities::Vec3<T>& mask
                                               /*std::list<Medium_t*> medium_list*/) :
        origin_(origin), direction_(direction), colour_(colour), mask_(mask), dist_(0), /*medium_list_(std::move(medium_list))*/ time_(1) {}

template<typename T>
constexpr AGPTracer::Entities::Ray_t<T>::Ray_t(const AGPTracer::Entities::Vec3<T>& origin,
                                               const AGPTracer::Entities::Vec3<T>& direction,
                                               const AGPTracer::Entities::Vec3<T>& colour,
                                               const AGPTracer::Entities::Vec3<T>& mask,
                                               /*std::list<Medium_t*> medium_list*/
                                               T time) :
        origin_(origin), direction_(direction), colour_(colour), mask_(mask), dist_(0), /*medium_list_(std::move(medium_list))*/ time_(time) {}

/*template<typename T>
auto AGPTracer::Entities::Ray_t<T>::raycast(const Scene_t* scene, unsigned int max_bounces, const Skybox_t* skybox) -> void {
    unsigned int bounces = 0;

    while ((bounces < max_bounces) && (mask_.magnitudeSquared() > 0.01)) { // Should maybe make magnitudesquared min value lower
        T t;
        std::array<T, 2> uv;

        const Shape_t* hit_obj = scene->intersect(*this, t, uv);

        if (hit_obj == nullptr) {
            colour_ += mask_ * skybox->get(direction_);
            return;
        }
        dist_ = t;
        bounces++;

        if (!medium_list_.front()->scatter(*this)) {
            hit_obj->material_->bounce(uv, hit_obj, *this);
        }
    }
}

template<typename T>
auto AGPTracer::Entities::Ray_t<T>::add_to_mediums(Medium_t* medium) -> void {
    for (auto i = medium_list_.begin(); i != medium_list_.end(); ++i) {
        if ((*i)->priority_ <= medium->priority_) {
            medium_list_.insert(i, medium);
            return;
        }
    }
    medium_list_.push_back(medium);
}

template<typename T>
auto AGPTracer::Entities::Ray_t<T>::remove_from_mediums(Medium_t* medium) -> void {
    for (auto i = medium_list_.begin(); i != medium_list_.end(); ++i) {
        if (*i == medium) {
            medium_list_.erase(i);
            return;
        }
    }
}*/