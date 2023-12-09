using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/platform/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace RoseGold.Client
{
    [Generate]
    public class WindowsClient : RoseGold.Core.Library
    {
        public WindowsClient()
        {
            Name = "Windows";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "client", "windows");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold/client";

            conf.AddPublicDependency<Core.Platform>(target);
            
            conf.AddPrivateDependency<RoseCommon>(target);
        }
    }
}