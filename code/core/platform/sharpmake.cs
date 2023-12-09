using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace RoseGold.Core
{
    [Generate]
    public class Platform : Core.Library
    {
        public Platform()
        {
            Name = "Platform";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "core", "platform");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "rose-gold/core";
            conf.AddPrivateDependency<RoseCommon>(target);
        }
    }
}