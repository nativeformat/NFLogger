#!/usr/bin/env python

import fnmatch
import os
import plistlib
import re
import shutil
import subprocess
import sys

from distutils import dir_util
from nfbuild import NFBuild


class NFBuildLinux(NFBuild):
    clang_format_binary = 'clang-format'

    def __init__(self):
        super(self.__class__, self).__init__()
        self.project_file = 'build.ninja'
        self.cmake_binary = 'cmake'
        self.clang_format_binary = 'clang-format'

    def generateProject(self,
                        code_coverage=False,
                        address_sanitizer=False,
                        thread_sanitizer=False,
                        undefined_behaviour_sanitizer=False,
                        ios=False,
                        gcc=False):
        cmake_call = [
            self.cmake_binary,
            '..',
            '-GNinja']
        if self.build_type == 'Release':
            cmake_call.append('-DCREATE_RELEASE_BUILD=1')
        else:
            cmake_call.append('-DCREATE_RELEASE_BUILD=0')
        if undefined_behaviour_sanitizer:
            cmake_call.append('-DUSE_UB_SANITIZER=1')
        if address_sanitizer:
            cmake_call.append('-DUSE_ADDRESS_SANITIZER=1')
        if gcc:
            cmake_call.extend(['-DLLVM_STDLIB=0'])
        else:
            cmake_call.extend(['-DLLVM_STDLIB=1'])
        cmake_result = subprocess.call(cmake_call, cwd=self.build_directory)
        if cmake_result != 0:
            sys.exit(cmake_result)

    def targetBinary(self, target):
        for root, dirnames, filenames in os.walk(self.build_directory):
            for filename in fnmatch.filter(filenames, target):
                full_target_file = os.path.join(root, filename)
                return full_target_file
        return ''

    def buildTarget(self, target, sdk='linux', arch='x86_64'):
        ninja_result = subprocess.call([
            'ninja',
            '-C',
            self.build_directory,
            '-f',
            self.project_file,
            target])
        if ninja_result != 0:
            sys.exit(ninja_result)
