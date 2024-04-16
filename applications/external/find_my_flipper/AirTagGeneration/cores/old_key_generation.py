#!/usr/bin/env python3
import base64, argparse
import os

from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes

parser = argparse.ArgumentParser()
parser.add_argument(
    "-n", "--nkeys", help="number of keys to generate", type=int, default=1
)
parser.add_argument("-p", "--prefix", help="prefix of the keyfiles")
parser.add_argument(
    "-y", "--yaml", help="yaml file where to write the list of generated keys"
)
parser.add_argument(
    "-v", "--verbose", help="print keys as they are generated", action="store_true"
)
args = parser.parse_args()

if args.yaml:
    yaml = open(args.yaml + ".yaml", "w")
    yaml.write("  keys:\n")

if not os.path.exists("keys"):
    os.makedirs("keys")

for i in range(args.nkeys):
    while True:
        private_key = ec.generate_private_key(ec.SECP224R1(), default_backend())
        public_key = private_key.public_key()

        private_key_bytes = private_key.private_numbers().private_value.to_bytes(
            28, byteorder="big"
        )
        public_key_bytes = public_key.public_numbers().x.to_bytes(28, byteorder="big")

        private_key_b64 = base64.b64encode(private_key_bytes).decode("ascii")
        public_key_b64 = base64.b64encode(public_key_bytes).decode("ascii")

        public_key_hash = hashes.Hash(hashes.SHA256())
        public_key_hash.update(public_key_bytes)
        s256_b64 = base64.b64encode(public_key_hash.finalize()).decode("ascii")

        if "/" in s256_b64[:7]:
            pass
        else:
            if args.verbose:
                print("%d)" % (i + 1))
                print("Private key: %s" % private_key_b64)
                print("Advertisement key: %s" % public_key_b64)
                print("Hashed adv key: %s" % s256_b64)

            if args.prefix:
                fname = "%s_%s.keys" % (args.prefix, s256_b64[:7])
            else:
                fname = "%s.keys" % s256_b64[:7]

            # mkdir keys

            with open(f"keys/{fname}", "w") as f:
                f.write("Private key: %s\n" % private_key_b64)
                f.write("Advertisement key: %s\n" % public_key_b64)
                f.write("Hashed adv key: %s\n" % s256_b64)

            if args.yaml:
                yaml.write('    - "%s"\n' % public_key_b64)
        break
