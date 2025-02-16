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


bool contiguous_allocator::dump(const char* file_path) {

	FILE* File = fopen(file_path, "wb");

	if (!File) {
		printf(" x Failed to open ");
		printf(file_path);
		printf(".\n");
		return false;
	}

	fwrite((void*)Data_root, 1, Data_head, File);
	fclose(File);

	return true;
}


bool contiguous_allocator::load(const char* file_path) {

	FILE* File = fopen(file_path, "rb");
	
	if (!File) {
		printf(" x Failed to open ");
		printf(file_path);
		printf(".\n");
		return false;
	}

	fseek(File, 0, SEEK_END);
	size_t Data_size = ftell(File);

	if (Data_size > Malloc_size_current) {
		printf(" x Not enough memory to load dump file.\n"); // Temporary solution
		return false;
	}

	Data_head = Data_size;

	fread((void*)Data_root, 1, Data_head, File);
	fclose(File);

	return true;
}


void contiguous_allocator::clear(const size_t data_head) {
	Data_head = (data_head > Data_head) ? Data_head : data_head;
};


void contiguous_allocator::info() {
	printf(" Size ------- %zu\n", Malloc_size_current);
	printf(" Occupied --- %zu\n", Data_head);
	printf(" Remaining -- %zu\n", Malloc_size_current - Data_head);
	printf(" Available -- %zu\n", Malloc_size_maximum - Data_head);
}