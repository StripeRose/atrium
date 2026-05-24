using Sharpmake;

[Generate]
public class VulkanBootstrap : Project
{
	public VulkanBootstrap()
	{
		Name = "vk-bootstrap";

		string repositoryPath = ExternalProject.Git(
			"vk-bootstrap",
			"https://github.com/charles-lunarg/vk-bootstrap/",
			"v1.4.350"
		);

		SourceRootPath = $"{repositoryPath}/src";

		AddTargets(new Target(
			Platform.win32 | Platform.win64 | Platform.linux,
			Util.AllFlags<DevEnv>(),
			Util.AllFlags<Optimization>()
		));
	}

	[Configure]
	public void ConfigureAll(Configuration conf, Target target)
	{
		Util.SetDefaultBuildArguments(conf, target);
		conf.SolutionFolder = "Atrium/External";

		conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Disable);
		conf.Options.Add(Options.Vc.General.WarningLevel.Level0);

		conf.IncludePaths.Add("$(VULKAN_SDK)/Include");
		switch (target.Platform)
		{
			case Platform.win32:
				conf.Defines.Add("VK_USE_PLATFORM_WIN32_KHR");
				conf.LibraryPaths.Add("$(VULKAN_SDK)/Lib32");
				break;
			case Platform.win64:
				conf.Defines.Add("VK_USE_PLATFORM_WIN32_KHR");
				conf.LibraryPaths.Add("$(VULKAN_SDK)/Lib");
				break;
			case Platform.linux:
				conf.LibraryFiles.Add("${CMAKE_DL_LIBS}");
				break;
		}
		conf.LibraryFiles.Add("vulkan-1.lib");
	}
}

namespace Atrium.Graphics
{
	[Generate]
	public class Vulkan : Project
	{
		public static bool IsSupported(Target target)
		{
			// Todo: Add checks to determine support based on intended build target also.

			return !string.IsNullOrEmpty(
				System.Environment.GetEnvironmentVariable("VULKAN_SDK")
			);
		}

		public Vulkan()
		{
			Name = "Vulkan";
			SourceRootPath = "[project.SharpmakeCsPath]";

			AddTargets(new Target(
				Platform.win64,
				Util.AllFlags<DevEnv>(),
				Util.AllFlags<Optimization>()
			));
		}

		[Configure]
		public void ConfigureAll(Project.Configuration conf, Target target)
		{
			Util.SetDefaultBuildArguments(conf, target);
			conf.SolutionFolder = "Atrium/Graphics";

			conf.AddPrivateDependency<Atrium.Core>(target);
			conf.AddPrivateDependency<VulkanBootstrap>(target);

			conf.Defines.Add("VULKAN_HPP_NO_STRUCT_CONSTRUCTORS");
			conf.Defines.Add("VULKAN_HPP_NO_EXCEPTIONS");
			conf.Defines.Add("VULKAN_HPP_USE_STD_EXPECTED ");

			conf.IncludePaths.Add("$(VULKAN_SDK)/Include");
			switch (target.Platform)
			{
				case Sharpmake.Platform.win32:
					conf.Defines.Add("VK_USE_PLATFORM_WIN32_KHR");
					conf.LibraryPaths.Add("$(VULKAN_SDK)/Lib32");
					break;
				case Sharpmake.Platform.win64:
					conf.Defines.Add("VK_USE_PLATFORM_WIN32_KHR");
					conf.LibraryPaths.Add("$(VULKAN_SDK)/Lib");
					break;
			}
			conf.LibraryFiles.Add("vulkan-1.lib");
		}
	}
}