using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class DearImGui : Atrium.ExternalLibraryProject
{
    public DearImGui()
    {
        Name = "imgui";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "imgui");
        
        SourceFilesExcludeRegex.Add(@".*imgui(\\|\/)(examples|misc).*");
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);
        conf.SolutionFolder = "Atrium/external";

        conf.Defines.Add("IMGUI_DEFINE_MATH_OPERATORS");
        conf.ExportDefines.Add("IS_IMGUI_ENABLED=1");

        conf.IncludePrivatePaths.Add(Path.Combine("[project.SharpmakeCsPath]", "imgui"));
        conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);

        SetupBackendExcludes(conf, target);
    }

    private void SetupBackendExcludes(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        if (!target.Platform.HasFlag(Sharpmake.Platform.win32) && !target.Platform.HasFlag(Sharpmake.Platform.win64))
        {
            conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx12.*");
            conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*win32.*");
        }

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
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*sdlrenderer.*");
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*vulkan.*");
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*wgpu.*");
    }
}