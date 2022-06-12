from dataclasses import dataclass
from typing import List, Optional, Any


@dataclass
class FirmwareOutput:
    project: str
    dfu: Optional[Any]
    elf: Optional[Any]
    bin: Optional[Any]
    json: Optional[Any]
    extapps: Optional[List[Any]]

    @property
    def artifacts(self):
        return list(
            filter(lambda o: o is not None, (self.dfu, self.elf, self.bin, self.json))
        )
