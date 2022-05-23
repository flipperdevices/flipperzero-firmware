#/*
# * FreeRTOS Kernel <DEVELOPMENT BRANCH>
# * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# *
# * SPDX-License-Identifier: MIT
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of
# * this software and associated documentation files (the "Software"), to deal in
# * the Software without restriction, including without limitation the rights to
# * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# * the Software, and to permit persons to whom the Software is furnished to do so,
# * subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all
# * copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# * https://www.FreeRTOS.org
# * https://github.com/FreeRTOS
# *
# */

import os
import shutil

_THIS_FILE_DIRECTORY_ = os.path.dirname(os.path.realpath(__file__))
_FREERTOS_PORTABLE_DIRECTORY_ = os.path.dirname(_THIS_FILE_DIRECTORY_)

_COMPILERS_ = ['GCC', 'IAR']
_ARCH_NS_ = ['ARM_CM33', 'ARM_CM33_NTZ', 'ARM_CM23', 'ARM_CM23_NTZ']
_ARCH_S_ = ['ARM_CM33', 'ARM_CM23']

_SUPPORTED_CONFIGS_ =   {
                            'GCC' : ['ARM_CM33', 'ARM_CM33_NTZ', 'ARM_CM23', 'ARM_CM23_NTZ'],
                            'IAR' : ['ARM_CM33', 'ARM_CM33_NTZ', 'ARM_CM23', 'ARM_CM23_NTZ']
                        }

# Files to be complied in the Secure Project
_SECURE_FILE_PATHS_ = [
    os.path.join('secure', 'context'),
    os.path.join('secure', 'context', 'portable', '_COMPILER_ARCH_'),
    os.path.join('secure', 'heap'),
    os.path.join('secure', 'init'),
    os.path.join('secure', 'macros')
]

# Files to be complied in the Non-Secure Project
_NONSECURE_FILE_PATHS_ = [
    'non_secure',
    os.path.join('non_secure', 'portable', '_COMPILER_ARCH_')
]


def is_supported_config(compiler, arch):
    return arch in _SUPPORTED_CONFIGS_[compiler]


def copy_files_in_dir(src_abs_path, dst_abs_path):
    for src_file in os.listdir(src_abs_path):
        src_file_abs_path = os.path.join(src_abs_path, src_file)
        if os.path.isfile(src_file_abs_path) and src_file != 'ReadMe.txt':
            if not os.path.exists(dst_abs_path):
                os.makedirs(dst_abs_path)
            print('Copying {}...'.format(os.path.basename(src_file_abs_path)))
            shutil.copy2(src_file_abs_path, dst_abs_path)


def copy_files_for_compiler_and_arch(compiler, arch, src_paths, dst_path):
    _COMPILER_ARCH_ = os.path.join(compiler, arch)
    for src_path in src_paths:
        src_path_sanitized = src_path.replace('_COMPILER_ARCH_', _COMPILER_ARCH_ )

        src_abs_path = os.path.join(_THIS_FILE_DIRECTORY_, src_path_sanitized)
        dst_abs_path = os.path.join(_FREERTOS_PORTABLE_DIRECTORY_, _COMPILER_ARCH_, dst_path)

        copy_files_in_dir(src_abs_path, dst_abs_path)


def copy_files():
    # Copy Secure Files
    for compiler in _COMPILERS_:
        for arch in _ARCH_S_:
            if is_supported_config(compiler, arch):
                copy_files_for_compiler_and_arch(compiler, arch, _SECURE_FILE_PATHS_, 'secure')

    # Copy Non-Secure Files
    for compiler in _COMPILERS_:
        for arch in _ARCH_NS_:
            if is_supported_config(compiler, arch):
                copy_files_for_compiler_and_arch(compiler, arch, _NONSECURE_FILE_PATHS_, 'non_secure')


def main():
    copy_files()


if __name__ == '__main__':
    main()
