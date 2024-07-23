using System.Collections.Generic;
using System.IO;

namespace Atrium
{
    public static class Configuration
    {
        public static string BuildDirectory = "";

        public static Sharpmake.Platform Platform = Sharpmake.Platform.win32 | Sharpmake.Platform.win64;
        public static Sharpmake.DevEnv DevEnv = Sharpmake.DevEnv.vs2022;
        public static Sharpmake.Optimization Optimization = Sharpmake.Optimization.Debug | Sharpmake.Optimization.Release | Sharpmake.Optimization.Retail;
    }
    
    public class FilteredProject : Sharpmake.Project
    {
        public FilteredProject()
        {
            Name = "Unnamed project";
        }

        public override bool ResolveFilterPathForFile(string relativePath, out string filterPath)
        {
            string commentStartString = "//";
            string filterIdentifierString = "Filter";
            string openFilterString = "\"";
            string closeFilterString = "\"";

            IEnumerable<string> fileLines = File.ReadLines(Path.Combine(SourceRootPath, relativePath));
            foreach (string line in fileLines)
            {
                if (string.IsNullOrWhiteSpace(line))
                    continue;

                int commentStartIndex = line.IndexOf(commentStartString);
                if (commentStartIndex == -1) continue; // Skip lines without comments.

                string comment = line.Substring(commentStartIndex + commentStartString.Length);
                int filterIdentifier = comment.IndexOf(filterIdentifierString, System.StringComparison.CurrentCultureIgnoreCase);
                if (filterIdentifier == -1) break;

                int openFilterIndex = comment.IndexOf(openFilterString, filterIdentifier + filterIdentifierString.Length);
                if (openFilterIndex == -1) break;

                int closeFilterIndex = comment.IndexOf(closeFilterString, openFilterIndex + openFilterString.Length);
                if (closeFilterIndex == -1) break;

                int filterNameStart = openFilterIndex + openFilterString.Length;
                int filterNameLength = closeFilterIndex - filterNameStart;
                filterPath = comment.Substring(filterNameStart, filterNameLength).Replace("/", "\\");
                return true;
            }

            filterPath = null;
            return false;
        }
    }

    public class BasicProject : FilteredProject
    {
        public BasicProject()
        {
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
        }

        [Sharpmake.Configure]
        public virtual void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
        {
            conf.Name = @"[target.Optimization] [target.Platform] [target.DevEnv]";

            conf.ProjectPath = "[project.SharpmakeCsPath]";
            conf.ProjectFileName = "[project.Name] [target.DevEnv]";

            conf.IntermediatePath = Path.Combine(Atrium.Configuration.BuildDirectory, "Intermediate/[project.Name] [target.Optimization] [target.DevEnv]/");
            
            conf.TargetPath = Atrium.Configuration.BuildDirectory;
            conf.TargetLibraryPath = Path.Combine(conf.TargetPath, "Lib");
            conf.TargetFileName = "[project.Name] [target.Optimization] [target.DevEnv]";

            conf.Options.Add(Sharpmake.Options.Vc.Linker.SubSystem.Windows);
            conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level4);
            conf.Options.Add(Sharpmake.Options.Vc.General.WindowsTargetPlatformVersion.Latest);
            //conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.EnableAllWarnings);

            //conf.Options.Add(Sharpmake.Options.Vc.Compiler.ConformanceMode.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.CppLanguageStandard.CPP20);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.MultiProcessorCompilation.Enable);

            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);

            conf.IncludePaths.Add("[project.SourceRootPath]");
        }
    }

    public class EngineProject : BasicProject
    {
        public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
        {
            base.ConfigureAll(conf, target);

            conf.Defines.Add("NOMINMAX");

            if (target.Optimization == Sharpmake.Optimization.Debug)
                conf.Defines.Add("IS_DEBUG_BUILD");
            if (target.Optimization != Sharpmake.Optimization.Retail)
                conf.Defines.Add("IS_EDITOR_BUILD");
        }
    }

    public class Solution : Sharpmake.Solution
    {
        public Solution()
        {
            Name = "Solution";
            IsFileNameToLower = false;

            AddTargets(new Sharpmake.Target(
                Atrium.Configuration.Platform,
                Atrium.Configuration.DevEnv,
                Atrium.Configuration.Optimization));
        }

        [Sharpmake.Configure]
        public virtual void ConfigureAll(Sharpmake.Solution.Configuration conf, Sharpmake.Target target)
        {
            conf.SolutionFileName = "[solution.Name] [target.DevEnv]";
        }
    }

    public class ExecutableProject : EngineProject
    {
        public ExecutableProject()
        {
            Name = "Executable";

            AddTargets(new Sharpmake.Target(
                Atrium.Configuration.Platform,
                Atrium.Configuration.DevEnv,
                Atrium.Configuration.Optimization));
        }
    }

    public class StaticLibraryProject : EngineProject
    {
        public StaticLibraryProject()
        {
            Name = "Static library";

            AddTargets(new Sharpmake.Target(
                Atrium.Configuration.Platform,
                Atrium.Configuration.DevEnv,
                Atrium.Configuration.Optimization,
                Sharpmake.OutputType.Lib));
        }

        public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
        {
            base.ConfigureAll(conf, target);
            conf.Output = Sharpmake.Project.Configuration.OutputType.Lib;
        }
    }

    public class ExternalLibraryProject : BasicProject
    {
        public ExternalLibraryProject()
        {
            Name = "External library";

            AddTargets(new Sharpmake.Target(
                Atrium.Configuration.Platform,
                Atrium.Configuration.DevEnv,
                Atrium.Configuration.Optimization,
                Sharpmake.OutputType.Lib));
        }

        public override void ConfigureAll(Sharpmake.Project.Configuration conf, Sharpmake.Target target)
        {
            base.ConfigureAll(conf, target);
            
            conf.Output = Sharpmake.Project.Configuration.OutputType.Lib;
        }
    }
}
