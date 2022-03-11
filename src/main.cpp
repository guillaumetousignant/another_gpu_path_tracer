#include <CL/sycl.hpp>
#include "entities/Ray_t.h"
#include "entities/Vec3.h"

using APTracer::Entities::Vec3;

int main() {
	// Creating buffer of 4 ints to be used inside the kernel code
	cl::sycl::buffer<Vec3<double>, 2> Buffer(cl::sycl::range<2>{1024, 1024});

	// Creating SYCL queue
	cl::sycl::queue Queue;

	// Size of index space for kernel
	cl::sycl::range<2> NumOfWorkItems{Buffer.get_range()};

	// Submitting command group(work) to queue
	Queue.submit([&](cl::sycl::handler &cgh) {
	// Getting write only access to the buffer on a device
	auto Accessor = Buffer.get_access<cl::sycl::access::mode::write>(cgh);
	// Executing kernel
	cgh.parallel_for<class FillBuffer>(
		NumOfWorkItems, [=](cl::sycl::id<2> WIid) {
	    	// Fill buffer with indexes
	        Accessor[WIid] = Vec3<double>(WIid.get(0), WIid.get(1), 0);
		});
	});

	// Getting read only access to the buffer on the host.
	// Implicit barrier waiting for queue to complete the work.
	const auto HostAccessor = Buffer.get_access<cl::sycl::access::mode::read>();

	// Check the results
	bool MismatchFound = false;
	for (size_t i = 0; i < Buffer.get_range()[0]; ++i) {
		for (size_t j = 0; j < Buffer.get_range()[1]; ++j) {
			if (HostAccessor[i][j].x() != double(i) || HostAccessor[i][j].y() != double(j) || HostAccessor[i][j].z() != 0.0) {
				std::cout << "The result is incorrect for element: (" << i << ", " << j
						<< "), expected: " << Vec3<double>(i, j, 0) << " , got: " << HostAccessor[i][j]
						<< std::endl;
				MismatchFound = true;
			}
		}
	}

	if (!MismatchFound) {
		std::cout << "The results are correct!" << std::endl;
	}

	return MismatchFound;
}
