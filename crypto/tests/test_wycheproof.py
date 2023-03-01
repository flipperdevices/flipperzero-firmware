#!/usr/bin/env python
import ctypes
import json
import os
from binascii import hexlify, unhexlify

import pytest
from pyasn1.codec.ber.decoder import decode as ber_decode
from pyasn1.codec.der.decoder import decode as der_decode
from pyasn1.codec.der.encoder import encode as der_encode
from pyasn1.type import namedtype, univ


class EcSignature(univ.Sequence):
    componentType = namedtype.NamedTypes(
        namedtype.NamedType("r", univ.Integer()),
        namedtype.NamedType("s", univ.Integer()),
    )


class EcKeyInfo(univ.Sequence):
    componentType = namedtype.NamedTypes(
        namedtype.NamedType("key_type", univ.ObjectIdentifier()),
        namedtype.NamedType("curve_name", univ.ObjectIdentifier()),
    )


class EcPublicKey(univ.Sequence):
    componentType = namedtype.NamedTypes(
        namedtype.NamedType("key_info", EcKeyInfo()),
        namedtype.NamedType("public_key", univ.BitString()),
    )


class EdKeyInfo(univ.Sequence):
    componentType = namedtype.NamedTypes(
        namedtype.NamedType("key_type", univ.ObjectIdentifier())
    )


class EdPublicKey(univ.Sequence):
    componentType = namedtype.NamedTypes(
        namedtype.NamedType("key_info", EdKeyInfo()),
        namedtype.NamedType("public_key", univ.BitString()),
    )


class ParseError(Exception):
    pass


class NotSupported(Exception):
    pass


class DataError(Exception):
    pass


class curve_info(ctypes.Structure):
    _fields_ = [("bip32_name", ctypes.c_char_p), ("params", ctypes.c_void_p)]


def keys_in_dict(dictionary, keys):
    return keys <= set(dictionary.keys())


def parse_eddsa_signature(signature):
    if len(signature) != 64:
        raise ParseError("Not a valid EdDSA signature")
    return signature


def parse_ecdh256_privkey(private_key):
    if private_key < 0 or private_key.bit_length() > 256:
        raise ParseError("Not a valid 256 bit ECDH private key")
    return private_key.to_bytes(32, byteorder="big")


def parse_signed_hex(string):
    if len(string) % 2 == 1:
        string = "0" + string
    number = int(string, 16)
    if int(string[0], 16) & 8:
        return -number
    else:
        return number


def parse_result(result):
    if result == "valid":
        return True
    elif result == "invalid":
        return False
    elif result == "acceptable":
        return None
    else:
        raise DataError()


def is_valid_der(data):
    try:
        structure, _ = der_decode(data)
        return data == der_encode(structure)
    except Exception:
        return False


def parse_ed_pubkey(public_key):
    try:
        public_key, _ = ber_decode(public_key, asn1Spec=EdPublicKey())
    except Exception:
        raise ParseError("Not a BER encoded Edwards curve public key")

    if not public_key["key_info"]["key_type"] == univ.ObjectIdentifier("1.3.101.112"):
        raise ParseError("Not a BER encoded Edwards curve public key")

    public_key = bytes(public_key["public_key"].asOctets())

    return public_key


def parse_ec_pubkey(public_key):
    try:
        public_key, _ = ber_decode(public_key, asn1Spec=EcPublicKey())
    except Exception:
        raise ParseError("Not a BER encoded named elliptic curve public key")

    if not public_key["key_info"]["key_type"] == univ.ObjectIdentifier(
        "1.2.840.10045.2.1"
    ):
        raise ParseError("Not a BER encoded named elliptic curve public key")
    curve_identifier = public_key["key_info"]["curve_name"]
    curve_name = get_curve_name_by_identifier(curve_identifier)

    if curve_name is None:
        raise NotSupported(
            "Unsupported named elliptic curve: {}".format(curve_identifier)
        )

    try:
        public_key = bytes(public_key["public_key"].asOctets())
    except Exception:
        raise ParseError("Not a BER encoded named elliptic curve public key")

    return curve_name, public_key


def parse_ecdsa256_signature(signature):
    s = signature
    if not is_valid_der(signature):
        raise ParseError("Not a valid DER")
    try:
        signature, _ = der_decode(signature, asn1Spec=EcSignature())
    except Exception:
        raise ParseError("Not a valid DER encoded ECDSA signature")
    try:
        r = int(signature["r"]).to_bytes(32, byteorder="big")
        s = int(signature["s"]).to_bytes(32, byteorder="big")
        signature = r + s
    except Exception:
        raise ParseError("Not a valid DER encoded 256 bit ECDSA signature")
    return signature


def parse_digest(name):
    if name == "SHA-256":
        return 0
    else:
        raise NotSupported("Unsupported hash function: {}".format(name))


def get_curve_by_name(name):
    lib.get_curve_by_name.restype = ctypes.c_void_p
    curve = lib.get_curve_by_name(bytes(name, "ascii"))
    if curve is None:
        return None
    curve = ctypes.cast(curve, ctypes.POINTER(curve_info))
    return ctypes.c_void_p(curve.contents.params)


def parse_curve_name(name):
    if name == "secp256r1":
        return "nist256p1"
    elif name == "secp256k1":
        return "secp256k1"
    elif name == "curve25519":
        return "curve25519"
    else:
        return None


def get_curve_name_by_identifier(identifier):
    if identifier == univ.ObjectIdentifier("1.3.132.0.10"):
        return "secp256k1"
    elif identifier == univ.ObjectIdentifier("1.2.840.10045.3.1.7"):
        return "nist256p1"
    else:
        return None


def chacha_poly_encrypt(key, iv, associated_data, plaintext):
    context = bytes(context_structure_length)
    tag = bytes(16)
    ciphertext = bytes(len(plaintext))
    lib.rfc7539_init(context, key, iv)
    lib.rfc7539_auth(context, associated_data, len(associated_data))
    lib.chacha20poly1305_encrypt(context, plaintext, ciphertext, len(plaintext))
    lib.rfc7539_finish(context, len(associated_data), len(plaintext), tag)
    return ciphertext, tag


def chacha_poly_decrypt(key, iv, associated_data, ciphertext, tag):
    context = bytes(context_structure_length)
    computed_tag = bytes(16)
    plaintext = bytes(len(ciphertext))
    lib.rfc7539_init(context, key, iv)
    lib.rfc7539_auth(context, associated_data, len(associated_data))
    lib.chacha20poly1305_decrypt(context, ciphertext, plaintext, len(ciphertext))
    lib.rfc7539_finish(context, len(associated_data), len(ciphertext), computed_tag)
    return plaintext if tag == computed_tag else False


def add_pkcs_padding(data):
    padding_length = 16 - len(data) % 16
    return data + bytes([padding_length] * padding_length)


def remove_pkcs_padding(data):
    padding_length = data[-1]
    if not (
        0 < padding_length <= 16
        and data[-padding_length:] == bytes([padding_length] * padding_length)
    ):
        return False
    else:
        return data[:-padding_length]


def aes_encrypt_initialise(key, context):
    if len(key) == (128 / 8):
        lib.aes_encrypt_key128(key, context)
    elif len(key) == (192 / 8):
        lib.aes_encrypt_key192(key, context)
    elif len(key) == (256 / 8):
        lib.aes_encrypt_key256(key, context)
    else:
        raise NotSupported("Unsupported key length: {}".format(len(key) * 8))


def aes_cbc_encrypt(key, iv, plaintext):
    plaintext = add_pkcs_padding(plaintext)
    context = bytes(context_structure_length)
    ciphertext = bytes(len(plaintext))
    aes_encrypt_initialise(key, context)
    lib.aes_cbc_encrypt(
        plaintext, ciphertext, len(plaintext), bytes(bytearray(iv)), context
    )
    return ciphertext


def aes_decrypt_initialise(key, context):
    if len(key) == (128 / 8):
        lib.aes_decrypt_key128(key, context)
    elif len(key) == (192 / 8):
        lib.aes_decrypt_key192(key, context)
    elif len(key) == (256 / 8):
        lib.aes_decrypt_key256(key, context)
    else:
        raise NotSupported("Unsupported AES key length: {}".format(len(key) * 8))


def aes_cbc_decrypt(key, iv, ciphertext):
    context = bytes(context_structure_length)
    plaintext = bytes(len(ciphertext))
    aes_decrypt_initialise(key, context)
    lib.aes_cbc_decrypt(ciphertext, plaintext, len(ciphertext), iv, context)
    return remove_pkcs_padding(plaintext)


def load_json_testvectors(filename):
    try:
        result = json.loads(open(os.path.join(testvectors_directory, filename)).read())
    except Exception:
        raise DataError()
    return result


def generate_aes(filename):
    vectors = []

    data = load_json_testvectors(filename)

    if not keys_in_dict(data, {"algorithm", "testGroups"}):
        raise DataError()

    if data["algorithm"] != "AES-CBC-PKCS5":
        raise DataError()

    for test_group in data["testGroups"]:
        if not keys_in_dict(test_group, {"tests"}):
            raise DataError()

        for test in test_group["tests"]:
            if not keys_in_dict(test, {"key", "iv", "msg", "ct", "result"}):
                raise DataError()
            try:
                key = unhexlify(test["key"])
                iv = unhexlify(test["iv"])
                plaintext = unhexlify(test["msg"])
                ciphertext = unhexlify(test["ct"])
                result = parse_result(test["result"])
            except Exception:
                raise DataError()

            if len(key) not in [128 / 8, 192 / 8, 256 / 8]:
                continue

            if result is None:
                continue

            vectors.append(
                (
                    hexlify(key),
                    hexlify(iv),
                    hexlify(plaintext),
                    hexlify(ciphertext),
                    result,
                )
            )
    return vectors


def generate_chacha_poly(filename):
    vectors = []

    data = load_json_testvectors(filename)

    if not keys_in_dict(data, {"algorithm", "testGroups"}):
        raise DataError()

    if data["algorithm"] != "CHACHA20-POLY1305":
        raise DataError()

    for test_group in data["testGroups"]:
        if not keys_in_dict(test_group, {"tests"}):
            raise DataError()

        for test in test_group["tests"]:
            if not keys_in_dict(
                test, {"key", "iv", "aad", "msg", "ct", "tag", "result"}
            ):
                raise DataError()
            try:
                key = unhexlify(test["key"])
                iv = unhexlify(test["iv"])
                associated_data = unhexlify(test["aad"])
                plaintext = unhexlify(test["msg"])
                ciphertext = unhexlify(test["ct"])
                tag = unhexlify(test["tag"])
                result = parse_result(test["result"])
            except Exception:
                raise DataError()

            if result is None:
                continue

            vectors.append(
                (
                    hexlify(key),
                    hexlify(iv),
                    hexlify(associated_data),
                    hexlify(plaintext),
                    hexlify(ciphertext),
                    hexlify(tag),
                    result,
                )
            )
    return vectors


def generate_curve25519_dh(filename):
    vectors = []

    data = load_json_testvectors(filename)

    if not keys_in_dict(data, {"algorithm", "testGroups"}):
        raise DataError()

    if data["algorithm"] != "XDH":
        raise DataError()

    for test_group in data["testGroups"]:
        if not keys_in_dict(test_group, {"tests", "curve"}):
            raise DataError()

        try:
            curve_name = parse_curve_name(test_group["curve"])
        except Exception:
            raise DataError()

        for test in test_group["tests"]:
            if not keys_in_dict(test, {"public", "private", "shared", "result"}):
                raise DataError()

            try:
                public_key = unhexlify(test["public"])
                private_key = unhexlify(test["private"])
                shared = unhexlify(test["shared"])
                result = parse_result(test["result"])
            except Exception:
                raise DataError()

            if curve_name != "curve25519":
                continue
            if result is None:
                continue

            vectors.append(
                (hexlify(public_key), hexlify(private_key), hexlify(shared), result)
            )

    return vectors


def generate_ecdh(filename):
    vectors = []

    data = load_json_testvectors(filename)

    if not keys_in_dict(data, {"algorithm", "testGroups"}):
        raise DataError()

    if data["algorithm"] != "ECDH":
        raise DataError()

    for test_group in data["testGroups"]:
        if not keys_in_dict(test_group, {"tests", "curve"}):
            raise DataError()

        try:
            curve_name = parse_curve_name(test_group["curve"])
        except Exception:
            raise DataError()

        for test in test_group["tests"]:
            if not keys_in_dict(test, {"public", "private", "shared", "result"}):
                raise DataError()

            try:
                public_key = unhexlify(test["public"])
                private_key = parse_signed_hex(test["private"])
                shared = unhexlify(test["shared"])
                result = parse_result(test["result"])
            except Exception:
                raise DataError()

            try:
                private_key = parse_ecdh256_privkey(private_key)
            except ParseError:
                continue

            try:
                key_curve_name, public_key = parse_ec_pubkey(public_key)
            except NotSupported:
                continue
            except ParseError:
                continue

            if key_curve_name != curve_name:
                continue
            if result is None:
                continue

            vectors.append(
                (
                    curve_name,
                    hexlify(public_key),
                    hexlify(private_key),
                    hexlify(shared),
                    result,
                )
            )

    return vectors


def generate_ecdsa(filename):
    vectors = []

    data = load_json_testvectors(filename)

    if not keys_in_dict(data, {"algorithm", "testGroups"}):
        raise DataError()

    if data["algorithm"] != "ECDSA":
        raise DataError()

    for test_group in data["testGroups"]:
        if not keys_in_dict(test_group, {"tests", "keyDer", "sha"}):
            raise DataError()

        try:
            public_key = unhexlify(test_group["keyDer"])
        except Exception:
            raise DataError()

        try:
            curve_name, public_key = parse_ec_pubkey(public_key)
        except NotSupported:
            continue
        except ParseError:
            continue

        try:
            hasher = parse_digest(test_group["sha"])
        except NotSupported:
            continue

        for test in test_group["tests"]:
            if not keys_in_dict(test, {"sig", "msg", "result"}):
                raise DataError()

            try:
                signature = unhexlify(test["sig"])
                message = unhexlify(test["msg"])
                result = parse_result(test["result"])
            except Exception:
                raise DataError()

            if result is None:
                continue

            try:
                signature = parse_ecdsa256_signature(signature)
            except ParseError:
                continue

            vectors.append(
                (
                    curve_name,
                    hexlify(public_key),
                    hasher,
                    hexlify(message),
                    hexlify(signature),
                    result,
                )
            )

    return vectors


def generate_eddsa(filename):
    vectors = []

    data = load_json_testvectors(filename)

    if not keys_in_dict(data, {"algorithm", "testGroups"}):
        raise DataError()

    if data["algorithm"] != "EDDSA":
        raise DataError()

    for test_group in data["testGroups"]:
        if not keys_in_dict(test_group, {"tests", "keyDer"}):
            raise DataError()

        try:
            public_key = unhexlify(test_group["keyDer"])
        except Exception:
            raise DataError()

        try:
            public_key = parse_ed_pubkey(public_key)
        except ParseError:
            continue

        for test in test_group["tests"]:
            if not keys_in_dict(test, {"sig", "msg", "result"}):
                raise DataError()

            try:
                signature = unhexlify(test["sig"])
                message = unhexlify(test["msg"])
                result = parse_result(test["result"])
            except Exception:
                raise DataError()

            if result is None:
                continue

            try:
                signature = parse_eddsa_signature(signature)
            except ParseError:
                continue

            vectors.append(
                (hexlify(public_key), hexlify(message), hexlify(signature), result)
            )

    return vectors


dir = os.path.abspath(os.path.dirname(__file__))
lib = ctypes.cdll.LoadLibrary(os.path.join(dir, "libtrezor-crypto.so"))
if not lib.zkp_context_is_initialized():
    assert lib.zkp_context_init() == 0
testvectors_directory = os.path.join(dir, "wycheproof/testvectors")
context_structure_length = 1024

curve25519_dh_vectors = generate_curve25519_dh("x25519_test.json")
eddsa_vectors = generate_eddsa("eddsa_test.json")
ecdsa_vectors = (
    generate_ecdsa("ecdsa_test.json")
    + generate_ecdsa("ecdsa_secp256k1_sha256_test.json")
    + generate_ecdsa("ecdsa_secp256r1_sha256_test.json")
)
ecdh_vectors = (
    generate_ecdh("ecdh_test.json")
    + generate_ecdh("ecdh_secp256k1_test.json")
    + generate_ecdh("ecdh_secp256r1_test.json")
)
chacha_poly_vectors = generate_chacha_poly("chacha20_poly1305_test.json")
aes_vectors = generate_aes("aes_cbc_pkcs5_test.json")


@pytest.mark.parametrize("public_key, message, signature, result", eddsa_vectors)
def test_eddsa(public_key, message, signature, result):
    public_key = unhexlify(public_key)
    signature = unhexlify(signature)
    message = unhexlify(message)

    computed_result = (
        lib.ed25519_sign_open(message, len(message), public_key, signature) == 0
    )
    assert result == computed_result


@pytest.mark.parametrize(
    "curve_name, public_key, hasher, message, signature, result", ecdsa_vectors
)
def test_ecdsa(curve_name, public_key, hasher, message, signature, result):
    curve = get_curve_by_name(curve_name)
    if curve is None:
        raise NotSupported("Curve not supported: {}".format(curve_name))

    public_key = unhexlify(public_key)
    signature = unhexlify(signature)
    message = unhexlify(message)

    computed_result = (
        lib.ecdsa_verify(curve, hasher, public_key, signature, message, len(message))
        == 0
    )
    assert result == computed_result


@pytest.mark.parametrize(
    "curve_name, public_key, hasher, message, signature, result",
    filter(lambda v: v[0] == "secp256k1", ecdsa_vectors),
)
def test_ecdsa_zkp(curve_name, public_key, hasher, message, signature, result):
    curve = get_curve_by_name(curve_name)
    if curve is None:
        raise NotSupported("Curve not supported: {}".format(curve_name))

    public_key = unhexlify(public_key)
    signature = unhexlify(signature)
    message = unhexlify(message)

    computed_result = (
        lib.zkp_ecdsa_verify(
            curve, hasher, public_key, signature, message, len(message)
        )
        == 0
    )
    assert result == computed_result


@pytest.mark.parametrize(
    "public_key, private_key, shared, result", curve25519_dh_vectors
)
def test_curve25519_dh(public_key, private_key, shared, result):
    public_key = unhexlify(public_key)
    private_key = unhexlify(private_key)
    shared = unhexlify(shared)

    computed_shared = bytes([0] * 32)
    lib.curve25519_scalarmult(computed_shared, private_key, public_key)
    computed_result = shared == computed_shared
    assert result == computed_result


@pytest.mark.parametrize(
    "curve_name, public_key, private_key, shared, result", ecdh_vectors
)
def test_ecdh(curve_name, public_key, private_key, shared, result):
    curve = get_curve_by_name(curve_name)
    if curve is None:
        raise NotSupported("Curve not supported: {}".format(curve_name))

    public_key = unhexlify(public_key)
    private_key = unhexlify(private_key)
    shared = unhexlify(shared)

    computed_shared = bytes([0] * 2 * 32)
    lib.ecdh_multiply(curve, private_key, public_key, computed_shared)
    computed_shared = computed_shared[1:33]
    computed_result = shared == computed_shared
    assert result == computed_result


@pytest.mark.parametrize(
    "key, iv, associated_data, plaintext, ciphertext, tag, result", chacha_poly_vectors
)
def test_chacha_poly(key, iv, associated_data, plaintext, ciphertext, tag, result):
    key = unhexlify(key)
    iv = unhexlify(iv)
    associated_data = unhexlify(associated_data)
    plaintext = unhexlify(plaintext)
    ciphertext = unhexlify(ciphertext)
    tag = unhexlify(tag)

    computed_ciphertext, computed_tag = chacha_poly_encrypt(
        key, iv, associated_data, plaintext
    )
    computed_result = ciphertext == computed_ciphertext and tag == computed_tag
    assert result == computed_result

    computed_plaintext = chacha_poly_decrypt(key, iv, associated_data, ciphertext, tag)
    computed_result = plaintext == computed_plaintext
    assert result == computed_result


@pytest.mark.parametrize("key, iv, plaintext, ciphertext, result", aes_vectors)
def test_aes(key, iv, plaintext, ciphertext, result):
    key = unhexlify(key)
    iv = unhexlify(iv)
    plaintext = unhexlify(plaintext)
    ciphertext = unhexlify(ciphertext)

    computed_ciphertext = aes_cbc_encrypt(key, iv, plaintext)
    computed_result = ciphertext == computed_ciphertext
    assert result == computed_result

    computed_plaintext = aes_cbc_decrypt(key, bytes(iv), ciphertext)
    computed_result = plaintext == computed_plaintext
    assert result == computed_result
