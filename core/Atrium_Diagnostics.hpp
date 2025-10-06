#pragma once

#include <rose-common/Debug.hpp>

#ifdef TRACY_ENABLE
#pragma warning(push)
#pragma warning(disable: 26495 4100 4714 6201)
#include <tracy/Tracy.hpp>
#pragma warning(pop)

_NODISCARD
_Ret_notnull_
_Post_writable_byte_size_(count)
_VCRT_ALLOCATOR
void* operator new(std::size_t count) noexcept(false);
void operator delete(void* ptr) noexcept;

#define PROFILE_FRAMEMARK() FrameMark
#define PROFILE_SCOPE() ZoneScoped
#define PROFILE_SCOPE_NAME(aName) ZoneScopedN(aName)
#define PROFILE_SCOPE_COLOR(anARGB) ZoneScoped; ZoneColor(0x00FFFFFF & anARGB)
#define PROFILE_SCOPE_NAME_COLOR(aName, anARGB) ZoneScopedN(aName); ZoneColor(0x00FFFFFF & anARGB)

#else

#define PROFILE_FRAMEMARK()
#define PROFILE_SCOPE()
#define PROFILE_SCOPE_NAME(aName)
#define PROFILE_SCOPE_COLOR(anARGB)
#define PROFILE_SCOPE_NAME_COLOR(aName, anARGB)

#endif