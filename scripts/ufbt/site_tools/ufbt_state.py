from SCons.Errors import SConsEnvironmentError

import json
import os
import sys
import pathlib
from functools import reduce


def _load_sdk_data(sdk_root):
    split_vars = {
        "cc_args",
        "cpp_args",
        "linker_args",
        "linker_libs",
    }
    subst_vars = split_vars | {
        "sdk_symbols",
    }
    sdk_data = {}
    with open(os.path.join(sdk_root, "sdk.opts")) as f:
        sdk_json_data = json.load(f)
        replacements = {
            sdk_json_data["app_ep_subst"]: "${APP_ENTRY}",
            sdk_json_data["sdk_path_subst"]: sdk_root.replace("\\", "/"),
            sdk_json_data["map_file_subst"]: "${TARGET}",
        }

        def do_value_substs(src_value):
            if isinstance(src_value, str):
                return reduce(
                    lambda acc, kv: acc.replace(*kv), replacements.items(), src_value
                )
            elif isinstance(src_value, list):
                return [do_value_substs(v) for v in src_value]
            else:
                return src_value

        for key, value in sdk_json_data.items():
            if key in split_vars:
                value = value.split()
            if key in subst_vars:
                value = do_value_substs(value)
            sdk_data[key] = value

    return sdk_data


def generate(env, **kw):
    ufbt_work_dir = env["UFBT_WORK_DIR"]
    sdk_state_dir_node = env["UFBT_STATE_DIR"]

    sdk_meta_filename = kw.get("SDK_META", "sdk_state.json")

    sdk_meta_path = os.path.join(sdk_state_dir_node.abspath, sdk_meta_filename)
    if not os.path.exists(sdk_meta_path):
        raise SConsEnvironmentError(f"SDK state file {sdk_meta_path} not found")

    with open(sdk_meta_path, "r") as f:
        sdk_state = json.load(f)

    if not (sdk_components := sdk_state.get("components", {})):
        raise SConsEnvironmentError("SDK state file doesn't contain components data")

    sdk_options_path = os.path.join(
        sdk_state_dir_node.abspath, sdk_components.get("sdk", "sdk")
    )
    sdk_data = _load_sdk_data(sdk_options_path)
    if not sdk_state["meta"]["hw_target"].endswith(sdk_data["hardware"]):
        raise SConsEnvironmentError("SDK state file doesn't match hardware target")

    scripts_dir = sdk_state_dir_node.Dir(sdk_components.get("scripts", ".")).Dir(
        "scripts"
    )
    env.SetDefault(
        # Paths
        SDK_DEFINITION=env.File(sdk_data["sdk_symbols"]),
        FBT_DEBUG_DIR=pathlib.Path(
            sdk_state_dir_node.Dir(sdk_components.get("scripts", "."))
            .Dir("debug")
            .abspath
        ).as_posix(),
        FBT_SCRIPT_DIR=scripts_dir,
        LIBPATH=sdk_state_dir_node.Dir(sdk_components.get("lib", "lib")),
        FW_ELF=sdk_state_dir_node.File(sdk_components.get("fwelf")),
        FW_BIN=sdk_state_dir_node.File(sdk_components.get("fwbin")),
        UPDATE_BUNDLE_DIR=sdk_state_dir_node.Dir(sdk_components.get("fwbundle")),
        SVD_FILE="${FBT_DEBUG_DIR}/STM32WB55_CM4.svd",
        # Build variables
        ROOT_DIR=env.Dir("#"),
        FIRMWARE_BUILD_CFG="firmware",
        TARGET_HW=int(sdk_data["hardware"]),
        CFLAGS_APP=sdk_data["cc_args"],
        CXXFLAGS_APP=sdk_data["cpp_args"],
        LINKFLAGS_APP=sdk_data["linker_args"],
        LIBS=sdk_data["linker_libs"],
        # ufbt state
        UFBT_WORK_DIR=ufbt_work_dir,
        UFBT_STATE_DIR=sdk_state_dir_node,
        UFBT_SDK_META=sdk_state["meta"],
        UFBT_BOOTSTRAP_SCRIPT=env.File("#/bootstrap.py"),
        UFBT_SCRIPT_ROOT=scripts_dir.Dir("ufbt"),
    )

    sys.path.insert(0, env["FBT_SCRIPT_DIR"].abspath)


def exists(env):
    return True
