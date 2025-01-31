// Filter "Plugin"

#include "Engine_PluginVisibility.hpp"

#include <rose-common/EventSlot.hpp>
#include <rose-common/SemanticVersion.hpp>

#include <functional>
#include <memory>
#include <string>

/**
 * Creating an engine plugin:
 * 
 * Create a class inheriting Atrium::Plugin and define it as a valid plugin
 * by using the ATRIUM_DECLARE_PLUGIN macro in it's source-file.
 */

namespace Atrium
{
	class Plugin;

	/**
	 * @brief Basic interface for an engine plugin.
	 */
	class Plugin
	{
		friend class PluginManager;

	public:
		virtual ~Plugin() = default;

		// Todo: Define required functions

		// Todo: Define function to set up dependencies. Returning false will cancel plugin init and load.

		// Todo: Define interface for fetching plugin capabilities, such as:
		// - Graphics API
		// - Input API
		// - Audio API
		// - Window API
		// - Asset loader / importer
		// - ...
	};

	/**
	 * @brief Contains the information required for a plugin to be created and used.
	 */
	struct PluginInfo
	{
		/**
		 * @brief Defines which function creates an instance of the plugin.
		 */
		std::function<std::shared_ptr<Plugin>()> Create;

		enum class Instancing
		{
			// Each reference is a unique instance.
			Unique,

			// Only one instance of the plugin can exist, and is shared across all users.
			One,

			// Defaults to sharing a single instance, but allows additional instances if explicitly specified.
			AllowMultiple
		} Instancing = Instancing::One;

		/**
		 * @brief The plugin's name.
		 */
		std::wstring Name;

		inline bool IsValid() const { return Create && !Name.empty(); }
	};

	using PluginInfoFunction = Atrium::PluginInfo(*)(const RoseCommon::SemanticVersion&);
}

#define ATRIUM_DECLARE_PLUGIN(T) \
extern "C" { \
	ATRIUM_PLUGIN_EXPORT Atrium::PluginInfoFunction GetPluginInfoFunction() { return &T::GetPluginInfo; } \
}