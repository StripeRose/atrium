using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace Atrium
{
	[Generate]
	public class WindowsClient : Atrium.StaticLibraryProject
	{
		public WindowsClient()
		{
			Name = "Windows";
			SourceRootPath = "[project.SharpmakeCsPath]";
		}

		public override void ConfigureAll(Project.Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);
			conf.SolutionFolder = "Atrium/client";

			conf.AddPrivateDependency<Atrium.Core>(target);

			conf.IncludePaths.Add("[project.SharpmakeCsPath]/../../libraries/GameInput/include");
			conf.LibraryPaths.Add("[project.SharpmakeCsPath]/../../libraries/GameInput/lib/x64");
			conf.LibraryFiles.Add("GameInput.lib");
		}
	}
}