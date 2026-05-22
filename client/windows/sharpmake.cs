using Sharpmake;

namespace Atrium.Client
{
	[Export]
	public class GameInput : Project
	{
		public string BasePath = "";

		public GameInput()
		{
			string repositoryPath = ExternalProject.NuGet(
				"Microsoft.GameInput",
				"3.4.218"
			);

			BasePath = $"{repositoryPath}/native";

			AddTargets(new Target(
				Platform.win64,
				Util.AllFlags<DevEnv>(),
				Util.AllFlags<Optimization>()
			));
		}

		[Configure]
		public void ConfigureAll(Configuration conf, Target target)
		{
			conf.IncludePaths.Add(@"[project.BasePath]/include");
			conf.LibraryPaths.Add(@"[project.BasePath]/lib/x64");
			conf.LibraryFiles.Add("GameInput.lib");
		}
	}

	[Generate]
	public class Windows : Project
	{
		public Windows()
		{
			Name = "Windows";
			SourceRootPath = "[project.SharpmakeCsPath]";

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
			conf.SolutionFolder = "Atrium/Client";

			conf.AddPrivateDependency<Atrium.Core>(target);
			conf.AddPrivateDependency<GameInput>(target);

		}
	}
}