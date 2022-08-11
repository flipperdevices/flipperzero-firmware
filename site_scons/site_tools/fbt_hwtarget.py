from SCons.Builder import Builder
from SCons.Action import Action
import json


class HardwareTargetLoader:
    def __init__(self, env, target_scons_dir, target_id):
        self.env = env
        self.target_scons_dir = target_scons_dir
        self.target_dir = self._getTargetDir(target_id)
        # self.target_id = target_id
        self.layered_target_dirs = []

        self.include_paths = []
        self.startup_script = None
        self.linker_script_flash = None
        self.linker_script_ram = None
        self.linker_script_app = None
        self.linker_dependencies = []
        self.excluded_sources = []
        self._processTargetDefinitions(target_id)

    def _getTargetDir(self, target_id):
        return self.target_scons_dir.Dir(f"f{target_id}")

    def _loadDescription(self, target_id):
        target_json_file = self._getTargetDir(target_id).File("target.json")
        if not target_json_file.exists():
            raise Exception(f"Target file {target_json_file} does not exist")
        with open(target_json_file.get_abspath(), "r") as f:
            vals = json.load(f)
            return vals

    def _processTargetDefinitions(self, target_id):
        self.layered_target_dirs.append(f"targets/f{target_id}")
        config = self._loadDescription(target_id)
        if include_paths := config.get("include_paths", None):
            self.include_paths.extend(
                f"#/firmware/targets/f{target_id}/{p}" for p in include_paths
            )

        self.excluded_sources.extend(config.get("excluded_sources", []))

        attrs = (
            "startup_script",
            "linker_script_flash",
            "linker_script_ram",
            "linker_script_app",
            "linker_dependencies",
        )

        for attr in attrs:
            if (val := config.get(attr, None)) and not getattr(self, attr):
                if attr == "linker_dependencies":
                    setattr(self, attr, val)
                else:
                    setattr(
                        self,
                        attr,
                        self.env.File(f"firmware/targets/f{target_id}/{val}"),
                    )

        if inherited_target := config.get("inherit", None):
            self._processTargetDefinitions(inherited_target)

    def gatherSources(self):
        sources = [self.startup_script]
        seen_filenames = set(self.excluded_sources)
        # print("Layers: ", self.layered_target_dirs)
        for target_dir in self.layered_target_dirs:
            accepted_sources = list(
                filter(
                    lambda f: f.name not in seen_filenames,
                    self.env.GlobRecursive("*.c", target_dir),
                )
            )
            seen_filenames.update(f.name for f in accepted_sources)
            sources.extend(accepted_sources)
        # print(f"Found {len(sources)} sources: {list(f.name for f in sources)}")
        return sources


def ConfigureForTarget(env, target_id):
    target_loader = HardwareTargetLoader(env, env.Dir("#/firmware/targets"), target_id)
    env.Replace(TARGET_CFG=target_loader)
    env.Append(CPPPATH=target_loader.include_paths)


def ApplyLibFlags(env):
    flags_to_apply = env["FW_LIB_OPTS"].get(
        env.get("FW_LIB_NAME"),
        env["FW_LIB_OPTS"]["Default"],
    )
    # print("Flags for ", env.get("FW_LIB_NAME", "Default"), flags_to_apply)
    env.MergeFlags(flags_to_apply)


def generate(env):
    env.AddMethod(ConfigureForTarget)
    env.AddMethod(ApplyLibFlags)


def exists(env):
    return True
