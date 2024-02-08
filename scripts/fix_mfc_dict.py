import pathlib
import string

file = (
    pathlib.Path(__file__).parent
    / "../applications/main/nfc/resources/nfc/assets/mf_classic_dict.nfc"
)
try:
    lines = file.read_text(encoding="ascii").splitlines()
except UnicodeDecodeError:
    print(
        "Fix non-ASCII characters: https://pteo.paranoiaworks.mobi/diacriticsremover/"
    )
    exit()

for i, line in enumerate(lines):
    lines[i] = line = line.strip()
    if line.startswith("#"):
        continue
    lines[i] = line = line.upper()
    if not line:
        continue
    if len(line) != 12 or any(char not in string.hexdigits for char in line):
        print(f"line {i + 1} is not correct: {line}")
    for j in reversed(range(i + 1, len(lines))):
        if lines[j].upper().strip() == line:
            del lines[j]

file.write_text("\n".join(line for line in lines if line.removeprefix("#")))
