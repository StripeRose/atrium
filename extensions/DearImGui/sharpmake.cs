using System.IO;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]

[module: Sharpmake.Include("../../apis/dx12/sharpmake.cs")]
[module: Sharpmake.Include("../../client/windows/sharpmake.cs")]
[module: Sharpmake.Include("../../engine/sharpmake.cs")]

[System.Flags]
public enum DearImGuiBackend
{
	None = 0,
	DirectX12 = 1 << 1,
	Win32 = 1 << 2
}

public static class DearImGuiTargetBackends
{
	public static DearImGuiBackend GetBackendsForTarget(Sharpmake.Target target)
	{
		if (target.Platform.HasFlag(Sharpmake.Platform.win32) || target.Platform.HasFlag(Sharpmake.Platform.win64))
		{
			return DearImGuiBackend.DirectX12 | DearImGuiBackend.Win32;
		}

		return DearImGuiBackend.None;
	}
}

[Sharpmake.Generate]
public class DearImGuiLibrary : Atrium.ExternalLibraryProject
{
	public DearImGuiLibrary()
	{
		Name = "imgui";
		SourceRootPath = "[project.SharpmakeCsPath]/../../libraries/imgui";

		SourceFilesExcludeRegex.Add(@".*imgui(\\|\/)(examples|misc).*");
	}

	public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
	{
		base.ConfigureAll(conf, target);
		conf.SolutionFolder = "Atrium/external";

		conf.Defines.Add("IMGUI_DEFINE_MATH_OPERATORS");
		conf.Defines.Add("IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
		conf.ExportDefines.Add("IMGUI_DISABLE_OBSOLETE_FUNCTIONS");

		conf.IncludePrivatePaths.Add(SourceRootPath);
		conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);

		SetupBackendExcludes(conf, target);
	}

	private void SetupBackendExcludes(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
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

[Sharpmake.Generate]
public class DearImGui : Atrium.StaticLibraryProject
{
	public DearImGui()
	{
		Name = "Dear ImGui";
		SourceRootPath = "[project.SharpmakeCsPath]";
	}

	public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
	{
		base.ConfigureAll(conf, target);
		conf.SolutionFolder = "Atrium/extensions";

		conf.ExportDefines.Add("IS_IMGUI_ENABLED=1");

		conf.AddPrivateDependency<Atrium.Engine>(target);
		conf.AddPublicDependency<DearImGuiLibrary>(target);

		DearImGuiBackend backends = DearImGuiTargetBackends.GetBackendsForTarget(target);

		if (backends.HasFlag(DearImGuiBackend.DirectX12))
		{
			conf.AddPrivateDependency<Atrium.DirectX12>(target);
			conf.Defines.Add("IS_IMGUI_BACKEND_DIRECTX12=1");
		}
		if (backends.HasFlag(DearImGuiBackend.Win32))
		{
			conf.AddPrivateDependency<Atrium.WindowsClient>(target);
			conf.Defines.Add("IS_IMGUI_BACKEND_WIN32=1");
		}
	}
}