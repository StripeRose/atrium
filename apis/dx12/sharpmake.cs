using Sharpmake;

[Generate]
public class D3D12MemoryAllocator : Project
{
	public D3D12MemoryAllocator()
	{
		Name = "D3D12MemoryAllocator";

		string repositoryPath = ExternalProject.Git(
			"D3D12MemoryAllocator",
			"https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator",
			"v3.1.0"
		);

		SourceRootPath = $"{repositoryPath}/include";
		SourceFiles.Add($"{repositoryPath}/src/D3D12MemAlloc.cpp");

		AddTargets(new Target(
			Platform.win64,
			Util.AllFlags<DevEnv>(),
			Util.AllFlags<Optimization>()
		));
	}

	[Configure]
	public void ConfigureAll(Configuration conf, Target target)
	{
		Util.SetDefaultBuildArguments(conf, target);
		conf.SolutionFolder = "Atrium/External";

		conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Disable);
		conf.Options.Add(Options.Vc.General.WarningLevel.Level0);

		conf.LibraryFiles.Add("d3d12.lib");
		conf.LibraryFiles.Add("dxgi.lib");
		conf.LibraryFiles.Add("dxguid.lib");
	}
}

[Generate]
public class DirectXTex : Project
{
	public DirectXTex()
	{
		Name = "DirectXTex";

		string repositoryPath = ExternalProject.Git(
			"DirectXTex",
			"https://github.com/microsoft/DirectXTex",
			"mar2026"
		);

		SourceRootPath = $"{repositoryPath}/DirectXTex";

		AddTargets(new Target(
			Platform.win64,
			Util.AllFlags<DevEnv>(),
			Util.AllFlags<Optimization>()
		));
	}

	[Configure]
	public void ConfigureAll(Configuration conf, Target target)
	{
		Util.SetDefaultBuildArguments(conf, target);
		conf.SolutionFolder = "Atrium/External";

		conf.SourceFilesBuildExclude.Add("BCDirectCompute.cpp");
		conf.IncludePrivatePaths.Add(@"[project.SourceRootPath]/../Common");
	}
}

namespace Atrium.Graphics
{
	[Generate]
	public class DirectX12 : Project
	{
		public DirectX12()
		{
			Name = "DirectX 12";
			SourceRootPath = "[project.SharpmakeCsPath]";

			AddTargets(new Target(
				Platform.win64,
				Util.AllFlags<DevEnv>(),
				Util.AllFlags<Optimization>()
			));

			// Allow core shaders to be part of the project and built with it.
			SourceFilesExtensions.Add(".hlsl");
		}

		[Configure]
		public void ConfigureAll(Configuration conf, Target target)
		{
			Util.SetDefaultBuildArguments(conf, target);
			conf.SolutionFolder = "Atrium/Graphics";

			conf.AddPrivateDependency<Atrium.Core>(target);
			conf.AddPrivateDependency<D3D12MemoryAllocator>(target);
			conf.AddPrivateDependency<DirectXTex>(target);

			conf.Defines.Add("DX12_FRAMES_IN_FLIGHT=2");

			conf.LibraryFiles.Add("d3d12.lib");
			conf.LibraryFiles.Add("dxgi.lib");
			conf.LibraryFiles.Add("dxguid.lib");
		}
	}
}