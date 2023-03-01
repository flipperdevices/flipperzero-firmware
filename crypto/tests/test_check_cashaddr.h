#include "cash_addr.h"

static const char* valid_cashchecksum[] = {
    "prefix:x64nx6hz",
    "p:gpf8m4h7",
    "bitcoincash:qpzry9x8gf2tvdw0s3jn54khce6mua7lcw20ayyn",
    "bchtest:testnetaddress4d6njnut",
    "bchreg:555555555555555555555555555555555555555555555udxmlmrz",
};

struct valid_cashaddr_data {
  const char* legacy;
  const char* cashaddress;
};

static struct valid_cashaddr_data valid_cashaddr[] = {
    {"1BpEi6DfDAUFd7GtittLSdBeYJvcoaVggu",
     "bitcoincash:qpm2qsznhks23z7629mms6s4cwef74vcwvy22gdx6a"},
    {"1KXrWXciRDZUpQwQmuM1DbwsKDLYAYsVLR",
     "bitcoincash:qr95sy3j9xwd2ap32xkykttr4cvcu7as4y0qverfuy"},
    {"16w1D5WRVKJuZUsSRzdLp9w3YGcgoxDXb",
     "bitcoincash:qqq3728yw0y47sqn6l2na30mcw6zm78dzqre909m2r"},
    {"3CWFddi6m4ndiGyKqzYvsFYagqDLPVMTzC",
     "bitcoincash:ppm2qsznhks23z7629mms6s4cwef74vcwvn0h829pq"},
    {"3LDsS579y7sruadqu11beEJoTjdFiFCdX4",
     "bitcoincash:pr95sy3j9xwd2ap32xkykttr4cvcu7as4yc93ky28e"},
    {"31nwvkZwyPdgzjBJZXfDmSWsC4ZLKpYyUw",
     "bitcoincash:pqq3728yw0y47sqn6l2na30mcw6zm78dzq5ucqzc37"}};

START_TEST(test_cashaddr) {
  size_t i;
  for (i = 0; i < sizeof(valid_cashchecksum) / sizeof(valid_cashchecksum[0]);
       ++i) {
    uint8_t data[82];
    char rebuild[92];
    char hrp[84];
    size_t data_len;
    int res = cash_decode(hrp, data, &data_len, valid_cashchecksum[i]);
    ck_assert_int_eq(res, 1);
    res = cash_encode(rebuild, hrp, data, data_len);
    ck_assert_int_eq(res, 1);
    ck_assert_int_eq(my_strncasecmp(rebuild, valid_cashchecksum[i], 92), 0);
  }
  for (i = 0; i < sizeof(valid_cashaddr) / sizeof(valid_cashaddr[0]); ++i) {
    uint8_t prog[65];
    size_t prog_len;
    const char* hrp = "bitcoincash";
    uint8_t rawdata[65];
    size_t rawdata_len;
    char rebuild[93];
    int ret =
        cash_addr_decode(prog, &prog_len, hrp, valid_cashaddr[i].cashaddress);
    ck_assert_int_eq(ret, 1);
    ck_assert_uint_eq(prog_len, 21);
    rawdata_len = base58_decode_check(valid_cashaddr[i].legacy, HASHER_SHA2D,
                                      rawdata, sizeof(rawdata));
    ck_assert_uint_eq(rawdata_len, 21);

    int addr_type = -1;
    if (rawdata[0] == 0) {
      addr_type = 0x00;  // P2PKH
    } else if (rawdata[0] == 5) {
      addr_type = 0x08;  // P2SH
    } else {
      ck_abort();
    }
    ck_assert_uint_eq(prog[0], addr_type);

    ck_assert_int_eq(memcmp(rawdata + 1, prog + 1, 20), 0);
    ret = cash_addr_encode(rebuild, hrp, prog, 21);
    ck_assert_int_eq(ret, 1);
    ck_assert_int_eq(my_strncasecmp(rebuild, valid_cashaddr[i].cashaddress, 92),
                     0);
  }
}
END_TEST
