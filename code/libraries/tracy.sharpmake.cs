using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class Tracy : RoseGold.ExternalLibraryProject
{
    public Tracy()
    {
        Name = "Tracy";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "tracy", "public");

        // Ignore all .cpp but TracyClient.cpp, which in turn includes the rest.
        SourceFilesExcludeRegex.Add(@"^((?!TracyClient).)*\.cpp$");
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);
        
        conf.SolutionFolder = "rose-gold/external";

        // Match with EngineProject.ConfigureAll()
        conf.Defines.Add("TRACY_ENABLE");
        conf.Defines.Add("TRACY_CALLSTACK=4");
    }
}
