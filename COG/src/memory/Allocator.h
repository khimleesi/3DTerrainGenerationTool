#pragma once
/*!
	@file Allocator.h
	@brief Manages all heap memory allocations within the Pretty Engine.
	@author PrettyBinary
	@date 24/07/2018

	@class Pretty::Utilities::Allocator
	@brief Static class that handles all memory allocations/de-allocations.
	
	@note You must include "Memory.h" whenever you allocate heap memory if you wish to use this internal allocator.

	@warning __NO DIRECT ACCESS__
	- There are functions within this class that should not be accessed directly - these are pre-fixed `_`

	This class handles and logs all heap memory allocations/de-allocations through-out the entire program.
	
	> Usage

	You can access the memory statistics at any time by calling any of the following:
		
		GetTotalAllocated()
		GetTotalFreed()
		GetCurrentUsed()
		GetAllocationCount()

	@note The internal allocator has a memory alignment of base 16. This is important when using shaders,
	specifically with both the DirectX and OpenGL API's. You can rest assured that all your data will be
	aligned correctly CPU side, before passing it to the GPU.

*******************************************************************************************************************/

#include <stdint.h>
#include <string>

	extern const uint64_t KiB, MiB, GiB, TiB;

	class Allocator {
	
	private:
		/*! @brief Struct that contains the overall memory statistics. */
		struct MemoryStatistics {

			uint64_t totalAllocated;	/*!< Total heap memory allocated since the beginning of the program. */
			uint64_t totalFreed;		/*!< Total heap memory freed since the beginning of the program. */
			uint64_t currentUsed;		/*!< Total heap memory currently in use (not yet been freed). */
			uint64_t allocationCount;	/*!< Total heap allocation count overall. */
		};

	public:
		static const char* GetTotalAllocated();
		static const char* GetTotalFreed();
		static const char* GetCurrentUsed();
		static const char* GetAllocationCount();
		
	public:
		static void*	Allocate(size_t size);
		static void		Free(void* block);

	private:	
		static const char* BytesToString(const uint64_t& bytes);

	private:
		static MemoryStatistics s_Statistics; /*!< Stores all of the memory statistics since the program launched. */
	};