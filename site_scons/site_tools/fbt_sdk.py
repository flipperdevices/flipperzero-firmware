from SCons.Builder import Builder
from SCons.Action import Action

# from SCons.Scanner import C
from SCons.Script import Mkdir, Copy, Delete, Entry
from SCons.Util import LogicalLines

import os.path
import posixpath

from fbt.sdk import Sdk


# class SdkPreBuilder:
#     def __init__(self, env) -> None:
#         self.env = env

#     def generate_actions(self):
#         return [
#             self._pregen_sdk_origin_file,
#             "$CC -o $TARGET -E -P $CCFLAGS $_CCCOMCOM $SDK_PP_FLAGS -MMD ${TARGET}.c",
#         ]


def prebuild_sdk_emitter(target, source, env):
    target.append(env.ChangeFileExtension(target[0], ".d"))
    return target, source


def prebuild_sdk(source, target, env, for_signature):
    def _pregen_sdk_origin_file(source, target, env):
        mega_file = env.subst("${TARGET}.c", target=target[0])
        with open(mega_file, "wt") as sdk_c:
            sdk_c.write("\n".join(f"#include <{h.path}>" for h in env["SDK_HEADERS"]))

    return [
        _pregen_sdk_origin_file,
        "$CC -o $TARGET -E -P $CCFLAGS $_CCCOMCOM $SDK_PP_FLAGS -MMD ${TARGET}.c",
    ]


class SdkTreeBuilder:
    def __init__(self, env, target, source) -> None:
        self.env = env
        self.target = target
        self.source = source

        self.header_depends = []
        self.header_dirs = []

        self.target_sdk_dir = env.subst("f${TARGET_HW}_sdk")
        self.sdk_deploy_dir = target[0].Dir(self.target_sdk_dir)

    def _parse_sdk_depends(self):
        deps_file = self.source[0]
        with open(deps_file.path, "rt") as deps_f:
            lines = LogicalLines(deps_f).readlines()
            _, depends = lines[0].split(":", 1)
            self.header_depends = list(
                filter(lambda fname: fname.endswith(".h"), depends.split()),
            )
            self.header_dirs = sorted(set(map(os.path.dirname, self.header_depends)))

    def _generate_sdk_meta(self):
        filtered_paths = [self.target_sdk_dir]
        expanded_paths = self.env.subst(
            "$_CPPINCFLAGS",
            target=Entry("dummy"),
        )
        for dir in self.header_dirs:
            if dir in expanded_paths:
                # print("approved", dir)
                filtered_paths.append(
                    posixpath.normpath(posixpath.join(self.target_sdk_dir, dir))
                )

        sdk_env = self.env.Clone()
        sdk_env.Replace(CPPPATH=filtered_paths)
        with open(self.target[0].path, "wt") as f:
            f.write(sdk_env.subst("$CCFLAGS $_CCCOMCOM", target=Entry("dummy")))

    def _create_deploy_commands(self):
        dirs_to_create = set(
            self.sdk_deploy_dir.Dir(dirpath) for dirpath in self.header_dirs
        )
        actions = [
            Delete(self.sdk_deploy_dir),
            Mkdir(self.sdk_deploy_dir),
        ]
        actions += [Mkdir(d) for d in dirs_to_create]

        actions += [
            Copy(
                self.sdk_deploy_dir.File(h).path,
                h,
            )
            for h in self.header_depends
        ]
        return actions

    def generate_actions(self):
        self._parse_sdk_depends()
        self._generate_sdk_meta()

        return self._create_deploy_commands()


def deploy_sdk_tree(target, source, env, for_signature):
    if for_signature:
        return []

    sdk_tree = SdkTreeBuilder(env, target, source)
    return sdk_tree.generate_actions()


def generate(env, **kw):
    env.Append(
        BUILDERS={
            "SDKPrebuilder": Builder(
                emitter=prebuild_sdk_emitter,
                generator=prebuild_sdk,
                suffix=".i",
            ),
            "SDKTree": Builder(
                generator=deploy_sdk_tree,
                src_suffix=".d",
            ),
            # "SDKBuilder": Builder(
            #     # generator=generate_sdk,
            # ),
        }
    )


def exists(env):
    return True
