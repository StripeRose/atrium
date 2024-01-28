using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../.sharpmake/project.sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

namespace RoseGold.Example
{
    [Generate]
    public class Example_CoreOnly : RoseGold.Executable
    {
        public Example_CoreOnly()
        {
            Name = "Example game (Core only)";
            SourceRootPath = "[project.SharpmakeCsPath]";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.AddPrivateDependency<RoseCommon>(target);
        }
    }

    [Generate]
    public class ExampleGame_CoreOnlySolution : RoseGold.Solution
    {
        public ExampleGame_CoreOnlySolution()
        {
            Name = "Example game (Core only)";

            AddTargets(new Target(
                Sharpmake.Platform.win32 | Sharpmake.Platform.win64,
                Sharpmake.DevEnv.vs2022,
                Sharpmake.Optimization.Debug | Sharpmake.Optimization.Release | Sharpmake.Optimization.Retail));
        }

        public override void ConfigureAll(Solution.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionPath = "[solution.SharpmakeCsPath]/../../";
            conf.AddProject<Example_CoreOnly>(target);
        }
    }

    public static class Main
    {
        [Sharpmake.Main]
        public static void SharpmakeMain(Sharpmake.Arguments arguments)
        {
            FileInfo fileInfo = Util.GetCurrentSharpmakeFileInfo();
            Globals.RootDirectory = Util.SimplifyPath(Path.Combine(fileInfo.DirectoryName, "..", ".."));
            
            arguments.Generate<ExampleGame_CoreOnlySolution>();
        }
    }
}