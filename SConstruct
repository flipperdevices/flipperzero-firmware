SConscript("sconscfg/environ.scons")
SConscript("sconscfg/cc.scons")
SConscript("sconscfg/git.scons")
SConscript("sconscfg/builders.scons")
SConscript("sconscfg/firmwareopts.scons")

Import("env")
# Progress(["-\r", "\\\r", "|\r", "/\r"], interval=5)

SConscript(
    "firmware.scons",
    variant_dir=f"build/f{env.subst('$FLIPPER_TARGET')}-{env.subst('$FIRMWARE_BUILD_CFG')}",
    duplicate=0,
)
