import os
import tempfile

from SCons.Errors import UserError
from SCons.Script import Builder
from SCons.Script import Flatten


def _get_device_serials(search_str="STLink"):
    import serial.tools.list_ports as list_ports

    return set([device.serial_number for device in list_ports.grep(search_str)])


def GetDevices(env):
    serials = _get_device_serials()
    if len(serials) == 0:
        raise UserError("No devices found")

    print("\n".join(serials))


class _GdbCommandlineWrapper:
    def __init__(self, binary_name, command_list_var_name):
        print("new _GdbCommandlineWrapper")
        self.command_list_var_name = command_list_var_name
        self.gdb_binary = binary_name
        self.fd = self.tmpfile = None

    def create_args_action(self, target, source, env):
        assert self.fd is None
        fd, self.tmpfile = tempfile.mkstemp(text=True)
        print("tmpfile", self.tmpfile)

        self._gdb_args = Flatten(env[self.command_list_var_name])
        print("gdb args", self._gdb_args)

        filedata = bytearray("\n".join(self._gdb_args), "utf-8")
        env.Replace(_GDB_CMD_FILE=self.tmpfile)
        os.write(fd, filedata)
        os.close(fd)

    def get_gdb_commandline(self):
        # print("get_gdb_commandline", self, self.tmpfile)
        return [self.gdb_binary, "-ex", "source ${_GDB_CMD_FILE}", "${SOURCES}"]

    def cleanup_tmpfile(self, target, source, env):
        # print("cleanup_tmpfile", self)
        assert self.tmpfile is not None
        env.Replace(_GDB_CMD_FILE=None)
        os.unlink(self.tmpfile)
        self.tmpfile = None


def _gdb_generator(source, target, env, for_signature):
    if for_signature:
        return [
            "${GDB}",
            "${GDBOPTS}",
            "${SOURCES}",
        ]

    wrapper = _GdbCommandlineWrapper("${GDBPY}", "GDBOPTS")
    return [
        env.Action(wrapper.create_args_action, None),
        wrapper.get_gdb_commandline(),
        env.Action(wrapper.cleanup_tmpfile, None),
    ]


def _gdb_emitter(target, source, env):
    # print("_gdb_emitter", target, source, env)
    # return [], source
    return target, source


def generate(env, **kw):
    env.AddMethod(GetDevices)
    env.SetDefault(
        FBT_DEBUG_DIR="${FBT_SCRIPT_DIR}/debug",
        OPENOCD_OPTS=[
            "-f",
            "interface/stlink.cfg",
            "-c",
            "transport select hla_swd",
            "-f",
            "${FBT_DEBUG_DIR}/stm32wbx.cfg",
            "-c",
            "stm32wbx.cpu configure -rtos auto",
        ],
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
            "|openocd -c 'gdb_port pipe; log_output ${FBT_DEBUG_DIR}/openocd.log' ${[SINGLEQUOTEFUNC(OPENOCD_OPTS)]}"
        ],
        GDBOPTS_BASE=[
            "-ex",
            "source ${FBT_DEBUG_DIR}/gdbinit",
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
            "source ${FBT_DEBUG_DIR}/FreeRTOS/FreeRTOS.py'",
            "-ex",
            "source ${FBT_DEBUG_DIR}/flipperapps.py",
            "-ex",
            "source ${FBT_DEBUG_DIR}/flipperversion.py",
            "-ex",
            "fap-set-debug-elf-root '${FBT_FAP_DEBUG_ELF_ROOT}",
            "-ex",
            "source ${FBT_DEBUG_DIR}/PyCortexMDebug/PyCortexMDebug.py",
            "-ex",
            "svd_load ${SVD_FILE}",
            "-ex",
            "compare-sections",
            "-ex",
            "fw-version",
        ],
        JFLASHPROJECT="${FBT_DEBUG_DIR}/fw.jflash",
    )

    env.Append(
        BUILDERS={
            "GDB": Builder(
                generator=_gdb_generator,
                emitter=_gdb_emitter,
                src_suffix=".elf",
                # action=[
                #     [
                #         "${GDB}",
                #         "${GDBOPTS}",
                #         "${GDBPYOPTS}",
                #         "${SOURCES}",
                #         "${GDBFLASH}",
                #     ]
                # ],
                # suffix=".elf",
            ),
        }
    )


def exists(env):
    return True
