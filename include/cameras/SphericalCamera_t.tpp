#include <chrono>
#include <iostream>
#include <numbers>

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::SphericalCamera_t(Entities::TransformMatrix_t<T> transformation,
                                                                                                                                                   std::filesystem::path filename,
                                                                                                                                                   Entities::Vec3<T> up,
                                                                                                                                                   std::array<T, 2> fov,
                                                                                                                                                   std::array<unsigned int, 2> subpix,
                                                                                                                                                   Entities::MediumList_t<N> medium_list,
                                                                                                                                                   K<T> skybox,
                                                                                                                                                   unsigned int max_bounces,
                                                                                                                                                   T gammaind,
                                                                                                                                                   I<T> image) :
        transformation_(std::move(transformation)),
        filename_(std::move(filename)),
        fov_(fov),
        fov_buffer_(fov),
        subpix_(subpix),
        medium_list_(std::move(medium_list)),
        skybox_(std::move(skybox)),
        max_bounces_(max_bounces),
        gammaind_(gammaind),
        up_(up),
        up_buffer_(up),
        image_(std::move(image)) {
    origin_    = transformation_.multVec(Entities::Vec3<T>());
    direction_ = transformation_.multDir(Entities::Vec3<T>(T{0}, T{1}, T{0}));
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::update() -> void {
    origin_    = transformation_.multVec(Entities::Vec3<T>());
    direction_ = transformation_.multDir(Entities::Vec3<T>(T{0}, T{1}, T{0}));
    up_        = up_buffer_;
    fov_       = fov_buffer_;
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> template<class R, template<typename> typename U, template<typename> typename S>
requires AGPTracer::Entities::Shape<S, T> auto
AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::raytrace(sycl::queue& queue, Entities::RandomGenerator_t<T, R, U>& random_generator, Entities::Scene_t<T, S>& scene) -> void {
    const T tot_subpix                 = subpix_[0] * subpix_[1];
    const T pixel_span_y               = fov_[0] / static_cast<T>(image_.size_y_);
    const T pixel_span_x               = fov_[1] / static_cast<T>(image_.size_x_);
    const T subpix_span_y              = pixel_span_y / subpix_[0];
    const T subpix_span_x              = pixel_span_x / subpix_[1];
    const Entities::Vec3<T> horizontal = direction_.cross(up_).normalize_inplace();
    const Entities::Vec3<T> vertical   = horizontal.cross(direction_).normalize_inplace();

    // Copy of members
    const std::array<unsigned int, 2> subpix = subpix_;
    const Entities::Vec3<T> direction        = direction_;
    const Entities::Vec3<T> origin           = origin_;
    Entities::MediumList_t<N> medium_list    = medium_list_;
    unsigned int max_bounces                 = max_bounces_;
    K<T> skybox                              = skybox_; // This is especially bad, maybe skyboxes should be in the scene too

    image_.update();

    // Size of index space for kernel
    const sycl::range<2> num_work_items{image_.size_x_, image_.size_y_};

    // Submitting command group(work) to queue
    queue.submit([&](sycl::handler& cgh) {
        // Getting read write access to the buffer on a device
        auto image_accessor  = image_.getAccessor(cgh);
        auto scene_accessor  = scene.getAccessor(cgh);
        auto random_accessor = random_generator.getAccessor(cgh);

        // Executing kernel
        cgh.parallel_for<class SphericalCameraRaytrace>(num_work_items, [=](sycl::id<2> WIid) {
            Entities::Vec3<T> col           = Entities::Vec3<T>();
            const Entities::Vec3<T> pix_vec = Entities::Vec3<T>(T{1},
                                                                std::numbers::pi_v<T> / T{2} + (static_cast<T>(WIid[1]) - static_cast<double>(num_work_items[1]) / T{2} + T{0.5}) * pixel_span_y,
                                                                (static_cast<double>(WIid[0]) - static_cast<double>(num_work_items[0]) / T{2} + T{0.5}) * pixel_span_x);
            R& rng                          = random_accessor.rng_[WIid];
            U<T>& unif                      = random_accessor.unif_[WIid];

            for (unsigned int subindex = 0; subindex < subpix[0] * subpix[1]; ++subindex) {
                const unsigned int l  = subindex % subpix[1]; // x
                const unsigned int k  = subindex / subpix[1]; // y
                const double jitter_y = unif(rng);
                const double jitter_x = unif(rng);

                const Entities::Vec3<T> subpix_vec = (pix_vec
                                                      + Entities::Vec3<T>(T{0},
                                                                          (static_cast<double>(k) - static_cast<double>(subpix[0]) / T{2} + jitter_y) * subpix_span_y,
                                                                          (static_cast<double>(l) - static_cast<double>(subpix[1]) / T{2} + jitter_x) * subpix_span_x))
                                                         .to_xyz_offset(direction, horizontal, vertical);

                Entities::Ray_t ray(origin, subpix_vec, Entities::Vec3<T>(), Entities::Vec3<T>(T{1}), medium_list);
                scene_accessor.raycast(rng, unif, ray, max_bounces, skybox);
                col += ray.colour_;
            }
            col = col / tot_subpix;
            image_accessor.update(col, WIid);
        });
    });
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> template<class R, template<typename> typename U, template<typename> typename S>
requires AGPTracer::Entities::Shape<S, T> auto
AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::accumulate(sycl::queue& queue, Entities::RandomGenerator_t<T, R, U>& random_generator, Entities::Scene_t<T, S>& scene, unsigned int n_iter) -> void {
    unsigned int n = 0;
    while (n < n_iter) {
        ++n;

        auto t_start = std::chrono::high_resolution_clock::now();
        raytrace(queue, random_generator, scene);
        auto t_end = std::chrono::high_resolution_clock::now();

        std::cout << "Iteration " << n << " done in " << std::chrono::duration<T>(t_end - t_start).count() << "s." << std::endl;
    }
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> template<class R, template<typename> typename U, template<typename> typename S>
requires AGPTracer::Entities::Shape<S, T> auto
AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::accumulate(sycl::queue& queue, Entities::RandomGenerator_t<T, R, U>& random_generator, Entities::Scene_t<T, S>& scene) -> void {
    unsigned int n = 0;
    while (true) {
        ++n;

        auto t_start = std::chrono::high_resolution_clock::now();
        raytrace(queue, random_generator, scene);
        auto t_end = std::chrono::high_resolution_clock::now();

        std::cout << "Iteration " << n << " done in " << std::chrono::duration<T>(t_end - t_start).count() << "s." << std::endl;
    }
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> template<class R, template<typename> typename U, template<typename> typename S>
requires AGPTracer::Entities::Shape<S, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::accumulateWrite(
    sycl::queue& queue, Entities::RandomGenerator_t<T, R, U>& random_generator, Entities::Scene_t<T, S>& scene, unsigned int n_iter, unsigned int interval) -> void {
    // std::chrono::steady_clock::time_point t_start, t_end;
    unsigned int n = 0;
    while (n < n_iter) {
        ++n;

        auto t_start = std::chrono::high_resolution_clock::now();
        raytrace(queue, random_generator, scene);
        auto t_end = std::chrono::high_resolution_clock::now();

        std::cout << "Iteration " << n << " done in " << std::chrono::duration<T>(t_end - t_start).count() << "s." << std::endl;

        if (n % interval == 0) {
            std::cout << "Writing started." << std::endl;
            auto t_start2 = std::chrono::high_resolution_clock::now();
            write();
            auto t_end2 = std::chrono::high_resolution_clock::now();

            std::cout << "Writing done in " << std::chrono::duration<T>(t_end2 - t_start2).count() << "s." << std::endl;
        }
    }
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> template<class R, template<typename> typename U, template<typename> typename S>
requires AGPTracer::Entities::Shape<S, T> auto
AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::accumulateWrite(sycl::queue& queue, Entities::RandomGenerator_t<T, R, U>& random_generator, Entities::Scene_t<T, S>& scene, unsigned int interval)
    -> void {
    unsigned int n = 0;
    while (true) {
        ++n;

        auto t_start = std::chrono::high_resolution_clock::now();
        raytrace(queue, random_generator, scene);
        auto t_end = std::chrono::high_resolution_clock::now();

        std::cout << "Iteration " << n << " done in " << std::chrono::duration<T>(t_end - t_start).count() << "s." << std::endl;

        if (n % interval == 0) {
            std::cout << "Writing started." << std::endl;
            auto t_start2 = std::chrono::high_resolution_clock::now();
            write();
            auto t_end2 = std::chrono::high_resolution_clock::now();

            std::cout << "Writing done in " << std::chrono::duration<T>(t_end2 - t_start2).count() << "s." << std::endl;
        }
    }
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> template<class R, template<typename> typename U, template<typename> typename S>
requires AGPTracer::Entities::Shape<S, T> auto
AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::accumulateWrite(sycl::queue& queue, Entities::RandomGenerator_t<T, R, U>& random_generator, Entities::Scene_t<T, S>& scene) -> void {
    unsigned int n = 0;
    while (true) {
        ++n;

        auto t_start = std::chrono::high_resolution_clock::now();
        raytrace(queue, random_generator, scene);
        auto t_end = std::chrono::high_resolution_clock::now();

        std::cout << "Iteration " << n << " done in " << std::chrono::duration<T>(t_end - t_start).count() << "s." << std::endl;

        std::cout << "Writing started." << std::endl;
        auto t_start2 = std::chrono::high_resolution_clock::now();
        write();
        auto t_end2 = std::chrono::high_resolution_clock::now();

        std::cout << "Writing done in " << std::chrono::duration<T>(t_end2 - t_start2).count() << "s." << std::endl;
    }
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::setUp(Entities::Vec3<T> new_up) -> void {
    up_buffer_ = new_up;
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::zoom(T factor) -> void {
    fov_buffer_ = {fov_[0] * factor, fov_[1] * factor};
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::zoom(std::array<T, 2> fov) -> void {
    fov_buffer_ = fov;
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::write(const std::filesystem::path& file_name) -> void {
    image_.write(file_name, gammaind_);
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::write() -> void {
    image_.write(filename_, gammaind_);
}

template<typename T, template<typename> typename K, template<typename> typename I, size_t N>
requires AGPTracer::Entities::Skybox<K, T>&& AGPTracer::Entities::Image<I, T> auto AGPTracer::Cameras::SphericalCamera_t<T, K, I, N>::reset() -> void {
    image_.reset();
}
