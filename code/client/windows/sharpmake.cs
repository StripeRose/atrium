using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../../core/sharpmake.cs")]

namespace RoseGold.Client
{
    [Generate]
    public class WindowsClient : RoseGold.Core.Library
    {
        public WindowsClient()
        {
            Name = "Windows client";
            SourceRootPath = Path.Combine(Globals.RootDirectory, "client", "windows");
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "rose-gold/client";
            //conf.AddPrivateDependency<Fireblade.Core>(target);
        }
    }
}