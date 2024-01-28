using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../libraries/rose-common.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class Module : RoseGold.Core.Library
    {
        public Module()
        {
            Name = "New module";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "rose-gold/modules";

            conf.AddPrivateDependency<RoseCommon>(target);
            // conf.AddPrivateDependency<Fireblade.Core>(target);
            // conf.AddPrivateDependency<Fireblade.Editor>(target);
            // conf.AddPrivateDependency<ThirdParty.IMGUI>(target);
        }
    }
}