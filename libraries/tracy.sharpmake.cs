using Sharpmake;

[Sharpmake.Generate]
public class Tracy : Project
{
	public Tracy()
	{
		Name = "Tracy";
		
		string repositoryPath = ExternalProject.Git(
			"Tracy",
			"https://github.com/wolfpld/tracy",
			"master"
		);

		SourceRootPath = $"{repositoryPath}/public";

		// Ignore all .cpp but TracyClient.cpp, which in turn includes the rest.
		SourceFilesExcludeRegex.Add(@"^((?!TracyClient).)*\.cpp$");

		AddTargets(new Target(
			Platform.win32 | Platform.win64,
			Util.AllFlags<DevEnv>(),
			Util.AllFlags<Optimization>()
		));
	}

	[Configure]
	public void ConfigureAll(Configuration conf, Target target)
	{
		Util.SetDefaultBuildArguments(conf, target);
		
		conf.SolutionFolder = "Atrium/external";

		conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);
		conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level0);

		if (target.Optimization != Sharpmake.Optimization.Retail)
		{
			conf.Defines.Add("TRACY_ENABLE");
			conf.Defines.Add("TRACY_CALLSTACK=4");
			
			conf.ExportDefines.Add("IS_PROFILING_ENABLED=1");
			conf.ExportDefines.Add("TRACY_ENABLE");
			conf.ExportDefines.Add("TRACY_CALLSTACK=4");
		}
	}
}
