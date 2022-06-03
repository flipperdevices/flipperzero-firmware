import os

SConscript("sconscfg/environ.scons")
SConscript("sconscfg/cc.scons")
SConscript("sconscfg/git.scons")
SConscript("sconscfg/builders.scons")

Import("env")

# Progress(["-\r", "\\\r", "|\r", "/\r"], interval=5)
# variant_dir_name = f"build/f{env.subst('$FLIPPER_TARGET')}-{env.subst('$FIRMWARE_BUILD_CFG')}"

root_path = Dir(".").abspath
env["ROOT_DIR"] = root_path
build_path = os.path.join(root_path, "build", "fw")
env["BUILD_DIR"] = build_path

SConscript(
    "firmware.scons",
    variant_dir=build_path,
    duplicate=0,
)
