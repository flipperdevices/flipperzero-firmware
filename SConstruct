#
# Main Fipper Build System entry point.
#
# This file is evaluated every time scons is invoked.
# Scons constructs all referenced environments & their targets' dependency
# trees on startup. So, to keep startup time as low as possible, we're hiding
# construction of certain targets behind command-line options.

from fbt.utils import get_variant_dirname
from SCons.Action import CommandGeneratorAction

import os

cmd_vars = SConscript("site_scons/commandline.scons")

# Building basic environment - tools, utility methods, cross-compilation
# settings, gcc flags for Cortex-M4, basic builders and more
coreenv = SConscript(
    "site_scons/environ.scons",
    exports={"VARIABLES": cmd_vars},
)
SConscript("site_scons/cc.scons", exports={"ENV": coreenv})
SConscript("site_scons/builders.scons", exports={"ENV": coreenv})

# Store root dir in environment for certain tools
coreenv["ROOT_DIR"] = Dir(".")

# Progress(["OwO\r", "owo\r", "uwu\r", "owo\r"], interval=15)


# Prepare variant dir for given fw configuration & current options
def create_fw_build_targets(env, configuration_name):
    build_dir = Dir("build").Dir(get_variant_dirname(env, configuration_name)).abspath
    return SConscript(
        "firmware.scons",
        variant_dir=build_dir,
        duplicate=0,
        exports={
            "ENV": env,
            "fw_build_meta": {
                "type": configuration_name,
                "build_dir": build_dir,
            },
        },
    )


firmware_out = create_fw_build_targets(coreenv, "firmware")
Default(firmware_out.artifacts)

# If enabled, configure updater-related targets
if GetOption("fullenv"):
    updater_out = create_fw_build_targets(coreenv, "updater")
