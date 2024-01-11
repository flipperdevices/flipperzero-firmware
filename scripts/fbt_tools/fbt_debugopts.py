from SCons.Errors import UserError


def _get_device_serials(search_str="STLink"):
    import serial.tools.list_ports as list_ports

    return set([device.serial_number for device in list_ports.grep(search_str)])


def GetDevices(env):
    serials = _get_device_serials()
    if len(serials) == 0:
        raise UserError("No devices found")

    print("\n".join(serials))


def generate(env, **kw):
    env.AddMethod(GetDevices)
    env.SetDefault(
        FBT_DEBUG_DIR="${FBT_SCRIPT_DIR.get_subst_proxy().posix}/debug",
    )

    if (adapter_serial := env.subst("$SWD_TRANSPORT_SERIAL")) != "auto":
        env.Append(
            OPENOCD_OPTS=[
                "-c",
                f"adapter serial {adapter_serial}",
            ]
        )

    # Final command is "init", always explicitly added
    env.Append(
        OPENOCD_OPTS=["-c", "init"],
    )

    env.SetDefault(
        OPENOCD_GDB_PIPE=[
            "|openocd -c 'gdb_port pipe; log_output ${FBT_DEBUG_DIR.get_subst_proxy().posix}/openocd.log' ${[SINGLEQUOTEFUNC(OPENOCD_OPTS)]}"
        ],
        GDBOPTS_BASE=[
            "-ex",
            "source ${FBT_DEBUG_DIR.get_subst_proxy().posix}/gdbinit",
            "-ex",
            "target extended-remote ${GDBREMOTE}",
        ],
        GDBOPTS_BLACKMAGIC=[
            "-q",
            "-ex",
            "monitor swdp_scan",
            "-ex",
            "monitor debug_bmp enable",
            "-ex",
            "attach 1",
            "-ex",
            "set mem inaccessible-by-default off",
        ],
        GDBPYOPTS=[
            "-ex",
            "source ${FBT_DEBUG_DIR.get_subst_proxy().posix}/FreeRTOS/FreeRTOS.py",
            "-ex",
            "source ${FBT_DEBUG_DIR.get_subst_proxy().posix}/flipperapps.py",
            "-ex",
            "source ${FBT_DEBUG_DIR.get_subst_proxy().posix}/flipperversion.py",
            "-ex",
            "fap-set-debug-elf-root ${FBT_FAP_DEBUG_ELF_ROOT}",
            "-ex",
            "source ${FBT_DEBUG_DIR.get_subst_proxy().posix}/PyCortexMDebug/PyCortexMDebug.py",
            "-ex",
            "svd_load ${SVD_FILE}",
            "-ex",
            "compare-sections",
            "-ex",
            "fw-version",
        ],
        JFLASHPROJECT="${FBT_DEBUG_DIR.get_subst_proxy().posix}/fw.jflash",
    )


def exists(env):
    return True
