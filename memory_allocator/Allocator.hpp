// Title ------ Memory allocator
// Author ----- Delectros
// Last edit -- 2025.02.14


// Preprocessor definitions required by MSVC:
/*
	_CRT_SECURE_NO_DEPRECATE
	_CRT_NONSTDC_NO_DEPRECATE
*/




#pragma once


// Standard headers
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>




namespace allocator {

	static constexpr bool SAFETY_MEASURES = true; // User-defined

	// Set it to  0, and watch the world burn
	static constexpr size_t MALLOC_SIZE_MINIMUM =  512; // User-defined
	static constexpr size_t MALLOC_SIZE_MAXIMUM = 2048; // User-defined
	// Set it to ~0, and watch your system freeze

	// Set to whatever is the highest alignment your intrinsics need, otherwise keep it  16  (malloc's default alignment)
	// x Non-power of two values will result in an undefined behaviour and/or crash
	// > Use  16  for  SSE family, AES
	// > Use  32  for  AVX family, FMA
	// > Use  64  for  AVX-512 family, SVML
	static constexpr size_t ALLOCATION_ALIGNMENT = 64; // User-defined

	static constexpr uintptr_t ALLOCATION_FAILURE = ~uintptr_t(0);




	class contiguous_allocator {

	private:
		
		void* Malloc = nullptr;

		size_t Malloc_size_current = 0;
		size_t Malloc_size_maximum = 0;

		size_t Malloc_expansion_increment = 0;

		uintptr_t Data_root = 0;
		size_t    Data_head = 0;


		size_t address_alignment_offset(const uintptr_t address, const size_t alignment);


	public:

		 contiguous_allocator(size_t malloc_size_minimum = MALLOC_SIZE_MINIMUM, size_t malloc_size_maximum = MALLOC_SIZE_MAXIMUM);
		~contiguous_allocator();

		
		template<typename type>
		uintptr_t allocate(const size_t allocation_size, size_t allocation_alignment = alignof(type)) {
		
			if (SAFETY_MEASURES) {

				if (allocation_alignment < alignof(type)) {
					allocation_alignment = alignof(type);
				}

				if (allocation_alignment > ALLOCATION_ALIGNMENT) {
					printf(" x Memory allocation canceled.\n");
					printf("   Requested allocation alignment was too high.\n");
					return ALLOCATION_FAILURE;
				}

				if (allocation_alignment & (allocation_alignment - 1)) {
					allocation_alignment--;
					allocation_alignment |= allocation_alignment >>  1;
					allocation_alignment |= allocation_alignment >>  2;
					allocation_alignment |= allocation_alignment >>  4;
					allocation_alignment |= allocation_alignment >>  8;
					allocation_alignment |= allocation_alignment >> 16;
					allocation_alignment |= allocation_alignment >> 32;
					allocation_alignment++;
				}
			}

			size_t Allocation_size   = allocation_size * sizeof(type);
			size_t Allocation_offset = address_alignment_offset(Data_head, allocation_alignment);


			size_t Memory_remaining = Malloc_size_current - Data_head;
			size_t Memory_available = Malloc_size_maximum - Data_head;

			size_t Memory_required = Allocation_size + Allocation_offset;


			if (Memory_required > Memory_available) {
				printf(" x Memory allocation canceled.\n");
				printf("   Estimated allocation size exceeded the limit by %zu byte(s).\n", Memory_required - Memory_available);
				return ALLOCATION_FAILURE;
			}


			if (Memory_required > Memory_remaining) {

				size_t Malloc_expansion_integer = (Memory_required - Memory_remaining) / Malloc_expansion_increment;
				size_t Malloc_expansion_decimal = (Memory_required - Memory_remaining) % Malloc_expansion_increment;

				size_t Malloc_size_new = Malloc_size_current + (Malloc_expansion_integer + (Malloc_expansion_decimal > 0)) * Malloc_expansion_increment;
				       Malloc_size_new = (Malloc_size_new > MALLOC_SIZE_MAXIMUM) ? MALLOC_SIZE_MAXIMUM : Malloc_size_new;


				void* Malloc_new = malloc(Malloc_size_new + ALLOCATION_ALIGNMENT);

				if (Malloc_new == nullptr) {
					printf(" x Memory reallocation failed.\n");
					printf("   Attempted reallocation size was %zu byte(s).\n", Malloc_size_new);
					return ALLOCATION_FAILURE;
				}


				uintptr_t Data_root_new = uintptr_t(Malloc_new) + address_alignment_offset(uintptr_t(Malloc_new), ALLOCATION_ALIGNMENT);

				memmove((void*)Data_root_new, (void*)Data_root, Data_head);
				free(Malloc);

				
				Malloc              = Malloc_new;
				Malloc_size_current = Malloc_size_new;

				Data_root = Data_root_new;
			}

			
			size_t Data_address = Data_head + Allocation_offset;
			       Data_head    = Data_address + Allocation_size;

			return Data_address;
		}
		

		template<typename type>
		type* locate(const size_t data_address) {
			return (data_address < Data_head) ? (type*)(Data_root + data_address) : nullptr;
		}


		bool member(const void* data);

		void clear();
		
		void info();
	};
}