import base64
import os
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes


def advertisement_template():
    adv = ""
    adv += "1e"  # length (30)
    adv += "ff"  # manufacturer specific data
    adv += "4c00"  # company ID (Apple)
    adv += "1219"  # offline finding type and length
    adv += "00"  # state
    for _ in range(22):
        adv += "00"
    adv += "00"  # first two bits of key[0]
    adv += "00"  # hint
    return bytearray.fromhex(adv)


def convert_key_to_hex(private_key, public_key):
    private_key_hex = (
        private_key.private_numbers().private_value.to_bytes(28, byteorder="big").hex()
    )
    public_key_hex = public_key.public_numbers().x.to_bytes(28, byteorder="big").hex()
    return private_key_hex, public_key_hex


def generate_mac_and_payload(public_key):
    public_key_bytes = public_key.public_numbers().x.to_bytes(28, byteorder="big")
    mac = public_key_bytes[:6].hex()
    payload = advertisement_template()
    payload[7:29] = public_key_bytes[6:22]
    return mac, payload.hex()


def main():
    nkeys = int(input("Enter the number of keys to generate: "))
    prefix = input("Enter a prefix for the keyfiles (optional, press enter to skip): ")
    txt_filename = input(
        "Enter a .txt file name where to write the list of generated keys (optional, press enter to skip): "
    )
    print()

    if txt_filename:
        with open(txt_filename + ".txt", "w") as txt:
            txt.write("Generated Keys:\n")

    if not os.path.exists("keys"):
        os.makedirs("keys")

    for i in range(nkeys):
        while True:
            private_key = ec.generate_private_key(ec.SECP224R1(), default_backend())
            public_key = private_key.public_key()

            private_key_bytes = private_key.private_numbers().private_value.to_bytes(
                28, byteorder="big"
            )
            public_key_bytes = public_key.public_numbers().x.to_bytes(
                28, byteorder="big"
            )

            private_key_b64 = base64.b64encode(private_key_bytes).decode("ascii")
            public_key_b64 = base64.b64encode(public_key_bytes).decode("ascii")

            private_key_hex, public_key_hex = convert_key_to_hex(
                private_key, public_key
            )
            mac, payload = generate_mac_and_payload(public_key)

            public_key_hash = hashes.Hash(hashes.SHA256())
            public_key_hash.update(public_key_bytes)
            s256_b64 = base64.b64encode(public_key_hash.finalize()).decode("ascii")

            if "/" not in s256_b64[:7]:
                fname = (
                    f"{prefix}_{s256_b64[:7]}.keys"
                    if prefix
                    else f"{s256_b64[:7]}.keys"
                )

                with open(f"keys/{fname}", "w") as f:
                    f.write(f"Private key: {private_key_b64}\n")
                    f.write(f"Public key: {public_key_b64}\n")
                    f.write(f"Hashed adv key: {s256_b64}\n")
                    f.write(f"Private key (Hex): {private_key_hex}\n")
                    f.write(f"Public key (Hex): {public_key_hex}\n")
                    f.write(f"MAC: {mac}\n")
                    f.write(f"Payload: {payload}\n")

                if txt_filename:
                    with open(txt_filename + ".txt", "a") as txt:
                        txt.write(f"Private key: {private_key_b64}\n")
                        txt.write(f"Public key: {public_key_b64}\n")
                        txt.write(f"Hashed adv key: {s256_b64}\n")
                        txt.write(f"Private key (Hex): {private_key_hex}\n")
                        txt.write(f"Public key (Hex): {public_key_hex}\n")
                        txt.write(f"MAC: {mac}\n")
                        txt.write(f"Payload: {payload}\n")
                break


main()
