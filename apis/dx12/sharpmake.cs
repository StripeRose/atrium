using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/imgui.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/d3d12ma.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/directxtex.sharpmake.cs")]

namespace Atrium
{
    [Generate]
    public class DirectX12 : Atrium.StaticLibraryProject
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
            conf.SolutionFolder = "Atrium/apis";

            conf.AddPrivateDependency<Atrium.Core>(target);
            conf.AddPrivateDependency<D3D12MemoryAllocator>(target);
            conf.AddPrivateDependency<DirectXTex>(target);

			if (target.Optimization != Sharpmake.Optimization.Retail || Atrium.Configuration.ImGuiInRetail)
                conf.AddPublicDependency<DearImGui>(target);

            conf.Defines.Add("DX12_FRAMES_IN_FLIGHT=2");

            conf.LibraryFiles.Add("d3d12.lib");
            conf.LibraryFiles.Add("dxgi.lib");
            conf.LibraryFiles.Add("dxguid.lib");
        }
    }
}