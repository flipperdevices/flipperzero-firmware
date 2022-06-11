# Main Fipper Build System entry point.
#
# This file is evaluated every time every time scons is invoked.
# scons builds all referenced environments & their targets' dependency trees
# on startup. So, to keep startup time as low as possible, we're hiding
# construction of certain targets behind command-line options.

from sutils import get_variant_dirname

import os

# To build updater-related targets, you need to set this option
AddOption(
    "--with-updater",
    dest="fullenv",
    action="store_true",
    help="Full firmware environment",
)


# Building basic environment - tools, utility methods, cross-compilation
# settings, gcc flags for Cortex-M4, basic builders and more
coreenv = SConscript("site_scons/environ.scons")
SConscript("site_scons/cc.scons", exports={"ENV": coreenv})
SConscript("site_scons/builders.scons", exports={"ENV": coreenv})

# Store root dir in environment for certain tools
coreenv["ROOT_DIR"] = Dir(".")

# Progress(["OwO\r", "owo\r", "uwu\r", "owo\r"], interval=15)


# Prepare variant dir for current configuration
def create_fw_build_targets(env, configuration_name):
    build_dir = Dir("build").Dir(get_variant_dirname(env, "firmware")).abspath
    return SConscript(
        "firmware.scons",
        variant_dir=build_dir,
        duplicate=0,
        exports={
            "ENV": env,
            "fw_build_meta": {
                "type": "firmware",
                "build_dir": build_dir,
            },
        },
    )


firmware = create_fw_build_targets(coreenv, "firmware")
Default(firmware)

# If enabled, configure updater-related targets
if GetOption("fullenv"):
    updater = create_fw_build_targets(coreenv, "updater")
