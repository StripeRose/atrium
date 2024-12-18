using System.IO;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

[Sharpmake.Generate]
public class DirectXTex : Atrium.ExternalLibraryProject
{
    public DirectXTex()
    {
        Name = "DirectXTex";
        SourceRootPath = Path.Combine("[project.SharpmakeCsPath]", "DirectXTex", "DirectXTex");
    }

    public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
    {
        base.ConfigureAll(conf, target);

        conf.SolutionFolder = "Atrium/external";
        conf.SourceFilesBuildExclude.Add("BCDirectCompute.cpp");

        conf.IncludePrivatePaths.Add(Path.Combine("[project.SharpmakeCsPath]", "DirectXTex", "Common"));
    }
}
