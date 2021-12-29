import multiprocessing
import logging
import os
import sys

from .icon import *


def _pack_animation(pair: set):
    source, destination = pair
    image = file2image(source)
    image.write(destination)


def pack_animations(source: str, destination: str):
    assert os.path.exists(source)
    # Create list for processing
    to_pack = []
    dirpath, dirnames = next(os.walk(source))[:2]
    dirnames.sort()
    for dirname in dirnames:
        current_directory = os.path.join(dirpath, dirname)
        # Ensure destination folder
        destination_directory = os.path.join(destination, dirname)
        os.makedirs(destination_directory, exist_ok=True)
        # Find all files
        filenames = next(os.walk(current_directory))[2]
        filenames.sort()
        for filename in filenames:
            if is_file_an_icon(filename):
                source_filename = os.path.join(current_directory, filename)
                destination_filename = os.path.join(
                    destination_directory, os.path.splitext(filename)[0] + ".bm"
                )
                to_pack.append((source_filename, destination_filename))
    # Process images in parallel
    pool = multiprocessing.Pool()
    pool.map(_pack_animation, to_pack)


def pack_dolphin(source_directory: str, destination_directory: str):
    pack_animations(
        os.path.join(source_directory, "animations"),
        os.path.join(destination_directory, "animations"),
    )
