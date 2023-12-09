using System.IO;
using Sharpmake;

[module: Sharpmake.Include("../module.sharpmake.cs")]

namespace RoseGold.Modules
{
    [Generate]
    public class NodeGraphs : RoseGold.Module
    {
        public NodeGraphs()
            : base("node-graphs")
        {
            Name = "Node graphs";
        }
    }
}