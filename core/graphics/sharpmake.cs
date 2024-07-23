using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../common/sharpmake.cs")]
[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]

namespace Atrium
{
    [Generate]
    public class Graphics : StaticLibraryProject
    {
        public Graphics()
        {
            Name = "Graphics";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "Atrium/core";

            conf.AddPublicDependency<Atrium.Common>(target);
        }
    }
}