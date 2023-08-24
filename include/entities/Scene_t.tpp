#include <algorithm>
#include <limits>

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T>
AGPTracer::Entities::Scene_t<T, S, M, D>::Scene_t(std::span<S<T>> shapes, std::span<M<T>> materials, std::span<D<T>> mediums) :
        shapes_(shapes.size()), materials_(materials.size()), mediums_(mediums.size()) {
    const sycl::host_accessor<S<T>, 1, sycl::access_mode::write> shape_accessor(shapes_, sycl::no_init);
    std::copy(shapes.begin(), shapes.end(), shape_accessor.begin());

    const sycl::host_accessor<M<T>, 1, sycl::access_mode::write> material_accessor(materials_, sycl::no_init);
    std::copy(materials.begin(), materials.end(), material_accessor.begin());

    const sycl::host_accessor<D<T>, 1, sycl::access_mode::write> medium_accessor(mediums_, sycl::no_init);
    std::copy(mediums.begin(), mediums.end(), medium_accessor.begin());
}

/*template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
AGPTracer::Entities::Scene_t<T, S, M, D>::Scene_t(MeshTop_t* mesh) : shapes_(mesh->triangles_.size()) {
    for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
        shapes_[i] = mesh->triangles_[i].get();
    }
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
AGPTracer::Entities::Scene_t<T, S, M, D>::Scene_t(const std::vector<MeshTop_t*>& meshes) {
    size_t additional_size = 0;
    for (const auto& mesh: meshes) {
        additional_size += mesh->triangles_.size();
    }

    shapes_ = std::vector<Shape_t*>(additional_size);

    size_t index = 0;
    for (const auto& mesh: meshes) {
        for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
            shapes_[index + i] = mesh->triangles_[i].get();
        }
        index += mesh->triangles_.size();
    }
}*/

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(S<T> shape) -> void {
    sycl::buffer<S<T>, 1> new_shapes(sycl::range<1>{shapes_.get_range()[0] + 1});

    const sycl::host_accessor<S<T>, 1, sycl::access_mode::write> new_host_accessor(new_shapes, sycl::no_init);
    const sycl::host_accessor<S<T>, 1, sycl::access_mode::read> old_host_accessor(shapes_);

    std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
    new_host_accessor[shapes_.get_range()[0]] = shape;

    shapes_ = std::move(new_shapes);
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(std::span<S<T>> shapes) -> void {
    sycl::buffer<S<T>, 1> new_shapes(sycl::range<1>{shapes_.get_range()[0] + shapes.size()});

    const sycl::host_accessor<S<T>, 1, sycl::access_mode::write> new_host_accessor(new_shapes, sycl::no_init);
    const sycl::host_accessor<S<T>, 1, sycl::access_mode::read> old_host_accessor(shapes_);

    std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
    std::copy(shapes.begin(), shapes.end(), new_host_accessor.begin() + shapes_.get_range()[0]);

    shapes_ = std::move(new_shapes);
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(M<T> material) -> void {
    sycl::buffer<M<T>, 1> new_materials(sycl::range<1>{materials_.get_range()[0] + 1});

    const sycl::host_accessor<M<T>, 1, sycl::access_mode::write> new_host_accessor(new_materials, sycl::no_init);
    const sycl::host_accessor<M<T>, 1, sycl::access_mode::read> old_host_accessor(materials_);

    std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
    new_host_accessor[materials_.get_range()[0]] = material;

    materials_ = std::move(new_materials);
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(std::span<M<T>> materials)
    -> void {
    sycl::buffer<M<T>, 1> new_materials(sycl::range<1>{materials_.get_range()[0] + materials.size()});

    const sycl::host_accessor<M<T>, 1, sycl::access_mode::write> new_host_accessor(new_materials, sycl::no_init);
    const sycl::host_accessor<M<T>, 1, sycl::access_mode::read> old_host_accessor(materials_);

    std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
    std::copy(materials.begin(), materials.end(), new_host_accessor.begin() + materials_.get_range()[0]);

    materials_ = std::move(new_materials);
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(D<T> medium) -> void {
    sycl::buffer<D<T>, 1> new_mediums(sycl::range<1>{mediums_.get_range()[0] + 1});

    const sycl::host_accessor<D<T>, 1, sycl::access_mode::write> new_host_accessor(new_mediums, sycl::no_init);
    const sycl::host_accessor<D<T>, 1, sycl::access_mode::read> old_host_accessor(mediums_);

    std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
    new_host_accessor[mediums_.get_range()[0]] = medium;

    mediums_ = std::move(new_mediums);
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(std::span<D<T>> mediums)
    -> void {
    sycl::buffer<D<T>, 1> new_mediums(sycl::range<1>{mediums_.get_range()[0] + mediums.size()});

    const sycl::host_accessor<D<T>, 1, sycl::access_mode::write> new_host_accessor(new_mediums, sycl::no_init);
    const sycl::host_accessor<D<T>, 1, sycl::access_mode::read> old_host_accessor(mediums_);

    std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
    std::copy(mediums.begin(), mediums.end(), new_host_accessor.begin() + mediums_.get_range()[0]);

    mediums_ = std::move(new_mediums);
}

/*template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(MeshTop_t* mesh) -> void {

    const size_t index = shapes_.size();
    shapes_.resize(shapes_.size() + mesh->triangles_.size());

    for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
        shapes_[index + i] = mesh->triangles_[i].get();
    }
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::add(const std::vector<MeshTop_t*>& meshes) -> void {
    size_t additional_size = 0;
    for (const auto& mesh: meshes) {
        additional_size += mesh->triangles_.size();
    }

    size_t index = shapes_.size();
    shapes_.resize(shapes_.size() + additional_size);

    for (const auto& mesh: meshes) {
        for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
            shapes_[index + i] = mesh->triangles_[i].get();
        }
        index += mesh->triangles_.size();
    }
}*/

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(S<T> shape) -> void {
    const sycl::host_accessor<S<T>, 1, sycl::access_mode::read_write> host_accessor(shapes_);

    host_accessor.erase(std::remove(host_accessor.begin(), host_accessor.end(), shape), host_accessor.end());
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(std::span<S<T>> shapes)
    -> void {
    const sycl::host_accessor<S<T>, 1, sycl::access_mode::read_write> host_accessor(shapes_);
    auto end = host_accessor.end();

    for (const auto& shape: shapes) {
        end = std::remove(host_accessor.begin(), end, shape);
    }

    host_accessor.erase(end, host_accessor.end());
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(M<T> material) -> void {
    const sycl::host_accessor<M<T>, 1, sycl::access_mode::read_write> host_accessor(materials_);

    host_accessor.erase(std::remove(host_accessor.begin(), host_accessor.end(), material), host_accessor.end());
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(std::span<M<T>> materials)
    -> void {
    const sycl::host_accessor<M<T>, 1, sycl::access_mode::read_write> host_accessor(materials_);
    auto end = host_accessor.end();

    for (const auto& material: materials) {
        end = std::remove(host_accessor.begin(), end, material);
    }

    host_accessor.erase(end, host_accessor.end());
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(D<T> medium) -> void {
    const sycl::host_accessor<D<T>, 1, sycl::access_mode::read_write> host_accessor(mediums_);

    host_accessor.erase(std::remove(host_accessor.begin(), host_accessor.end(), medium), host_accessor.end());
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(std::span<D<T>> mediums)
    -> void {
    const sycl::host_accessor<D<T>, 1, sycl::access_mode::read_write> host_accessor(mediums_);
    auto end = host_accessor.end();

    for (const auto& medium: mediums) {
        end = std::remove(host_accessor.begin(), end, medium);
    }

    host_accessor.erase(end, host_accessor.end());
}

/*template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(MeshTop_t* mesh) -> void {
    if (!mesh->triangles_.empty()) {
        const auto triangle = std::find(shapes_.begin(), shapes_.end(), mesh->triangles_[0].get());
        if (triangle != shapes_.end()) {
            shapes_.erase(triangle, triangle + static_cast<long long>(mesh->triangles_.size()));
        }
    }
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::remove(const std::vector<MeshTop_t*>& meshes) -> void {
    for (const auto& mesh: meshes) {
        if (!mesh->triangles_.empty()) {
            const auto triangle = std::find(shapes_.begin(), shapes_.end(), mesh->triangles_[0].get());
            if (triangle != shapes_.end()) {
                shapes_.erase(triangle, triangle + static_cast<long long>(mesh->triangles_.size()));
            }
        }
    }
}*/

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::update(sycl::queue& queue) -> void {
    // Size of index space for kernel
    const sycl::range<1> num_work_items{shapes_.get_range()};

    // Submitting command group(work) to queue
    queue.submit([&](sycl::handler& cgh) {
        // Getting read write access to the buffer on a device
        auto accessor = shapes_.get_access<sycl::access::mode::read_write>(cgh);

        // Executing kernel
        cgh.parallel_for<class UpdateScene>(num_work_items, [=](sycl::id<1> WIid) { accessor[WIid].update(); });
    });
}

/*template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::build_acc() -> void {
    constexpr size_t grid_max_res          = 128;
    constexpr size_t grid_max_cell_content = 32;
    acc_                                   = std::make_unique<AccelerationMultiGridVector_t>(shapes_, 1, grid_max_res, grid_max_cell_content, 1);
}*/

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> template<size_t N>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::intersect_brute(sycl::handler& cgh, const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<std::reference_wrapper<S<T>>> {
    T t_temp = std::numeric_limits<T>::max();
    std::array<T, 2> uv_temp{};

    t = std::numeric_limits<T>::max();
    std::optional<std::reference_wrapper<S<T>>> hit_obj{};

    auto accessor = shapes_.get_access<sycl::access::mode::read>(cgh);
    for (auto shape: accessor) {
        if (shape.intersection(ray, t_temp, uv_temp) && (t_temp < t)) {
            hit_obj = shape;
            uv      = uv_temp;
            t       = t_temp;
        }
    }
    return hit_obj;
}

/*template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::intersect(const Ray_t& ray, double& t, std::array<double, 2>& uv) const -> Shape_t* {
    return acc_->intersect(ray, t, uv);
}*/

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> template<class R, template<typename> typename K, size_t N>
requires AGPTracer::Entities::Skybox<K, T> auto
AGPTracer::Entities::Scene_t<T, S, M, D>::raycast(R& random_generator, sycl::handler& cgh, Ray_t<T, N>& ray, unsigned int max_bounces, const K<T>& skybox) const -> void {
    unsigned int bounces = 0;

    constexpr T minimum_mask = 0.01;
    while ((bounces < max_bounces) && (ray.mask_.magnitudeSquared() > minimum_mask)) { // Should maybe make magnitudeSquared min value lower
        T t{};
        std::array<T, 2> uv{};

        // const std::optional<std::reference_wrapper<S<T>>> hit_obj = acc_->intersect(ray, t, uv);
        const std::optional<std::reference_wrapper<S<T>>> hit_obj = intersect_brute(cgh, ray, t, uv);

        if (!hit_obj) {
            ray.colour_ += ray.mask_ * skybox.get(ray.direction_);
            return;
        }
        ray.dist_ = t;
        ++bounces;

        auto medium_accessor = mediums_.get_access<sycl::access::mode::read>(cgh);
        if (!medium_accessor[ray.medium_list_[0]].scatter(ray)) {
            auto material_accessor = materials_.get_access<sycl::access::mode::read>(cgh);
            material_accessor[hit_obj->material_].bounce(random_generator, uv, *hit_obj, ray);
        }
    }
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::getAccessor(sycl::handler& cgh)
    -> Accessor_t {
    return Accessor_t(cgh, shapes_, materials_, mediums_);
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T>
AGPTracer::Entities::Scene_t<T, S, M, D>::Accessor_t::Accessor_t(sycl::handler& cgh, sycl::buffer<S<T>, 1>& shapes, sycl::buffer<M<T>, 1>& materials, sycl::buffer<D<T>, 1>& mediums) :
        shapes_(shapes.template get_access<sycl::access::mode::read>(cgh)),
        materials_(materials.template get_access<sycl::access::mode::read>(cgh)),
        mediums_(mediums.template get_access<sycl::access::mode::read>(cgh)) {}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> template<class R, template<typename> typename K, size_t N>
requires AGPTracer::Entities::Skybox<K, T> auto AGPTracer::Entities::Scene_t<T, S, M, D>::Accessor_t::raycast(R& random_generator, Ray_t<T, N>& ray, unsigned int max_bounces, const K<T>& skybox) const
    -> void {
    unsigned int bounces = 0;

    constexpr T minimum_mask = 0.01;
    while ((bounces < max_bounces) && (ray.mask_.magnitudeSquared() > minimum_mask)) { // Should maybe make magnitudeSquared min value lower
        T t{};
        std::array<T, 2> uv{};

        // const std::optional<std::reference_wrapper<S<T>>> hit_obj = acc_->intersect(ray, t, uv);
        const std::optional<std::reference_wrapper<S<T>>> hit_obj = intersect_brute(ray, t, uv);

        if (!hit_obj) {
            ray.colour_ += ray.mask_ * skybox.get(ray.direction_);
            return;
        }
        ray.dist_ = t;
        ++bounces;

        if (!mediums_[ray.medium_list_.mediums_[0]].scatter(ray)) {
            materials_[hit_obj->get().material_].bounce(random_generator, uv, hit_obj->get(), ray);
        }
    }
}

template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T>&& AGPTracer::Entities::Material<M, T>&& AGPTracer::Entities::Medium<D, T> template<size_t N>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::Accessor_t::intersect_brute(const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<std::reference_wrapper<S<T>>> {
    T t_temp = std::numeric_limits<T>::max();
    std::array<T, 2> uv_temp{};

    t = std::numeric_limits<T>::max();
    std::optional<std::reference_wrapper<S<T>>> hit_obj{};

    for (auto shape: shapes_) {
        if (shape.intersection(ray, t_temp, uv_temp) && (t_temp < t)) {
            hit_obj = shape;
            uv      = uv_temp;
            t       = t_temp;
        }
    }
    return hit_obj;
}

/*template<typename T, template<typename> typename S, template<typename> typename M, template<typename> typename D>
requires AGPTracer::Entities::Shape<S, T> && AGPTracer::Entities::Medium<D, T>
auto AGPTracer::Entities::Scene_t<T, S, M, D>::Accessor_t::intersect(double& t, std::array<double, 2>& uv) const -> Shape_t* {
    return acc_->intersect(ray, t, uv);
}*/
