from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Defaults import Touch
import SCons

import os

__OPENOCD_BIN = "openocd"

_oocd_action = Action(
    "${OPENOCD} ${OPENOCD_OPTS} ${OPENOCD_COMMAND}",
    "${OPENOCDCOMSTR}",
)


def generate(env):
    if env["OPENOCD_LOGS_SWO"]:
        os.environ["OPENOCD_LOGS_OUTPUT"] = env["OPENOCD_LOGS_OUTPUT"]
        env.Append(OPENOCD_OPTS=["-f", "debug/swo.cfg"])
    env.SetDefault(
        OPENOCD=__OPENOCD_BIN,
        OPENOCD_OPTS="",
        OPENOCD_COMMAND="",
        OPENOCDCOM="${OPENOCD} ${OPENOCD_OPTS} ${OPENOCD_COMMAND}",
        OPENOCDCOMSTR="",
    )

    env.Append(
        BUILDERS={
            "OpenOCDFlash": Builder(
                action=[
                    _oocd_action,
                    Touch("${TARGET}"),
                ],
                suffix=".flash",
                src_suffix=".bin",
            ),
        }
    )


def exists(env):
    try:
        return env["OPENOCD"]
    except KeyError:
        pass

    if openocd := env.WhereIs(__OPENOCD_BIN):
        return openocd

    raise SCons.Errors.StopError("Could not detect openocd")
