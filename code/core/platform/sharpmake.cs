using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../common/sharpmake.cs")]
[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]

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

            conf.Output = Configuration.OutputType.Utility; // Remove when there's code to build.

            conf.SolutionFolder = "rose-gold/core";
            conf.AddPublicDependency<RoseGold.Core.Common>(target);
        }
    }
}