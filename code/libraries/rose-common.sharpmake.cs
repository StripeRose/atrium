using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Generate]
public class RoseCommon : RoseGold.Core.Library
{
    public RoseCommon()
    {
        Name = "Rose common";
        SourceRootPath = Path.Combine(Globals.RootDirectory, "libraries/rose-common/include");
        AdditionalSourceRootPaths.Add(Path.Combine(Globals.RootDirectory, "libraries/rose-common/source"));
    }

    public override void ConfigureAll(Configuration conf, Target target)
    {
        base.ConfigureAll(conf, target);

        conf.ProjectPath = Path.Combine(Globals.RootDirectory, "libraries");
    }
}
