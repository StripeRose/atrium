using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../module.sharpmake.cs")]

namespace RoseGold.Modules
{
    [Generate]
    public class NodeGraphs : RoseGold.Module
    {
        public NodeGraphs()
        {
            Name = "Node graphs";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            
            conf.Output = Configuration.OutputType.Utility; // Remove when there's code to build.
        }
    }
}