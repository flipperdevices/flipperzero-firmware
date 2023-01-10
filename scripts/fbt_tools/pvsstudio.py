from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Script import Delete
import SCons
import multiprocessing


def generate(env):
    env.SetDefault(
        PVSCHECKBIN="pvs-studio-analyzer",
        PVSCONVBIN="plog-converter",
        PVSNCORES=multiprocessing.cpu_count(),
        PVSOPTIONS=["@.pvsoptions", "-j${PVSNCORES}"],
        PVSCONVOPTIONS=[
            "-a",
            "GA:1,2,3",
            "-t",
            "fullhtml",
            "--indicate-warnings",
        ],
    )

    if not env["VERBOSE"]:
        env.SetDefault(
            PVSCHECKCOMSTR="\tPVS\t${TARGET}",
            PVSCONVCOMSTR="\tPVSREP\t${TARGET}",
        )

    env.Append(
        BUILDERS={
            # pvs-studio-analyzer analyze @.pvsoptions -C gccarm -j8 -f build/latest/compile_commands.json -o PVS-Studio.log
            "PVSCheck": Builder(
                action=Action(
                    '${PVSCHECKBIN} analyze ${PVSOPTIONS} -f "${SOURCE}" -o "${TARGET}"',
                    "${PVSCHECKCOMSTR}",
                ),
                suffix=".log",
                src_suffix=".json",
            ),
            "PVSReport": Builder(
                action=[
                    Delete("${TARGET.dir}"),
                    Action(
                        '${PVSCONVBIN} ${PVSCONVOPTIONS} "${SOURCE}" -o "${TARGET.dir}"',
                        "${PVSCONVCOMSTR}",
                    ),
                ],
                src_suffix=".log",
            ),
        }
    )


def exists(env):
    return True
