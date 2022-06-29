import SCons
from SCons.Subst import quote_spaces

import re
import os


WINPATHSEP_RE = re.compile(r"\\([^\"'\\]|$)")


def tempfile_arg_esc_func(arg):
    arg = quote_spaces(arg)
    if SCons.Platform.platform_default() != "win32":
        return arg
    # GCC requires double Windows slashes, let's use UNIX separator
    return WINPATHSEP_RE.sub(r"/\1", arg)


def wrap_tempfile(env, command):
    env[command] = '${TEMPFILE("' + env[command] + '","$' + command + 'STR")}'


def link_dir(target, source, env):
    target_path = os.path.realpath(target[0].abspath)
    source_path = os.path.realpath(source[0].abspath)
    os.unlink(target_path)
    if env["PLATFORM"] == "win32":
        # Crete junction
        import _winapi

        if not os.path.isdir(source_path):
            raise Exception(f"Source directory {source_path} is not a directory")

        if not os.path.exists(target_path):
            _winapi.CreateJunction(target_path, source_path)
    else:
        os.symlink(source, target)
