#!/usr/bin/env python

import os
import sys

from nfbuildlinux import NFBuildLinux
from build_options import BuildOptions


def main():
    buildOptions = BuildOptions()
    buildOptions.addOption("lintCmake", "Lint cmake files")
    buildOptions.addOption("lintCppWithInlineChange",
                           "Lint CPP Files and fix them")

    buildOptions.addOption("makeBuildDirectory",
                           "Wipe existing build directory")
    buildOptions.addOption("generateProject", "Regenerate xcode project")

    buildOptions.addOption("buildTargetLibrary", "Build Target: Library")
    buildOptions.addOption("gnuToolchain", "Build with gcc and libstdc++")
    buildOptions.addOption("llvmToolchain", "Build with clang and libc++")

    buildOptions.addOption("unitTests", "Run Unit Tests")

    buildOptions.setDefaultWorkflow("Empty workflow", [])

    buildOptions.addWorkflow("lint", "Run lint workflow", [
        'lintCmake',
        'lintCppWithInlineChange'
    ])

    buildOptions.addWorkflow("clang_build", "Production Build", [
        'llvmToolchain',
        'lintCmake',
        'makeBuildDirectory',
        'generateProject',
        'buildTargetLibrary',
        'unitTests'
    ])

    buildOptions.addWorkflow("gcc_build", "Production Build", [
        'gnuToolchain',
        'lintCmake',
        'makeBuildDirectory',
        'generateProject',
        'buildTargetLibrary',
        'unitTests'
    ])

    options = buildOptions.parseArgs()
    buildOptions.verbosePrintBuildOptions(options)

    nfbuild = NFBuildLinux()
    library_target = "NFLoggerTest"

    if buildOptions.checkOption(options, 'lintCmake'):
        nfbuild.lintCmake()

    if buildOptions.checkOption(options, 'lintCppWithInlineChange'):
        nfbuild.lintCPP(make_inline_changes=True)

    if buildOptions.checkOption(options, 'makeBuildDirectory'):
        nfbuild.makeBuildDirectory()

    if buildOptions.checkOption(options, 'generateProject'):
        if buildOptions.checkOption(options, 'gnuToolchain'):
            os.environ['CC'] = 'gcc'
            os.environ['CXX'] = 'g++'
            nfbuild.generateProject(address_sanitizer='addressSanitizer' in options,
                                    undefined_behaviour_sanitizer='ubSanitizer' in options,
                                    gcc=True)
        elif buildOptions.checkOption(options, 'llvmToolchain'):
            os.environ['CC'] = 'clang'
            os.environ['CXX'] = 'clang++'
            nfbuild.generateProject(address_sanitizer='addressSanitizer' in options,
                                    undefined_behaviour_sanitizer='ubSanitizer' in options,
                                    gcc=False)
        else:
            nfbuild.generateProject(address_sanitizer='addressSanitizer' in options,
                                    undefined_behaviour_sanitizer='ubSanitizer' in options)

    if buildOptions.checkOption(options, 'buildTargetLibrary'):
        nfbuild.buildTarget(library_target)

    if buildOptions.checkOption(options, 'unitTests'):
        nfbuild.runUnitTests()


if __name__ == "__main__":
    main()
