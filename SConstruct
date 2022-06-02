SConscript("sconscfg/environ.scons")
SConscript("sconscfg/cc.scons")
SConscript("sconscfg/git.scons")
SConscript("sconscfg/builders.scons")

Import("env")
# Progress(["-\r", "\\\r", "|\r", "/\r"], interval=5)
# variant_dir_name = f"build/f{env.subst('$FLIPPER_TARGET')}-{env.subst('$FIRMWARE_BUILD_CFG')}"
variant_dir_name = "build/fw"
SConscript(
    "firmware.scons",
    variant_dir=variant_dir_name,
    exports="variant_dir_name",
    duplicate=0,
)
