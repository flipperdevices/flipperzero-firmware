import SCons
from SCons.Subst import quote_spaces

import re


WINPATHSEP_RE = re.compile(r"\\([^\"'\\]|$)")


def tempfile_arg_esc_func(arg):
    arg = quote_spaces(arg)
    if SCons.Platform.platform_default() != "win32":
        return arg
    # GCC requires double Windows slashes, let's use UNIX separator
    return WINPATHSEP_RE.sub(r"/\1", arg)


def wrap_tempfile(env, command):
    env[command] = '${TEMPFILE("' + env[command] + '","$' + command + 'STR")}'


def prefix_commands(env, command_prefix, cmd_list):
    for command in cmd_list:
        if command in env:
            env[command] = command_prefix + env[command]


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


def add_project_to_distenv(dist_env, core_env, fw_type, fw_env_key):
    project_env = dist_env[fw_env_key] = create_fw_build_targets(core_env, fw_type)
    dist_env.Append(
        DIST_PROJECTS=[
            project_env["FW_FLAVOR"],
        ],
        DIST_DEPENDS=[
            project_env["FW_ARTIFACTS"],
        ],
    )
    return project_env
