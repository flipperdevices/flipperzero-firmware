#!/usr/bin/env python3

import os
import sys
import subprocess

script_dir = os.path.dirname(os.path.realpath(__file__))
script_path = os.path.realpath(os.path.dirname(__file__))

if script_path in sys.path:
    sys.path.remove(script_path)

repo_root = os.path.realpath(os.path.join(script_path, os.pardir))
black_root = os.path.join(repo_root, "lib/black")
black_src = os.path.join(repo_root, "lib/black/src")
black_version_dir = os.path.join(repo_root, "lib/black_version")

sys.path.insert(0, black_src)
sys.path.insert(1, black_version_dir)

black_version_raw = subprocess.run(
    ["git", "describe", "--dirty", "--tags", "--long"],
    stdout=subprocess.PIPE,
    encoding="utf-8",
    cwd=black_root,
)

black_version = black_version_raw.stdout.split("-")[0]
black_version_file = os.path.join(black_version_dir, "_black_version.py")

with open(black_version_file, "w", encoding="utf-8") as ver:
    ver.write('version = "' + black_version + '"')

if __name__ == "__main__":
    from black import __main__

    __main__()
