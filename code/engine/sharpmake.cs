using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[module: Sharpmake.Include("../core/common/sharpmake.cs")]
[module: Sharpmake.Include("../core/graphics/sharpmake.cs")]
[module: Sharpmake.Include("../core/platform/sharpmake.cs")]

[module: Sharpmake.Include("../apis/dx12/sharpmake.cs")]
[module: Sharpmake.Include("../client/windows/sharpmake.cs")]

[module: Sharpmake.Include("../libraries/imgui.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class Engine : RoseGold.StaticLibraryProject
    {
        public Engine()
        {
            Name = "Engine";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold";

            conf.AddPublicDependency<Common>(target);
            conf.AddPublicDependency<Platform>(target);
            conf.AddPublicDependency<Graphics>(target);

            if (target.Optimization != Sharpmake.Optimization.Retail)
                conf.AddPublicDependency<DearImGui>(target);

            switch (target.Platform)
            {
                case Sharpmake.Platform.win32:
                case Sharpmake.Platform.win64:
                    conf.AddPrivateDependency<WindowsClient>(target);
                    conf.AddPrivateDependency<DirectX12>(target);
                    break;
            }
        }
    }
}