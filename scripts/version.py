#!/usb/bin/env python3

from flipper.app import App

import subprocess
import os
from datetime import date


class GitVersion:
    def __init__(self, source_dir):
        self.source_dir = source_dir

    def get_version_info(self):
        commit = self._exec_git("rev-parse --short HEAD") or "unknown"

        dirty = False
        try:
            self._exec_git("diff --quiet")
        except subprocess.CalledProcessError as e:
            if e.returncode == 1:
                dirty = True

        branch = self._exec_git("rev-parse --abbrev-ref HEAD") or "unknown"
        branch_num = self._exec_git("rev-list --count HEAD") or "n/a"
        build_date = date.today().isoformat()

        try:
            version = self._exec_git("describe --tags --abbrev=0 --exact-match")
        except subprocess.CalledProcessError:
            version = "unknown"

        return {
            "GIT_COMMIT": f'"{commit}"',
            "GIT_BRANCH": f'"{branch}"',
            "GIT_BRANCH_NUM": f'"{branch_num}"',
            "VERSION": f'"{version}"',
            "BUILD_DATE": f'"{build_date}"',
            "BUILD_DIRTY": dirty and 1 or 0,
        }

    def _exec_git(self, args):
        cmd = ["git"]
        cmd.extend(args.split(" "))
        return (
            subprocess.check_output(cmd, cwd=self.source_dir, stderr=subprocess.STDOUT)
            .strip()
            .decode()
        )


class Main(App):
    def init(self):
        self.subparsers = self.parser.add_subparsers(help="sub-command help")

        # generate
        self.parser_generate = self.subparsers.add_parser(
            "generate", help="Generate version header"
        )

        self.parser_generate.add_argument("-o", dest="output", required=True)
        self.parser_generate.add_argument("--dir", dest="sourcedir", required=True)
        self.parser_generate.set_defaults(func=self.generate)

    def generate(self):
        current_info = GitVersion(self.args.sourcedir).get_version_info()
        new_version_info_fmt = (
            "\n".join(f"#define {key} {current_info[key]}" for key in current_info)
            + "\n"
        )

        current_version_info = None

        try:
            with open(self.args.output, "r") as file:
                current_version_info = file.read()
        except EnvironmentError as e:
            if self.args.debug:
                print(e)

        if current_version_info != new_version_info_fmt:
            if self.args.debug:
                print("old: ", current_version_info)
                print("new: ", new_version_info_fmt)
            with open(self.args.output, "w", newline="\n") as file:
                file.write(new_version_info_fmt)
            # os.utime("../lib/toolbox/version.c", None)
            print("Version information updated")
        else:
            if self.args.debug:
                print("Version information hasn't changed")

        return 0


if __name__ == "__main__":
    Main()()
