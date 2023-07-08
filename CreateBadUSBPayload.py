import zipfile
import base64
import os
import argparse

FORMATS={
    "powershell": {
        "line": "STRING $b64 += \"{}\"",
        "template": """
STRING $b64 = ""
ENTER
{lines}
ENTER
STRING $de = [System.Convert]::FromBase64String($b64)
ENTER
STRING Set-Content {outName}.zip -Value $de -Encoding Byte
""".strip()
    },

    "bash": {
        "line": "STRING {}",
        "template": """
STRING #!/bin/bash
STRING base64 -d << EOF
ENTER
{lines}
STRING EOF > {outName}.zip
ENTER
""".strip(),
    },
}

class Config(object):
    def __init__(self, args) -> None:
        if not args:
            return
        
        self.inFile: str = args.inFile
        self.out: str | None = args.out
        self.format: str = args.format

ARGS=Config(None)


def compress() -> str:
    zip_path = "/tmp/file.zip"
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as f:
        f.write(ARGS.inFile, arcname=os.path.basename(ARGS.inFile))

    content = None
    with open(zip_path, "rb") as f:
        content = f.read()
    
    os.unlink(zip_path)

    return base64.b64encode(content).decode("utf-8")


def encodeToFormat(b64: str):
    ins = []
    tmpl = FORMATS[ARGS.format]
    while len(b64) > 0:
        c = b64[:1000]
        ins.append(tmpl["line"].format(c))
        b64 = b64[1000:]

    outFile = ARGS.out
    if not outFile:
        path = os.path.abspath(ARGS.inFile)
        outFile = os.path.join(os.path.dirname(path), os.path.basename(path) + "_badusb.txt")
    
    with open(outFile, "wt") as f:
        f.write(tmpl["template"].format(
            lines="\nENTER\n".join(ins), 
            outName=f"{os.path.basename(ARGS.inFile)}.zip"
        ))

def main():
    global ARGS
    parser = argparse.ArgumentParser()
    parser.add_argument("-in", type=str, dest="inFile", required=True)
    parser.add_argument("-out", type=str, required=False, default=None)
    parser.add_argument("-f", "--format", choices=["powershell", "bash"], type=str, required=True)

    ARGS = Config(parser.parse_args())

    if not os.path.exists(ARGS.inFile):
        print(f"{ARGS.inFile} does not exitst!")
        exit(1)

    b64 = compress()
    encodeToFormat(b64)



if __name__ == "__main__":
    main()