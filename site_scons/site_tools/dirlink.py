import os
from SCons.Builder import Builder
from SCons.Action import Action
from fbt.util import link_dir


def link_action(env, source, target):
    # target_path = os.path.realpath(target[0].abspath)
    target_path = os.path.realpath(target[0].abspath)
    source_path = source[0].read()
    return link_dir(target_path, source_path, env["PLATFORM"] == "win32")


def generate(env):
    env.Append(
        BUILDERS={
            "LinkDir": Builder(
                action=Action(link_action, ""),
            ),
        }
    )


def exists(env):
    return True
