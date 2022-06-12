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
    flavor = get_variant_dirname(env, configuration_name)
    build_dir = Dir("build").Dir(flavor).abspath
    return SConscript(
        "firmware.scons",
        variant_dir=build_dir,
        duplicate=0,
        exports={
            "ENV": env,
            "fw_build_meta": {
                "type": configuration_name,
                "flavor": flavor,
                "build_dir": build_dir,
            },
        },
    )


def add_project_to_distenv(distenv, fw_type, fw_env_key):
    project_env = distenv[fw_env_key] = create_fw_build_targets(coreenv, fw_type)
    distenv.Append(
        DIST_PROJECTS=[
            project_env["FW_FLAVOR"],
        ],
        DIST_DEPENDS=[
            project_env["FW_ARTIFACTS"],
        ],
    )
    return project_env


distenv = coreenv.Clone()
firmware_out = add_project_to_distenv(distenv, "firmware", "FW_ENV")
Default(firmware_out["FW_ARTIFACTS"])


# If enabled, configure updater-related targets
if GetOption("fullenv"):
    updater_out = add_project_to_distenv(distenv, "updater", "UPD_ENV")


# Everything needs a target. So, we're using a dir for that
dist = distenv.Command(
    Dir("dist"),
    [],
    '${PYTHON3} ${ROOT_DIR.abspath}/scripts/sconsdist.py copy -p ${DIST_PROJECTS} -s "${DIST_SUFFIX}"',
)
Depends(dist, distenv["DIST_DEPENDS"])
AlwaysBuild(dist)
Alias("do_dist", dist)
