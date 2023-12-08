using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../core/sharpmake.cs")]

[Generate]
public class RoseCommon : RoseGold.Core.Library
{
    public RoseCommon()
    {
        Name = "rose-common";
        SourceRootPath = Path.Combine(Globals.RootDirectory, "libraries/rose-common");

        SourceFilesExcludeRegex.Add(@".*notready.*");
    }

    public override void ConfigureAll(Configuration conf, Target target)
    {
        base.ConfigureAll(conf, target);

        conf.ProjectPath = Path.Combine(Globals.RootDirectory, "libraries");
    }
}
