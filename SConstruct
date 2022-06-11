# Main Fipper Build System entry point.
#
# This file is evaluated every time every time scons is invoked.
# scons builds all referenced environments & their targets' dependency trees
# on startup. So, to keep startup time as low as possible, we're hiding
# construction of certain targets behind command-line options.

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


Progress(["OwO\r", "owo\r", "uwu\r", "owo\r"], interval=15)

# Variant dir setup
variant_dir_name = f"f{coreenv.subst('$TARGET_HW')}-FWTYPE"
suffix = ""
if coreenv["DEBUG"]:
    suffix += "D"
if coreenv["COMPACT"]:
    suffix += "C"
if suffix:
    variant_dir_name += "-" + suffix

# Store root dir in environment for certain tools
coreenv["ROOT_DIR"] = Dir(".")
# Prepare variant dir for current configuration
build_path = os.path.join(Dir(".").abspath, "build", variant_dir_name)

# Configure basic firmware targets
firmware = SConscript(
    "firmware.scons",
    variant_dir=build_path.replace("FWTYPE", "firmware"),
    duplicate=0,
    exports={
        "ENV": coreenv,
        "fw_build_meta": {
            "type": "firmware",
            "build_dir": build_path.replace("FWTYPE", "firmware"),
        },
    },
)
Default(firmware)

# If enabled, configure updater-related targets
if GetOption("fullenv"):
    updater = SConscript(
        "firmware.scons",
        variant_dir=build_path.replace("FWTYPE", "updater"),
        duplicate=0,
        exports={
            "ENV": coreenv,
            "fw_build_meta": {
                "type": "updater",
                "build_dir": build_path.replace("FWTYPE", "updater"),
            },
        },
    )
