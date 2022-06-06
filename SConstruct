import os

SConscript("sconscfg/environ.scons")
SConscript("sconscfg/cc.scons")
SConscript("sconscfg/builders.scons")

Import("env")

if env["RAM_EXEC"]:
    env.Append(
        FIRMWARE_BUILD_CFG="updater",
    )
else:
    env.Append(
        FIRMWARE_BUILD_CFG="firmware",
    )

# Progress(["-\r", "\\\r", "|\r", "/\r"], interval=5)
variant_dir_name = f"f{env.subst('$TARGET_HW')}-{env.subst('$FIRMWARE_BUILD_CFG')}"
suffix = ""
if env["DEBUG"]:
    suffix += "D"
if env["COMPACT"]:
    suffix += "C"
if suffix:
    variant_dir_name += "-" + suffix

root_path = Dir(".").abspath
env["ROOT_DIR"] = root_path
build_path = os.path.join(root_path, "build", variant_dir_name)
env["BUILD_DIR"] = build_path

SConscript(
    "firmware.scons",
    variant_dir=build_path,
    duplicate=0,
)
