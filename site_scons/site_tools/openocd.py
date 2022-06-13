from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Defaults import Touch
import SCons

__OPENOCD_BIN = "openocd"


def generate(env):
    env.SetDefault(
        OPENOCD=__OPENOCD_BIN,
    )
    env.Append(
        BUILDERS={
            "OOCDFlash": Builder(
                action=[
                    Action(
                        '${OPENOCD} ${OPENOCD_OPTS} -c "program ${SOURCE.posix} reset exit ${IMAGE_BASE_ADDRESS}"',
                        "${FLASHCOMSTR}",
                    ),
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
