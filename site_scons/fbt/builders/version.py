import SCons
from SCons.Builder import Builder
from SCons.Action import Action

import subprocess
import datetime


def get_fast_git_version_id():
    try:
        version = (
            subprocess.check_output(
                [
                    "git",
                    "describe",
                    "--always",
                    "--dirty",
                    "--all",
                    "--long",
                ]
            )
            .strip()
            .decode()
        )
        return (version, datetime.date.today())
    except Exception as e:
        print("Failed to check for git changes", e)


def version_emitter(target, source, env):
    target_dir = target[0]
    target = [
        target_dir.File("version.inc.h"),
        target_dir.File("version.json"),
    ]
    return target, source


def add_version_builders(env):
    env.Append(
        BUILDERS={
            "VersionBuilder": Builder(
                action=Action(
                    "${PYTHON3} ${ROOT_DIR.abspath}/scripts/version.py generate -t ${TARGET_HW} -o ${TARGET.dir.posix} --dir ${ROOT_DIR}",
                    "${VERSIONCOMSTR}",
                ),
                emitter=version_emitter,
            ),
        }
    )
