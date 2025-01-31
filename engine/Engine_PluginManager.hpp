// Filter "Plugin"

#include "Engine_Plugin.hpp"

#include <any>
#include <filesystem>
#include <map>

namespace Atrium
{
	/**
	 * @brief Handles loading and unloading of engine-plugin from dynamically loaded libraries or statically linked sources.
	 * 
	 * Expects the plugin file pattern:
	 *  - Windows: "library-name.dll"; "library-name.d.dll"
	 *  - Linux: Not currently implemented, but will expected something like "liblibrary-name.so"; "liblibrary-name.d.so"
	 * 
	 * Todo:
	 *  - Add support for plugins loading other plugins via some Plugin function.
	 *    Will require enforcing plugins to be all deleted before the manager disappears.
	 */
	class PluginManager
	{
	public:
		/**
		 * @brief Create a PluginManager instance with a given plugin root-directory.
		 * @param aPluginDirectory The path each dynamically loaded plugin is in. If omitted, dynamic plugin-loading is disabled.
		 */
		PluginManager(const std::optional<std::filesystem::path> aPluginDirectory);

		/**
		 * @brief Register a statically linked plugin, to make it available with Require().
		 *        Dynamically loaded plugins of the same name will take presedence to allow replacement.
		 *        Additionally, dynamically loaded plugins can load statically linked plugins. (Todo: Not yet implemented)
		 * 
		 *        Note:
		 *        Because this should be fully controlled before compilation, the function does not allow
		 *        a plugin with invalid information, and will throw an exception if this is the case.
		 *
		 *        If a plugin shouldn't be registered, avoid the call to Register() instead of providing bad info.
		 * 
		 * @tparam T The plugin class to register.
		 */
		template <typename T>
		void Register()
		{
			RegisterStaticPlugin(T::GetPluginInfo());
		}

		/**
		 * @brief Request an instance of a plugin with a given name, based on the plugin instancing rules.
		 * @param aPluginName Name of the plugin to load.
		 * @param isUniqueInstance Requests a unique instance, if the plugin allows multiple instances.
		 * @return A reference to the loaded plugin, if successfully initialized.
		 */
		std::shared_ptr<Plugin> Require(const std::wstring_view& aPluginName, std::optional<bool> isUniqueInstance = {});

	private:
		struct PluginSource
		{
			~PluginSource();

			PluginInfo Info;
			std::any Handle;

			std::weak_ptr<Plugin> mySharedInstance;
		};

		std::shared_ptr<PluginSource> GetOrCreatePluginLibrary(const std::wstring_view& aPluginName);

		std::shared_ptr<PluginSource> LoadSharedPluginLibrary(const std::filesystem::path& aLibraryPath);

		std::filesystem::path FindFileInDirectory(const std::filesystem::path& aDirectory, const std::wstring_view& aFileName);

		std::filesystem::path FindPluginPath(const std::wstring_view& aPluginName);

		void RegisterStaticPlugin(const PluginInfo& someInfo);

		std::map<std::wstring, std::shared_ptr<PluginSource>> mySharedPluginLibraries;

		std::map<std::wstring, std::shared_ptr<PluginSource>> myStaticPluginLibraries;

		std::optional<std::filesystem::path> myPluginRootPath;
	};
}