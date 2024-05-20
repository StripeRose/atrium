using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class DearImGui : RoseGold.ExternalLibraryProject
{
    public DearImGui()
    {
        Name = "imgui";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "imgui");
        
        SourceFilesExcludeRegex.Add(@".*docs.*");
        SourceFilesExcludeRegex.Add(@".*examples.*");
        SourceFilesExcludeRegex.Add(@".*misc.*");
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);
        
        conf.SolutionFolder = "rose-gold/external";

        conf.Defines.Add("IMGUI_DEFINE_MATH_OPERATORS");

        conf.IncludePrivatePaths.Add(Path.Combine("[project.SharpmakeCsPath]", "imgui"));

        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*allegro5.*");
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*android.*");
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx9.*");
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx10.*");
        conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx11.*");
        //conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*dx12.*");
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
        //conf.SourceFilesBuildExcludeRegex.Add(@".*backends.*win32.*");

        conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);
    }
}