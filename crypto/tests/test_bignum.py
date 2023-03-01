#!/usr/bin/python
import ctypes
import itertools
import os
import random
from ctypes import (
    c_bool,
    c_char,
    c_int,
    c_size_t,
    c_uint,
    c_uint8,
    c_uint16,
    c_uint32,
    c_uint64,
)
from math import floor, log, sqrt

import pytest

dir = os.path.abspath(os.path.dirname(__file__))
lib = ctypes.cdll.LoadLibrary(os.path.join(dir, "libtrezor-crypto.so"))

limbs_number = 9
bits_per_limb = 29


@pytest.fixture()
def prime(request):
    return 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F


@pytest.fixture(params=range(limbs_number * bits_per_limb))
def bignum_bit_index(request):
    return request.param


max_decimal_digits = floor(limbs_number * bits_per_limb * log(2, 10))


@pytest.fixture(params=range(max_decimal_digits))
def bignum_decimal_digit_index(request):
    return request.param


iterations = int(os.environ.get("ITERS", 1000))


@pytest.fixture(params=range(iterations))
def r(request):
    return Random(request.param)


def implication(p, c):
    return not p or c


def uint32_p_to_int(pointer):
    return pointer.contents.value


def uint32_p():
    return ctypes.POINTER(c_int)(c_int())


limb_type = c_uint32


def bignum(limbs_number=limbs_number):
    return (limbs_number * limb_type)()


def limbs_to_bignum(limbs):
    return (limbs_number * limb_type)(*limbs)


def int_to_bignum(number, limbs_number=limbs_number):
    assert number >= 0
    assert number.bit_length() <= limbs_number * bits_per_limb

    bn = (limbs_number * limb_type)()
    for i in range(limbs_number):
        bn[i] = number % 2**bits_per_limb
        number //= 2**bits_per_limb

    return bn


def bignum_to_int(bignum, limbs_number=limbs_number):
    number = 0

    for i in reversed(range(limbs_number)):
        number *= 2**bits_per_limb
        number += bignum[i]

    return number


def raw_number():
    return (32 * c_uint8)()


def raw_number_to_integer(raw_number, endianess):
    return int.from_bytes(raw_number, endianess)


def integer_to_raw_number(number, endianess):
    return (32 * c_uint8)(*number.to_bytes(32, endianess))


def bignum_is_normalised(bignum):
    for limb in bignum:
        if limb > 2**bits_per_limb:
            return False
    return True


def number_is_partly_reduced(number, prime):
    return number < 2 * prime


def number_is_fully_reduced(number, prime):
    return number < prime


class Random(random.Random):
    def rand_int_normalized(self):
        return self.randrange(0, 2 ** (limbs_number * bits_per_limb))

    def rand_int_256(self):
        return self.randrange(0, 2**256)

    def rand_int_reduced(self, p):
        return self.randrange(0, 2 * p)

    def rand_int_bitsize(self, bitsize):
        return self.randrange(0, 2**bitsize)

    def rand_bit_index(self):
        return self.randrange(0, limbs_number * bits_per_limb)

    def rand_bignum(self, limbs_number=limbs_number):
        return (limb_type * limbs_number)(
            *[self.randrange(0, 256**4) for _ in range(limbs_number)]
        )


def assert_bn_read_be(in_number):
    raw_in_number = integer_to_raw_number(in_number, "big")
    bn_out_number = bignum()
    lib.bn_read_be(raw_in_number, bn_out_number)
    out_number = bignum_to_int(bn_out_number)

    assert bignum_is_normalised(bn_out_number)
    assert out_number == in_number


def assert_bn_read_le(in_number):
    raw_in_number = integer_to_raw_number(in_number, "little")
    bn_out_number = bignum()
    lib.bn_read_le(raw_in_number, bn_out_number)
    out_number = bignum_to_int(bn_out_number)

    assert bignum_is_normalised(bn_out_number)
    assert out_number == in_number


def assert_bn_write_be(in_number):
    bn_in_number = int_to_bignum(in_number)
    raw_out_number = raw_number()
    lib.bn_write_be(bn_in_number, raw_out_number)
    out_number = raw_number_to_integer(raw_out_number, "big")

    assert out_number == in_number


def assert_bn_write_le(in_number):
    bn_in_number = int_to_bignum(in_number)
    raw_out_number = raw_number()
    lib.bn_write_le(bn_in_number, raw_out_number)
    out_number = raw_number_to_integer(raw_out_number, "little")

    assert out_number == in_number


def assert_bn_read_uint32(x):
    bn_out_number = bignum()
    lib.bn_read_uint32(c_uint32(x), bn_out_number)
    out_number = bignum_to_int(bn_out_number)

    assert bignum_is_normalised(bn_out_number)
    assert out_number == x


def assert_bn_read_uint64(x):
    bn_out_number = bignum()
    lib.bn_read_uint64(c_uint64(x), bn_out_number)
    out_number = bignum_to_int(bn_out_number)

    assert bignum_is_normalised(bn_out_number)
    assert out_number == x


def assert_bn_bitcount(x):
    bn_x = int_to_bignum(x)
    return_value = lib.bn_bitcount(bn_x)

    assert return_value == x.bit_length()


def assert_bn_digitcount(x):
    bn_x = int_to_bignum(x)
    return_value = lib.bn_digitcount(bn_x)

    assert return_value == len(str(x))


def assert_bn_zero():
    bn_x = bignum()
    lib.bn_zero(bn_x)
    x = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x == 0


def assert_bn_one():
    bn_x = bignum()
    lib.bn_one(bn_x)
    x = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x == 1


def assert_bn_is_zero(x):
    bn_x = int_to_bignum(x)
    return_value = lib.bn_is_zero(bn_x)

    assert return_value == (x == 0)


def assert_bn_is_one(x):
    bn_x = int_to_bignum(x)
    return_value = lib.bn_is_one(bn_x)

    assert return_value == (x == 1)


def assert_bn_is_less(x, y):
    bn_x = int_to_bignum(x)
    bn_y = int_to_bignum(y)
    return_value = lib.bn_is_less(bn_x, bn_y)

    assert return_value == (x < y)


def assert_bn_is_equal(x, y):
    bn_x = int_to_bignum(x)
    bn_y = int_to_bignum(y)
    return_value = lib.bn_is_equal(bn_x, bn_y)

    assert return_value == (x == y)


def assert_bn_cmov(cond, truecase, falsecase):
    bn_res = bignum()
    bn_truecase = int_to_bignum(truecase)
    bn_falsecase = int_to_bignum(falsecase)
    lib.bn_cmov(bn_res, c_uint32(cond), bn_truecase, bn_falsecase)
    res = bignum_to_int(bn_res)

    assert res == truecase if cond else falsecase


def assert_bn_cnegate(cond, x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_cnegate(c_uint32(cond), bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_partly_reduced(x_new, prime)
    assert x_new % prime == -x_old % prime if cond else x_old % prime


def assert_bn_lshift(x_old):
    bn_x = int_to_bignum(x_old)
    lib.bn_lshift(bn_x)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x_new == (x_old << 1)


def assert_bn_rshift(x_old):
    bn_x = int_to_bignum(x_old)
    lib.bn_rshift(bn_x)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x_new == (x_old >> 1)


def assert_bn_setbit(x_old, i):
    bn_x = int_to_bignum(x_old)
    lib.bn_setbit(bn_x, c_uint16(i))
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old | (1 << i)


def assert_bn_clearbit(x_old, i):
    bn_x = int_to_bignum(x_old)
    lib.bn_clearbit(bn_x, c_uint16(i))
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old & ~(1 << i)


def assert_bn_testbit(x_old, i):
    bn_x = int_to_bignum(x_old)
    return_value = lib.bn_testbit(bn_x, c_uint16(i))

    assert return_value == x_old >> i & 1


def assert_bn_xor(x, y):
    bn_res = bignum()
    bn_x = int_to_bignum(x)
    bn_y = int_to_bignum(y)
    lib.bn_xor(bn_res, bn_x, bn_y)
    res = bignum_to_int(bn_res)

    assert res == x ^ y


def assert_bn_mult_half(x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_mult_half(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert implication(
        number_is_partly_reduced(x_old, prime), number_is_partly_reduced(x_new, prime)
    )
    assert x_new == (x_old + prime) >> 1 if x_old & 1 else x_old >> 1


def assert_bn_mult_k(x_old, k, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_mult_k(bn_x, c_uint8(k), bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_partly_reduced(x_new, prime)
    assert x_new == (x_old * k) % prime


def assert_bn_mod(x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_mod(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_fully_reduced(x_new, prime)
    assert x_new == x_old % prime


def assert_bn_multiply_long(k_old, x_old):
    bn_k = int_to_bignum(k_old)
    bn_x = int_to_bignum(x_old)
    bn_res = bignum(2 * limbs_number)
    lib.bn_multiply_long(bn_k, bn_x, bn_res)
    res = bignum_to_int(bn_res, 2 * limbs_number)

    assert res == k_old * x_old


def assert_bn_multiply_reduce_step(res_old, prime, d):
    bn_res = int_to_bignum(res_old, 2 * limbs_number)
    bn_prime = int_to_bignum(prime)
    lib.bn_multiply_reduce_step(bn_res, bn_prime, d)
    res_new = bignum_to_int(bn_res, 2 * limbs_number)

    assert bignum_is_normalised(bn_res)
    assert res_new < 2 * prime * 2 ** (d * bits_per_limb)


def assert_bn_multiply(k, x_old, prime):
    bn_k = int_to_bignum(k)
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_multiply(bn_k, bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_partly_reduced(x_new, prime)
    assert x_new == (k * x_old) % prime


def assert_bn_fast_mod(x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_fast_mod(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_partly_reduced(x_new, prime)
    assert x_new % prime == x_old % prime


def assert_bn_fast_mod_bn(bn_x, prime):
    bn_x
    x_old = bignum_to_int(bn_x)
    bn_prime = int_to_bignum(prime)
    lib.bn_fast_mod(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_partly_reduced(x_new, prime)
    assert x_new % prime == x_old % prime


def assert_bn_power_mod(x, e, prime):
    bn_x = int_to_bignum(x)
    bn_e = int_to_bignum(e)
    bn_prime = int_to_bignum(prime)
    bn_res_new = bignum()
    lib.bn_power_mod(bn_x, bn_e, bn_prime, bn_res_new)
    res_new = bignum_to_int(bn_res_new)

    assert bignum_is_normalised(bn_res_new)
    assert number_is_partly_reduced(res_new, prime)
    assert res_new % prime == pow(x, e, prime)


def assert_bn_sqrt(x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_sqrt(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_fully_reduced(x_new, prime)
    assert x_new**2 % prime == x_old % prime


def assert_inverse_mod_power_two(x, m):
    return_value = lib.inverse_mod_power_two(c_uint32(x), c_uint32(m))

    assert return_value * x % 2**m == 1


def assert_bn_divide_base(x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_divide_base(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert implication(
        number_is_fully_reduced(x_old, prime), number_is_fully_reduced(x_new, prime)
    )
    assert implication(
        number_is_partly_reduced(x_old, prime), number_is_partly_reduced(x_new, prime)
    )
    assert x_new * 2**bits_per_limb % prime == x_old % prime


def assert_bn_inverse(x_old, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_inverse(bn_x, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_fully_reduced(x_new, prime)
    assert (x_old == 0 and x_new == 0) or (x_old != 0 and (x_old * x_new) % prime == 1)


def assert_bn_normalize(bn_x):
    x_old = bignum_to_int(bn_x)
    lib.bn_normalize(bn_x)
    x_new = bignum_to_int(bn_x)

    assert x_new == x_old % 2 ** (bits_per_limb * limbs_number)
    assert bignum_is_normalised(bn_x)


def assert_bn_add(x_old, y):
    bn_x = int_to_bignum(x_old)
    bn_y = int_to_bignum(y)
    lib.bn_add(bn_x, bn_y)
    x_new = bignum_to_int(bn_x)
    y = bignum_to_int(bn_y)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old + y


def assert_bn_addmod(x_old, y, prime):
    bn_x = int_to_bignum(x_old)
    bn_y = int_to_bignum(y)
    bn_prime = int_to_bignum(prime)
    lib.bn_addmod(bn_x, bn_y, bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert number_is_partly_reduced(x_new, prime)
    assert x_new % prime == (x_old + y) % prime


def assert_bn_addi(x_old, y):
    bn_x = int_to_bignum(x_old)
    lib.bn_addi(bn_x, c_uint32(y))
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old + y


def assert_bn_subi(x_old, y, prime):
    bn_x = int_to_bignum(x_old)
    bn_prime = int_to_bignum(prime)
    lib.bn_subi(bn_x, c_uint32(y), bn_prime)
    x_new = bignum_to_int(bn_x)

    assert bignum_is_normalised(bn_x)
    assert implication(
        number_is_fully_reduced(x_old, prime), number_is_partly_reduced(x_new, prime)
    )
    assert x_new % prime == (x_old - y) % prime


def assert_bn_subtractmod(x, y, prime):
    bn_x = int_to_bignum(x)
    bn_y = int_to_bignum(y)
    bn_prime = int_to_bignum(prime)
    bn_res = bignum()
    lib.bn_subtractmod(bn_x, bn_y, bn_res, bn_prime)
    res = bignum_to_int(bn_res)

    assert bignum_is_normalised(bn_x)
    assert res % prime == (x - y) % prime


def assert_bn_subtract(x, y):
    bn_x = int_to_bignum(x)
    bn_y = int_to_bignum(y)
    bn_res = bignum()
    lib.bn_subtract(bn_x, bn_y, bn_res)
    res = bignum_to_int(bn_res)

    assert bignum_is_normalised(bn_x)
    assert res == x - y


def assert_bn_long_division(x, d):
    bn_x = int_to_bignum(x)
    bn_q = bignum()
    uint32_p_r = uint32_p()
    lib.bn_long_division(bn_x, d, bn_q, uint32_p_r)
    r = uint32_p_to_int(uint32_p_r)
    q = bignum_to_int(bn_q)

    assert bignum_is_normalised(bn_q)
    assert q == x // d
    assert r == x % d


def assert_bn_divmod58(x_old):
    bn_x = int_to_bignum(x_old)
    uint32_p_r = uint32_p()
    lib.bn_divmod58(bn_x, uint32_p_r)
    x_new = bignum_to_int(bn_x)
    r = uint32_p_to_int(uint32_p_r)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old // 58
    assert r == x_old % 58


def assert_bn_divmod1000(x_old):
    bn_x = int_to_bignum(x_old)
    uint32_p_r = uint32_p()
    lib.bn_divmod1000(bn_x, uint32_p_r)
    x_new = bignum_to_int(bn_x)
    r = uint32_p_to_int(uint32_p_r)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old // 1000
    assert r == x_old % 1000


def assert_bn_divmod10(x_old):
    bn_x = int_to_bignum(x_old)
    uint32_p_r = uint32_p()
    lib.bn_divmod10(bn_x, uint32_p_r)
    x_new = bignum_to_int(bn_x)
    r = uint32_p_to_int(uint32_p_r)

    assert bignum_is_normalised(bn_x)
    assert x_new == x_old // 10
    assert r == x_old % 10


def assert_bn_format(x, prefix, suffix, decimals, exponent, trailing, thousands):
    def format(amount, prefix, suffix, decimals, exponent, trailing, thousands):
        if exponent >= 0:
            amount *= 10**exponent
        else:
            amount //= 10 ** (-exponent)

        d = pow(10, decimals)

        integer_part = amount // d
        integer_str = f"{integer_part:,}".replace(",", thousands or "")

        if decimals:
            decimal_part = amount % d
            decimal_str = f".{decimal_part:0{decimals}d}"
            if not trailing:
                decimal_str = decimal_str.rstrip("0").rstrip(".")
        else:
            decimal_str = ""

        return prefix + integer_str + decimal_str + suffix

    def string_to_char_p(string):
        return ctypes.create_string_buffer(string.encode("ascii"))

    def char_p_to_string(pointer):
        return str(pointer.value, "ascii")

    bn_x = int_to_bignum(x)
    output_length = 100
    output = string_to_char_p("?" * output_length)
    return_value = lib.bn_format(
        bn_x,
        string_to_char_p(prefix),
        string_to_char_p(suffix),
        c_uint(decimals),
        c_int(exponent),
        c_bool(trailing),
        c_char(0),
        output,
        c_size_t(output_length),
    )

    correct_output = format(x, prefix, suffix, decimals, exponent, trailing, "")
    correct_return_value = len(correct_output)
    if len(correct_output) >= output_length:
        correct_output = ""
        correct_return_value = 0

    assert char_p_to_string(output) == correct_output
    assert return_value == correct_return_value


def test_bn_read_be(r):
    assert_bn_read_be(r.rand_int_256())


def test_bn_read_le(r):
    assert_bn_read_le(r.rand_int_256())


def test_bn_write_be(r):
    assert_bn_write_be(r.rand_int_256())


def test_bn_write_le(r):
    assert_bn_write_le(r.rand_int_256())


def test_bn_read_uint32(r):
    assert_bn_read_uint32(r.rand_int_bitsize(32))


def test_bn_read_uint64(r):
    assert_bn_read_uint64(r.rand_int_bitsize(64))


def test_bn_bitcount_1(r):
    assert_bn_bitcount(r.rand_int_normalized())


def test_bn_bitcount_2(bignum_bit_index):
    assert_bn_bitcount(2**bignum_bit_index - 1)
    assert_bn_bitcount(2**bignum_bit_index)


def test_bn_digitcount_1(r):
    assert_bn_digitcount(r.rand_int_normalized())


def test_bn_digitcount_2(bignum_decimal_digit_index):
    assert_bn_digitcount(10**bignum_decimal_digit_index - 1)
    assert_bn_digitcount(10**bignum_decimal_digit_index)


def test_bn_zero():
    assert_bn_zero()


def test_bn_one():
    assert_bn_one()


def test_bn_is_zero_1():
    assert_bn_is_zero(0)
    assert_bn_is_zero(1)


def test_bn_is_zero_2(bignum_bit_index):
    assert_bn_is_zero(2**bignum_bit_index)


def test_bn_is_one_1():
    assert_bn_is_one(0)
    assert_bn_is_one(1)


def test_bn_is_one_2(bignum_bit_index):
    assert_bn_is_one(2**bignum_bit_index)


def test_bn_is_less_1(r):
    a = r.rand_int_normalized()
    b = r.rand_int_normalized()
    assert_bn_is_less(a, a)
    assert_bn_is_less(a, b)
    assert_bn_is_less(b, a)


def test_bn_is_less_2(r):
    a = r.rand_int_normalized()
    i = r.rand_bit_index()
    b = a ^ 2**i
    assert_bn_is_less(a, b)


def test_bn_is_less_3():
    assert_bn_is_less(0, 0)
    assert_bn_is_less(1, 0)
    assert_bn_is_less(0, 1)
    assert_bn_is_less(1, 1)


def test_bn_is_equal_1(r):
    a = r.rand_int_normalized()
    b = r.rand_int_normalized()
    assert_bn_is_equal(a, a)
    assert_bn_is_equal(a, b)


def test_bn_is_equal_2():
    assert_bn_is_equal(0, 0)
    assert_bn_is_equal(1, 0)
    assert_bn_is_equal(0, 1)
    assert_bn_is_equal(1, 1)


def test_bn_cmov(r):
    a = r.rand_int_normalized()
    b = r.rand_int_normalized()
    assert_bn_cmov(0, a, b)
    assert_bn_cmov(1, a, b)


def test_bn_cnegate(r, prime):
    a = r.rand_int_reduced(prime)
    assert_bn_cnegate(0, a, prime)
    assert_bn_cnegate(1, a, prime)


def test_bn_lshift(r):
    assert_bn_lshift(r.rand_int_normalized() // 2)


def test_bn_rshift(r):
    assert_bn_rshift(r.rand_int_normalized())


def test_bn_testbit(r):
    assert_bn_testbit(r.rand_int_normalized(), r.rand_bit_index())


def test_bn_setbit(r):
    assert_bn_setbit(r.rand_int_normalized(), r.rand_bit_index())


def test_bn_clearbit(r):
    assert_bn_clearbit(r.rand_int_normalized(), r.rand_bit_index())


def test_bn_xor(r):
    assert_bn_xor(r.rand_int_normalized(), r.rand_int_normalized())


def test_bn_mult_half_1(r, prime):
    assert_bn_mult_half(r.rand_int_reduced(prime), prime)


def test_bn_mult_half_2(r, prime):
    assert_bn_mult_half(r.rand_int_normalized(), prime)


def test_bn_mult_k(r, prime):
    assert_bn_mult_k(r.rand_int_normalized(), r.randrange(9), prime)


def test_bn_mod_1(r, prime):
    assert_bn_mod(r.rand_int_reduced(prime), prime)


def test_bn_mod_2(r, prime):
    for x in [
        0,
        1,
        2,
        prime - 2,
        prime - 1,
        prime,
        prime + 1,
        prime + 2,
        2 * prime - 2,
        2 * prime - 1,
    ]:
        assert_bn_mod(x, prime)


def test_bn_multiply_long(r, prime):
    x = r.randrange(floor(sqrt(2**519)))
    k = r.randrange(floor(sqrt(2**519)))
    assert_bn_multiply_long(k, x)


def test_bn_multiply_reduce_step(r, prime):
    k = r.randrange(0, limbs_number)
    res = r.randrange(2 ** (256 + 29 * k + 31))
    assert_bn_multiply_reduce_step(res, prime, k)


def test_bn_multiply(r, prime):
    x = r.randrange(floor(sqrt(2**519)))
    k = r.randrange(floor(sqrt(2**519)))
    assert_bn_multiply(k, x, prime)


def test_bn_fast_mod_1(r, prime):
    assert_bn_fast_mod(r.rand_int_normalized(), prime)


def test_bn_fast_mod_2(r, prime):
    bn_x = r.rand_bignum()
    assert_bn_fast_mod_bn(bn_x, prime)


def test_bn_power_mod(r, prime):
    x = r.rand_int_bitsize(259)
    e = r.rand_int_normalized()
    assert_bn_power_mod(x, e, prime)


def test_bn_sqrt_1(prime):
    assert_bn_sqrt(0, prime)
    assert_bn_sqrt(1, prime)


def test_bn_sqrt_2(r, prime):
    def is_quadratic_residuum(x, p):
        return pow(x, (p - 1) // 2, p) == 1

    while True:
        x = r.rand_int_bitsize(259)
        if is_quadratic_residuum(x, prime):
            break

    assert_bn_sqrt(x, prime)


def test_inverse_mod_power_two(r):
    m = r.randrange(1, 33)
    i = r.randrange(1, 2**29, 2)
    assert_inverse_mod_power_two(i, m)


def test_bn_divide_base(r, prime):
    assert_bn_divide_base(r.rand_int_256(), prime)


def test_bn_inverse_1(prime):
    assert_bn_inverse(0, prime)
    assert_bn_inverse(1, prime)


def test_bn_inverse_2(r, prime):
    from math import gcd

    while True:
        n = r.randrange(0, prime)
        if gcd(n, prime) == 1:
            break

    assert_bn_inverse(n, prime)


def test_bn_normalize(r):
    assert_bn_normalize(r.rand_bignum())


def test_bn_add_1(r):
    assert_bn_add(r.rand_int_256(), r.rand_int_256())


def test_bn_add_2(r):
    while True:
        a = r.rand_int_normalized()
        b = r.rand_int_normalized()
        if a + b < 2 ** (limbs_number * bits_per_limb):
            break
    assert_bn_add(a, b)


def test_bn_add_3():
    a = Random().rand_int_normalized()
    b = 2 ** (limbs_number * bits_per_limb) - 1 - a
    assert_bn_add(a, b)


def test_bn_addmod(r, prime):
    assert_bn_addmod(r.rand_int_normalized(), r.rand_int_normalized(), prime)


def test_bn_addi_1(r):
    while True:
        a = r.rand_int_normalized()
        b = r.randrange(2**32 - 2**bits_per_limb + 1)
        if a + b < 2 ** (limbs_number * bits_per_limb):
            break
    assert_bn_addi(a, b)


def test_bn_addi_2():
    b = 2**32 - 2**bits_per_limb
    a = 2 ** (limbs_number * bits_per_limb) - 1 - b
    assert_bn_addi(a, b)


def test_bn_subi_1(r, prime):
    while True:
        a = r.rand_int_normalized()
        b = r.randrange(prime % 2**bits_per_limb)
        if a + prime - b < 2 ** (limbs_number * bits_per_limb):
            break
    assert_bn_subi(a, b, prime)


def test_bn_subi_2(prime):
    b = (prime % 2**bits_per_limb) - 1
    a = 2 ** (limbs_number * bits_per_limb) - 1 - prime + b
    assert_bn_subi(a, b, prime)


def test_bn_subtractmod_1(r, prime):
    assert_bn_subtractmod(r.rand_int_256(), r.rand_int_256(), prime)


def test_bn_subtractmod_2(r, prime):
    while True:
        a = r.rand_int_normalized()
        b = r.rand_int_reduced(prime)
        if a + 2 * prime - b < 2 ** (limbs_number * bits_per_limb):
            break
    assert_bn_subtractmod(a, b, prime)


def test_bn_subtractmod_3(prime):
    b = 2 * prime - 1
    a = 2 ** (limbs_number * bits_per_limb) - 1 - (2 * prime - b)
    assert_bn_subtractmod(a, b, prime)


def test_bn_subtract_1(r):
    a = r.rand_int_256()
    b = r.rand_int_256()
    if a < b:
        a, b = b, a
    assert_bn_subtract(a, b)


def test_bn_subtract_2(r):
    a = r.rand_int_normalized()
    b = r.rand_int_normalized()
    if a < b:
        a, b = b, a
    assert_bn_subtract(a, b)


def test_bn_long_division(r):
    x = r.rand_int_normalized()
    d = r.randrange(1, 61304 + 1)
    assert_bn_long_division(x, d)


def test_bn_divmod58(r):
    x = r.rand_int_normalized()
    assert_bn_divmod58(x)


def test_bn_divmod1000(r):
    x = r.rand_int_normalized()
    assert_bn_divmod1000(x)


def test_bn_divmod10(r):
    x = r.rand_int_normalized()
    assert_bn_divmod10(x)


@pytest.mark.parametrize(
    "decimals,exponent,trailing,prefix,suffix,thousands,value",
    itertools.product(
        range(0, 5),
        range(-5, 5),
        [True, False],
        ["", "prefix"],
        ["", "suffix"],
        ["", ",", " "],
        [123, 120, 123_456, 12_345, 100001, 10001000],
    ),
)
def test_bn_format(decimals, exponent, trailing, prefix, suffix, thousands, value):
    assert_bn_format(value, prefix, suffix, decimals, exponent, trailing, thousands)
