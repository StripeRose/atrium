using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../libraries/rose-common.sharpmake.cs")]
[module: Sharpmake.Include("../libraries/tracy.sharpmake.cs")]

namespace Atrium
{
	[Generate]
	public class Core : StaticLibraryProject
	{
		public Core()
		{
			Name = "Atrium Core";
			SourceRootPath = "[project.SharpmakeCsPath]";
		}

		public override void ConfigureAll(Project.Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.SolutionFolder = "Atrium";
			conf.AddPublicDependency<RoseCommon>(target);
			if (Atrium.Configuration.EnableProfiling)
				conf.AddPublicDependency<Tracy>(target);
		}
	}
}