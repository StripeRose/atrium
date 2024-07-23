using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/tracy.sharpmake.cs")]

namespace Atrium
{
    [Generate]
    public class Common : StaticLibraryProject
    {
        public Common()
        {
            Name = "Common";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "Atrium/core";
            conf.AddPublicDependency<RoseCommon>(target);
            conf.AddPublicDependency<Tracy>(target);
        }
    }
}