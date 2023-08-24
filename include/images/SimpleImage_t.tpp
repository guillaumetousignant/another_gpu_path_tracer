#ifdef APTRACER_USE_JPEG
    #define cimg_use_jpeg
#endif
#ifdef APTRACER_USE_PNG
    #define cimg_use_png
#endif
#ifdef APTRACER_USE_TIFF
    #define cimg_use_tiff
#endif
// #define cimg_use_tinyexr // Can't put this twice for some reason, is on in Texture_t.tpp
#define cimg_use_cpp11 1
#define cimg_display 0
#include "external/CImg.h"
#include <algorithm>
#include <cmath>

template<typename T>
AGPTracer::Images::SimpleImage_t<T>::SimpleImage_t(size_t size_x, size_t size_y) : size_x_(size_x), size_y_(size_y), updates_(0), img_(sycl::range<2>{size_x, size_y}) {
    const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);
    std::fill(accessor.begin(), accessor.end(), Entities::Vec3<T>());
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::reset() -> void {
    updates_ = 0;
    const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);
    std::fill(accessor.begin(), accessor.end(), Entities::Vec3<T>());
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::update(const std::vector<Entities::Vec3<T>>& img) -> void {
    updates_++;

    const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read_write> accessor(img_);
    std::transform(accessor.begin(), accessor.end(), img.begin(), accessor.begin(), std::plus<>());
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::update() -> void {
    ++updates_;
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::update(const Entities::Vec3<T>& colour, size_t pos_x, size_t pos_y) -> void {
    const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read_write> accessor(img_);
    img_[sycl::id<2>{pos_x, pos_y}] += colour;
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::set(const std::vector<Entities::Vec3<T>>& img) -> void {
    updates_ = 1;
    const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_, sycl::no_init);
    std::copy(img.begin(), img.end(), accessor.begin());
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::set(const Entities::Vec3<T>& colour, size_t pos_x, size_t pos_y) -> void {
    const sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::write> accessor(img_); // Not sure if it should be no_init. Probably not
    accessor[sycl::id<2>{pos_x, pos_y}] = colour;
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::write(const std::filesystem::path& filename) -> void {
    const T update_mult = T{1} / static_cast<T>(updates_);
    cimg_library::CImg<unsigned short> image(static_cast<unsigned int>(size_x_), static_cast<unsigned int>(size_y_), 1, 3);
    const auto n = static_cast<unsigned int>(size_x_ * size_y_);

    constexpr unsigned int bit_depth = 16;
    const T bit_multiplier           = std::pow(T{2}, bit_depth) - T{1}; // With msvc std::pow is not constexpr :(

    sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

    for (size_t j = 0; j < size_y_; ++j) {
        for (size_t i = 0; i < size_x_; ++i) {
            Entities::Vec3<T> colour = accessor[sycl::id<2>{i, j}] * update_mult;
            colour.clamp(T{0}, T{1});
            colour *= bit_multiplier;
            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 0, n, n) = static_cast<unsigned short>(std::lround(colour[0]));
            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 1, n, n) = static_cast<unsigned short>(std::lround(colour[1]));
            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 2, n, n) = static_cast<unsigned short>(std::lround(colour[2]));
        }
    }

    image.save(filename.c_str());
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::write(const std::filesystem::path& filename, T gammaind) -> void {
    const T update_mult = T{1} / static_cast<T>(updates_);
    cimg_library::CImg<unsigned short> image(static_cast<unsigned int>(size_x_), static_cast<unsigned int>(size_y_), 1, 3);
    const auto n = static_cast<unsigned int>(size_x_ * size_y_);

    constexpr unsigned int bit_depth = 16;
    const T bit_multiplier           = std::pow(T{2}, bit_depth) - T{1}; // With msvc std::pow is not constexpr :(

    sycl::host_accessor<Entities::Vec3<T>, 2, sycl::access_mode::read> accessor(img_);

    for (size_t j = 0; j < size_y_; ++j) {
        for (size_t i = 0; i < size_x_; ++i) {
            Entities::Vec3<T> colour = accessor[sycl::id<2>{i, j}] * update_mult;
            colour.pow_inplace(gammaind).clamp(T{0}, T{1});
            colour *= bit_multiplier;
            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 0, n, n) = static_cast<unsigned short>(std::lround(colour[0]));
            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 1, n, n) = static_cast<unsigned short>(std::lround(colour[1]));
            image(static_cast<unsigned int>(i), static_cast<unsigned int>(j), 0, 2, n, n) = static_cast<unsigned short>(std::lround(colour[2]));
        }
    }

    image.save(filename.c_str());
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::getAccessor(sycl::handler& cgh) -> Accessor_t {
    return Accessor_t(cgh, img_);
}

template<typename T>
AGPTracer::Images::SimpleImage_t<T>::Accessor_t::Accessor_t(sycl::handler& cgh, sycl::buffer<Entities::Vec3<T>, 2>& img) : img_(img.template get_access<sycl::access::mode::read_write>(cgh)) {}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::Accessor_t::update(const Entities::Vec3<T>& colour, sycl::id<2> pos) const -> void {
    img_[pos] += colour;
}

template<typename T>
auto AGPTracer::Images::SimpleImage_t<T>::Accessor_t::set(const Entities::Vec3<T>& colour, sycl::id<2> pos) const -> void {
    img_[pos] = colour;
}
