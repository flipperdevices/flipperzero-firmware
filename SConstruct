import os

AddOption(
    "--with-updater",
    dest="fullenv",
    action="store_true",
    help="Full firmware environment",
)

SConscript("site_scons/environ.scons")
SConscript("site_scons/cc.scons")
SConscript("site_scons/builders.scons")

Import("coreenv")

# Progress(["-\r", "\\\r", "|\r", "/\r"], interval=5)
variant_dir_name = f"f{coreenv.subst('$TARGET_HW')}-FWTYPE"
suffix = ""
if coreenv["DEBUG"]:
    suffix += "D"
if coreenv["COMPACT"]:
    suffix += "C"
if suffix:
    variant_dir_name += "-" + suffix

coreenv["ROOT_DIR"] = Dir(".")
build_path = os.path.join(Dir(".").abspath, "build", variant_dir_name)

firmware = SConscript(
    "firmware.scons",
    variant_dir=build_path.replace("FWTYPE", "firmware"),
    duplicate=0,
    exports={
        "fw_build_meta": {
            "type": "firmware",
            "build_dir": build_path.replace("FWTYPE", "firmware"),
        },
    },
)
Default(firmware)

if GetOption("fullenv"):
    updater = SConscript(
        "firmware.scons",
        variant_dir=build_path.replace("FWTYPE", "updater"),
        duplicate=0,
        exports={
            "fw_build_meta": {
                "type": "updater",
                "build_dir": build_path.replace("FWTYPE", "updater"),
            },
        },
    )
