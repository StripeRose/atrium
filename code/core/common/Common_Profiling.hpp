#pragma once

#include <tracy/Tracy.hpp>

void* operator new(std::size_t count);
void operator delete(void* ptr) noexcept;

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