#include "segwit_addr.h"

struct checksum_vectors_data {
  const char* str;
  bech32_encoding spec;
};

static struct checksum_vectors_data checksum_vectors[] = {
    // BIP-173 valid
    {"A12UEL5L", BECH32_ENCODING_BECH32},
    {"a12uel5l", BECH32_ENCODING_BECH32},
    {"an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcha"
     "ractersbio1tt5tgs",
     BECH32_ENCODING_BECH32},
    {"abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw", BECH32_ENCODING_BECH32},
    {"11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
     "qqqqqqqqqqqc8247j",
     BECH32_ENCODING_BECH32},
    {"split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w",
     BECH32_ENCODING_BECH32},
    {"?1ezyfcl", BECH32_ENCODING_BECH32},
    // BIP-350 valid
    {"A1LQFN3A", BECH32_ENCODING_BECH32M},
    {"a1lqfn3a", BECH32_ENCODING_BECH32M},
    {"an83characterlonghumanreadablepartthatcontainsthetheexcludedcharactersbio"
     "andnumber11sg7hg6",
     BECH32_ENCODING_BECH32M},
    {"abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx", BECH32_ENCODING_BECH32M},
    {"11lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll"
     "lllllllllllludsr8",
     BECH32_ENCODING_BECH32M},
    {"split1checkupstagehandshakeupstreamerranterredcaperredlc445v",
     BECH32_ENCODING_BECH32M},
    {"?1v759aa", BECH32_ENCODING_BECH32M},
    // BIP-173 invalid
    {" 1nwldj5", BECH32_ENCODING_NONE},
    {"\x7f"
     "1axkwrx",
     BECH32_ENCODING_NONE},
    {"\x80"
     "1eym55h",
     BECH32_ENCODING_NONE},
    {"an84characterslonghumanreadablepartthatcontainsthenumber1andtheexcludedch"
     "aractersbio1569pvx",
     BECH32_ENCODING_NONE},
    {"pzry9x0s0muk", BECH32_ENCODING_NONE},
    {"1pzry9x0s0muk", BECH32_ENCODING_NONE},
    {"x1b4n0q5v", BECH32_ENCODING_NONE},
    {"li1dgmt3", BECH32_ENCODING_NONE},
    {"de1lg7wt\xff", BECH32_ENCODING_NONE},
    {"A1G7SGD8", BECH32_ENCODING_NONE},
    {"10a06t8", BECH32_ENCODING_NONE},
    {"1qzzfhee", BECH32_ENCODING_NONE},
    // BIP-350 invalid
    {" 1xj0phk", BECH32_ENCODING_NONE},
    {"\x7f"
     "1g6xzxy",
     BECH32_ENCODING_NONE},
    {"\x80"
     "1vctc34",
     BECH32_ENCODING_NONE},
    {"an84characterslonghumanreadablepartthatcontainsthetheexcludedcharactersbi"
     "o"
     "andnumber11d6pts4",
     BECH32_ENCODING_NONE},
    {"qyrz8wqd2c9m", BECH32_ENCODING_NONE},
    {"1qyrz8wqd2c9m", BECH32_ENCODING_NONE},
    {"y1b0jsk6g", BECH32_ENCODING_NONE},
    {"lt1igcx5c0", BECH32_ENCODING_NONE},
    {"in1muywd", BECH32_ENCODING_NONE},
    {"mm1crxm3i", BECH32_ENCODING_NONE},
    {"au1s5cgom", BECH32_ENCODING_NONE},
    {"M1VUXWEZ", BECH32_ENCODING_NONE},
    {"16plkw9", BECH32_ENCODING_NONE},
    {"1p2gdwpf", BECH32_ENCODING_NONE},
};

struct valid_address_data {
  const char* address;
  size_t scriptPubKeyLen;
  const uint8_t scriptPubKey[42];
};

struct invalid_address_data {
  const char* hrp;
  int version;
  size_t program_length;
};

static struct valid_address_data valid_address[] = {
    // BIP-173
    {"BC1QW508D6QEJXTDG4Y5R3ZARVARY0C5XW7KV8F3T4",
     22,
     {0x00, 0x14, 0x75, 0x1e, 0x76, 0xe8, 0x19, 0x91, 0x96, 0xd4, 0x54,
      0x94, 0x1c, 0x45, 0xd1, 0xb3, 0xa3, 0x23, 0xf1, 0x43, 0x3b, 0xd6}},
    {"tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7",
     34,
     {0x00, 0x20, 0x18, 0x63, 0x14, 0x3c, 0x14, 0xc5, 0x16, 0x68, 0x04, 0xbd,
      0x19, 0x20, 0x33, 0x56, 0xda, 0x13, 0x6c, 0x98, 0x56, 0x78, 0xcd, 0x4d,
      0x27, 0xa1, 0xb8, 0xc6, 0x32, 0x96, 0x04, 0x90, 0x32, 0x62}},
    {"tb1qqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesrxh6hy",
     34,
     {0x00, 0x20, 0x00, 0x00, 0x00, 0xc4, 0xa5, 0xca, 0xd4, 0x62, 0x21, 0xb2,
      0xa1, 0x87, 0x90, 0x5e, 0x52, 0x66, 0x36, 0x2b, 0x99, 0xd5, 0xe9, 0x1c,
      0x6c, 0xe2, 0x4d, 0x16, 0x5d, 0xab, 0x93, 0xe8, 0x64, 0x33}},
    // BIP-350
    {"BC1QW508D6QEJXTDG4Y5R3ZARVARY0C5XW7KV8F3T4",
     22,
     {0x00, 0x14, 0x75, 0x1e, 0x76, 0xe8, 0x19, 0x91, 0x96, 0xd4, 0x54,
      0x94, 0x1c, 0x45, 0xd1, 0xb3, 0xa3, 0x23, 0xf1, 0x43, 0x3b, 0xd6}},
    {"tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7",
     34,
     {0x00, 0x20, 0x18, 0x63, 0x14, 0x3c, 0x14, 0xc5, 0x16, 0x68, 0x04, 0xbd,
      0x19, 0x20, 0x33, 0x56, 0xda, 0x13, 0x6c, 0x98, 0x56, 0x78, 0xcd, 0x4d,
      0x27, 0xa1, 0xb8, 0xc6, 0x32, 0x96, 0x04, 0x90, 0x32, 0x62}},
    {"bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7kt5nd6"
     "y",
     42,
     {0x51, 0x28, 0x75, 0x1e, 0x76, 0xe8, 0x19, 0x91, 0x96, 0xd4, 0x54,
      0x94, 0x1c, 0x45, 0xd1, 0xb3, 0xa3, 0x23, 0xf1, 0x43, 0x3b, 0xd6,
      0x75, 0x1e, 0x76, 0xe8, 0x19, 0x91, 0x96, 0xd4, 0x54, 0x94, 0x1c,
      0x45, 0xd1, 0xb3, 0xa3, 0x23, 0xf1, 0x43, 0x3b, 0xd6}},
    {"BC1SW50QGDZ25J", 4, {0x60, 0x02, 0x75, 0x1e}},
    {"bc1zw508d6qejxtdg4y5r3zarvaryvaxxpcs",
     18,
     {0x52, 0x10, 0x75, 0x1e, 0x76, 0xe8, 0x19, 0x91, 0x96, 0xd4, 0x54, 0x94,
      0x1c, 0x45, 0xd1, 0xb3, 0xa3, 0x23}},
    {"tb1qqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesrxh6hy",
     34,
     {0x00, 0x20, 0x00, 0x00, 0x00, 0xc4, 0xa5, 0xca, 0xd4, 0x62, 0x21, 0xb2,
      0xa1, 0x87, 0x90, 0x5e, 0x52, 0x66, 0x36, 0x2b, 0x99, 0xd5, 0xe9, 0x1c,
      0x6c, 0xe2, 0x4d, 0x16, 0x5d, 0xab, 0x93, 0xe8, 0x64, 0x33}},
    {"tb1pqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesf3hn0c",
     34,
     {0x51, 0x20, 0x00, 0x00, 0x00, 0xc4, 0xa5, 0xca, 0xd4, 0x62, 0x21, 0xb2,
      0xa1, 0x87, 0x90, 0x5e, 0x52, 0x66, 0x36, 0x2b, 0x99, 0xd5, 0xe9, 0x1c,
      0x6c, 0xe2, 0x4d, 0x16, 0x5d, 0xab, 0x93, 0xe8, 0x64, 0x33}},
    {"bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vqzk5jj0",
     34,
     {0x51, 0x20, 0x79, 0xbe, 0x66, 0x7e, 0xf9, 0xdc, 0xbb, 0xac, 0x55, 0xa0,
      0x62, 0x95, 0xce, 0x87, 0x0b, 0x07, 0x02, 0x9b, 0xfc, 0xdb, 0x2d, 0xce,
      0x28, 0xd9, 0x59, 0xf2, 0x81, 0x5b, 0x16, 0xf8, 0x17, 0x98}},
};

static const char* invalid_address[] = {
    // BIP-173
    "tc1qw508d6qejxtdg4y5r3zarvary0c5xw7kg3g4ty",
    "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t5",
    "BC13W508D6QEJXTDG4Y5R3ZARVARY0C5XW7KN40WF2",
    "bc1rw5uspcuh",
    "bc10w508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7kw5rljs"
    "90",
    "BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P",
    "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sL5k7",
    "bc1zw508d6qejxtdg4y5r3zarvaryvqyzf3du",
    "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3pjxtptv",
    "bc1gmk9yu",
    // BIP-350
    "tc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq5zuyut",
    "bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vqh2y7hd",
    "tb1z0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vqglt7rf",
    "BC1S0XLXVLHEMJA6C4DQV22UAPCTQUPFHLXM9H8Z3K2E72Q4K9HCZ7VQ54WELL",
    "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kemeawh",
    "tb1q0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq24jc47",
    "bc1p38j9r5y49hruaue7wxjce0updqjuyyx0kh56v8s25huc6995vvpql3jow4",
    "BC130XLXVLHEMJA6C4DQV22UAPCTQUPFHLXM9H8Z3K2E72Q4K9HCZ7VQ7ZWS8R",
    "bc1pw5dgrnzv",
    "bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7v8n0nx0muaewav253zge"
    "av",
    "BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P",
    "tb1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq47Zagq",
    "bc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7v07qwwzcrf",
    "tb1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vpggkg4j",
    "bc1gmk9yu",
};

static struct invalid_address_data invalid_address_enc[] = {
    {"BC", 0, 20}, {"bc", 0, 21}, {"bc", 17, 32}, {"bc", 1, 1}, {"bc", 16, 41},
};

static void segwit_scriptpubkey(uint8_t* scriptpubkey, size_t* scriptpubkeylen,
                                int witver, const uint8_t* witprog,
                                size_t witprog_len) {
  scriptpubkey[0] = witver ? (0x50 + witver) : 0;
  scriptpubkey[1] = witprog_len;
  memcpy(scriptpubkey + 2, witprog, witprog_len);
  *scriptpubkeylen = witprog_len + 2;
}

START_TEST(test_segwit) {
  size_t i;
  for (i = 0; i < sizeof(checksum_vectors) / sizeof(checksum_vectors[0]); ++i) {
    uint8_t data[82];
    char hrp[84];
    size_t data_len;
    bech32_encoding res =
        bech32_decode(hrp, data, &data_len, checksum_vectors[i].str);
    ck_assert_int_eq(res, checksum_vectors[i].spec);
    if (checksum_vectors[i].spec == BECH32_ENCODING_NONE) {
      continue;
    }
    // the following check makes only sense for valid addresses
    char rebuild[92];
    int resi =
        bech32_encode(rebuild, hrp, data, data_len, checksum_vectors[i].spec);
    ck_assert_int_eq(resi, 1);
    ck_assert_int_eq(my_strncasecmp(rebuild, checksum_vectors[i].str, 92), 0);
  }
  for (i = 0; i < sizeof(valid_address) / sizeof(valid_address[0]); ++i) {
    uint8_t witprog[40];
    size_t witprog_len;
    int witver;
    const char* hrp = "bc";
    if (memcmp(valid_address[i].address, "tb1", 3) == 0) {
      hrp = "tb";
    }
    uint8_t scriptpubkey[42];
    size_t scriptpubkey_len;
    char rebuild[93];
    int ret = segwit_addr_decode(&witver, witprog, &witprog_len, hrp,
                                 valid_address[i].address);
    ck_assert_int_eq(ret, 1);
    segwit_scriptpubkey(scriptpubkey, &scriptpubkey_len, witver, witprog,
                        witprog_len);
    ck_assert_uint_eq(scriptpubkey_len, valid_address[i].scriptPubKeyLen);
    ck_assert_int_eq(
        memcmp(scriptpubkey, valid_address[i].scriptPubKey, scriptpubkey_len),
        0);
    ck_assert_int_eq(
        segwit_addr_encode(rebuild, hrp, witver, witprog, witprog_len), 1);
    ck_assert_int_eq(my_strncasecmp(valid_address[i].address, rebuild, 93), 0);
  }
  for (i = 0; i < sizeof(invalid_address) / sizeof(invalid_address[0]); ++i) {
    uint8_t witprog[40];
    size_t witprog_len;
    int witver;
    int ret = segwit_addr_decode(&witver, witprog, &witprog_len, "bc",
                                 invalid_address[i]);
    ck_assert_int_eq(ret, 0);
    ret = segwit_addr_decode(&witver, witprog, &witprog_len, "tb",
                             invalid_address[i]);
    ck_assert_int_eq(ret, 0);
  }
  for (i = 0; i < sizeof(invalid_address_enc) / sizeof(invalid_address_enc[0]);
       ++i) {
    char rebuild[93];
    static const uint8_t program[42] = {0};
    int ret = segwit_addr_encode(rebuild, invalid_address_enc[i].hrp,
                                 invalid_address_enc[i].version, program,
                                 invalid_address_enc[i].program_length);
    ck_assert_int_eq(ret, 0);
  }
}
END_TEST
