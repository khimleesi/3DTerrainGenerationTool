#pragma once
/*!
	@file Memory.h
	@brief Contains overloaded versions of new and delete operators.
	@author PrettyBinary
	@date 24/07/2018

	@warning To use the overloaded new and delete operators within the Pretty Engine, you must make
	sure you include this header file within all header/source files where you allocate memory.

*******************************************************************************************************************/

#include "Allocator.h"

#pragma warning(disable : 4595)

inline void* operator new(size_t size)
{
	return Allocator::Allocate(size);
}

inline void* operator new[](size_t size)
{
	return Allocator::Allocate(size);
}

inline void operator delete(void* block)
{
	Allocator::Free(block);
}

inline void operator delete[](void* block)
{
	Allocator::Free(block);
}

#pragma warning(default : 4595)