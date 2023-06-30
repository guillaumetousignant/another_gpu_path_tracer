#include <CL/sycl.hpp>
#include <numbers>
#include "entities/Ray_t.h"
#include "entities/Vec3.h"
#include "shapes/Triangle_t.h"
#include "entities/MeshGeometry_t.h"

using AGPTracer::Entities::Vec3;
using AGPTracer::Shapes::Triangle_t;

auto fill_triangles(cl::sycl::buffer<Triangle_t, 1>& buffer) -> void {
    const auto host_accessor = buffer.get_access<cl::sycl::access::mode::discard_write>();
    host_accessor[0]  = Triangle_t({Vec3<double>{0, 2, 0}, Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 1}}, {}, {});
    host_accessor[1]  = Triangle_t({Vec3<double>{1, 2, 0}, Vec3<double>{1, 2, 1}, Vec3<double>{0, 2, 1}}, {}, {});
    host_accessor[2]  = Triangle_t({Vec3<double>{1, 3, 0}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 3, 1}}, {}, {});
    host_accessor[3]  = Triangle_t({Vec3<double>{0, 3, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{1, 3, 0}}, {}, {});

    host_accessor[4]  = Triangle_t({Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 0}, Vec3<double>{0, 3, 0}}, {}, {});
    host_accessor[5]  = Triangle_t({Vec3<double>{0, 3, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 2, 0}}, {}, {});
    host_accessor[6]  = Triangle_t({Vec3<double>{0, 3, 1}, Vec3<double>{0, 2, 1}, Vec3<double>{1, 2, 1}}, {}, {});
    host_accessor[7]  = Triangle_t({Vec3<double>{1, 2, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{0, 3, 1}}, {}, {});

    host_accessor[8]  = Triangle_t({Vec3<double>{1, 3, 1}, Vec3<double>{1, 2, 1}, Vec3<double>{1, 2, 0}}, {}, {});
    host_accessor[9]  = Triangle_t({Vec3<double>{1, 2, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 3, 1}}, {}, {});
    host_accessor[10] = Triangle_t({Vec3<double>{0, 2, 0}, Vec3<double>{0, 2, 1}, Vec3<double>{0, 3, 1}}, {}, {});
    host_accessor[11] = Triangle_t({Vec3<double>{0, 3, 1}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 2, 0}}, {}, {});
}

auto check_results(cl::sycl::buffer<Triangle_t, 1>& buffer) -> bool {
    // Implicit barrier waiting for queue to complete the work.
    const auto host_accessor = buffer.get_access<cl::sycl::access::mode::read>();

    // Check the results
    bool MismatchFound = false;
    for (size_t i = 0; i < buffer.get_range()[0]; ++i) {
        if (host_accessor[i].points_[0] != Vec3<double>{}) {
            std::cout << "The result is incorrect for triangle: " << i
                    << ", expected: (" << Vec3<double>{} << ", " << Vec3<double>{} << ", " << Vec3<double>{} 
                    << "), got: (" << host_accessor[i].points_[0] << ", " << host_accessor[i].points_[1] << ", " << host_accessor[i].points_[2] << ")"
                    << std::endl;
            MismatchFound = true;
        }
    }

    if (!MismatchFound) {
        std::cout << "The results are correct!" << std::endl;
    }

    return MismatchFound;
}

auto main() -> int {
    // Creating buffer of triangles to be used inside the kernel code
    cl::sycl::buffer<Triangle_t, 1> triangle_buffer(cl::sycl::range<1>{12});

    AGPTracer::Entities::MeshGeometry_t geom("assets/Zombie_Beast4.obj");

    fill_triangles(triangle_buffer);

    // Creating SYCL queue
    cl::sycl::queue queue;

    // Size of index space for kernel
    cl::sycl::range<1> num_work_items{triangle_buffer.get_range()};

    // Submitting command group(work) to queue
    queue.submit([&](cl::sycl::handler &cgh) {
    // Getting read write access to the buffer on a device
    auto triangle_accessor = triangle_buffer.get_access<cl::sycl::access::mode::read_write>(cgh);
    // Executing kernel
    cgh.parallel_for<class RotateTriangles>(
        num_work_items, [=](cl::sycl::id<1> WIid) {
            triangle_accessor[WIid].transformation_.rotateZAxis(std::numbers::pi/4);
            triangle_accessor[WIid].update();
        });
    });

    const bool MismatchFound = check_results(triangle_buffer);

    return MismatchFound;
}
