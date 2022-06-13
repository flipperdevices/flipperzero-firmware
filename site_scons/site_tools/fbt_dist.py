from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Script import Mkdir


def get_variant_dirname(env, project=None):
    parts = [f"f{env['TARGET_HW']}"]
    if project:
        parts.append(project)

    suffix = ""
    if env["DEBUG"]:
        suffix += "D"
    if env["COMPACT"]:
        suffix += "C"
    if suffix:
        parts.append(suffix)

    return "-".join(parts)


def create_fw_build_targets(env, configuration_name):
    flavor = get_variant_dirname(env, configuration_name)
    build_dir = env.Dir("build").Dir(flavor).abspath
    return env.SConscript(
        "firmware.scons",
        variant_dir=build_dir,
        duplicate=0,
        exports={
            "ENV": env,
            "fw_build_meta": {
                "type": configuration_name,
                "flavor": flavor,
                "build_dir": build_dir,
            },
        },
    )


def AddFwProject(env, base_env, fw_type, fw_env_key):
    project_env = env[fw_env_key] = create_fw_build_targets(base_env, fw_type)
    env.Append(
        DIST_PROJECTS=[
            project_env["FW_FLAVOR"],
        ],
        DIST_DEPENDS=[
            project_env["FW_ARTIFACTS"],
        ],
    )
    return project_env


def generate(env):
    env.AddMethod(AddFwProject)
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
