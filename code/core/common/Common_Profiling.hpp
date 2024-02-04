#pragma once

#pragma warning(push)
#pragma warning(disable: 26495 4100 6201)
#include <tracy/Tracy.hpp>
#pragma warning(pop)

#ifdef TRACY_ENABLE
_NODISCARD
_Ret_notnull_
_Post_writable_byte_size_(count)
_VCRT_ALLOCATOR
void* operator new(std::size_t count) noexcept(false);
void operator delete(void* ptr) noexcept;
#endif

//#ifdef TRACY_ENABLE
//
////namespace RoseGold::Profiling
////{
////	static constexpr const char* l = "Test";
////}
//
//#define ROSE_PROFILING_SCOPE ZoneScoped
//#define ROSE_PROFILING_SCOPE_NAMED(Name) ZoneScopedN(Name)
//#define ROSE_PROFILING_COLOR(ARGB) ZoneColor(0x00FFFFFF & ARGB)
//
//#else
//
//#define ROSE_PROFILING_SCOPE
//#define ROSE_PROFILING_SCOPE_NAMED(Name)
//#define ROSE_PROFILING_COLOR(ARGB)
//
//#endif