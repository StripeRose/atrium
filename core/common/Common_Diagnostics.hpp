#pragma once

#include <rose-common/Debug.hpp>

namespace RoseGold
{
	using Debug = RoseCommon::Debug;
}

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

//#define PROFILE_SCOPE() ZoneScoped
//#define PROFILE_SCOPE_NAMED(aName) ZoneScopedN(aName)
//#define PROFILE_SCOPE_COLOR(anARGB) ZoneScoped; ZoneColor(0x00FFFFFF & anARGB)
//#define PROFILE_SCOPE_COLOREDNAMED(aName, anARGB) ZoneScopedN(aName); ZoneColor(0x00FFFFFF & anARGB)
//
//#else
//
//#define PROFILE_SCOPE()
//#define PROFILE_SCOPE_NAMED(aName)
//#define PROFILE_SCOPE_COLOR(anARGB)
//#define PROFILE_SCOPE_COLOREDNAMED(aName, anARGB)

#endif