using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../libraries/rose-common.sharpmake.cs")]

namespace RoseGold.Core
{
    [Generate]
    public class Common : Core.Library
    {
        public Common()
        {
            Name = "Common";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "core");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "rose-gold";
            conf.AddPrivateDependency<RoseCommon>(target);
        }
    }
}