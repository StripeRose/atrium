using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Generate]
public class RoseCommon : RoseGold.Core.Library
{
    public RoseCommon()
    {
        Name = "Rose common";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "rose-common", "include");
        AdditionalSourceRootPaths.Add(Path.Combine("[project.SharpmakeCsPath]", "rose-common", "source"));
    }

    public override void ConfigureAll(Configuration conf, Target target)
    {
        base.ConfigureAll(conf, target);
        
        conf.SolutionFolder = "rose-gold/external";
        conf.ProjectPath = "[project.SharpmakeCsPath]";
    }
}
