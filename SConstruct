#
# Main Fipper Build System entry point.
#
# This file is evaluated every time scons is invoked.
# Scons constructs all referenced environments & their targets' dependency
# trees on startup. So, to keep startup time as low as possible, we're hiding
# construction of certain targets behind command-line options.


DefaultEnvironment(tools=[])
# Progress(["OwO\r", "owo\r", "uwu\r", "owo\r"], interval=15)


# This environment is created only for loading options & validating file/dir existance
fbt_variables = SConscript("site_scons/commandline.scons")
cmd_environment = Environment(tools=[], variables=fbt_variables)
Help(fbt_variables.GenerateHelpText(cmd_environment))


# Building basic environment - tools, utility methods, cross-compilation
# settings, gcc flags for Cortex-M4, basic builders and more
coreenv = SConscript(
    "site_scons/environ.scons",
    exports={"VAR_ENV": cmd_environment},
)
SConscript("site_scons/cc.scons", exports={"ENV": coreenv})

# Store root dir in environment for certain tools
coreenv["ROOT_DIR"] = Dir(".")

# Create a separate "dist" environment and add construction envs to it
distenv = coreenv.Clone(
    tools=["fbt_dist"],
    GDBOPTS="-ex 'target extended-remote | openocd.exe -c \"gdb_port pipe\" ${OPENOCD_OPTS}' "
    '-ex "set confirm off" ',
)

firmware_out = distenv.AddFwProject(
    base_env=coreenv,
    fw_type="firmware",
    fw_env_key="FW_ENV",
)
# Default(firmware_out["FW_ARTIFACTS"])


# If enabled, initialize updater-related targets
if GetOption("fullenv"):
    updater_out = distenv.AddFwProject(
        base_env=coreenv,
        fw_type="updater",
        fw_env_key="UPD_ENV",
    )

    # Target for self-update package
    selfupdate_dist = distenv.DistBuilder(
        "selfupdate.pseudo",
        (distenv["DIST_DEPENDS"], firmware_out["FW_RESOURCES"]),
        DIST_EXTRA=[
            "-r",
            '"${ROOT_DIR.abspath}/assets/resources"',
            "--bundlever",
            '"${UPDATE_VERSION_STRING}"',
            "--radio",
            '"${ROOT_DIR.abspath}/${COPRO_STACK_BIN_DIR}/${COPRO_STACK_BIN}"',
            "--radiotype",
            "${COPRO_STACK_TYPE}",
            "${COPRO_DISCLAIMER}",
            "--obdata",
            '"${ROOT_DIR.abspath}/${COPRO_OB_DATA}"',
        ],
    )
    distenv.Pseudo("selfupdate.pseudo")
    AlwaysBuild(selfupdate_dist)
    Alias("updater_package", selfupdate_dist)

# Target for copying & renaming binaries to dist folder
basic_dist = distenv.DistBuilder("dist.pseudo", distenv["DIST_DEPENDS"])
distenv.Pseudo("dist.pseudo")
AlwaysBuild(basic_dist)
Alias("fw_dist", basic_dist)
Default(basic_dist)

# Target for bundling core2 package for qFlipper
copro_dist = distenv.CoproBuilder(
    Dir("assets/core2_firmware"),
    [],
)
AlwaysBuild(copro_dist)
Alias("copro_dist", copro_dist)


# Debugging firmware
debug_elf = distenv.GDBPy(
    "debug.pseudo",
    firmware_out["FW_ELF"],
    GDBPYOPTS='-ex "source ${ROOT_DIR.abspath}/debug/FreeRTOS/FreeRTOS.py" '
    '-ex "source ${ROOT_DIR.abspath}/debug/PyCortexMDebug/PyCortexMDebug.py" '
    '-ex "svd_load ${SVD_FILE}" '
    '-ex "compare-sections"',
)
distenv.Depends(debug_elf, firmware_out["FW_FLASH"])
distenv.Pseudo("debug.pseudo")
AlwaysBuild(debug_elf)
Alias("debug", debug_elf)


debug_other = distenv.GDBPy(
    "debugother.pseudo",
    distenv.subst("$OTHER_ELF"),
    GDBPYOPTS=
    # '-ex "source ${ROOT_DIR.abspath}/debug/FreeRTOS/FreeRTOS.py" '
    '-ex "source ${ROOT_DIR.abspath}/debug/PyCortexMDebug/PyCortexMDebug.py" '
    '-ex "svd_load ${SVD_FILE}" ',
)
distenv.Pseudo("debugother.pseudo")
AlwaysBuild(debug_other)
Alias("debug_other", debug_other)
