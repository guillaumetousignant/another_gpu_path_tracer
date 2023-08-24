#include "cameras/SphericalCamera_t.hpp"
#include "entities/MediumList_t.hpp"
#include "entities/MeshGeometry_t.hpp"
#include "entities/RandomGenerator_t.hpp"
#include "entities/Ray_t.hpp"
#include "entities/Scene_t.hpp"
#include "entities/Texture_t.hpp"
#include "entities/Vec3.hpp"
#include "images/SimpleImage_t.hpp"
#include "materials/Diffuse_t.hpp"
#include "mediums/NonAbsorber_t.hpp"
#include "shapes/Triangle_t.hpp"
#include "skyboxes/SkyboxFlat_t.hpp"
#include <numbers>
#include <random>
#include <span>
#include <sycl/sycl.hpp>

using AGPTracer::Entities::TransformMatrix_t;
using AGPTracer::Entities::Vec3;
using AGPTracer::Materials::Diffuse_t;
using AGPTracer::Mediums::NonAbsorber_t;
using AGPTracer::Shapes::Triangle_t;

constexpr size_t TRIANGLE_BUFFER_SIZE = 12;
auto get_triangles() -> std::array<Triangle_t<double>, TRIANGLE_BUFFER_SIZE> {
    return {
        Triangle_t<double>{0, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{0, 2, 0}, Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 1}}, {}, {}},
        Triangle_t<double>{1, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 0}, Vec3<double>{1, 2, 1}, Vec3<double>{0, 2, 1}}, {}, {}},
        Triangle_t<double>{2, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{1, 3, 0}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 3, 1}}, {}, {}},
        Triangle_t<double>{3, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{1, 3, 0}}, {}, {}},
        Triangle_t<double>{4, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 0}, Vec3<double>{0, 3, 0}}, {}, {}},
        Triangle_t<double>{0, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 2, 0}}, {}, {}},
        Triangle_t<double>{1, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 1}, Vec3<double>{0, 2, 1}, Vec3<double>{1, 2, 1}}, {}, {}},
        Triangle_t<double>{2, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{0, 3, 1}}, {}, {}},
        Triangle_t<double>{3, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{1, 3, 1}, Vec3<double>{1, 2, 1}, Vec3<double>{1, 2, 0}}, {}, {}},
        Triangle_t<double>{4, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{1, 2, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 3, 1}}, {}, {}},
        Triangle_t<double>{0, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{0, 2, 0}, Vec3<double>{0, 2, 1}, Vec3<double>{0, 3, 1}}, {}, {}},
        Triangle_t<double>{1, TransformMatrix_t{}, std::array<Vec3<double>, 3>{Vec3<double>{0, 3, 1}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 2, 0}}, {}, {}}
    };
}

constexpr size_t MATERIAL_BUFFER_SIZE = 5;
auto get_materials() -> std::array<Diffuse_t<double>, MATERIAL_BUFFER_SIZE> {
    return {
        Diffuse_t<double>{Vec3<double>{0, 0, 0}, Vec3<double>{0.98, 0.7, 0.85}, 1  },
        Diffuse_t<double>{Vec3<double>{2, 2, 2}, Vec3<double>{1, 1, 1},         1  },
        Diffuse_t<double>{Vec3<double>{0, 0, 0}, Vec3<double>{0.8, 0.95, 0.6},  1  },
        Diffuse_t<double>{Vec3<double>{0, 0, 0}, Vec3<double>{0.98, 1.0, 0.9},  0  },
        Diffuse_t<double>{Vec3<double>{0, 0, 0}, Vec3<double>{1, 1, 1},         0.5}
    };
}

constexpr size_t MEDIUMS_BUFFER_SIZE = 1;
auto get_mediums() -> std::array<NonAbsorber_t<double>, MEDIUMS_BUFFER_SIZE> {
    return {
        NonAbsorber_t<double>{1, 0}
    };
}

auto fill_triangles(sycl::buffer<Triangle_t<double>, 1>& buffer) -> void {
    const sycl::host_accessor<Triangle_t<double>, 1, sycl::access_mode::write> host_accessor(buffer, sycl::no_init);
    const auto triangles = get_triangles();
    for (size_t i = 0; i < buffer.get_range()[0]; ++i) {
        host_accessor[i] = triangles[i];
    }
}

auto check_results(sycl::buffer<Triangle_t<double>, 1>& buffer) -> bool {
    // Implicit barrier waiting for queue to complete the work.
    const sycl::host_accessor<Triangle_t<double>, 1, sycl::access_mode::read> host_accessor(buffer);

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
        sycl::buffer<Triangle_t<double>, 1> triangle_buffer(sycl::range<1>{12});

        constexpr size_t size_x = 300;
        constexpr size_t size_y = 200;
        const AGPTracer::Shapes::Triangle_t<double> triangle{
            0, TransformMatrix_t{},
             std::array<Vec3<double>, 3>{Vec3<double>{0, 2, 0}, Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 1}},
             {},
             {}
        };
        const AGPTracer::Entities::MeshGeometry_t<double> geom("assets/Zombie_Beast4.obj");
        const AGPTracer::Skyboxes::SkyboxFlat_t<double> skybox_flat(Vec3<double>{1, 0, 1});
        auto triangles = get_triangles();
        auto materials = get_materials();
        auto mediums   = get_mediums();
        AGPTracer::Entities::Scene_t<double, Triangle_t, Diffuse_t, NonAbsorber_t> scene(triangles, materials, mediums);
        AGPTracer::Entities::RandomGenerator_t<std::mt19937> random_generator(size_x, size_y);
        const AGPTracer::Materials::Diffuse_t<double> diffuse(Vec3<double>(0, 0, 0), Vec3<double>(0.5, 0.5, 0.5), 1);
        const AGPTracer::Mediums::NonAbsorber_t<double> non_absorber(1, 32);
        const AGPTracer::Entities::Texture_t<double> texture("assets/Zombie beast_texture5.png");
        AGPTracer::Images::SimpleImage_t<double> simple_image(size_x, size_y);
        const AGPTracer::Entities::MediumList_t<16> medium_list{
            2, std::array<size_t, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        };
        AGPTracer::Cameras::SphericalCamera_t<double, AGPTracer::Skyboxes::SkyboxFlat_t, AGPTracer::Images::SimpleImage_t> spherical_camera(
            TransformMatrix_t{}, "images/default.png", Vec3<double>(0, 0, 1), std::array<double, 2>{0.93084, 1.3963}, std::array<unsigned int, 2>{1, 1}, medium_list, skybox_flat, 8, 1, simple_image);

        fill_triangles(triangle_buffer);

        std::cout << "Devices:" << std::endl;
        for (auto device: sycl::device::get_devices()) {
            std::cout << '\t' << device.get_info<sycl::info::device::platform>().get_info<sycl::info::platform::name>() << ": " << device.get_info<sycl::info::device::name>() << std::endl;
        }

        // Creating SYCL queue
        sycl::queue queue(sycl::default_selector_v);

        std::cout << "Selected " << queue.get_info<sycl::info::queue::device>().get_info<sycl::info::device::name>() << std::endl << std::endl;

        spherical_camera.raytrace(queue, random_generator, scene);
        spherical_camera.write("test.png");

        // Size of index space for kernel
        const sycl::range<1> num_work_items{triangle_buffer.get_range()};

        // Submitting command group(work) to queue
        queue.submit([&](sycl::handler& cgh) {
            // Getting read write access to the buffer on a device
            auto triangle_accessor = triangle_buffer.get_access<sycl::access::mode::read_write>(cgh);

            // Executing kernel
            cgh.parallel_for<class RotateTriangles>(num_work_items, [=](sycl::id<1> WIid) {
                triangle_accessor[WIid].transformation_.rotateZAxis(std::numbers::pi / 4);
                triangle_accessor[WIid].update();
            });
        });

        const bool MismatchFound = check_results(triangle_buffer);

        return MismatchFound;
    }
    catch (sycl::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
