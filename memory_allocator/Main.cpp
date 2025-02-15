#include "Allocator.hpp"


using namespace allocator;


void batch_malloc(size_t malloc_left, size_t size) {

	if (malloc_left > 0) {

		contiguous_allocator Memory;
		size_t Alloc = Memory.allocate<int>(1);
		 
		printf(" > Malloc %2zu alignment deviation -- %zu\n", size - malloc_left, uintptr_t(Memory.locate<int>(Alloc)) % ALLOCATION_ALIGNMENT);

		batch_malloc(malloc_left - 1, size);
	}
}


int main() {

	{
		printf(" Contiguous memory allocator test\n\n");

		printf(" ! Initialization\n\n");

		printf(" > Default allocation\n");
		contiguous_allocator Memory0;
		Memory0.info();
		printf("\n");

		printf(" > Minimal allocation\n");
		contiguous_allocator Memory1(0, 0);
		Memory1.info();
		printf("\n");

		printf(" > Maximal allocation\n");
		contiguous_allocator Memory2(~size_t(0), ~size_t(0));
		Memory2.info();
		printf("\n\n");


		printf(" ! Oversized allocation\n\n");

		printf(" > Default allocation\n");
		Memory0.allocate<char>(MALLOC_SIZE_MINIMUM + 1);
		Memory0.info();
		printf("\n");

		printf(" > Minimal allocation\n");
		Memory1.allocate<char>(MALLOC_SIZE_MINIMUM + 1);
		Memory1.info();
		printf("\n");

		printf(" > Maximal allocation\n");
		Memory2.allocate<char>(MALLOC_SIZE_MAXIMUM + 1);
		Memory2.info();
		printf("\n\n");


		printf(" ! Allocation alignment\n\n");

		batch_malloc(100, 100);
		printf("\n\n");


		printf(" ! Allocation assignment and retrieval\n\n");

		struct vec3 { float x, y, z; };

		printf(" > Allocating 8 int(s)\n");
		size_t Alloc0 = Memory0.allocate<int>(8);
		*Memory0.locate<int>(Alloc0) = 12345678;
		Memory0.info();
		printf("\n");

		printf(" > Allocating 1 char(s)\n");
		size_t Alloc1 = Memory0.allocate<char>(1);
		*Memory0.locate<char>(Alloc1) = 'x';
		Memory0.info();
		printf("\n");

		printf(" > Allocating 3 float(s)\n");
		size_t Alloc2 = Memory0.allocate<float>(3);
		*Memory0.locate<float>(Alloc2) = 1234.5678;
		Memory0.info();
		printf("\n");

		printf(" > Allocating 1 double(s) with alignment of 33\n");
		size_t Alloc3 = Memory0.allocate<double>(1, 33);
		*Memory0.locate<double>(Alloc3) = 8765.4321;
		Memory0.info();
		printf("\n");

		printf(" > Allocating 65 char(s)\n");
		size_t Alloc4 = Memory0.allocate<char>(65);
		*Memory0.locate<char>(Alloc4) = 'd';
		Memory0.info();
		printf("\n");

		printf(" > Allocating 2 vec3(s) with alignment of 41\n");
		size_t Alloc5 = Memory0.allocate<vec3>(2, 41);
		*Memory0.locate<vec3>(Alloc5) = vec3{1.2, 3.4, 5.6};
		Memory0.info();
		printf("\n");

		printf(" > Retrieving assigned int ----- %i\n", *Memory0.locate<int>(Alloc0));
		printf(" > Retrieving assigned char ---- %c\n", *Memory0.locate<char>(Alloc1));
		printf(" > Retrieving assigned float --- %f\n", *Memory0.locate<float>(Alloc2));
		printf(" > Retrieving assigned double -- %f\n", *Memory0.locate<double>(Alloc3));
		printf(" > Retrieving assigned char ---- %c\n", *Memory0.locate<char>(Alloc4));

		vec3& Temp = *Memory0.locate<vec3>(Alloc5);
		printf(" > Retrieving assigned vec3 ---- %f %f %f\n", Temp.x, Temp.y, Temp.z);
		printf("\n");

		printf(" >   Valid address is nullptr -- %i\n", Memory0.locate<int>(Alloc0) == nullptr);
		printf(" > Invalid address is nullptr -- %i\n", Memory0.locate<char>(MALLOC_SIZE_MAXIMUM + 1) == nullptr);
		printf("\n\n");
	}


	return 0;
}


