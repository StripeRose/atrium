using Sharpmake;

[System.Flags]
public enum DearImGuiBackend
{
	None = 0,
	DirectX12 = 1 << 1,
	Win32 = 1 << 2
}

public static class DearImGuiTargetBackends
{
	public static DearImGuiBackend GetBackendsForTarget(Target target)
	{
		if (target.Platform.HasFlag(Platform.win32) || target.Platform.HasFlag(Platform.win64))
		{
			return DearImGuiBackend.DirectX12 | DearImGuiBackend.Win32;
		}

		return DearImGuiBackend.None;
	}
}

[Generate]
public class DearImGuiLibrary : Project
{
	public DearImGuiLibrary()
	{
		Name = "imgui";

		string repositoryPath = ExternalProject.Git(
			"imgui",
			"https://github.com/ocornut/imgui",
			"v1.92.7-docking"
		);

		SourceRootPath = $"{repositoryPath}";
		SourceFilesExcludeRegex.Add(@"(examples|misc).*");

		AddTargets(new Target(
			Platform.win64,
			Util.AllFlags<DevEnv>(),
			Util.AllFlags<Optimization>()
		));
	}

	[Configure]
	public void ConfigureAll(Configuration conf, Target target)
	{
		Util.SetDefaultBuildArguments(conf, target);
		conf.SolutionFolder = "Atrium/External";

		conf.Defines.Add("IMGUI_DEFINE_MATH_OPERATORS");
		conf.Defines.Add("IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
		conf.ExportDefines.Add("IMGUI_DISABLE_OBSOLETE_FUNCTIONS");

		conf.IncludePrivatePaths.Add(SourceRootPath);
		conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Disable);

		SetupBackendExcludes(conf, target);
	}

	private void SetupBackendExcludes(Configuration conf, Target target)
	{
		DearImGuiBackend backends = DearImGuiTargetBackends.GetBackendsForTarget(target);

		if (!backends.HasFlag(DearImGuiBackend.DirectX12))
			conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx12.*");

		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*allegro5.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*android.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx9.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx10.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx11.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*glfw.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*glut.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*metal.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*opengl2.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*opengl3.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*osx.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*sdl2.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*sdl3.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*sdlgpu3.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*sdlrenderer.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*vulkan.*");
		conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*wgpu.*");

		if (!backends.HasFlag(DearImGuiBackend.Win32))
			conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*win32.*");
	}
}

namespace Atrium.Extension
{
	[Generate]
	public class DearImGui : Project
	{
		public DearImGui()
		{
			Name = "Dear ImGui";
			SourceRootPath = "[project.SharpmakeCsPath]";

			AddTargets(new Target(
				Platform.win64,
				Util.AllFlags<DevEnv>(),
				Util.AllFlags<Optimization>()
			));
		}

		[Configure]
		public void ConfigureAll(Configuration conf, Target target)
		{
			Util.SetDefaultBuildArguments(conf, target);
			conf.SolutionFolder = "Atrium/Extension";

			conf.ExportDefines.Add("IS_IMGUI_ENABLED=1");

			conf.AddPrivateDependency<Engine>(target);
			conf.AddPublicDependency<DearImGuiLibrary>(target);

			DearImGuiBackend backends = DearImGuiTargetBackends.GetBackendsForTarget(target);

			if (backends.HasFlag(DearImGuiBackend.DirectX12))
			{
				conf.AddPrivateDependency<Graphics.DirectX12>(target);
				conf.Defines.Add("IS_IMGUI_BACKEND_DIRECTX12=1");
			}
			if (backends.HasFlag(DearImGuiBackend.Win32))
			{
				conf.AddPrivateDependency<Client.Windows>(target);
				conf.Defines.Add("IS_IMGUI_BACKEND_WIN32=1");
			}
		}
	}
}