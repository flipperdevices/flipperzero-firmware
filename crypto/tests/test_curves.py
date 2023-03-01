#!/usr/bin/py.test
import binascii
import ctypes as c
import hashlib
import os
import random

import curve25519
import ecdsa
import pytest


def bytes2num(s):
    res = 0
    for i, b in enumerate(reversed(bytearray(s))):
        res += b << (i * 8)
    return res


curves = {"nist256p1": ecdsa.curves.NIST256p, "secp256k1": ecdsa.curves.SECP256k1}


class Point:
    def __init__(self, name, x, y):
        self.curve = name
        self.x = x
        self.y = y


points = [
    Point(
        "secp256k1",
        0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798,
        0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8,
    ),
    Point(
        "secp256k1",
        0x1,
        0x4218F20AE6C646B363DB68605822FB14264CA8D2587FDD6FBC750D587E76A7EE,
    ),
    Point(
        "secp256k1",
        0x2,
        0x66FBE727B2BA09E09F5A98D70A5EFCE8424C5FA425BBDA1C511F860657B8535E,
    ),
    Point(
        "secp256k1",
        0x1B,
        0x1ADCEA1CF831B0AD1653E769D1A229091D0CC68D4B0328691B9CAACC76E37C90,
    ),
    Point(
        "nist256p1",
        0x6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296,
        0x4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5,
    ),
    Point(
        "nist256p1",
        0x0,
        0x66485C780E2F83D72433BD5D84A06BB6541C2AF31DAE871728BF856A174F93F4,
    ),
    Point(
        "nist256p1",
        0x0,
        0x99B7A386F1D07C29DBCC42A27B5F9449ABE3D50DE25178E8D7407A95E8B06C0B,
    ),
    Point(
        "nist256p1",
        0xAF8BBDFE8CDD5577ACBF345B543D28CF402F4E94D3865B97EA0787F2D3AA5D22,
        0x35802B8B376B995265918B078BC109C21A535176585C40F519ACA52D6AFC147C,
    ),
    Point(
        "nist256p1",
        0x80000,
        0x580610071F440F0DCC14A22E2D5D5AFC1224C0CD11A3B4B51B8ECD2224EE1CE2,
    ),
]

random_iters = int(os.environ.get("ITERS", 1))

DIR = os.path.abspath(os.path.dirname(__file__))
lib = c.cdll.LoadLibrary(os.path.join(DIR, "libtrezor-crypto.so"))
if not lib.zkp_context_is_initialized():
    assert lib.zkp_context_init() == 0

BIGNUM = c.c_uint32 * 9


class curve_info(c.Structure):
    _fields_ = [("bip32_name", c.c_char_p), ("params", c.c_void_p)]


class curve_point(c.Structure):
    _fields_ = [("x", BIGNUM), ("y", BIGNUM)]


class ecdsa_curve(c.Structure):
    _fields_ = [
        ("prime", BIGNUM),
        ("G", curve_point),
        ("order", BIGNUM),
        ("order_half", BIGNUM),
        ("a", c.c_int),
        ("b", BIGNUM),
    ]


lib.get_curve_by_name.restype = c.POINTER(curve_info)


class Random(random.Random):
    def randbytes(self, n):
        buf = (c.c_uint8 * n)()
        for i in range(n):
            buf[i] = self.randrange(0, 256)
        return buf

    def randpoint(self, curve):
        k = self.randrange(0, curve.order)
        return k * curve.generator


def int2bn(x, bn_type=BIGNUM):
    b = bn_type()
    b._int = x
    for i in range(len(b)):
        b[i] = x % (1 << 29)
        x = x >> 29
    return b


def bn2int(b):
    x = 0
    for i in range(len(b)):
        x += b[i] << (29 * i)
    return x


@pytest.fixture(params=range(random_iters))
def r(request):
    seed = request.param
    return Random(seed + int(os.environ.get("SEED", 0)))


def get_curve_obj(name):
    curve_ptr = lib.get_curve_by_name(bytes(name, "ascii")).contents.params
    assert curve_ptr, "curve {} not found".format(name)
    curve_obj = curves[name]
    curve_obj.ptr = c.cast(curve_ptr, c.POINTER(ecdsa_curve))
    curve_obj.p = curve_obj.curve.p()  # shorthand
    return curve_obj


@pytest.fixture(params=list(sorted(curves)))
def curve(request):
    return get_curve_obj(request.param)


@pytest.fixture(params=points)
def point(request):
    name = request.param.curve
    curve_ptr = lib.get_curve_by_name(bytes(name, "ascii")).contents.params
    assert curve_ptr, "curve {} not found".format(name)
    curve_obj = curves[name]
    curve_obj.ptr = c.c_void_p(curve_ptr)
    curve_obj.p = ecdsa.ellipticcurve.Point(
        curve_obj.curve, request.param.x, request.param.y
    )
    return curve_obj


POINT = BIGNUM * 2


def to_POINT(p):
    return POINT(int2bn(p.x()), int2bn(p.y()))


def from_POINT(p):
    return (bn2int(p[0]), bn2int(p[1]))


JACOBIAN = BIGNUM * 3


def to_JACOBIAN(jp):
    return JACOBIAN(int2bn(jp[0]), int2bn(jp[1]), int2bn(jp[2]))


def from_JACOBIAN(p):
    return (bn2int(p[0]), bn2int(p[1]), bn2int(p[2]))


def test_curve_parameters(curve):
    assert curve.curve.p() == bn2int(curve.ptr.contents.prime)
    assert curve.generator.x() == bn2int(curve.ptr.contents.G.x)
    assert curve.generator.y() == bn2int(curve.ptr.contents.G.y)
    assert curve.order == bn2int(curve.ptr.contents.order)
    assert curve.order // 2 == bn2int(curve.ptr.contents.order_half)
    assert curve.curve.a() == curve.ptr.contents.a
    assert curve.curve.b() == bn2int(curve.ptr.contents.b)


def test_point_multiply(curve, r):
    p = r.randpoint(curve)
    k = r.randrange(0, 2**256)
    kp = k * p
    res = POINT(int2bn(0), int2bn(0))
    lib.point_multiply(curve.ptr, int2bn(k), to_POINT(p), res)
    res = from_POINT(res)
    assert res == (kp.x(), kp.y())


def test_point_add(curve, r):
    p1 = r.randpoint(curve)
    p2 = r.randpoint(curve)
    # print '-' * 80
    q = p1 + p2
    q1 = to_POINT(p1)
    q2 = to_POINT(p2)
    lib.point_add(curve.ptr, q1, q2)
    q_ = from_POINT(q2)
    assert q_ == (q.x(), q.y())


def test_point_double(curve, r):
    p = r.randpoint(curve)
    q = p.double()
    q_ = to_POINT(p)
    lib.point_double(curve.ptr, q_)
    q_ = from_POINT(q_)
    assert q_ == (q.x(), q.y())


def test_point_to_jacobian(curve, r):
    p = r.randpoint(curve)
    jp = JACOBIAN()
    lib.curve_to_jacobian(to_POINT(p), jp, int2bn(curve.p))
    jx, jy, jz = from_JACOBIAN(jp)
    assert jx % curve.p == (p.x() * jz**2) % curve.p
    assert jy % curve.p == (p.y() * jz**3) % curve.p

    q = POINT()
    lib.jacobian_to_curve(jp, q, int2bn(curve.p))
    q = from_POINT(q)
    assert q == (p.x(), p.y())


def test_jacobian_add(curve, r):
    p1 = r.randpoint(curve)
    p2 = r.randpoint(curve)
    prime = int2bn(curve.p)
    q = POINT()
    jp2 = JACOBIAN()
    lib.curve_to_jacobian(to_POINT(p2), jp2, prime)
    lib.point_jacobian_add(to_POINT(p1), jp2, curve.ptr)
    lib.jacobian_to_curve(jp2, q, prime)
    q = from_POINT(q)
    p_ = p1 + p2
    assert (p_.x(), p_.y()) == q


def test_jacobian_add_double(curve, r):
    p1 = r.randpoint(curve)
    p2 = p1
    prime = int2bn(curve.p)
    q = POINT()
    jp2 = JACOBIAN()
    lib.curve_to_jacobian(to_POINT(p2), jp2, prime)
    lib.point_jacobian_add(to_POINT(p1), jp2, curve.ptr)
    lib.jacobian_to_curve(jp2, q, prime)
    q = from_POINT(q)
    p_ = p1 + p2
    assert (p_.x(), p_.y()) == q


def test_jacobian_double(curve, r):
    p = r.randpoint(curve)
    p2 = p.double()
    prime = int2bn(curve.p)
    q = POINT()
    jp = JACOBIAN()
    lib.curve_to_jacobian(to_POINT(p), jp, prime)
    lib.point_jacobian_double(jp, curve.ptr)
    lib.jacobian_to_curve(jp, q, prime)
    q = from_POINT(q)
    assert (p2.x(), p2.y()) == q


def sigdecode(sig, _):
    return map(bytes2num, [sig[:32], sig[32:]])


def test_sign(curve, r):
    priv = r.randbytes(32)
    digest = r.randbytes(32)
    sig = r.randbytes(64)

    lib.ecdsa_sign_digest(curve.ptr, priv, digest, sig, c.c_void_p(0), c.c_void_p(0))

    exp = bytes2num(priv)
    sk = ecdsa.SigningKey.from_secret_exponent(exp, curve, hashfunc=hashlib.sha256)
    vk = sk.get_verifying_key()

    sig_ref = sk.sign_digest_deterministic(
        digest, hashfunc=hashlib.sha256, sigencode=ecdsa.util.sigencode_string_canonize
    )
    assert binascii.hexlify(sig) == binascii.hexlify(sig_ref)

    assert vk.verify_digest(sig, digest, sigdecode)


def test_sign_zkp(r):
    curve = get_curve_obj("secp256k1")

    priv = r.randbytes(32)
    digest = r.randbytes(32)
    sig = r.randbytes(64)

    lib.zkp_ecdsa_sign_digest(
        curve.ptr, priv, digest, sig, c.c_void_p(0), c.c_void_p(0)
    )

    exp = bytes2num(priv)
    sk = ecdsa.SigningKey.from_secret_exponent(exp, curve, hashfunc=hashlib.sha256)
    vk = sk.get_verifying_key()

    sig_ref = sk.sign_digest_deterministic(
        digest, hashfunc=hashlib.sha256, sigencode=ecdsa.util.sigencode_string_canonize
    )
    assert binascii.hexlify(sig) == binascii.hexlify(sig_ref)

    assert vk.verify_digest(sig, digest, sigdecode)


def test_validate_pubkey(curve, r):
    p = r.randpoint(curve)
    assert lib.ecdsa_validate_pubkey(curve.ptr, to_POINT(p))


def test_validate_pubkey_direct(point):
    assert lib.ecdsa_validate_pubkey(point.ptr, to_POINT(point.p))


def test_curve25519(r):
    sec1 = bytes(bytearray(r.randbytes(32)))
    sec2 = bytes(bytearray(r.randbytes(32)))
    pub1 = curve25519.Private(sec1).get_public()
    pub2 = curve25519.Private(sec2).get_public()

    session1 = r.randbytes(32)
    lib.curve25519_scalarmult(session1, sec2, pub1.public)
    session2 = r.randbytes(32)
    lib.curve25519_scalarmult(session2, sec1, pub2.public)
    assert bytearray(session1) == bytearray(session2)

    shared1 = curve25519.Private(sec2).get_shared_key(pub1, hashfunc=lambda x: x)
    shared2 = curve25519.Private(sec1).get_shared_key(pub2, hashfunc=lambda x: x)
    assert shared1 == shared2
    assert bytearray(session1) == shared1
    assert bytearray(session2) == shared2


def test_curve25519_pubkey(r):
    sec = bytes(bytearray(r.randbytes(32)))
    pub = curve25519.Private(sec).get_public()
    res = r.randbytes(32)
    lib.curve25519_scalarmult_basepoint(res, sec)
    assert bytearray(res) == pub.public


def test_curve25519_scalarmult_from_gpg(r):
    sec = binascii.unhexlify(
        "4a1e76f133afb29dbc7860bcbc16d0e829009cc15c2f81ed26de1179b1d9c938"
    )
    pub = binascii.unhexlify(
        "5d6fc75c016e85b17f54e0128a216d5f9229f25bac1ec85cecab8daf48621b31"
    )
    res = r.randbytes(32)
    lib.curve25519_scalarmult(res, sec[::-1], pub[::-1])
    expected = "a93dbdb23e5c99da743e203bd391af79f2b83fb8d0fd6ec813371c71f08f2d4d"
    assert binascii.hexlify(bytearray(res)) == bytes(expected, "ascii")
