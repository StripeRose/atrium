using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace RoseGold.Core
{
    [Generate]
    public class Graphics : Core.Library
    {
        public Graphics()
        {
            Name = "Graphics";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "core", "graphics");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "rose-gold/core";
            conf.AddPrivateDependency<RoseCommon>(target);
        }
    }
}