using System.IO;

[module: Sharpmake.Include("../atrium/engine/sharpmake.cs")]

[Sharpmake.Generate]
public class Example_Executable : Atrium.ExecutableProject
{
	public Example_Executable()
	{
		Name = "Example executable";
		SourceRootPath = "[project.SharpmakeCsPath]";
	}

	public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
	{
		base.ConfigureAll(conf, target);

		conf.SolutionFolder = "Executables";
		
		conf.AddPrivateDependency<Atrium.Engine>(target);
	}
}

[Sharpmake.Generate]
public class Example_Solution : Atrium.Solution
{
	public Example_Solution()
	{
		Name = "Example";
	}

	public override void ConfigureAll(Sharpmake.Solution.Configuration conf, Sharpmake.Target target)
	{
		base.ConfigureAll(conf, target);
		conf.SolutionPath = "[solution.SharpmakeCsPath]";

		conf.AddProject<Example_Executable>(target);
	}
}

public static class Main
{
	[Sharpmake.Main]
	public static void SharpmakeMain(Sharpmake.Arguments arguments)
	{
		FileInfo fileInfo = Sharpmake.Util.GetCurrentSharpmakeFileInfo();

		Atrium.Configuration.SolutionDirectory = fileInfo.DirectoryName;
		Atrium.Configuration.BuildDirectory = Path.Combine(
			fileInfo.DirectoryName,
			"build"
		);
		
		arguments.Generate<Example_Solution>();
	}
}