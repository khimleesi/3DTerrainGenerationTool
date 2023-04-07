/*!
	@file Allocator.cpp
	@brief Manages all heap memory allocations within the Pretty Engine.
	@author PrettyBinary
	@date 24/07/2018

*******************************************************************************************************************/

/*! @cond */
#define COG_MEMORY_ALIGNMENT 16
/*! @endcond */

#include <malloc.h>
#include <cstring>
#include "Allocator.h"
#include "utilities/Log.h"
#include "utilities/String.h"

	const uint64_t KiB = 1024;		/*!< 1 Kibibyte. */
	const uint64_t MiB = KiB * KiB;	/*!< 1 Mebibyte. */
	const uint64_t GiB = MiB * KiB;	/*!< 1 Gibibyte. */
	const uint64_t TiB = GiB * KiB;	/*!< 1 Tebibyte. */
	

	/*!
	@brief
		A function that allocates aligned memory on the heap.

	@param size
		The size of the memory block.

	@return
		Allocates memory on the heap (done internally).
	*******************************************************************************************************************/
	void* Allocator::Allocate(size_t size)
	{
		COG_ASSERT(size < GiB);

		/* Store extra size so we can store the size of this memory allocation before the memory block */
		size_t totalSize	= size + sizeof(size_t);
		uint8_t* result		= (uint8_t*)_aligned_malloc(totalSize, COG_MEMORY_ALIGNMENT);

		/* Set the memory to null */
		memset(result, 0, totalSize);
		
		/* Store the size of this memory block in the memory allocated so we can grab it later */
		memcpy(result, &size, sizeof(size_t));
	
#if defined(COG_DEBUG)
		if (size > MiB) { COG_WARNING("Large allocation (> 1MiB) at location: 0x", (void*)result, "with size:", size, "bytes"); }
		else			{ COG_MEMORY("Allocated at location: 0x", (void*)result, "with size:", size, "bytes"); }
		
		s_Statistics.totalAllocated += size;
		s_Statistics.currentUsed += size;
		s_Statistics.allocationCount++;
#endif

		/* Now we have stored the size, move result ptr to start of memory block */
		result += sizeof(size_t);

		return result;
	}


	/*!
	@brief
		A function that de-allocates aligned memory on the heap.

	@param block
		The block of memory to be freed.

	@return
		De-allocates memory on the heap (done internally).
	*******************************************************************************************************************/
	void Allocator::Free(void* block)
	{
		/*	Because we aligned extra memory in allocations to store the size
			we move the block memory ptr back, so we can delete the extra memory allocated aswell */
		uint8_t* memory = ((uint8_t*)block) - sizeof(size_t);

		/* Now de-reference the memory ptr and we will get the value we stored (the size of the memory allocated (block)) */
		size_t size = *(size_t*)memory;
		
#if defined(COG_DEBUG)
		COG_MEMORY("De-allocated at location: 0x", (void*)memory, "with size:", size, "bytes");

		s_Statistics.totalFreed += size;
		s_Statistics.currentUsed -= size;
#endif

		/* Now free the size we stored and the memory we stored */
		_aligned_free(memory);
	}


	/*!
	@brief
		A helper function that converts bytes into a string.

	@param bytes
		The bytes to convert.

	@return
		A string representation of the bytes.
	*******************************************************************************************************************/
	const char* Allocator::BytesToString(const uint64_t& bytes)
	{
		if (bytes > KiB) { return strcat(String::Float(bytes / (float)KiB), " KiB"); }
		if (bytes > MiB) { return strcat(String::Float(bytes / (float)MiB), " MiB"); }
		if (bytes > GiB) { return strcat(String::Float(bytes / (float)GiB), " GiB"); }

		return String::Float((float)bytes, 0);
	}


	/*!
	@brief
		A function that gets the total heap allocations since the program launched.

	@return
		A string representation of the total heap allocations.
	*******************************************************************************************************************/
	const char* Allocator::GetTotalAllocated()
	{
		return BytesToString(s_Statistics.totalAllocated);
	}


	/*!
	@brief
		A function that gets the total heap de-allocations since the program launched.

	@return
		A string representation of the total heap de-allocations.
	*******************************************************************************************************************/
	const char* Allocator::GetTotalFreed()
	{
		return BytesToString(s_Statistics.totalFreed);
	}


	/*!
	@brief
		A function that gets the currently used (not yet freed) heap allocations.

	@return
		A string representation of the heap allocations currently used.
	*******************************************************************************************************************/
	const char* Allocator::GetCurrentUsed()
	{
		return BytesToString(s_Statistics.currentUsed);
	}


	/*!
	@brief
		A function that gets the total heap allocation count.

	@return
		A string representation of the number of heap allocations.
	*******************************************************************************************************************/
	const char* Allocator::GetAllocationCount()
	{
		return BytesToString(s_Statistics.allocationCount);
	}

	Allocator::MemoryStatistics Allocator::s_Statistics;