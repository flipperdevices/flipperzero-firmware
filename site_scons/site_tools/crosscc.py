from SCons.Tool import asm
from SCons.Tool import gcc
from SCons.Tool import gxx
from SCons.Tool import ar
from SCons.Tool import gnulink
import strip

from fbt import utils


def generate(env):
    for orig_tool in (asm, gcc, gxx, ar, gnulink, strip):
        orig_tool.generate(env)
    env.SetDefault(TOOLCHAIN_PREFIX="arm-none-eabi-")

    utils.prefix_commands(
        env,
        env.subst("$TOOLCHAIN_PREFIX"),
        [
            "AR",
            "AS",
            "CC",
            "CXX",
            "OBJCOPY",
            "RANLIB",
            "STRIP",
        ],
    )


def exists(env):
    return True
