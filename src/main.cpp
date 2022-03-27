#include <CL/sycl.hpp>
#include <numbers>
#include "entities/Ray_t.h"
#include "entities/Vec3.h"
#include "shapes/Triangle_t.h"

using AGPTracer::Entities::Vec3;
using AGPTracer::Shapes::Triangle_t;

int main() {
	// Creating buffer of 4 ints to be used inside the kernel code
	cl::sycl::buffer<Triangle_t, 1> triangle_buffer(cl::sycl::range<1>{12});

    auto host_triangle_accessor = triangle_buffer.get_access<cl::sycl::access::mode::discard_write>();
    host_triangle_accessor[0]  = Triangle_t({Vec3<double>{0, 2, 0}, Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 1}}, {}, {});
    host_triangle_accessor[1]  = Triangle_t({Vec3<double>{1, 2, 0}, Vec3<double>{1, 2, 1}, Vec3<double>{0, 2, 1}}, {}, {});
    host_triangle_accessor[2]  = Triangle_t({Vec3<double>{1, 3, 0}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 3, 1}}, {}, {});
    host_triangle_accessor[3]  = Triangle_t({Vec3<double>{0, 3, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{1, 3, 0}}, {}, {});

    host_triangle_accessor[4]  = Triangle_t({Vec3<double>{1, 2, 0}, Vec3<double>{0, 2, 0}, Vec3<double>{0, 3, 0}}, {}, {});
    host_triangle_accessor[5]  = Triangle_t({Vec3<double>{0, 3, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 2, 0}}, {}, {});
    host_triangle_accessor[6]  = Triangle_t({Vec3<double>{0, 3, 1}, Vec3<double>{0, 2, 1}, Vec3<double>{1, 2, 1}}, {}, {});
    host_triangle_accessor[7]  = Triangle_t({Vec3<double>{1, 2, 1}, Vec3<double>{1, 3, 1}, Vec3<double>{0, 3, 1}}, {}, {});

    host_triangle_accessor[8]  = Triangle_t({Vec3<double>{1, 3, 1}, Vec3<double>{1, 2, 1}, Vec3<double>{1, 2, 0}}, {}, {});
    host_triangle_accessor[9]  = Triangle_t({Vec3<double>{1, 2, 0}, Vec3<double>{1, 3, 0}, Vec3<double>{1, 3, 1}}, {}, {});
    host_triangle_accessor[10] = Triangle_t({Vec3<double>{0, 2, 0}, Vec3<double>{0, 2, 1}, Vec3<double>{0, 3, 1}}, {}, {});
    host_triangle_accessor[11] = Triangle_t({Vec3<double>{0, 3, 1}, Vec3<double>{0, 3, 0}, Vec3<double>{0, 2, 0}}, {}, {});

	// Creating SYCL queue
	cl::sycl::queue queue;

	// Size of index space for kernel
	cl::sycl::range<1> num_work_items{triangle_buffer.get_range()};

	// Submitting command group(work) to queue
	queue.submit([&](cl::sycl::handler &cgh) {
	// Getting write only access to the buffer on a device
	auto triangle_accessor = triangle_buffer.get_access<cl::sycl::access::mode::read_write>(cgh);
	// Executing kernel
	cgh.parallel_for<class FillBuffer>(
		num_work_items, [=](cl::sycl::id<1> WIid) {
	    	// Fill buffer with indexes
	        triangle_accessor[WIid].transformation_.rotateZ(std::numbers::pi/2);
            triangle_accessor[WIid].update();
		});
	});

	// Getting read only access to the buffer on the host.
	// Implicit barrier waiting for queue to complete the work.
	const auto host_accessor = triangle_buffer.get_access<cl::sycl::access::mode::read>();

	// Check the results
	bool MismatchFound = false;
	for (size_t i = 0; i < triangle_buffer.get_range()[0]; ++i) {
        if (host_accessor[i].points_[0] != Vec3<double>{}) {
            std::cout << "The result is incorrect for triangle: " << i
                    << ", expected: " << Vec3<double>{} << " , got: " << host_accessor[i].points_[0]
                    << std::endl;
            MismatchFound = true;
        }
	}

	if (!MismatchFound) {
		std::cout << "The results are correct!" << std::endl;
	}

	return MismatchFound;
}
