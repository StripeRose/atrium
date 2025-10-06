using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class D3D12MemoryAllocator : Atrium.ExternalLibraryProject
{
	public D3D12MemoryAllocator()
	{
		Name = "D3D12MemoryAllocator";
		SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "D3D12MemoryAllocator", "include");

		SourceFiles.Add(@"[project.SharpmakeCsPath]/D3D12MemoryAllocator/src/D3D12MemAlloc.cpp");
	}

	public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
	{
		base.ConfigureAll(conf, target);

		conf.SolutionFolder = "Atrium/external";

		conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);
		conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level0);
	}
}
