from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Script import Mkdir


def generate(env):
    env.Append(
        BUILDERS={
            "DistBuilder": Builder(
                action=Action(
                    '${PYTHON3} ${ROOT_DIR.abspath}/scripts/sconsdist.py copy -p ${DIST_PROJECTS} -s "${DIST_SUFFIX}" ${DIST_EXTRA}',
                    "${DISTCOMSTR}",
                ),
            ),
            "CoproBuilder": Builder(
                action=Action(
                    [
                        Mkdir("$TARGET"),
                        "${PYTHON3} ${ROOT_DIR.abspath}/scripts/assets.py "
                        "copro ${COPRO_CUBE_DIR} "
                        "${TARGET} ${COPRO_MCU_FAMILY} "
                        "--cube_ver=${COPRO_CUBE_VERSION} "
                        "--stack_type=${COPRO_STACK_TYPE} "
                        '--stack_file="${COPRO_STACK_BIN}" '
                        "--stack_addr=${COPRO_STACK_ADDR} ",
                    ],
                    "",
                )
            ),
        }
    )


def exists(env):
    return True
