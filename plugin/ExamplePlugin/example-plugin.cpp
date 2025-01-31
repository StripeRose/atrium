#include "example-plugin.hpp"

#include <Core_Diagnostics.hpp>

ExamplePlugin::ExamplePlugin()
{
	Atrium::Debug::Log("ExamplePlugin()");
}

ExamplePlugin::~ExamplePlugin()
{
	Atrium::Debug::Log("~ExamplePlugin()");
}

struct Atrium::PluginInfo ExamplePlugin::GetPluginInfo(const RoseCommon::SemanticVersion& /*anInterfaceVersion*/)
{
	Atrium::PluginInfo info;
	info.Create = []() { return std::make_shared<ExamplePlugin>(); };
	info.Name = L"example-plugin";
	info.Instancing = Atrium::PluginInfo::Instancing::AllowMultiple;
	return info;
}

ATRIUM_DECLARE_PLUGIN(ExamplePlugin);
