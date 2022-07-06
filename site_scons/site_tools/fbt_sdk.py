from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Scanner import C
from SCons.Script import Mkdir, Copy, Delete, Entry

import posixpath


class IncludeWalker:
    def __init__(self, env, use_defines: bool):
        print("IncludeWalker:", env.get("FW_FLAVOR"))
        self.env = env
        print(f"{self.env['CPPPATH']}")
        self.scanner = C.CConditionalScanner() if use_defines else C.CScanner()
        self.path = self.scanner.path(self.env)
        self.headers = set()
        self.visited = set()

    def _walk(self, node):
        # print("Walking", node.name, node.path)
        if node in self.visited:
            # print("-> Already seen")
            return
        # print("New", node.name)
        headers = self.scanner(node, self.env, self.path)
        self.headers.update(headers)
        self.visited.add(node)
        for header in headers:
            self._walk(header)

    def add_header(self, node):
        self.headers.add(node)
        self._walk(node)

    def walk(self, srcpath):
        self._walk(self.env.File(srcpath))


class SdkBuilder:
    # USE_CDEFS = False
    USE_CDEFS = True

    def __init__(self, env):
        self.env = env
        self.target_sdk_dir = env.subst("f${TARGET_HW}_sdk")
        self._process()

    def _process(self):
        self.includewalker = IncludeWalker(self.env, self.USE_CDEFS)
        print("src sdk", [f.path for f in self.env["SDK_HEADERS"]])
        for header in self.env["SDK_HEADERS"]:
            self.includewalker.add_header(header)

        self.sdk_dirs = list(set(h.dir.relpath for h in self.includewalker.headers))

    def get_headers(self):
        return self.includewalker.headers

    def generate_sdk_meta(self, source, target, env):
        # print(env.Dump())
        # print("generate_sdk_meta", source, target, env)
        filtered_paths = []
        expanded_paths = env.subst(
            # "${_concat('', CPPPATH, '', __env__)}", target="a", source="b"
            "$_CPPINCFLAGS",
            target=Entry("dummy"),
        )
        # print(f"{expanded_paths=}")
        # print(f"{self.sdk_dirs=}")
        for dir in self.sdk_dirs:
            if dir in expanded_paths:
                print("approved", dir)
                filtered_paths.append(
                    posixpath.normpath(posixpath.join(self.target_sdk_dir, dir))
                )

        sdk_env = env.Clone()
        sdk_env.Replace(CPPPATH=filtered_paths)
        # print(f"{sdk_env.Dump('CPPPATH')=}")
        with open(target[0].path, "wt") as f:
            # f.write("aaa")
            f.write(sdk_env.subst("$_CCCOMCOM", target=Entry("dummy")))
            # for h in self.includewalker.headers:
            # f.write(h.path + "\n")

    def gen_actions(self, sdk_root_dir):
        actions = [
            Action(self.generate_sdk_meta),
            # Touch(self.env["SDK_META"]),
        ]

        sdk_dir = sdk_root_dir.Dir(self.target_sdk_dir)
        dirs_to_create = set(sdk_dir.Dir(dirpath) for dirpath in self.sdk_dirs)
        # print([n.path for n in dirs_to_create])
        # actions = [Mkdir(sdk_dir), Touch(target[0])]
        actions += [Mkdir(d) for d in dirs_to_create]
        actions += [Copy(sdk_dir.File(h.path), h) for h in self.includewalker.headers]
        return actions


def generate_sdk(source, target, env, for_signature):
    print("generate_sdk", for_signature)
    sdk_root_dir = target[0].Dir(".")
    actions = [
        Delete(sdk_root_dir),
        Mkdir(sdk_root_dir),
    ]
    if for_signature:
        return actions

    sdk_gen = SdkBuilder(env)
    actions += sdk_gen.gen_actions(sdk_root_dir)

    # iw.walk("#applications/about/test.c")
    # dirs_to_create = set(sdk_dir.Dir(h.dir.relpath) for h in iw.headers)
    # print([n.path for n in dirs_to_create])
    # actions = [Mkdir(sdk_dir), Touch(target[0])]
    # actions += [Mkdir(d) for d in dirs_to_create]
    # actions += [Copy(sdk_dir.File(h.path), h) for h in iw.headers]
    return actions


def generate(env, **kw):
    env.Append(
        BUILDERS={
            "SDKBuilder": Builder(
                generator=generate_sdk,
            ),
        }
    )


def exists(env):
    return True
