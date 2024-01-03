using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Generate]
public class DirectXTex : RoseGold.Core.Library
{
    public DirectXTex()
    {
        Name = "DirectXTex";
        SourceRootPath = Path.Combine(Globals.RootDirectory, "libraries/DirectXTex/DirectXTex");
    }

    public override void ConfigureAll(Configuration conf, Target target)
    {
        base.ConfigureAll(conf, target);

        conf.ProjectPath = Path.Combine(Globals.RootDirectory, "libraries");

        conf.SourceFilesBuildExclude.Add("BCDirectCompute.cpp");
    }
}
