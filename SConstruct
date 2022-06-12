#
# Main Fipper Build System entry point.
#
# This file is evaluated every time scons is invoked.
# Scons constructs all referenced environments & their targets' dependency
# trees on startup. So, to keep startup time as low as possible, we're hiding
# construction of certain targets behind command-line options.

from fbt.builders.dist import add_dist_builders, add_project_to_distenv


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


distenv = coreenv.Clone()
add_dist_builders(distenv)

firmware_out = add_project_to_distenv(distenv, coreenv, "firmware", "FW_ENV")
Default(firmware_out["FW_ARTIFACTS"])


# If enabled, construct updater-related targets
if GetOption("fullenv"):
    updater_out = add_project_to_distenv(distenv, coreenv, "updater", "UPD_ENV")

    # Target producing self-update package
    selfupdate_dist = distenv.DistBuilder(
        "pseudo",
        (distenv["DIST_DEPENDS"], firmware_out["FW_RESOURCES"]),
        DIST_EXTRA=[
            "-r",
            "${ROOT_DIR.abspath}/assets/resources",
            "--bundlever",
            "${UPDATE_VERSION_STRING}",
            "--radio",
            "${ROOT_DIR.abspath}/${COPRO_STACK_BIN_PATH}",
            "--radiotype",
            "${COPRO_STACK_TYPE}",
            "${COPRO_DISCLAIMER}",
            "--obdata",
            "${ROOT_DIR.abspath}/${COPRO_OB_DATA}",
        ],
    )
    distenv.Pseudo("pseudo")
    AlwaysBuild(selfupdate_dist)
    Alias("updater_package", selfupdate_dist)

# Just copy binaries to dist folder
basic_dist = distenv.DistBuilder("pseudo2", distenv["DIST_DEPENDS"])

distenv.Pseudo("pseudo2")
AlwaysBuild(basic_dist)
Alias("fw_dist", basic_dist)
