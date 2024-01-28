using System.Collections.Generic;
using System.IO;
using Sharpmake;

public static class Globals
{
    public static string RootDirectory = "";
    //public static string SharpmakeDirectory { get { return Path.Combine(RootDirectory, ".sharpmake"); } }
}

namespace RoseGold
{
    namespace Core
    {
        public class Project : Sharpmake.Project
        {
            public Project()
            {
                IsFileNameToLower = false;
                IsTargetFileNameToLower = false;
            }

            [Configure]
            public virtual void ConfigureAll(Project.Configuration conf, Target target)
            {
                conf.Name = @"[target.Optimization] [target.OutputType]";
                conf.ProjectPath = "[project.SourceRootPath]";

                conf.IncludePaths.Add("[project.SourceRootPath]");
                conf.IncludePaths.Add(Path.Combine(Globals.RootDirectory, "Third-Party"));

                // Todo: Precompiled headers
                //conf.PrecompHeader = "precomp.hpp";
                //conf.PrecompSource = "precomp.cpp";

                conf.IntermediatePath = Path.Combine(Globals.RootDirectory, "../Build/Intermediate/[project.Name] [target.Optimization]/");
                conf.TargetPath = Path.Combine(new string[] {Globals.RootDirectory, "..", "Build"});
                conf.TargetLibraryPath = Path.Combine(conf.TargetPath, "Lib");
                conf.TargetFileName = "[project.Name] [target.Optimization]";

                conf.Defines.Add("NOMINMAX"); // Is this really needed?

                if (target.Optimization == Optimization.Debug)
                    conf.Defines.Add("IS_DEBUG_BUILD");
                if (target.Optimization != Optimization.Retail)
                    conf.Defines.Add("IS_EDITOR_BUILD");

                // Todo: Figure out how to do memory leak debugging.
                // if (target.Optimization == Optimization.Debug)
                // {
                //     conf.Defines.Add("_CRTDBG_MAP_ALLOC");
                //     conf.ForcedIncludes.Add("stdlib.h");
                //     conf.ForcedIncludes.Add("crtdbg.h");
                //     conf.Defines.Add("DEBUG_NEW=new(_NORMAL_BLOCK, __FILE__, __LINE__)");
                //     conf.Defines.Add("new=DEBUG_NEW");
                // }

                conf.Options.Add(Sharpmake.Options.Vc.Linker.SubSystem.Windows);
                conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Enable);
                conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level4);
                conf.Options.Add(Sharpmake.Options.Vc.General.WindowsTargetPlatformVersion.Latest);
                //conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.EnableAllWarnings);

                //conf.Options.Add(Sharpmake.Options.Vc.Compiler.ConformanceMode.Enable);
                conf.Options.Add(Sharpmake.Options.Vc.Compiler.CppLanguageStandard.CPP20);
                conf.Options.Add(Sharpmake.Options.Vc.Compiler.MultiProcessorCompilation.Enable);

                conf.Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);
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

        public class Library : Project
        {
            public Library()
            {
                Name = "New core library";

                AddTargets(new Target(
                    Sharpmake.Platform.win32 | Sharpmake.Platform.win64,
                    Sharpmake.DevEnv.vs2022,
                    Sharpmake.Optimization.Debug | Sharpmake.Optimization.Release | Sharpmake.Optimization.Retail,
                    Sharpmake.OutputType.Lib));
            }

            public override void ConfigureAll(Project.Configuration conf, Target target)
            {
                base.ConfigureAll(conf, target);
                conf.Output = Sharpmake.Project.Configuration.OutputType.Lib;
            }
        }

        public class Executable : Project
        {
            public Executable()
            {
                Name = "New core executable";

                AddTargets(new Target(
                    Sharpmake.Platform.win32 | Sharpmake.Platform.win64,
                    Sharpmake.DevEnv.vs2022,
                    Sharpmake.Optimization.Debug | Sharpmake.Optimization.Release | Sharpmake.Optimization.Retail));
            }
        }
    }
}
