using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class Tracy : Atrium.ExternalLibraryProject
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
        
        conf.SolutionFolder = "Atrium/external";

		conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);
        conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level0);

        if (target.Optimization != Sharpmake.Optimization.Retail)
        {
            conf.Defines.Add("TRACY_ENABLE");
            conf.Defines.Add("TRACY_CALLSTACK=4");
            
            conf.ExportDefines.Add("IS_PROFILING_ENABLED=1");
            conf.ExportDefines.Add("TRACY_ENABLE");
            conf.ExportDefines.Add("TRACY_CALLSTACK=4");
        }
    }
}
