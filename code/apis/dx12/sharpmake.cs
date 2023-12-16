using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/graphics/sharpmake.cs")]
[module: Sharpmake.Include("../../core/platform/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class DirectX12 : RoseGold.Core.Library
    {
        public DirectX12()
        {
            Name = "DirectX 12";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "apis", "dx12");

            // Allow core shaders to be part of the project and built with it.
            SourceFilesExtensions.Add(".hlsl");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold/apis";

            conf.AddPublicDependency<RoseCommon>(target);
            conf.AddPublicDependency<Core.Graphics>(target);
            conf.AddPrivateDependency<Core.Platform>(target);

            //conf.AddPrivateDependency<Client.WindowsClient>(target);
            
            // if (target.Optimization != Optimization.Retail)
            //     conf.AddPrivateDependency<ThirdParty.IMGUI>(target);

            conf.LibraryFiles.Add("d3d12.lib");
            conf.LibraryFiles.Add("dxgi.lib");
            conf.LibraryFiles.Add("dxguid.lib");
        }
    }
}