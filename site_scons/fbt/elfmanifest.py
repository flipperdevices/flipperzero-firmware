from dataclasses import dataclass
import os

import struct
from dataclasses import dataclass, field

from .appmanifest import FlipperApplication


_MANIFEST_MAGIC = 0x52474448


@dataclass
class ElfManifestBaseHeader:
    manifest_version: int
    api_version: int

    manifest_magic: int = 0x52474448

    def as_bytes(self):
        return struct.pack(
            "<III",
            self.manifest_magic,
            self.manifest_version,
            self.api_version,
        )


@dataclass
class ElfManifestV1:
    stack_size: int
    app_version: int
    name: str
    icon: bytes = field(default=b"")

    def as_bytes(self):
        return struct.pack(
            "<hI32s?32s",
            self.stack_size,
            self.app_version,
            bytes(self.name.encode("ascii")),
            bool(self.icon),
            self.icon,
        )


def assemble_manifest_data(app_manifest: FlipperApplication, sdk_version):
    if app_manifest.fap_icon:
        from flipper.assets.icon import file2image

        image = file2image(os.path.join(app_manifest._apppath, app_manifest.fap_icon))
        print(image)

    data = ElfManifestBaseHeader(1, sdk_version).as_bytes()
    data += ElfManifestV1(app_manifest.stack_size, 1, app_manifest.name).as_bytes()

    return data
