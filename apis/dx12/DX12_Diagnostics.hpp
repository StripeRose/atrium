#pragma once

#include "Atrium_Diagnostics.hpp"

#ifdef TRACY_ENABLE
#pragma warning(push)
#pragma warning(disable:26495 4100 6201)
#include <tracy/TracyD3D12.hpp>
#pragma warning(pop)

#else

#define TracyD3D12Zone(...)
#define TracyD3D12NewFrame(...)
#define TracyD3D12Collect(...)

#endif