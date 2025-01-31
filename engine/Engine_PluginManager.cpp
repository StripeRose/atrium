// Filter "Plugin"

#include "Engine_PluginManager.hpp"

#include <Core_Diagnostics.hpp>

#include <rose-common/SemanticVersion.hpp>

#include <filesystem>

#if _WIN32
#include <Windows.h>
#endif

namespace Atrium
{
	RoseCommon::SemanticVersion GetPluginInterfaceVersion()
	{
		return { "0.0.0-wip" };
	}

	PluginManager::PluginManager(const std::optional<std::filesystem::path> aPluginDirectory)
		: myPluginRootPath(aPluginDirectory)
	{
		if (myPluginRootPath && !std::filesystem::is_directory(myPluginRootPath.value()))
			throw std::invalid_argument("Given plugin root path was not a directory.");
	}

	std::shared_ptr<Plugin> PluginManager::Require(const std::wstring_view& aPluginName, std::optional<bool> isUniqueInstance)
	{
		std::shared_ptr<PluginSource> library = GetOrCreatePluginLibrary(aPluginName);

		switch (library->Info.Instancing)
		{
			case PluginInfo::Instancing::One:
				if (isUniqueInstance.has_value() && isUniqueInstance.value())
				{
					Debug::LogWarning(L"Requested a unique instance of the '%s' plugin, but only one is allowed.", library->Info.Name.c_str());
					return nullptr;
				}
				break;
			case PluginInfo::Instancing::AllowMultiple:
				break;
			case PluginInfo::Instancing::Unique:
				if (isUniqueInstance.has_value() && !isUniqueInstance.value())
				{
					Debug::LogWarning(L"Requested a shared reference of the '%s' plugin, but each reference must be unique.", library->Info.Name.c_str());
					return nullptr;
				}
				break;
		}

		if (library->mySharedInstance.expired())
		{
			std::shared_ptr<Plugin> newInstance = library->Info.Create();
			library->mySharedInstance = newInstance;
			return newInstance;
		}

		if (isUniqueInstance.value_or(false))
			return library->Info.Create();
		else
			return library->mySharedInstance.lock();
	}

	PluginManager::PluginSource::~PluginSource()
	{
		#ifdef _WIN32
		HMODULE handle = std::any_cast<HMODULE>(Handle);
		if (handle)
			FreeLibrary(handle);
		#else
		#error "Plugin source doesn't have a defined destructor on this platform."
		#endif
	}

	std::shared_ptr<PluginManager::PluginSource> PluginManager::GetOrCreatePluginLibrary(const std::wstring_view& aPluginName)
	{
		if (myPluginRootPath.has_value())
		{
			const auto sharedPluginLibraryIterator = std::find_if(
				mySharedPluginLibraries.begin(),
				mySharedPluginLibraries.end(),
				[&aPluginName](const auto& anIt)
				{
					return anIt.first == aPluginName;
				}
			);

			if (sharedPluginLibraryIterator != mySharedPluginLibraries.end())
				return sharedPluginLibraryIterator->second;

			std::shared_ptr<PluginManager::PluginSource> newPlugin = LoadSharedPluginLibrary(FindPluginPath(aPluginName));
			mySharedPluginLibraries[newPlugin->Info.Name] = newPlugin;
			return newPlugin;
		}

		const auto staticPluginLibraryIterator = std::find_if(
			myStaticPluginLibraries.begin(),
			myStaticPluginLibraries.end(),
			[&aPluginName](const auto& anIt)
			{
				return anIt.first == aPluginName;
			}
		);

		if (staticPluginLibraryIterator != mySharedPluginLibraries.end())
			return staticPluginLibraryIterator->second;

		return nullptr;
	}

	std::shared_ptr<PluginManager::PluginSource> PluginManager::LoadSharedPluginLibrary(const std::filesystem::path& aLibraryPath)
	{
		#ifdef _WIN32
		if (!std::filesystem::is_regular_file(aLibraryPath))
			return nullptr;

		HMODULE hModule = LoadLibraryW(aLibraryPath.c_str());

		if (!hModule)
		{
			Debug::LogError(L"Failed to load plugin file `%s`. The file was not a valid library.", aLibraryPath.c_str());
			return nullptr;
		}

		PluginInfoFunction(*getPluginInfoFunction)() = reinterpret_cast<PluginInfoFunction(*)()>(
			GetProcAddress(hModule, "GetPluginInfoFunction")
			);

		if (!getPluginInfoFunction)
		{
			Debug::LogError(L"Failed to find the plugin-info function from plugin file `%s`. Ensure the plugin uses ATRIUM_DECLARE_PLUGIN.", aLibraryPath.c_str());
			return nullptr;
		}

		PluginInfoFunction pluginInfoFunction = getPluginInfoFunction();

		if (!pluginInfoFunction)
		{
			Debug::LogError(L"Failed to get a valid a valid plugin-info function from plugin file `%s`. Is it returning a null pointer? "
				L"If the intent is to block loading in certain circumstances, please do so by sending back info without set Create-function from GetPluginInfo().",
				aLibraryPath.c_str());
			return nullptr;
		}

		std::shared_ptr<PluginSource> newSource(new PluginSource());
		newSource->Handle = hModule;
		newSource->Info = pluginInfoFunction(GetPluginInterfaceVersion());
		return newSource;
		#endif
	}

	std::filesystem::path PluginManager::FindFileInDirectory(const std::filesystem::path& aDirectory, const std::wstring_view& aFileName)
	{
		for (const auto& entry : std::filesystem::directory_iterator(aDirectory))
		{
			if (entry.is_directory())
			{
				const std::filesystem::path path = FindFileInDirectory(entry, aFileName);
				if (!path.empty())
					return path;
			}
			else if (entry.path().filename() == aFileName)
			{
				return entry.path();
			}
		}

		return std::filesystem::path();
	}

	std::filesystem::path PluginManager::FindPluginPath(const std::wstring_view& aPluginName)
	{
		#if IS_DEBUG_BUILD
		{
			const std::filesystem::path debugPath = FindFileInDirectory(
				myPluginRootPath.value(),
				std::format(L"{}.d.dll", aPluginName)
			);
			if (!debugPath.empty())
				return debugPath;
		}
		#endif

		return FindFileInDirectory(
			myPluginRootPath.value(),
			std::format(L"{}.dll", aPluginName)
		);
	}

	void PluginManager::RegisterStaticPlugin(const PluginInfo& someInfo)
	{
		if (!someInfo.IsValid())
			throw std::invalid_argument("Plugin provided invalid information. Please correct or avoid calling Register().");

		std::shared_ptr<PluginSource> newSource(new PluginSource());
		newSource->Handle = HMODULE();
		newSource->Info = someInfo;

		myStaticPluginLibraries[someInfo.Name] = newSource;
	}
}