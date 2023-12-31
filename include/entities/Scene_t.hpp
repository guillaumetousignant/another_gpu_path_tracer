#ifndef AGPTRACER_ENTITIES_SCENE_T_HPP
#define AGPTRACER_ENTITIES_SCENE_T_HPP

// #include "entities/AccelerationStructure_t.hpp"
#include "entities/Material.hpp"
#include "entities/Medium.hpp"
#include "entities/Ray_t.hpp"
#include "entities/Shape.hpp"
#include "entities/Skybox.hpp"
#include "materials/Diffuse_t.hpp"
#include "mediums/NonAbsorber_t.hpp"
#include "shapes/Triangle_t.hpp"
#include <array>
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <sycl/sycl.hpp>

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
    template<typename T = double, template<typename> typename S = Shapes::Triangle_t, template<typename> typename M = Materials::Diffuse_t, template<typename> typename D = Mediums::NonAbsorber_t>
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
                    Accessor_t(sycl::handler& cgh, sycl::buffer<S<T>, 1>& shapes, sycl::buffer<M<T>, 1>& materials, sycl::buffer<D<T>, 1>& mediums);

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
                    requires Entities::Skybox<K, T> auto raycast(R& rng, U<T>& unif, Ray_t<T, N>& ray, unsigned int max_bounces, const K<T>& skybox) const -> void;

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
                    auto intersect_brute(const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<size_t>;

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
                    // auto intersect(const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<std::reference_wrapper<S<T>>>;

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
            Scene_t(std::span<S<T>> shapes, std::span<M<T>> materials, std::span<D<T>> mediums);

            /**
             * @brief Construct a new Scene_t object containing a single mesh.
             *
             * @param mesh Mesh to add to the scene.
             */
            // explicit Scene_t(Shapes::MeshTop_t* mesh);

            /**
             * @brief Construct a new Scene_t object from multiple meshes.
             *
             * @param meshes Meshes to be added to the scene.
             */
            // explicit Scene_t(const std::vector<Shapes::MeshTop_t*>& meshes);

            sycl::buffer<S<T>, 1> shapes_; /**< @brief Vector of shapes to be drawn.*/
            sycl::buffer<M<T>, 1> materials_; /**< @brief Vector of materials for the shapes.*/
            sycl::buffer<D<T>, 1> mediums_; /**< @brief Vector of mediums for the materials.*/
            // std::unique_ptr<AccelerationStructure_t> acc_; /**< @brief Acceleration structure containing the shapes, used to accelerate intersection.*/

            /**
             * @brief Adds a single shape to the scene.
             *
             * @param shape Shape to be added to the scene.
             */
            auto add(S<T> shape) -> void;

            /**
             * @brief Adds several shapes to the scene.
             *
             * @param shapes Array of shapes to be added to the scene.
             */
            auto add(std::span<S<T>> shapes) -> void;

            /**
             * @brief Adds a single material to the scene.
             *
             * @param material Material to be added to the scene.
             */
            auto add(M<T> material) -> void;

            /**
             * @brief Adds several materials to the scene.
             *
             * @param materials Array of materials to be added to the scene.
             */
            auto add(std::span<M<T>> materials) -> void;

            /**
             * @brief Adds a single medium to the scene.
             *
             * @param medium Medium to be added to the scene.
             */
            auto add(D<T> medium) -> void;

            /**
             * @brief Adds several mediums to the scene.
             *
             * @param mediums Array of mediums to be added to the scene.
             */
            auto add(std::span<D<T>> mediums) -> void;

            /**
             * @brief Adds a single mesh to the scene.
             *
             * @param mesh Mesh to be added to the scene.
             */
            // auto add(Shapes::MeshTop_t* mesh) -> void;

            /**
             * @brief Adds multiple meshes to the scene.
             *
             * @param meshes Array of meshes to be added to the scene.
             */
            // auto add(const std::vector<Shapes::MeshTop_t*>& meshes) -> void;

            /**
             * @brief Removes a single shape from the scene.
             *
             * @param shape Shape to be removed from the scene.
             */
            auto remove(S<T> shape) -> void;

            /**
             * @brief Removes multiple shapes from a scene.
             *
             * @param shapes Array of shapes to be removed from the scene.
             */
            auto remove(std::span<S<T>> shapes) -> void;

            /**
             * @brief Removes a single material from the scene.
             *
             * @param material Material to be removed from the scene.
             */
            auto remove(M<T> material) -> void;

            /**
             * @brief Removes multiple materials from a scene.
             *
             * @param materials Array of materials to be removed from the scene.
             */
            auto remove(std::span<M<T>> materials) -> void;

            /**
             * @brief Removes a single medium from the scene.
             *
             * @param medium Medium to be removed from the scene.
             */
            auto remove(D<T> medium) -> void;

            /**
             * @brief Removes multiple mediums from a scene.
             *
             * @param mediums Array of mediums to be removed from the scene.
             */
            auto remove(std::span<D<T>> mediums) -> void;

            /**
             * @brief Removes a mesh from the scene.
             *
             * This works by finding the first shape pointer of the mesh and deleting it and the n_tris next shapes.
             * This works fine if the mesh is added with add and if the triangles of the mesh are not added individually
             * out of order.
             *
             * @param mesh Mesh to be removed from the scene.
             */
            // auto remove(Shapes::MeshTop_t* mesh) -> void;

            /**
             * @brief Removes multiple meshes from the scene.
             *
             * This works by finding the first shape pointer of the meshes and deleting it and the n_tris next shapes.
             * This works fine if the meshes are added with add and if the triangles of the meshes are not added individually
             * out of order.
             *
             * @param meshes Array of meshes to be removed from the scene.
             */
            // auto remove(const std::vector<Shapes::MeshTop_t*>& meshes) -> void;

            /**
             * @brief Updates all the shapes in the scene.
             *
             * Called to update all the shapes in the structure if their transformation matrix
             * has changed.
             *
             * @param queue Queue on which to submit the update.
             */
            auto update(sycl::queue& queue) -> void;

            /**
             * @brief Builds an acceleration structure with the scene's shapes.
             *
             * The type of the acceleration structure will be AccelerationMultiGridVector_t, the highest performing
             * one for most (static) scenes.
             */
            // auto build_acc() -> void;

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
            auto intersect_brute(sycl::handler& cgh, const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<size_t>;

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
            // auto intersect(sycl::handler& cgh, const Ray_t<T, N>& ray, T& t, std::array<T, 2>& uv) const -> std::optional<std::reference_wrapper<S<T>>>;

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
            requires Entities::Skybox<K, T> auto raycast(R& rng, U<T>& unif, sycl::handler& cgh, Ray_t<T, N>& ray, unsigned int max_bounces, const K<T>& skybox) const -> void;

            /**
             * @brief Get a Accessor_t object attached to this scene
             *
             * @return Accessor_t Accessor that can be used on the device to query the scene
             */
            auto getAccessor(sycl::handler& cgh) -> Accessor_t;
    };
}

#include "entities/Scene_t.tpp"

#endif
