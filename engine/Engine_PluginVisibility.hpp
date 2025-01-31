// Filter "Plugin"
#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef _MSC_VER
		#define ATRIUM_VISIBILITY_EXPORT __declspec(dllexport)
	#else
		#define ATRIUM_VISIBILITY_EXPORT __attribute__ ((dllexport))
	#endif
#else
	#define ATRIUM_VISIBILITY_EXPORT __attribute__ ((visibility("default")))
#endif

#ifdef ATRIUM_PLUGIN_DLL
#define ATRIUM_PLUGIN_EXPORT ATRIUM_VISIBILITY_EXPORT
#else
#define ATRIUM_PLUGIN_EXPORT
#endif
