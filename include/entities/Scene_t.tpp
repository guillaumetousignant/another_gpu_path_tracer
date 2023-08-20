#include <algorithm>
#include <limits>

template<typename T, typename S>
AGPTracer::Entities::Scene_t<T, S>::Scene_t(S shape) : geometry_(cl::sycl::range<1>{1}) {
    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::write> host_accessor(geometry_, cl::sycl::no_init);
    host_accessor[0] = shape;
}

template<typename T, typename S>
AGPTracer::Entities::Scene_t<T, S>::Scene_t(std::span<S> shapes) : geometry_(shapes.size()) {
    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::write> host_accessor(geometry_, cl::sycl::no_init);
    for (size_t i = 0; i < geometry_.get_range()[0]; ++i) {
        host_accessor[i] = shapes[i];
    }
}

/*template<typename T, typename S>
AGPTracer::Entities::Scene_t<T, S>::Scene_t(MeshTop_t* mesh) : geometry_(mesh->triangles_.size()) {
    for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
        geometry_[i] = mesh->triangles_[i].get();
    }
}

template<typename T, typename S>
AGPTracer::Entities::Scene_t<T, S>::Scene_t(const std::vector<MeshTop_t*>& meshes) {
    size_t additional_size = 0;
    for (const auto& mesh: meshes) {
        additional_size += mesh->triangles_.size();
    }

    geometry_ = std::vector<Shape_t*>(additional_size);

    size_t index = 0;
    for (const auto& mesh: meshes) {
        for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
            geometry_[index + i] = mesh->triangles_[i].get();
        }
        index += mesh->triangles_.size();
    }
}*/

template<typename T, typename S>
AGPTracer::Entities::Scene_t<T, S>::Scene_t(const Scene_t& other) : geometry_(other.geometry_) {
    // acc_ = other.acc_->clone();
}

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::operator=(const Scene_t& other) -> Scene_t& {
    if (this != &other) {
        *this = Scene_t(other);
    }
    return *this;
}

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::add(S shape) -> void {
    cl::sycl::buffer<S, 1> geometry(cl::sycl::range<1>{geometry_.get_range()[0] + 1});

    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::write> new_host_accessor(geometry, cl::sycl::no_init);
    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::read> old_host_accessor(geometry_);

    for (size_t i = 0; i < geometry_.get_range()[0]; ++i) {
        new_host_accessor[i] = old_host_accessor[i];
    }
    new_host_accessor[geometry_.get_range()[0]] = shape;

    geometry_ = std::move(geometry);
}

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::add(std::span<S> shapes) -> void {
    cl::sycl::buffer<S, 1> geometry(cl::sycl::range<1>{geometry_.get_range()[0] + shapes.size()});

    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::write> new_host_accessor(geometry, cl::sycl::no_init);
    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::read> old_host_accessor(geometry_);

    for (size_t i = 0; i < geometry_.get_range()[0]; ++i) {
        new_host_accessor[i] = old_host_accessor[i];
    }
    for (size_t i = 0; i < shapes.size(); ++i) {
        new_host_accessor[geometry_.get_range()[0] + i] = shapes[i];
    }

    geometry_ = std::move(geometry);
}

/*template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::add(MeshTop_t* mesh) -> void {

    const size_t index = geometry_.size();
    geometry_.resize(geometry_.size() + mesh->triangles_.size());

    for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
        geometry_[index + i] = mesh->triangles_[i].get();
    }
}

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::add(const std::vector<MeshTop_t*>& meshes) -> void {
    size_t additional_size = 0;
    for (const auto& mesh: meshes) {
        additional_size += mesh->triangles_.size();
    }

    size_t index = geometry_.size();
    geometry_.resize(geometry_.size() + additional_size);

    for (const auto& mesh: meshes) {
        for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
            geometry_[index + i] = mesh->triangles_[i].get();
        }
        index += mesh->triangles_.size();
    }
}*/

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::remove(S shape) -> void {
    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::read> old_host_accessor(geometry_);
    for (size_t i = 0; i < geometry_.get_range()[0]; ++i) {
        if (old_host_accessor[i] == shape) {
            cl::sycl::buffer<S, 1> geometry(cl::sycl::range<1>{geometry_.get_range()[0] - 1});
            const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::write> new_host_accessor(geometry, cl::sycl::no_init);

            for (size_t j = 0; j < i; ++j) {
                new_host_accessor[j] = old_host_accessor[j];
            }
            for (size_t j = i; j < geometry_.get_range()[0] - 1; ++j) {
                new_host_accessor[j] = old_host_accessor[j + 1];
            }

            geometry_ = std::move(geometry);
            break;
        }
    }
}

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::remove(std::span<S> shapes) -> void {
    const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::read> old_host_accessor(geometry_);
    size_t n_shapes_to_remove = 0;
    for (size_t i = 0; i < geometry_.get_range()[0]; ++i) {
        for (size_t k = 0; k < shapes.size(); ++k) {
            if (old_host_accessor[i] == shapes[k]) {
                ++n_shapes_to_remove;
                break;
            }
        }
    }
    if (n_shapes_to_remove > 0) {
        cl::sycl::buffer<S, 1> geometry(cl::sycl::range<1>{geometry_.get_range()[0] - n_shapes_to_remove});
        const cl::sycl::host_accessor<S, 1, cl::sycl::access_mode::write> new_host_accessor(geometry, cl::sycl::no_init);
        size_t index = 0;
        for (size_t i = 0; i < geometry_.get_range()[0]; ++i) {
            bool found = false;
            for (size_t k = 0; k < shapes.size(); ++k) {
                if (old_host_accessor[i] == shapes[k]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                new_host_accessor[index] = old_host_accessor[i];
                ++index;
            }
        }

        geometry_ = std::move(geometry);
    }
}

/*template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::remove(MeshTop_t* mesh) -> void {
    if (!mesh->triangles_.empty()) {
        const auto triangle = std::find(geometry_.begin(), geometry_.end(), mesh->triangles_[0].get());
        if (triangle != geometry_.end()) {
            geometry_.erase(triangle, triangle + static_cast<long long>(mesh->triangles_.size()));
        }
    }
}

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::remove(const std::vector<MeshTop_t*>& meshes) -> void {
    for (const auto& mesh: meshes) {
        if (!mesh->triangles_.empty()) {
            const auto triangle = std::find(geometry_.begin(), geometry_.end(), mesh->triangles_[0].get());
            if (triangle != geometry_.end()) {
                geometry_.erase(triangle, triangle + static_cast<long long>(mesh->triangles_.size()));
            }
        }
    }
}*/

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::update(cl::sycl::queue& queue) -> void {
    // Size of index space for kernel
    const cl::sycl::range<1> num_work_items{geometry_.get_range()};

    // Submitting command group(work) to queue
    queue.submit([&](cl::sycl::handler& cgh) {
        // Getting read write access to the buffer on a device
        auto accessor = geometry_.get_access<cl::sycl::access::mode::read_write>(cgh);

        // Executing kernel
        cgh.parallel_for<class UpdateScene>(num_work_items, [=](cl::sycl::id<1> WIid) { accessor[WIid].update(); });
    });
}

/*template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::build_acc() -> void {
    constexpr size_t grid_max_res          = 128;
    constexpr size_t grid_max_cell_content = 32;
    acc_                                   = std::make_unique<AccelerationMultiGridVector_t>(geometry_, 1, grid_max_res, grid_max_cell_content, 1);
}*/

template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::intersect_brute(cl::sycl::handler& cgh, const Ray_t<T>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<S&> {
    T t_temp = std::numeric_limits<T>::max();
    std::array<T, 2> uv_temp{};

    t = std::numeric_limits<T>::max();
    std::optional<S&> hit_obj{};

    auto accessor = geometry_.get_access<cl::sycl::access::mode::read>(cgh);
    for (auto shape: accessor) {
        if (shape.intersection(ray, t_temp, uv_temp) && (t_temp < t)) {
            hit_obj = shape;
            uv      = uv_temp;
            t       = t_temp;
        }
    }
    return hit_obj;
}

/*template<typename T, typename S>
auto AGPTracer::Entities::Scene_t<T, S>::intersect(const Ray_t& ray, double& t, std::array<double, 2>& uv) const -> Shape_t* {
    return acc_->intersect(ray, t, uv);
}*/

template<typename T, typename S>
template<class K>
auto AGPTracer::Entities::Scene_t<T, S>::raycast(cl::sycl::handler& cgh, Ray_t<T>& ray, unsigned int max_bounces, const K& skybox) const -> void {
    unsigned int bounces = 0;

    constexpr T minimum_mask = 0.01;
    while ((bounces < max_bounces) && (ray.mask_.magnitudeSquared() > minimum_mask)) { // Should maybe make magnitudeSquared min value lower
        T t{};
        std::array<T, 2> uv{};

        // const std::optional<S&>* hit_obj = acc_->intersect(ray, t, uv);
        const std::optional<S&>* hit_obj = intersect_brute(cgh, ray, t, uv);

        if (!hit_obj) {
            ray.colour_ += ray.mask_ * skybox.get(ray.direction_);
            return;
        }
        ray.dist_ = t;
        ++bounces;

        // if (!ray.medium_list_.front()->scatter(ray)) {
        // hit_obj->material_->bounce(uv, hit_obj, ray);
        //}
    }
}
