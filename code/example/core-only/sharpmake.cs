using System.IO;

[module: Sharpmake.Include("../../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("../../client/bootstrapper/sharpmake.cs")]
[module: Sharpmake.Include("../../libraries/rose-common.sharpmake.cs")]

[Sharpmake.Generate]
public class Example_CoreOnlyExecutable : RoseGold.ExecutableProject
{
    public Example_CoreOnlyExecutable()
    {
        Name = "Example game (Core only)";
        SourceRootPath = "[project.SharpmakeCsPath]";
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);

        conf.SolutionFolder = "Executables";
        conf.AddPrivateDependency<RoseGold.Common>(target);
        conf.AddPrivateDependency<RoseGold.Bootstrapper>(target);

        conf.VcxprojUserFile = new Sharpmake.Project.Configuration.VcxprojUserFileSettings();
        conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = "$(OutputPath)..\\data\\";
    }
}

[Sharpmake.Generate]
public class ExampleGame_CoreOnlySolution : RoseGold.Solution
{
    public ExampleGame_CoreOnlySolution()
    {
        Name = "Example game (Core only)";
    }

    public override void ConfigureAll(Sharpmake.Solution.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);
        conf.SolutionPath = "[solution.SharpmakeCsPath]/../../";

        conf.AddProject<Example_CoreOnlyExecutable>(target);
    }
}

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Sharpmake.Arguments arguments)
    {
        FileInfo fileInfo = Sharpmake.Util.GetCurrentSharpmakeFileInfo();

        RoseGold.Configuration.BuildDirectory = Path.Combine(
            fileInfo.DirectoryName,
            "../../../Build"
        );
        
        arguments.Generate<ExampleGame_CoreOnlySolution>();
    }
}