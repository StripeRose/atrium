using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../core/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace Microsoft
{
    public class GameInput
    {
        public static void Setup(Project project)
        {
            project.AdditionalSourceRootPaths.Add(
                Path.Combine(Atrium.Configuration.SolutionDirectory, @"packages/Microsoft.GameInput.0.2303.22621.3038/native/include")
                );
        }

        public static void Configure(Project.Configuration conf, Target target)
        {
            conf.ReferencesByNuGetPackage.Add(
                "Microsoft.GameInput",
                "0.2303.22621.3038",
                targetsPath: @"build\native\targets\[packageName].targets"
                );

            conf.LibraryPaths.Add(Path.Combine(Atrium.Configuration.SolutionDirectory, @"packages/Microsoft.GameInput.0.2303.22621.3038/native/lib/x64"));
            conf.LibraryFiles.Add("GameInput.lib");
        }
    }
}

namespace Atrium
{
    [Generate]
    public class WindowsClient : Atrium.StaticLibraryProject
    {
        public WindowsClient()
        {
            Name = "Windows";
            SourceRootPath = "[project.SharpmakeCsPath]";

            Microsoft.GameInput.Setup(this);
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "Atrium/client";

            conf.AddPrivateDependency<Atrium.Core>(target);

            Microsoft.GameInput.Configure(conf, target);
        }
    }
}