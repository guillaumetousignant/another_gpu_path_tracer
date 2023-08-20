template<typename T>
AGPTracer::Entities::RandomGenerator_t<T>::RandomGenerator_t(size_t size_x, size_t size_y) : size_x_(size_x), size_y_(size_y), rng_vec_(cl::sycl::range<2>{size_x, size_y}) {
    std::random_device rdev;
    const cl::sycl::host_accessor<T, 2, cl::sycl::access_mode::write> host_accessor(rng_vec_, cl::sycl::no_init);

    auto range = rng_vec_.get_range();

    for (size_t i = 0; i < rng_vec_.get_range()[0]; ++i) {
        for (size_t j = 0; j < rng_vec_.get_range()[1]; ++j) {
            host_accessor[cl::sycl::id<2>{i, j}] = T(rdev());
        }
    }
}
