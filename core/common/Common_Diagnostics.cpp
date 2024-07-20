#include "stdafx.hpp"

#include "Common_Diagnostics.hpp"

#ifdef TRACY_ENABLE
_NODISCARD
_Ret_notnull_
_Post_writable_byte_size_(count)
_VCRT_ALLOCATOR
void* operator new(std::size_t count) noexcept(false)
{
	auto ptr = malloc(count);
	if (!ptr)
		throw std::bad_alloc();
	TracyAlloc(ptr, count);
	return ptr;
}

void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}
#endif
