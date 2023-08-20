#include "shapes/Triangle_t.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <CL/sycl.hpp>
#include <iostream>

using AGPTracer::Entities::Vec3;
using AGPTracer::Shapes::Triangle_t;

TEST_CASE("stupid/1=1", "Prove that one equals 1") {
    int one = 1;
    REQUIRE(one == 1);
}

constexpr size_t TRIANGLE_BUFFER_SIZE = 12;

auto get_triangles() -> std::array<Triangle_t<double>, TRIANGLE_BUFFER_SIZE> {
    return {
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{0, 2, 0}, Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 0}, Vec3<double>{1, 2, 1}, Vec3<double>{0, 2, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{1, 3, 0}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 3, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{1, 3, 0}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 0}, Vec3<double>{0, 3, 0}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 2, 0}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 1}, Vec3<double>{0, 2, 1}, Vec3<double>{1, 2, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{0, 3, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{1, 3, 1}, Vec3<double>{1, 2, 1}, Vec3<double>{1, 2, 0}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 3, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{0, 2, 0}, Vec3<double>{0, 2, 1}, Vec3<double>{0, 3, 1}}, {}, {}},
        Triangle_t<double>{std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 1}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 2, 0}}, {}, {}}
    };
}

auto fill_triangles(cl::sycl::buffer<Triangle_t<double>, 1>& buffer) -> void {
    const cl::sycl::host_accessor<Triangle_t<double>, 1, cl::sycl::access_mode::write> host_accessor(buffer, cl::sycl::no_init);
    const auto triangles = get_triangles();
    for (size_t i = 0; i < buffer.get_range()[0]; ++i) {
        host_accessor[i] = triangles[i];
    }
}

TEST_CASE("TransformMatrix_t rotation", "Compares a triangle rotation between host code and SYCL") {
    // Creating buffer of triangles to be used inside the kernel code
    cl::sycl::buffer<Triangle_t<double>, 1> triangle_buffer(cl::sycl::range<1>{12});
    fill_triangles(triangle_buffer);

    // Creating SYCL queue
    cl::sycl::queue queue(cl::sycl::default_selector_v);

    // Size of index space for kernel
    const cl::sycl::range<1> num_work_items{triangle_buffer.get_range()};

    // Submitting command group(work) to queue
    queue.submit([&](cl::sycl::handler& cgh) {
        // Getting read write access to the buffer on a device
        auto triangle_accessor = triangle_buffer.get_access<cl::sycl::access::mode::read_write>(cgh);
        // Executing kernel
        cgh.parallel_for<class RotateTriangles>(num_work_items, [=](cl::sycl::id<1> WIid) {
            triangle_accessor[WIid].transformation_.rotateZAxis(std::numbers::pi / 4);
            triangle_accessor[WIid].update();
        });
    });

    // Implicit barrier waiting for queue to complete the work.
    const cl::sycl::host_accessor<Triangle_t<double>, 1, cl::sycl::access_mode::read> host_accessor(triangle_buffer);

    // Check the results
    const auto triangles = get_triangles();
    for (size_t i = 0; i < triangle_buffer.get_range()[0]; ++i) {
        Triangle_t<double> triangle = triangles[i];
        triangle.transformation_.rotateZAxis(std::numbers::pi / 4);
        triangle.update();

        for (size_t j = 0; j < host_accessor[i].points_.size(); ++j) {
            REQUIRE(host_accessor[i].points_[j] == triangle.points_[j]);
        }
    }
}
