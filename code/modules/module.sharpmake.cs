using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../core/sharpmake.cs")]
[module: Sharpmake.Include("../libraries/rose-common.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class Module : RoseGold.Core.Library
    {
        // moduleFolder specifies the sub-folder inside "Core modules" the module project is located.
        public Module(string moduleFolder)
        {
            Name = "New module";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "modules", moduleFolder);
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