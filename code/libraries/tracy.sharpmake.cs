using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Generate]
public class Tracy : RoseGold.Core.ExternalLibrary
{
    public Tracy()
    {
        Name = "Tracy";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "tracy", "public");

        // Ignore all .cpp but TracyClient.cpp, which in turn includes the rest.
        SourceFilesExcludeRegex.Add(@"^((?!TracyClient).)*\.cpp$");
    }

    public override void ConfigureAll(Configuration conf, Target target)
    {
        base.ConfigureAll(conf, target);

        conf.SolutionFolder = "rose-gold/external";
        conf.ProjectPath = "[project.SharpmakeCsPath]";
    }
}
