using Sharpmake;

namespace Atrium.Client
{
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

			conf.IncludePaths.Add("[project.SharpmakeCsPath]/../../libraries/GameInput/include");
			conf.LibraryPaths.Add("[project.SharpmakeCsPath]/../../libraries/GameInput/lib/x64");
			conf.LibraryFiles.Add("GameInput.lib");
		}
	}
}