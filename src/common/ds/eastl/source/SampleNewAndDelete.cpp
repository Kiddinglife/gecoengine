/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <EABase/eabase.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <new>

///////////////////////////////////////////////////////////////////////////////
// throw specification wrappers, which allow for portability.
///////////////////////////////////////////////////////////////////////////////

#if defined(EA_COMPILER_NO_EXCEPTIONS) && (!defined(__MWERKS__) || defined(_MSL_NO_THROW_SPECS)) && !defined(EA_COMPILER_RVCT)
#define THROW_SPEC_0    // Throw 0 arguments
#define THROW_SPEC_1(x) // Throw 1 argument
#else
#define THROW_SPEC_0    throw()
#define THROW_SPEC_1(x) throw(x)
#endif

#ifdef _WIN32
#pragma warning( disable : 4290 )
#define aligned_alloc _aligned_offset_malloc
#endif

///////////////////////////////////////////////////////////////////////////////
// operator new used by EASTL
///////////////////////////////////////////////////////////////////////////////

void* operator new[](size_t size, const char* /*name*/, int /*flags*/,
		unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
		THROW_SPEC_1(std::bad_alloc)
{
	return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
		const char* /*name*/, int flags, unsigned /*debugFlags*/,
		const char* /*file*/, int /*line*/) THROW_SPEC_1(std::bad_alloc)
{
	// Substitute your aligned malloc.

#ifdef _WIN32
	return aligned_alloc(size, alignment, alignmentOffset);
#else
	return aligned_alloc(size, alignment);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Other operator new as typically required by applications.
// malloc() will align by default by 4 bytes
///////////////////////////////////////////////////////////////////////////////

void* operator new(size_t size) THROW_SPEC_1(std::bad_alloc)
{
	return malloc(size);
}

void* operator new[](size_t size) THROW_SPEC_1(std::bad_alloc)
{
	return malloc(size);
}

///////////////////////////////////////////////////////////////////////////////
// Operator delete, which is shared between operator new implementations.
///////////////////////////////////////////////////////////////////////////////

void operator delete(void* p) THROW_SPEC_0
{
	if (p) // The standard specifies that 'delete NULL' is a valid operation.
		free(p);
}

void operator delete[](void* p) THROW_SPEC_0
{
	if (p)
		free(p);
}

