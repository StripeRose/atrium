using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../core/sharpmake.cs")]
[module: Sharpmake.Include("../../apis/dx12/sharpmake.cs")]
[module: Sharpmake.Include("../windows/sharpmake.cs")]

namespace RoseGold.Client
{
    [Generate]
    public class Bootstrapper : RoseGold.Core.Library
    {
        public Bootstrapper()
        {
            Name = "Bootstrapper";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "client", "bootstrapper");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold/client";
            // conf.AddPublicDependency<Fireblade.Core>(target);

            // if (target.Optimization != Optimization.Retail)
            //     conf.AddPrivateDependency<Fireblade.Editor>(target);

            switch (target.Platform)
            {
                case Platform.win32:
                case Platform.win64:
                    conf.AddPrivateDependency<WindowsClient>(target);
                    conf.AddPrivateDependency<DirectX12>(target);
                    break;
            }
        }
    }
}
