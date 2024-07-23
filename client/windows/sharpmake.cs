using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/platform/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace Atrium
{
    [Generate]
    public class WindowsClient : Atrium.StaticLibraryProject
    {
        public WindowsClient()
        {
            Name = "Windows";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "Atrium/client";

            conf.AddPrivateDependency<Atrium.Common>(target);
            conf.AddPrivateDependency<Platform>(target);
        }
    }
}