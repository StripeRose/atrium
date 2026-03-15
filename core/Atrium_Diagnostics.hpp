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

void TracyDebug_AttachLog();
void TracyDebug_DetachLog();

#define PROFILE_ATTACH_LOG() TracyDebug_AttachLog()
#define PROFILE_DETACH_LOG() TracyDebug_DetachLog()

#define PROFILE_APPINFO(aText) TracyAppInfo(aText, std::strlen(aText))

#define PROFILE_FRAMEMARK() FrameMark
#define PROFILE_FRAMEMARK_NAMED(aName) FrameMarkNamed(aName)

#define PROFILE_SCOPE() ZoneScoped
#define PROFILE_SCOPE_NAME(aName) ZoneScopedN(aName)
#define PROFILE_SCOPE_COLOR(anARGB) ZoneScoped; ZoneColor(0x00FFFFFF & anARGB)
#define PROFILE_SCOPE_NAME_COLOR(aName, anARGB) ZoneScopedNC(aName, 0x00FFFFFF & anARGB)

#define PROFILE_ACTIVE_SCOPE_TEXT(aFormat, ...) ZoneTextF(aFormat, ##__VA_ARGS__)
#define PROFILE_ACTIVE_SCOPE_NAME(aFormat, ...) ZoneNameF(aFormat, ##__VA_ARGS__)
#define PROFILE_ACTIVE_SCOPE_COLOR(aColor) ZoneColor(0x00FFFFFF & anARGB)
#define PROFILE_ACTIVE_SCOPE_VALUE(aValue) ZoneValue(aValue)

#define PROFILE_PLOT(aName, aValue) TracyPlot(aName, aValue)

#else

#define PROFILE_APPINFO(aText)

#define PROFILE_FRAMEMARK()
#define PROFILE_FRAMEMARK_NAMED(aName)

#define PROFILE_SCOPE()
#define PROFILE_SCOPE_NAME(aName)
#define PROFILE_SCOPE_COLOR(anARGB)
#define PROFILE_SCOPE_NAME_COLOR(aName, anARGB)

#define PROFILE_ACTIVE_SCOPE_TEXT(aFormat, ...)
#define PROFILE_ACTIVE_SCOPE_NAME(aFormat, ...)
#define PROFILE_ACTIVE_SCOPE_COLOR(aColor)
#define PROFILE_ACTIVE_SCOPE_VALUE(aValue)

#define PROFILE_PLOT(aName, aValue)

#endif