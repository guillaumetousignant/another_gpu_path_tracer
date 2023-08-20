#include "entities/MeshGeometry_t.hpp"
#include "entities/RandomGenerator_t.hpp"
#include "entities/Ray_t.hpp"
#include "entities/Scene_t.hpp"
#include "entities/Vec3.hpp"
#include "shapes/Triangle_t.hpp"
#include "skyboxes/SkyboxFlat_t.hpp"
#include <CL/sycl.hpp>
#include <numbers>
#include <random>
#include <span>

using AGPTracer::Entities::Vec3;
using AGPTracer::Shapes::Triangle_t;

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

auto check_results(cl::sycl::buffer<Triangle_t<double>, 1>& buffer) -> bool {
    // Implicit barrier waiting for queue to complete the work.
    const cl::sycl::host_accessor<Triangle_t<double>, 1, cl::sycl::access_mode::read> host_accessor(buffer);

    // Check the results
    const auto triangles = get_triangles();
    bool MismatchFound   = false;
    for (size_t i = 0; i < buffer.get_range()[0]; ++i) {
        Triangle_t<double> triangle = triangles[i];
        triangle.transformation_.rotateZAxis(std::numbers::pi / 4);
        triangle.update();

        if (host_accessor[i].points_[0] != triangle.points_[0] || host_accessor[i].points_[1] != triangle.points_[1] || host_accessor[i].points_[2] != triangle.points_[2]) {
            std::cout << "The result is incorrect for triangle: " << i << ", expected: (" << triangle.points_[0] << ", " << triangle.points_[1] << ", " << triangle.points_[2] << "), got: ("
                      << host_accessor[i].points_[0] << ", " << host_accessor[i].points_[1] << ", " << host_accessor[i].points_[2] << ")" << std::endl;
            MismatchFound = true;
        }
    }

    if (!MismatchFound) {
        std::cout << "The results are correct!" << std::endl;
    }

    return MismatchFound;
}

auto main() -> int {
    try {
        // Creating buffer of triangles to be used inside the kernel code
        cl::sycl::buffer<Triangle_t<double>, 1> triangle_buffer(cl::sycl::range<1>{12});

        const AGPTracer::Entities::MeshGeometry_t<double> geom("assets/Zombie_Beast4.obj");
        const AGPTracer::Skyboxes::SkyboxFlat_t<double> skybox(Vec3<double>{1, 0, 1});
        auto triangles = get_triangles();
        const AGPTracer::Entities::Scene_t<double, Triangle_t<double>> scene(triangles);
        const AGPTracer::Entities::RandomGenerator_t<std::mt19937> random_generator(300, 200);

        fill_triangles(triangle_buffer);

        std::cout << "Devices:" << std::endl;
        for (auto device: cl::sycl::device::get_devices()) {
            std::cout << '\t' << device.get_info<cl::sycl::info::device::platform>().get_info<cl::sycl::info::platform::name>() << ": " << device.get_info<cl::sycl::info::device::name>() << std::endl;
        }

        // Creating SYCL queue
        cl::sycl::queue queue(cl::sycl::default_selector_v);

        std::cout << "Selected " << queue.get_info<cl::sycl::info::queue::device>().get_info<cl::sycl::info::device::name>() << std::endl << std::endl;

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

        const bool MismatchFound = check_results(triangle_buffer);

        return MismatchFound;
    }
    catch (cl::sycl::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
