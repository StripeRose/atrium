[module: Sharpmake.Include("../plugin.sharpmake.cs")]

namespace Atrium
{
    [Sharpmake.Generate]
    public class ExamplePluginProject : Atrium.PluginProject
    {
        public ExamplePluginProject()
            : base("example-plugin")
        {
            
        }
    }
}