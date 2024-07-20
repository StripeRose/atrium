using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/graphics/sharpmake.cs")]
[module: Sharpmake.Include("../../core/platform/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/directxtex.sharpmake.cs")]

namespace RoseGold
{
    [Generate]
    public class DirectX12 : RoseGold.StaticLibraryProject
    {
        public DirectX12()
        {
            Name = "DirectX 12";
            SourceRootPath = "[project.SharpmakeCsPath]";

            // Allow core shaders to be part of the project and built with it.
            SourceFilesExtensions.Add(".hlsl");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold/apis";

            conf.AddPrivateDependency<RoseGold.Common>(target);
            conf.AddPrivateDependency<DirectXTex>(target);
            conf.AddPrivateDependency<Graphics>(target);
            conf.AddPrivateDependency<Platform>(target);

            conf.Defines.Add("DX12_FRAMES_IN_FLIGHT=2");

            conf.LibraryFiles.Add("d3d12.lib");
            conf.LibraryFiles.Add("dxgi.lib");
            conf.LibraryFiles.Add("dxguid.lib");
        }
    }
}