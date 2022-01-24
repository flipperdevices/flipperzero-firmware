import multiprocessing
import logging
import os
import sys
import shutil

from flipper.utils.fff import *
from .icon import *

class DolphinBubbleAnimation:

    FILE_TYPE = "Flipper Animation"
    FILE_VERSION = 1

    def __init__(self, name:str, min_butthurt:int, max_butthurt:int, min_level:int, max_level:int, weight:int):
        # Manifest
        self.name = name
        self.min_butthurt = min_butthurt
        self.max_butthurt = max_butthurt
        self.min_level = min_level
        self.max_level = max_level
        self.weight = weight
        # Meta
        self.meta = {}
        # Frames
        self.frames = []
        self.bubbles = []
        # Logging
        self.logger = logging.getLogger("DolphinBubbleAnimation")

    def load(self, animation_directory:str):
        if not os.path.isdir(animation_directory):
            raise Exception(f"Animation folder doesn't exists: { animation_directory }")

        meta_filename = os.path.join(animation_directory, "meta.txt")
        if not os.path.isfile(meta_filename):
            raise Exception(f"Animation meta file doesn't exists: { meta_filename }")

        self.logger.info(f"Loading meta from {meta_filename}")
        file = FlipperFormatFile()
        file.load(meta_filename)

        # Check file header
        filetype, version = file.getHeader()
        assert(filetype == self.FILE_TYPE)
        assert(version == self.FILE_VERSION)

        try:
            # Main meta
            self.meta["Width"] = file.readKeyInt("Width")
            self.meta["Height"] = file.readKeyInt("Height")
            self.meta["Passive frames"] = file.readKeyInt("Passive frames")
            self.meta["Active frames"] = file.readKeyInt("Active frames")
            self.meta["Frames order"] = file.readKeyIntArray("Frames order")
            self.meta["Active cycles"] = file.readKeyInt("Active cycles")
            self.meta["Frame rate"] = file.readKeyInt("Frame rate")
            self.meta["Duration"] = file.readKeyInt("Duration")
            self.meta["Active cooldown"] = file.readKeyInt("Active cooldown")

            # Total bubble slots
            bubble_slots = file.readKeyInt("Bubble slots")
        except EOFError as e:
            raise Exception("Invalid meta file: too short")

        bubble_slot = 0
        while bubble_slot < bubble_slots:
            try:
                # Bubble data
                bubble = {}
                bubble["Slot"] = file.readKeyInt("Slot")
                bubble["X"] = file.readKeyInt("X")
                bubble["Y"] = file.readKeyInt("Y")
                bubble["Text"] = file.readKey("Text")
                bubble["AlignH"] = file.readKey("AlignH")
                bubble["AlignV"] = file.readKey("AlignV")
                bubble["StartFrame"] = file.readKeyInt("StartFrame")
                bubble["EndFrame"] = file.readKeyInt("EndFrame")

                # Store bubble
                self.bubbles.append(bubble)
                bubble_slot += 1
            except EOFError as e: 
                self.logger.error(f"EOF when reading bubble data at slot {bubble_slot}")
                raise Exception("Meta file is incomplete: missing bubble data")


class DolphinManifest:

    FILE_TYPE = "Flipper Animation Manifest"
    FILE_VERSION = 1

    def __init__(self):
        self.animations = []
        self.logger = logging.getLogger("DolphinManifest")

    def load(self, filename:str):
        file = FlipperFormatFile()
        file.load(filename)

        # Check file header
        filetype, version = file.getHeader()
        assert(filetype == self.FILE_TYPE)
        assert(version == self.FILE_VERSION)

        # Load animation data
        while True:
            try:
                # Read animation spcification
                name = file.readKey("Name")
                min_butthurt = file.readKeyInt("Min butthurt")
                max_butthurt = file.readKeyInt("Max butthurt")
                min_level = file.readKeyInt("Min level")
                max_level = file.readKeyInt("Max level")
                weight = file.readKeyInt("Weight")

                # Initialize animation
                animation = DolphinBubbleAnimation(
                    name, min_butthurt, max_butthurt, min_level, max_level, weight
                )

                # Load Animation meta and frames
                head, tail = os.path.split(filename)
                animation_directory = os.path.join(head, name)
                animation.load(animation_directory)

                # Add to array
                self.animations.append(animation)
            except EOFError as e: 
                break

class Dolphin:
    def __init__(self):
        self.manifest = DolphinManifest()
        self.logger = logging.getLogger("Dolphin")

    @staticmethod
    def _pack_animation(pair: set):
        source, destination = pair
        image = file2image(source)
        image.write(destination)

    def _pack_internal(self, output_directory: str):
        pass

    def _pack_external(self, output_directory: str):
        pass

    def load(self, source_directory: str):
        assert(os.path.exists(source_directory))
        # Load Manifest
        manifest_filename = os.path.join(source_directory, "manifest.txt")
        self.logger.info(f"Loading manifest from {manifest_filename}")
        self.manifest.load(manifest_filename)

    def pack(self, output_directory: str, internal: bool):
        if internal:
            self._pack_internal(output_directory)
        else:
            self._pack_external(output_directory)
