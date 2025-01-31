#pragma once

#include <Engine_Plugin.hpp>

class ExamplePlugin : public Atrium::Plugin
{
public:
	ExamplePlugin();
	~ExamplePlugin();

	static Atrium::PluginInfo GetPluginInfo(const RoseCommon::SemanticVersion& anInterfaceVersion);
};