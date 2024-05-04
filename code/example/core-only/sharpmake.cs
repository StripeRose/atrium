using System.IO;

[module: Sharpmake.Include("../../engine/sharpmake.cs")]

[Sharpmake.Generate]
public class Example_CoreOnlyExecutable : RoseGold.ExecutableProject
{
    public Example_CoreOnlyExecutable()
    {
        Name = "Example game";
        SourceRootPath = "[project.SharpmakeCsPath]";
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);

        conf.SolutionFolder = "Executables";
        conf.AddPrivateDependency<RoseGold.Engine>(target);

        conf.VcxprojUserFile = new Sharpmake.Project.Configuration.VcxprojUserFileSettings();
        conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = "$(OutputPath)..\\data\\";
    }
}

[Sharpmake.Generate]
public class ExampleGame_CoreOnlySolution : RoseGold.Solution
{
    public ExampleGame_CoreOnlySolution()
    {
        Name = "Example game";
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