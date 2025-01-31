using System.IO;

[module: Sharpmake.Include("../engine/sharpmake.cs")]
[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]

namespace Atrium
{
    public class PluginProject : EngineProject
    {
        public PluginProject(string aPluginName)
        {
            Name = aPluginName;
            SourceRootPath = "[project.SharpmakeCsPath]";

            AddTargets(new Sharpmake.Target(
                Atrium.Configuration.Platform,
                Atrium.Configuration.DevEnv,
                Atrium.Configuration.Optimization));
        }

        public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
        {
            base.ConfigureAll(conf, target);

            conf.SolutionFolder = "Atrium/plugin";

            conf.Output = Sharpmake.Project.Configuration.OutputType.Dll;
            conf.TargetPath = Path.Combine(Atrium.Configuration.BuildDirectory, "plugins"); // Todo: Make directory an option.

            if (target.Optimization == Sharpmake.Optimization.Debug)
                conf.TargetFileName = "[project.Name].d";
            else
                conf.TargetFileName = "[project.Name]";

            conf.AddPublicDependency<Atrium.Engine>(target);

            conf.Defines.Add("ATRIUM_PLUGIN_DLL");
            // Todo: Make project name a define which can be used within the code, to ensure they always match.
        }

        // public static void GetPlugins(string aDirectory)
        // {
        //     // Todo:
        //     // Create a method for searching through a directory for plugin source projects,
        //     // and add them as a dependency.
        // }
    }
}