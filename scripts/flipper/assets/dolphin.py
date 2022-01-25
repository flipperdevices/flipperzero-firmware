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

    def __init__(
        self,
        name: str,
        min_butthurt: int,
        max_butthurt: int,
        min_level: int,
        max_level: int,
        weight: int,
    ):
        # Manifest
        self.name = name
        self.min_butthurt = min_butthurt
        self.max_butthurt = max_butthurt
        self.min_level = min_level
        self.max_level = max_level
        self.weight = weight
        # Meta and data
        self.meta = {}
        self.frames = []
        self.bubbles = []
        # Logging
        self.logger = logging.getLogger("DolphinBubbleAnimation")

    def load(self, animation_directory: str):
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
        assert filetype == self.FILE_TYPE
        assert version == self.FILE_VERSION

        max_frame_number = None
        unique_frames = None
        total_frames_count = None
        bubble_slots_count = None

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
            bubble_slots_count = file.readKeyInt("Bubble slots")

            # Sanity Check
            assert self.meta["Width"] > 0 and self.meta["Width"] <= 128
            assert self.meta["Height"] > 0 and self.meta["Height"] <= 128
            assert self.meta["Passive frames"] > 0
            assert self.meta["Active frames"] >= 0
            assert self.meta["Frames order"]
            if self.meta["Active frames"] > 0:
                assert self.meta["Active cooldown"] > 0
                assert self.meta["Active cycles"] > 0
            else:
                assert self.meta["Active cooldown"] == 0
                assert self.meta["Active cycles"] == 0
            assert self.meta["Frame rate"] > 0
            assert self.meta["Duration"] > 0

            # Frames sanity check
            max_frame_number = max(self.meta["Frames order"])
            ordered_frames_count = len(self.meta["Frames order"])
            for i in range(max_frame_number):
                frame_filename = os.path.join(animation_directory, f"frame_{i}.png")
                assert os.path.isfile(frame_filename)
                self.frames.append(frame_filename)
            # Sanity check
            unique_frames = set(self.meta["Frames order"])
            unique_frames_count = len(unique_frames)
            if unique_frames_count != max_frame_number + 1:
                self.logger.warning(f"Not all frames were used in {self.name}")
            total_frames_count = self.meta["Passive frames"] + (
                self.meta["Active frames"] * self.meta["Active cycles"]
            )

            # Extra checks
            assert self.meta["Passive frames"] <= total_frames_count
            assert self.meta["Active frames"] <= total_frames_count
            print(
                self.meta["Passive frames"] + self.meta["Active frames"],
                ordered_frames_count,
            )
            assert (
                self.meta["Passive frames"] + self.meta["Active frames"]
                == ordered_frames_count
            )
        except EOFError as e:
            raise Exception("Invalid meta file: too short")
        except AssertionError as e:
            self.logger.exception(e)
            self.logger.error(f"Animation {self.name} got incorrect meta")
            raise Exception("Meta file is invalid: incorrect data")

        # Bubbles
        bubble_slot = 0
        while bubble_slot < bubble_slots_count:
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

                # Sanity check
                assert bubble["Slot"] == bubble_slot
                assert bubble["X"] >= 0 and bubble["X"] < 128
                assert bubble["Y"] >= 0 and bubble["Y"] < 128
                assert len(bubble["Text"]) > 0
                assert bubble["AlignH"] in ["Left", "Center", "Right"]
                assert bubble["AlignV"] in ["Bottom", "Center", "Top"]
                assert bubble["StartFrame"] < total_frames_count
                assert bubble["EndFrame"] < total_frames_count
                assert bubble["EndFrame"] > bubble["StartFrame"]

                # Store bubble
                self.bubbles.append(bubble)
                bubble_slot += 1
            except EOFError as e:
                self.logger.error(f"EOF when reading bubble data at slot {bubble_slot}")
                raise Exception("Meta file is incomplete: missing bubble data")
            except AssertionError as e:
                self.logger.exception(e)
                self.logger.error(
                    f"Animation {self.name} bubble slot {bubble_slot} got incorrect data: {bubble}"
                )
                raise Exception("Meta file is invalid: incorrect bubble data")


class DolphinManifest:

    FILE_TYPE = "Flipper Animation Manifest"
    FILE_VERSION = 1

    def __init__(self):
        self.animations = []
        self.logger = logging.getLogger("DolphinManifest")

    def load(self, filename: str):
        file = FlipperFormatFile()
        file.load(filename)

        # Check file header
        filetype, version = file.getHeader()
        assert filetype == self.FILE_TYPE
        assert version == self.FILE_VERSION

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
        assert os.path.exists(source_directory)
        # Load Manifest
        manifest_filename = os.path.join(source_directory, "manifest.txt")
        self.logger.info(f"Loading manifest from {manifest_filename}")
        self.manifest.load(manifest_filename)

    def pack(self, output_directory: str, internal: bool):
        if internal:
            self._pack_internal(output_directory)
        else:
            self._pack_external(output_directory)
