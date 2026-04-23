using Sharpmake;

namespace Atrium
{
	[Generate]
	public class Engine : Project
	{
		public Engine()
		{
			Name = "Atrium";
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

			conf.AddPublicDependency<Atrium.Core>(target);

			switch (target.Platform)
			{
				case Platform.win32:
				case Platform.win64:
					conf.AddPrivateDependency<Client.Windows>(target);
					conf.AddPrivateDependency<Graphics.DirectX12>(target);
					break;
			}
		}
	}
}