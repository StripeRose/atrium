using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../core/sharpmake.cs")]

[Generate]
public class RoseCommon : RoseGold.Core.Library
{
    public RoseCommon()
    {
        Name = "rose-common";
        SourceRootPath = Path.Combine(Globals.RootDirectory, "libraries");
    }
}
