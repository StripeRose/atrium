using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/platform/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class WindowsClient : RoseGold.StaticLibraryProject
    {
        public WindowsClient()
        {
            Name = "Windows";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold/client";

            conf.AddPrivateDependency<RoseGold.Common>(target);
            conf.AddPrivateDependency<Platform>(target);
        }
    }
}