using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../.sharpmake/coreproject.sharpmake.cs")]
[module: Sharpmake.Include("./node-graphs/sharpmake.cs")]

namespace RoseGold.Modules
{
    public class DependOn
    {
        public static void All(Project.Configuration conf, Target target)
        {
            conf.AddPrivateDependency<RoseGold.Modules.NodeGraphs>(target);
        }
    }
}