using Sharpmake;

namespace Atrium
{
	[Generate]
	public class Core : Project
	{
		public Core()
		{
			Name = "Atrium Core";
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

			conf.SolutionFolder = "Atrium";

			RoseCommon.MainNamespace = "Atrium";
			RoseCommon.MathNamespace = "Atrium";
			conf.AddPublicDependency<RoseCommon>(target);

			conf.AddPublicDependency<Tracy>(target);
		}
	}
}