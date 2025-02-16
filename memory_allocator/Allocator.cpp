#include "Allocator.hpp"


using namespace allocator;


size_t contiguous_allocator::address_alignment_offset(const uintptr_t address, const size_t alignment) {
	return (alignment - (address % alignment)) % alignment;
}


contiguous_allocator::contiguous_allocator(size_t malloc_size_minimum, size_t malloc_size_maximum) {

	if (SAFETY_MEASURES) {
		malloc_size_minimum = (malloc_size_minimum < MALLOC_SIZE_MINIMUM) ? MALLOC_SIZE_MINIMUM : malloc_size_minimum;
		malloc_size_minimum = (malloc_size_minimum > MALLOC_SIZE_MAXIMUM) ? MALLOC_SIZE_MAXIMUM : malloc_size_minimum;

		malloc_size_maximum = (malloc_size_maximum > MALLOC_SIZE_MAXIMUM) ? MALLOC_SIZE_MAXIMUM : malloc_size_maximum;
		malloc_size_maximum = (malloc_size_maximum < malloc_size_minimum) ? malloc_size_minimum : malloc_size_maximum;
	}

	Malloc = malloc(malloc_size_minimum + ALLOCATION_ALIGNMENT);

	if (Malloc == nullptr) {
		printf(" x Memory initialization failed.\n");
		Malloc_size_current = 0;
	}
	else {
		printf(" > Memory initialized successfully.\n");
		Malloc_size_current = malloc_size_minimum;
	}

	Malloc_size_maximum        = malloc_size_maximum;
	Malloc_expansion_increment = malloc_size_minimum;

	Data_root = uintptr_t(Malloc) + address_alignment_offset(uintptr_t(Malloc), ALLOCATION_ALIGNMENT);
}


contiguous_allocator::~contiguous_allocator() {
	free(Malloc);
}


bool contiguous_allocator::member(const void* data) {
	bool Condition0 = uintptr_t(data) >= Data_root;
	bool Condition1 = uintptr_t(data) <  Data_root + Data_head;

	return Condition0 && Condition1;
}


void contiguous_allocator::clear() {
	Data_head = 0;
};


void contiguous_allocator::info() {
	printf(" Size ------- %zu\n", Malloc_size_current);
	printf(" Occupied --- %zu\n", Data_head);
	printf(" Remaining -- %zu\n", Malloc_size_current - Data_head);
	printf(" Available -- %zu\n", Malloc_size_maximum - Data_head);
}