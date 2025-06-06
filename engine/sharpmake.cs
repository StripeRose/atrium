using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[module: Sharpmake.Include("../core/sharpmake.cs")]

[module: Sharpmake.Include("../apis/dx12/sharpmake.cs")]
[module: Sharpmake.Include("../client/windows/sharpmake.cs")]

[module: Sharpmake.Include("../libraries/imgui.sharpmake.cs")]

namespace Atrium
{
    [Generate]
    public class Engine : Atrium.StaticLibraryProject
    {
        public Engine()
        {
            Name = "Atrium";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "Atrium";

            conf.AddPublicDependency<RoseCommon>(target);

            conf.AddPublicDependency<Atrium.Core>(target);

            if (target.Optimization != Sharpmake.Optimization.Retail || Atrium.Configuration.ImGuiInRetail)
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