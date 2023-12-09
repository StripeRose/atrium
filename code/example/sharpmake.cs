using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/project.sharpmake.cs")]
[module: Sharpmake.Include("../libraries/rose-common.sharpmake.cs")]
[module: Sharpmake.Include("../modules/sharpmake.cs")]

namespace ExampleGame
{
    [Generate]
    public class ExampleGame : RoseGold.Executable
    {
        public ExampleGame()
        {
            Name = "Example game";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "example");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.AddPrivateDependency<RoseCommon>(target);

            // Add explicit dependencies to all available core modules.
            // Todo: For non-editor builds, only depend on the actually used modules.
            RoseGold.Modules.DependOn.All(conf, target);
        }
    }

    [Generate]
    public class ExampleGameSolution : RoseGold.Solution
    {
        public ExampleGameSolution()
        {
            Name = "Example game";

            AddTargets(new Target(
                Sharpmake.Platform.win32 | Sharpmake.Platform.win64,
                Sharpmake.DevEnv.vs2022,
                Sharpmake.Optimization.Debug | Sharpmake.Optimization.Release | Sharpmake.Optimization.Retail));
        }

        public override void ConfigureAll(Solution.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddProject<ExampleGame>(target);
        }
    }

    public static class Main
    {
        [Sharpmake.Main]
        public static void SharpmakeMain(Sharpmake.Arguments arguments)
        {
            FileInfo fileInfo = Util.GetCurrentSharpmakeFileInfo();
            Globals.RootDirectory = Util.SimplifyPath(Path.Combine(fileInfo.DirectoryName, ".."));
            
            arguments.Generate<ExampleGameSolution>();
        }
    }
}