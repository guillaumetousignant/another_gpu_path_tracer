module;
#include <array>
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <algorithm>
#include <limits>
#include <sycl/sycl.hpp>

export module scene;

//import acceleration_structure;
import material;
import medium;
import ray;
import shape;
import skybox;
import diffuse_material;
import non_absorber_medium;
import triangle;

namespace AGPTracer::Entities {

    /**
     * @brief The scene class holds the shapes representing a scene to be rendered, and creates an acceleration structure containing them.
     *
     * Holds an array of shapes to be intersected by rays. Those shapes are sorted in an acceleration structure owned by the scene for
     * faster intersection. The acceleration has to be built using build_acc before intersecting the scene. It can also be intersected
     * without the acceleration structure with intersect_brute, at a much slower pace when there are many shapes. Shapes can be added and
     * removed from the scene, but these operations are costly so should be batched.
     *
     * @tparam T Floating point datatype to use
     * @tparam S Shape making up the scene
     */
    export template<typename T = double, template<typename> typename S = Shapes::Triangle_t, template<typename> typename M = Materials::DiffuseMaterial_t, template<typename> typename D = Mediums::NonAbsorberMedium_t>
    requires Entities::Shape<S, T>&& Entities::Material<M, T>&& Entities::Medium<D, T> class Scene_t {
        public:
            class Accessor_t {
                public:
                    /**
                     * @brief Construct a new Accessor_t object with the given buffers.
                     *
                     * @param cgh Device handler.
                     * @param shapes Shape buffer to access.
                     * @param materials Shape buffer to access.
                     * @param mediums Shape buffer to access.
                     */
                    Accessor_t(sycl::handler& cgh, sycl::buffer<S<T>, 1>& shapes, sycl::buffer<M<T>, 1>& materials, sycl::buffer<D<T>, 1>& mediums) :
                        shapes_(shapes.template get_access<sycl::access::mode::read>(cgh)),
                        materials_(materials.template get_access<sycl::access::mode::read>(cgh)),
                        mediums_(mediums.template get_access<sycl::access::mode::read>(cgh)) {}

                    /**
                     * @brief Intersects the ray with objects in the scene and bounces it on their material.
                     *
                     * This is the main function to compute the colour of a ray. A ray is sent through the scene,
                     * finding the closest object hit. Then, the ray is modified by this object's material.
                     * This change can change ray direction, origin, colour and mask. This process is repeated
                     * up to max_bounces times, or until no object is it, at which point the skybox is intersected.
                     * The ray is also modified by its first medium using the scatter function. If it is scattered,
                     * it won't be bounced on the hit object's material, as it intersects the medium instead of
                     * the object.
                     *
                     * @tparam R Random generator type
                     * @tparam U Random distribution type to use
                     * @tparam K Skybox type to intersect
                     * @tparam N Number of mediums in the ray's medium list
                     * @param[in] rng Random generator used to get random numbers.
                     * @param[in] unif Uniform distribution used to get random numbers.
                     * @param[in] ray Ray to intersect with the scene.
                     * @param[in] max_bounces Upper bound of number of bounces. Number of bounces may be less if no object is hit or ray can't be illuminated anymore.
                     * @param[in] skybox Skybox that will be intersected if no object is hit.
                     */
                    template<class R, template<typename> typename U, template<typename> typename K, size_t N>
                    requires Entities::Skybox<K, T> auto raycast(R& rng, U<T>& unif, Ray_t<T, N>& ray, unsigned int max_bounces, const K<T>& skybox) const -> void {
                        unsigned int bounces = 0;

                        constexpr T minimum_mask = 0.01;
                        while ((bounces < max_bounces) && (ray.mask_.magnitudeSquared() > minimum_mask)) { // Should maybe make magnitudeSquared min value lower
                            T t{};
                            std::array<T, 2> uv{};

                            // const std::optional<std::reference_wrapper<S<T>>> hit_obj = acc_->intersect(ray, t, uv);
                            const std::optional<size_t> hit_obj = intersect_brute(ray, t, uv);

                            if (!hit_obj) {
                                ray.colour_ += ray.mask_ * skybox.get(ray.direction_);
                                return;
                            }
                            ray.dist_ = t;
                            ++bounces;

                            if (!mediums_[ray.medium_list_.mediums_[0]].scatter(rng, unif, ray)) {
                                materials_[shapes_[*hit_obj].material_].bounce(rng, unif, uv, shapes_[*hit_obj], ray);
                            }
                        }
                    }

                    /**
                     * @brief Intersects the scene shapes directly one by one. Not to be used for general operation.
                     *
                     * @tparam N Number of mediums in the ray's medium list
                     * @param[in] ray Ray to be intersected with the scene, using its current origin and direction.
                     * @param[out] t Distance to intersection. It is stored in t if there is an intersection.
                     * @param[out] uv 2D object-space coordinates of the intersection.
                     * @return std::optional<size_t> Index of the intersected shape. Returns none if there is no intersection.
                     */
                    template<size_t N>
                    auto intersect_brute(const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<size_t> {
                        T t_temp = std::numeric_limits<T>::max();
                        std::array<T, 2> uv_temp{};

                        t = std::numeric_limits<T>::max();
                        std::optional<size_t> hit_obj{};

                        for (size_t i = 0; i < shapes_.get_range()[0]; ++i) {
                            if (shapes_[i].intersection(ray, t_temp, uv_temp) && (t_temp < t)) {
                                hit_obj = i;
                                uv      = uv_temp;
                                t       = t_temp;
                            }
                        }

                        return hit_obj;
                    }

                    /**
                     * @brief Intersects the scene using the acceleration structure. Main way to intersect shapes.
                     *
                     * @tparam N Number of mediums in the ray's medium list
                     * @param[in] ray Ray to be intersected with the scene, using its current origin and direction.
                     * @param[out] t Distance to intersection. It is stored in t if there is an intersection.
                     * @param[out] uv 2D object-space coordinates of the intersection.
                     * @return std::optional<std::reference_wrapper<S<T>>> Reference to the intersected shape. Returns none if there is no intersection.
                     */
                    // template<size_t N>
                    /* auto intersect(const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<std::reference_wrapper<S<T>>> {
                        return acc_->intersect(ray, t, uv);
                    }*/

                private:
                    sycl::accessor<S<T>, 1, sycl::access::mode::read> shapes_; /**< @brief Accessor to the shapes.*/
                    sycl::accessor<M<T>, 1, sycl::access::mode::read> materials_; /**< @brief Accessor to the materials.*/
                    sycl::accessor<D<T>, 1, sycl::access::mode::read> mediums_; /**< @brief Accessor to the mediums.*/
            };

            /**
             * @brief Construct a new empty Scene_t object.
             */
            Scene_t() = default;

            /**
             * @brief Construct a new Scene_t object from shapes, materials and mediums.
             *
             * @param shapes Shapes to be added to the scene.
             * @param materials Materials to be added to the scene.
             * @param mediums Mediums to be added to the scene.
             */
            Scene_t(std::span<S<T>> shapes, std::span<M<T>> materials, std::span<D<T>> mediums) :
                    shapes_(shapes.size()), materials_(materials.size()), mediums_(mediums.size()) {
                const sycl::host_accessor<S<T>, 1, sycl::access_mode::write> shape_accessor(shapes_, sycl::no_init);
                std::copy(shapes.begin(), shapes.end(), shape_accessor.begin());

                const sycl::host_accessor<M<T>, 1, sycl::access_mode::write> material_accessor(materials_, sycl::no_init);
                std::copy(materials.begin(), materials.end(), material_accessor.begin());

                const sycl::host_accessor<D<T>, 1, sycl::access_mode::write> medium_accessor(mediums_, sycl::no_init);
                std::copy(mediums.begin(), mediums.end(), medium_accessor.begin());
            }

            /**
             * @brief Construct a new Scene_t object containing a single mesh.
             *
             * @param mesh Mesh to add to the scene.
             */
            /* explicit Scene_t(Shapes::MeshTop_t* mesh) : shapes_(mesh->triangles_.size()) {
                for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
                    shapes_[i] = mesh->triangles_[i].get();
                }
            }*/

            /**
             * @brief Construct a new Scene_t object from multiple meshes.
             *
             * @param meshes Meshes to be added to the scene.
             */
            /* explicit Scene_t(const std::vector<Shapes::MeshTop_t*>& meshes) {
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

            sycl::buffer<S<T>, 1> shapes_; /**< @brief Vector of shapes to be drawn.*/
            sycl::buffer<M<T>, 1> materials_; /**< @brief Vector of materials for the shapes.*/
            sycl::buffer<D<T>, 1> mediums_; /**< @brief Vector of mediums for the materials.*/
            // std::unique_ptr<AccelerationStructure_t> acc_; /**< @brief Acceleration structure containing the shapes, used to accelerate intersection.*/

            /**
             * @brief Adds a single shape to the scene.
             *
             * @param shape Shape to be added to the scene.
             */
            auto add(S<T> shape) -> void {
                sycl::buffer<S<T>, 1> new_shapes(sycl::range<1>{shapes_.get_range()[0] + 1});

                const sycl::host_accessor<S<T>, 1, sycl::access_mode::write> new_host_accessor(new_shapes, sycl::no_init);
                const sycl::host_accessor<S<T>, 1, sycl::access_mode::read> old_host_accessor(shapes_);

                std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
                new_host_accessor[shapes_.get_range()[0]] = shape;

                shapes_ = std::move(new_shapes);
            }

            /**
             * @brief Adds several shapes to the scene.
             *
             * @param shapes Array of shapes to be added to the scene.
             */
            auto add(std::span<S<T>> shapes) -> void {
                sycl::buffer<S<T>, 1> new_shapes(sycl::range<1>{shapes_.get_range()[0] + shapes.size()});

                const sycl::host_accessor<S<T>, 1, sycl::access_mode::write> new_host_accessor(new_shapes, sycl::no_init);
                const sycl::host_accessor<S<T>, 1, sycl::access_mode::read> old_host_accessor(shapes_);

                std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
                std::copy(shapes.begin(), shapes.end(), new_host_accessor.begin() + shapes_.get_range()[0]);

                shapes_ = std::move(new_shapes);
            }

            /**
             * @brief Adds a single material to the scene.
             *
             * @param material Material to be added to the scene.
             */
            auto add(M<T> material) -> void {
                sycl::buffer<M<T>, 1> new_materials(sycl::range<1>{materials_.get_range()[0] + 1});

                const sycl::host_accessor<M<T>, 1, sycl::access_mode::write> new_host_accessor(new_materials, sycl::no_init);
                const sycl::host_accessor<M<T>, 1, sycl::access_mode::read> old_host_accessor(materials_);

                std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
                new_host_accessor[materials_.get_range()[0]] = material;

                materials_ = std::move(new_materials);
            }

            /**
             * @brief Adds several materials to the scene.
             *
             * @param materials Array of materials to be added to the scene.
             */
            auto add(std::span<M<T>> materials) -> void {
                sycl::buffer<M<T>, 1> new_materials(sycl::range<1>{materials_.get_range()[0] + materials.size()});

                const sycl::host_accessor<M<T>, 1, sycl::access_mode::write> new_host_accessor(new_materials, sycl::no_init);
                const sycl::host_accessor<M<T>, 1, sycl::access_mode::read> old_host_accessor(materials_);

                std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
                std::copy(materials.begin(), materials.end(), new_host_accessor.begin() + materials_.get_range()[0]);

                materials_ = std::move(new_materials);
            }

            /**
             * @brief Adds a single medium to the scene.
             *
             * @param medium Medium to be added to the scene.
             */
            auto add(D<T> medium) -> void {
                sycl::buffer<D<T>, 1> new_mediums(sycl::range<1>{mediums_.get_range()[0] + 1});

                const sycl::host_accessor<D<T>, 1, sycl::access_mode::write> new_host_accessor(new_mediums, sycl::no_init);
                const sycl::host_accessor<D<T>, 1, sycl::access_mode::read> old_host_accessor(mediums_);

                std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
                new_host_accessor[mediums_.get_range()[0]] = medium;

                mediums_ = std::move(new_mediums);
            }

            /**
             * @brief Adds several mediums to the scene.
             *
             * @param mediums Array of mediums to be added to the scene.
             */
            auto add(std::span<D<T>> mediums) -> void {
                sycl::buffer<D<T>, 1> new_mediums(sycl::range<1>{mediums_.get_range()[0] + mediums.size()});

                const sycl::host_accessor<D<T>, 1, sycl::access_mode::write> new_host_accessor(new_mediums, sycl::no_init);
                const sycl::host_accessor<D<T>, 1, sycl::access_mode::read> old_host_accessor(mediums_);

                std::copy(old_host_accessor.begin(), old_host_accessor.end(), new_host_accessor.begin());
                std::copy(mediums.begin(), mediums.end(), new_host_accessor.begin() + mediums_.get_range()[0]);

                mediums_ = std::move(new_mediums);
            }

            /**
             * @brief Adds a single mesh to the scene.
             *
             * @param mesh Mesh to be added to the scene.
             */
            /* auto add(Shapes::MeshTop_t* mesh) -> void {
                const size_t index = shapes_.size();
                shapes_.resize(shapes_.size() + mesh->triangles_.size());

                for (size_t i = 0; i < mesh->triangles_.size(); ++i) {
                    shapes_[index + i] = mesh->triangles_[i].get();
                }
            }*/

            /**
             * @brief Adds multiple meshes to the scene.
             *
             * @param meshes Array of meshes to be added to the scene.
             */
            /* auto add(const std::vector<Shapes::MeshTop_t*>& meshes) -> void {
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

            /**
             * @brief Removes a single shape from the scene.
             *
             * @param shape Shape to be removed from the scene.
             */
            auto remove(S<T> shape) -> void {
                const sycl::host_accessor<S<T>, 1, sycl::access_mode::read_write> host_accessor(shapes_);

                host_accessor.erase(std::remove(host_accessor.begin(), host_accessor.end(), shape), host_accessor.end());
            }

            /**
             * @brief Removes multiple shapes from a scene.
             *
             * @param shapes Array of shapes to be removed from the scene.
             */
            auto remove(std::span<S<T>> shapes) -> void {
                const sycl::host_accessor<S<T>, 1, sycl::access_mode::read_write> host_accessor(shapes_);
                auto end = host_accessor.end();

                for (const auto& shape: shapes) {
                    end = std::remove(host_accessor.begin(), end, shape);
                }

                host_accessor.erase(end, host_accessor.end());
            }

            /**
             * @brief Removes a single material from the scene.
             *
             * @param material Material to be removed from the scene.
             */
            auto remove(M<T> material) -> void {
                const sycl::host_accessor<M<T>, 1, sycl::access_mode::read_write> host_accessor(materials_);

                host_accessor.erase(std::remove(host_accessor.begin(), host_accessor.end(), material), host_accessor.end());
            }

            /**
             * @brief Removes multiple materials from a scene.
             *
             * @param materials Array of materials to be removed from the scene.
             */
            auto remove(std::span<M<T>> materials) -> void {
                const sycl::host_accessor<M<T>, 1, sycl::access_mode::read_write> host_accessor(materials_);
                auto end = host_accessor.end();

                for (const auto& material: materials) {
                    end = std::remove(host_accessor.begin(), end, material);
                }

                host_accessor.erase(end, host_accessor.end());
            }

            /**
             * @brief Removes a single medium from the scene.
             *
             * @param medium Medium to be removed from the scene.
             */
            auto remove(D<T> medium) -> void {
                const sycl::host_accessor<D<T>, 1, sycl::access_mode::read_write> host_accessor(mediums_);

                host_accessor.erase(std::remove(host_accessor.begin(), host_accessor.end(), medium), host_accessor.end());
            }

            /**
             * @brief Removes multiple mediums from a scene.
             *
             * @param mediums Array of mediums to be removed from the scene.
             */
            auto remove(std::span<D<T>> mediums) -> void {
                const sycl::host_accessor<D<T>, 1, sycl::access_mode::read_write> host_accessor(mediums_);
                auto end = host_accessor.end();

                for (const auto& medium: mediums) {
                    end = std::remove(host_accessor.begin(), end, medium);
                }

                host_accessor.erase(end, host_accessor.end());
            }

            /**
             * @brief Removes a mesh from the scene.
             *
             * This works by finding the first shape pointer of the mesh and deleting it and the n_tris next shapes.
             * This works fine if the mesh is added with add and if the triangles of the mesh are not added individually
             * out of order.
             *
             * @param mesh Mesh to be removed from the scene.
             */
            /* auto remove(Shapes::MeshTop_t* mesh) -> void {
                if (!mesh->triangles_.empty()) {
                    const auto triangle = std::find(shapes_.begin(), shapes_.end(), mesh->triangles_[0].get());
                    if (triangle != shapes_.end()) {
                        shapes_.erase(triangle, triangle + static_cast<long long>(mesh->triangles_.size()));
                    }
                }
            }*/

            /**
             * @brief Removes multiple meshes from the scene.
             *
             * This works by finding the first shape pointer of the meshes and deleting it and the n_tris next shapes.
             * This works fine if the meshes are added with add and if the triangles of the meshes are not added individually
             * out of order.
             *
             * @param meshes Array of meshes to be removed from the scene.
             */
            /* auto remove(const std::vector<Shapes::MeshTop_t*>& meshes) -> void {
                for (const auto& mesh: meshes) {
                    if (!mesh->triangles_.empty()) {
                        const auto triangle = std::find(shapes_.begin(), shapes_.end(), mesh->triangles_[0].get());
                        if (triangle != shapes_.end()) {
                            shapes_.erase(triangle, triangle + static_cast<long long>(mesh->triangles_.size()));
                        }
                    }
                }
            }*/

            /**
             * @brief Updates all the shapes in the scene.
             *
             * Called to update all the shapes in the structure if their transformation matrix
             * has changed.
             *
             * @param queue Queue on which to submit the update.
             */
            auto update(sycl::queue& queue) -> void {
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

            /**
             * @brief Builds an acceleration structure with the scene's shapes.
             *
             * The type of the acceleration structure will be AccelerationMultiGridVector_t, the highest performing
             * one for most (static) scenes.
             */
            /* auto build_acc() -> void {
                constexpr size_t grid_max_res          = 128;
                constexpr size_t grid_max_cell_content = 32;
                acc_                                   = std::make_unique<AccelerationMultiGridVector_t>(shapes_, 1, grid_max_res, grid_max_cell_content, 1);
            }*/

            /**
             * @brief Intersects the scene shapes directly one by one. Not to be used for general operation.
             *
             * @tparam N Number of mediums in the ray's medium list
             * @param[in] cgh Device handler for the device the code is running on.
             * @param[in] ray Ray to be intersected with the scene, using its current origin and direction.
             * @param[out] t Distance to intersection. It is stored in t if there is an intersection.
             * @param[out] uv 2D object-space coordinates of the intersection.
             * @return std::optional<size_t> Index of the intersected shape. Returns none if there is no intersection.
             */
            template<size_t N>
            auto intersect_brute(sycl::handler& cgh, const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<size_t> {
                T t_temp = std::numeric_limits<T>::max();
                std::array<T, 2> uv_temp{};

                t = std::numeric_limits<T>::max();
                std::optional<size_t> hit_obj{};

                auto accessor = shapes_.get_access<sycl::access::mode::read>(cgh);
                for (size_t i = 0; i < shapes_.get_range()[0]; ++i) {
                    if (accessor[i].intersection(ray, t_temp, uv_temp) && (t_temp < t)) {
                        hit_obj = i;
                        uv      = uv_temp;
                        t       = t_temp;
                    }
                }

                return hit_obj;
            }

            /**
             * @brief Intersects the scene using the acceleration structure. Main way to intersect shapes.
             *
             * @tparam N Number of mediums in the ray's medium list
             * @param[in] cgh Device handler for the device the code is running on.
             * @param[in] ray Ray to be intersected with the scene, using its current origin and direction.
             * @param[out] t Distance to intersection. It is stored in t if there is an intersection.
             * @param[out] uv 2D object-space coordinates of the intersection.
             * @return std::optional<std::reference_wrapper<S<T>>> Reference to the intersected shape. Returns none if there is no intersection.
             */
            // template<size_t N>
            /* auto intersect(sycl::handler& cgh, const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<std::reference_wrapper<S<T>>> {
                return acc_->intersect(ray, t, uv);
            }*/

            /**
             * @brief Intersects the ray with objects in the scene and bounces it on their material.
             *
             * This is the main function to compute the colour of a ray. A ray is sent through the scene,
             * finding the closest object hit. Then, the ray is modified by this object's material.
             * This change can change ray direction, origin, colour and mask. This process is repeated
             * up to max_bounces times, or until no object is it, at which point the skybox is intersected.
             * The ray is also modified by its first medium using the scatter function. If it is scattered,
             * it won't be bounced on the hit object's material, as it intersects the medium instead of
             * the object.
             *
             * @tparam R Random generator type
             * @tparam K Skybox type to intersect
             * @tparam N Number of mediums in the ray's medium list
             * @param[in] rng Random generator used to get random numbers.
             * @param[in] unif Uniform distribution used to get random numbers.
             * @param[in] cgh Device handler for the device the code is running on.
             * @param[in] ray Ray to intersect with the scene.
             * @param[in] max_bounces Upper bound of number of bounces. Number of bounces may be less if no object is hit or ray can't be illuminated anymore.
             * @param[in] skybox Skybox that will be intersected if no object is hit.
             */
            template<class R, template<typename> typename U, template<typename> typename K, size_t N>
            requires Entities::Skybox<K, T> auto raycast(R& rng, U<T>& unif, sycl::handler& cgh, Ray_t<T, N>& ray, unsigned int max_bounces, const K<T>& skybox) const -> void {
                unsigned int bounces = 0;

                constexpr T minimum_mask = 0.01;
                while ((bounces < max_bounces) && (ray.mask_.magnitudeSquared() > minimum_mask)) { // Should maybe make magnitudeSquared min value lower
                    T t{};
                    std::array<T, 2> uv{};

                    // const std::optional<std::reference_wrapper<S<T>>> hit_obj = acc_->intersect(ray, t, uv);
                    const std::optional<size_t> hit_obj = intersect_brute(cgh, ray, t, uv);

                    if (!hit_obj) {
                        ray.colour_ += ray.mask_ * skybox.get(ray.direction_);
                        return;
                    }
                    ray.dist_ = t;
                    ++bounces;

                    auto medium_accessor = mediums_.get_access<sycl::access::mode::read>(cgh);
                    auto shape_accessor  = shapes_.get_access<sycl::access::mode::read>(cgh);
                    if (!medium_accessor[ray.medium_list_[0]].scatter(ray)) {
                        auto material_accessor = materials_.get_access<sycl::access::mode::read>(cgh);
                        material_accessor[shape_accessor[hit_obj].material_].bounce(rng, unif, uv, shape_accessor[hit_obj], ray);
                    }
                }
            }

            /**
             * @brief Get a Accessor_t object attached to this scene
             *
             * @return Accessor_t Accessor that can be used on the device to query the scene
             */
            auto getAccessor(sycl::handler& cgh) -> Accessor_t {
                return Accessor_t(cgh, shapes_, materials_, mediums_);
            }
    };
}
