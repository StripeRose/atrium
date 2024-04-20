using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../core/common/sharpmake.cs")]
[module: Sharpmake.Include("../core/graphics/sharpmake.cs")]
[module: Sharpmake.Include("../core/platform/sharpmake.cs")]
[module: Sharpmake.Include("../libraries/rose-common.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class Engine : StaticLibraryProject
    {
        public Engine()
        {
            Name = "Engine";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.Output = Configuration.OutputType.Utility; // Remove when there's code to build.

            conf.SolutionFolder = "rose-gold";
            // conf.AddPublicDependency<RoseCommon>(target);
            // conf.AddPublicDependency<Common>(target);
            // conf.AddPublicDependency<Graphics>(target);
            // conf.AddPublicDependency<Platform>(target);
        }
    }
}