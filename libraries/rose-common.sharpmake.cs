using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class RoseCommon : Atrium.ExternalLibraryProject
{
    public RoseCommon()
    {
        Name = "Rose common";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "rose-common", "include");
        AdditionalSourceRootPaths.Add(Path.Combine("[project.SharpmakeCsPath]", "rose-common", "source"));
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);

        conf.SolutionFolder = "Atrium/external";
    }
}
