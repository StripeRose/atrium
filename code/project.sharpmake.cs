using System.IO;
using Sharpmake;

[module: Sharpmake.Include("core/sharpmake.cs")]
[module: Sharpmake.Include("client/bootstrapper/sharpmake.cs")]

namespace RoseGold
{
    public class Library : Core.Library
    {
        public Library()
        {
            Name = "New library";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "Logic";
            ///conf.AddPublicDependency<Fireblade.Core>(target);
        }
    }

    public class Executable : Core.Executable
    {
        public Executable()
        {
            Name = "Executable";
        }

        public override void ConfigureAll(Project.Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.SolutionFolder = "Executables";
            conf.AddPublicDependency<Client.Bootstrapper>(target);
        }
    }

    public class Solution : Sharpmake.Solution
    {
        public Solution()
        {
            Name = "Solution";
            IsFileNameToLower = false;
        }

        [Configure]
        public virtual void ConfigureAll(Solution.Configuration conf, Target target)
        {
            conf.SolutionPath = Globals.RootDirectory;
            conf.SolutionFileName = "[solution.Name] [target.DevEnv]";
        }
}
}