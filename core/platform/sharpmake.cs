using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../common/sharpmake.cs")]
[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]

namespace Atrium
{
    [Generate]
    public class Platform : StaticLibraryProject
    {
        public Platform()
        {
            Name = "Platform";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "Atrium/core";

            conf.AddPublicDependency<Atrium.Common>(target);

            conf.Output = Configuration.OutputType.Utility; // Remove when there's code to build.
        }
    }
}