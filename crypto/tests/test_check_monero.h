#if USE_MONERO
START_TEST(test_xmr_base58) {
  static const struct {
    uint64_t tag;
    char *v1;
    char *v2;
  } tests[] = {
      {0x12,
       "3bec484c5d7f0246af520aab550452b5b6013733feabebd681c4a60d457b7fc12d5918e"
       "31d3c003da3c778592c07b398ad6f961a67082a75fd49394d51e69bbe",
       "43tpGG9PKbwCpjRvNLn1jwXPpnacw2uVUcszAtgmDiVcZK4VgHwjJT9BJz1WGF9eMxSYASp"
       "8yNMkuLjeQfWqJn3CNWdWfzV"},
      {0x12,
       "639050436fa36c8288706771412c5972461578d564188cd7fc6f81d6973d064fa461afe"
       "66fb23879936d7225051bebbf7f3ae0c801a90bb99fbb346b2fd4d702",
       "45PwgoUKaDHNqLL8o3okzLL7biv7GqPVmd8LTcTrYVrMEKdSYwFcyJfMLSRpfU3nh8Z2m81"
       "FJD4sUY3nXCdGe61k1HAp8T1"},
      {53,
       "5a10cca900ee47a7f412cd661b29f5ab356d6a1951884593bb170b5ec8b6f2e83b1da41"
       "1527d062c9fedeb2dad669f2f5585a00a88462b8c95c809a630e5734c",
       "9vacMKaj8JJV6MnwDzh2oNVdwTLJfTDyNRiB6NzV9TT7fqvzLivH2dB8Tv7VYR3ncn8vCb3"
       "KdNMJzQWrPAF1otYJ9cPKpkr"},
      {0x12, "", "35EMFRj"},
      {53, "d910642d8b3372fe72676dbc925277974d0401d387e4024",
       "A1PkB4pLAiVjZTJpWewswLSJor6eEYDHj35UiF7"},
  };

  uint8_t rawn[512];
  char strn[512];
  int r;
  uint64_t tag;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    const char *raw = tests[i].v1;
    const char *str = tests[i].v2;
    const size_t len = strlen(raw) / 2;

    memcpy(rawn, fromhex(raw), len);

    r = xmr_base58_addr_encode_check(tests[i].tag, rawn, len, strn,
                                     sizeof(strn));
    ck_assert_uint_eq((size_t)r, strlen(str));
    ck_assert_mem_eq(strn, str, r);

    r = xmr_base58_addr_decode_check(strn, r, &tag, rawn, len);
    ck_assert_uint_eq((size_t)r, len);
    ck_assert_mem_eq(rawn, fromhex(raw), len);
  }
}
END_TEST

START_TEST(test_xmr_getset256_modm) {
  static const struct {
    uint64_t val;
    int r;
    char *a;
  } tests[] = {
      {0x0, 1,
       "0000000000000000000000000000000000000000000000000000000000000000"},
      {0x7fffffffULL, 1,
       "ffffff7f00000000000000000000000000000000000000000000000000000000"},
      {0x7fffffffffffffffULL, 1,
       "ffffffffffffff7f000000000000000000000000000000000000000000000000"},
      {0xdeadc0deULL, 1,
       "dec0adde00000000000000000000000000000000000000000000000000000000"},
      {0x0, 0,
       "dec0adde000000000000000000000000000000000000000000000000000000ff"},
      {0x0, 0,
       "ffffffffffffffffff0000000000000000000000000000000000000000000000"},
  };

  uint8_t rawn[32];
  uint64_t v1;
  bignum256modm a1 = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    int get_res = tests[i].r;
    if (get_res) {
      set256_modm(a1, tests[i].val);
      ck_assert_int_eq(get256_modm(&v1, a1), 1);
      ck_assert(v1 == tests[i].val);

      contract256_modm(rawn, a1);
      ck_assert_mem_eq(rawn, fromhex(tests[i].a), 32);

    } else {
      expand256_modm(a1, fromhex(tests[i].a), 32);
      ck_assert_int_eq(get256_modm(&v1, a1), 0);
    }
  }
}
END_TEST

START_TEST(test_xmr_cmp256_modm) {
  static const struct {
    char *a;
    char *b;
    int res_eq;
    int res_cmp;
    int res_is_zero_a;
  } tests[] = {
      {"0000000000000000000000000000000000000000000000000000000000000000",
       "0000000000000000000000000000000000000000000000000000000000000000", 1, 0,
       1},
      {"0000000000000000000000000000000000000000000000000000000000000000",
       "0100000000000000000000000000000000000000000000000000000000000000", 0,
       -1, 1},
      {"dec0adde00000000000000000000000000000000000000000000000000000000",
       "dec0adde00000000000000000000000000000000000000000000000000000000", 1, 0,
       0},
      {"863346d8863c461cde2ec7c2759352c2b952228f33a86ca06bb79574bbe5c30d",
       "3ddbd65a6d3ba5e2ab120603685a353a27ce3fd21dfdbea7952d2dd26f1ca00a", 0, 1,
       0},
      {"f7667f392edbea6e224b1aa9fbf2a3b238b4f977fb4a8f39130cc45f49b5c40a",
       "b41b9b1e7e80be71cf290ed4bded58924086b8ac6bdfa1faa0c80c255f074d07", 0, 1,
       0},
      {"0e4005c7826de8f9978749903f40efd140e4ae6d3bed09e558fcce8367b27501",
       "0e4005c7826de8f9978749903f40efd140e4ae6d3bed09e558fcce8367b27504", 0,
       -1, 0},
      {"0e4005c7826de8f9978749903f40efd140e4ae6d3bed09e558fcce8367b27504",
       "0e4005c7826de8f9978749903f40efd140e4ae6d3bed09e558fcce8367b27504", 1, 0,
       0},
      {"0e4005c7826de8f9978749903f40efd140e4ae6d3bed09e558fcce8367b27504",
       "0e4005c7826de8f9978749903f41efd140e4ae6d3bed09e558fcce8367b27504", 0,
       -1, 0},
  };

  bignum256modm a1 = {0}, a2 = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(a1, fromhex(tests[i].a), 32);
    expand256_modm(a2, fromhex(tests[i].b), 32);

    ck_assert_int_eq(eq256_modm(a1, a2), tests[i].res_eq);
    ck_assert_int_eq(cmp256_modm(a1, a2), tests[i].res_cmp);
    ck_assert_int_eq(iszero256_modm(a1), tests[i].res_is_zero_a);
  }
}
END_TEST

START_TEST(test_xmr_copy_check_modm) {
  static const struct {
    int check;
    char *a;
  } tests[] = {
      {0, "0000000000000000000000000000000000000000000000000000000000000000"},
      {1, "ffffff7f00000000000000000000000000000000000000000000000000000000"},
      {1, "ffffffffffffff7f000000000000000000000000000000000000000000000000"},
      {1, "dec0adde00000000000000000000000000000000000000000000000000000000"},
      {0, "dec0adde000000000000000000000fffffffffffffffffffffffffffffffffff"},
  };

  bignum256modm a1 = {0}, a2 = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand_raw256_modm(a1, fromhex(tests[i].a));
    copy256_modm(a2, a1);
    ck_assert_int_eq(eq256_modm(a1, a2), 1);
    ck_assert_int_eq(check256_modm(a1), tests[i].check);
  }
}
END_TEST

START_TEST(test_xmr_mulsub256_modm) {
  static const struct {
    char *a;
    char *b;
    char *c;
    char *r;
  } tests[] = {
      {
          "713c199348cf7d14b67ae6265ea49c02c8647f07afcbcb6f8d3254b3db972e02",
          "4e48a7b7a03ab1106fdfa9441a03c97c644395a12ac4b8effac7344e0719c200",
          "1a5711b8c43bcab0161a620368d82727e1d027dc248f420d9bb4db2486c16405",
          "6edcc08aa6ec3a5b3d333b5f826be7de9c268be8aaf9521586fbcccbed3b1c0c",
      },
      {
          "d4ade2c62d34af8cfd9daec6f46bf7e57962a8aa46935cb11fab64fa599b4700",
          "22ea7989a9f4d34cd8c9442e03b5062dfe8493757cd18a63411cb1a25e44960f",
          "772053e613f0859387badcefeb7fbe551a05b00b9337539c8d72661de5929806",
          "a5063258df4520b33e97c0a46d80feeace5c251fc7ef7a938d160b8f25795106",
      },
      {
          "01fd2ef25c8221277a2b6daf1f1642bacb8d6ac0dd4f62731cdd73e26eb77900",
          "0611b9357530aa638428002769ce0ad553421e971bea1f10d7009bf26d9af805",
          "dfece232068b2f8059ca569f345baaed13ab464eb3bebb99de5625dc90a8cf03",
          "85752e62bd8085c7c02d5edeb74969d22f1a5bb34349258d2e96de300176bb07",
      },
  };

  bignum256modm a = {0}, b = {0}, c = {0}, r = {0}, r2 = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(a, fromhex(tests[i].a), 32);
    expand256_modm(b, fromhex(tests[i].b), 32);
    expand256_modm(c, fromhex(tests[i].c), 32);
    expand256_modm(r, fromhex(tests[i].r), 32);
    mulsub256_modm(r2, a, b, c);
    ck_assert_int_eq(eq256_modm(r, r2), 1);
  }
}
END_TEST

START_TEST(test_xmr_muladd256_modm) {
  static const struct {
    char *a;
    char *b;
    char *c;
    char *r;
  } tests[] = {
      {
          "7c3fd8abfbe2be3739d91679ac8dbda086961b941e0d4a00561f758927d8aa09",
          "ac2d8d37e4f344aa4040d0f0fc29d45423ab7e69ecacb94ca9fc36819e0e990e",
          "2f03f1bac09bc7d002848b68be069dc98b2db028390ae37e13a5166fcae08105",
          "dce113add3392f08e3b38b7d31e237eba5066e5a95a1fdbf755b92d05e1ec70b",
      },
      {
          "6979b70f6198d043f4b14e2069f7b89cc9f09e3465e71d472946443989e0e80c",
          "8dd5177bc8d7c5bd58c0be74b336952a73ac259ebb812ac8cd755773c6aab807",
          "d7658e508a7454ccfb29e2890d6156ac10e18ebe6e00cc5a2d2d87a5080c7f06",
          "51b33f6263772781cdbab26ef48870eaf94899894a437dac39496f15b9d0ae00",
      },
      {
          "ebfdb4eabedb1fb9a45b3204735b0511871e20358392fa16a851c519e3a29b09",
          "59d98831e9f9e24260158986c4d4035438de9b8876cc11bdcf4c364c75f72908",
          "93bce4764eee97dc67f2e37da40bc5641f2cdc637285d273287a3d4383b68f02",
          "21547ca6855c85d5adcd673b9d801d0cb0f10dced8f8b68a8c2f74163defde0e",
      },
  };

  bignum256modm a = {0}, b = {0}, c = {0}, r = {0}, r2 = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(a, fromhex(tests[i].a), 32);
    expand256_modm(b, fromhex(tests[i].b), 32);
    expand256_modm(c, fromhex(tests[i].c), 32);
    expand256_modm(r, fromhex(tests[i].r), 32);
    muladd256_modm(r2, a, b, c);
    ck_assert_int_eq(eq256_modm(r, r2), 1);
  }
}
END_TEST

START_TEST(test_xmr_curve25519_set) {
  static const struct {
    uint32_t val;
    char *a;
  } tests[] = {
      {0x0, "0000000000000000000000000000000000000000000000000000000000000000"},
      {0x1, "0100000000000000000000000000000000000000000000000000000000000000"},
      {0xdeadc0deUL,
       "dec0adde00000000000000000000000000000000000000000000000000000000"},
  };

  unsigned char buff[32];
  bignum25519 a = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    curve25519_set(a, tests[i].val);
    curve25519_contract(buff, a);
    ck_assert_mem_eq(buff, fromhex(tests[i].a), 32);
  }
}
END_TEST

START_TEST(test_xmr_curve25519_consts) {
  char *d = "a3785913ca4deb75abd841414d0a700098e879777940c78c73fe6f2bee6c0352";
  char *d2 = "59f1b226949bd6eb56b183829a14e00030d1f3eef2808e19e7fcdf56dcd90624";
  char *sqrtneg1 =
      "b0a00e4a271beec478e42fad0618432fa7d7fb3d99004d2b0bdfc14f8024832b";

  unsigned char buff[32];
  bignum25519 a = {0};

  curve25519_set_d(a);
  curve25519_contract(buff, a);
  ck_assert_mem_eq(buff, fromhex(d), 32);

  curve25519_set_2d(a);
  curve25519_contract(buff, a);
  ck_assert_mem_eq(buff, fromhex(d2), 32);

  curve25519_set_sqrtneg1(a);
  curve25519_contract(buff, a);
  ck_assert_mem_eq(buff, fromhex(sqrtneg1), 32);
}
END_TEST

START_TEST(test_xmr_curve25519_tests) {
  static const struct {
    char *a;
    int res_neg;
    int res_nonzero;
  } tests[] = {
      {
          "0000000000000000000000000000000000000000000000000000000000000000",
          0,
          0,
      },
      {
          "0100000000000000000000000000000000000000000000000000000000000000",
          1,
          1,
      },
      {
          "05737aa6100ee54283dc0d483b8e39e61846f6b3736908243d0c824d250b3139",
          1,
          1,
      },
      {
          "95587a5ef6900fa8e32d6a41bd8090b1e33e694284323d1d1f02d69865f2bc15",
          1,
          1,
      },
      {
          "02587a5ef6900fa8e32d6a41bd8090b1e33e694284323d1d1f02d69865f2bc15",
          0,
          1,
      },
  };

  bignum25519 a = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    curve25519_expand(a, fromhex(tests[i].a));
    ck_assert_int_eq(curve25519_isnegative(a), tests[i].res_neg);
    ck_assert_int_eq(curve25519_isnonzero(a), tests[i].res_nonzero);
  }
}
END_TEST

START_TEST(test_xmr_curve25519_expand_reduce) {
  static const struct {
    char *a;
    char *b;
  } tests[] = {
      {"dec0adde00000000000000000000000000000000000000000000000000000000",
       "dec0adde00000000000000000000000000000000000000000000000000000000"},
      {"95587a5ef6900fa8e32d6a41bd8090b1e33e694284323d1d1f02d69865f2bc15",
       "95587a5ef6900fa8e32d6a41bd8090b1e33e694284323d1d1f02d69865f2bc15"},
      {"95587a5ef6900fa8e32d6a41bd8090b1e33e694284323d1d1f02d69865f2bcff",
       "a8587a5ef6900fa8e32d6a41bd8090b1e33e694284323d1d1f02d69865f2bc7f"},
      {"95587a5ef6900fa8e32d6affbd8090b1e33e694284323fffff02d69865f2bcff",
       "a8587a5ef6900fa8e32d6affbd8090b1e33e694284323fffff02d69865f2bc7f"},
  };

  unsigned char buff[32];
  bignum25519 a = {0};

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    curve25519_expand_reduce(a, fromhex(tests[i].a));
    curve25519_contract(buff, a);
    ck_assert_mem_eq(buff, fromhex(tests[i].b), 32);
  }
}
END_TEST

START_TEST(test_xmr_ge25519_base) {
  unsigned char buff[32];
  char *base =
      "5866666666666666666666666666666666666666666666666666666666666666";
  ge25519 b;
  ge25519_set_base(&b);
  ge25519_pack(buff, &b);
  ck_assert_mem_eq(buff, fromhex(base), 32);
}
END_TEST

START_TEST(test_xmr_ge25519_check) {
  static const struct {
    char *x;
    char *y;
    char *z;
    char *t;
    int r;
  } tests[] = {
      {"4ff97748221f954414f836d84e8e7e207786bcd20eb67044756dca307e792c60",
       "2c7be86ab07488ba43e8e03d85a67625cfbf98c8544de4c877241b7aaafc7f63",
       "0100000000000000000000000000000000000000000000000000000000000000",
       "3ec65b03954ce7432525b9b3f4a9f5747f57b40903d1bf8892527366325fe036", 1},
      {"358fd25e4b84397d207e23cf3a75819bd6b2254cabc990b31ad63873cc38fc7c",
       "ca48045f790145a1eec3946dfd73747fde0fdb4238607e0a203f8ef5bef90e0e",
       "0100000000000000000000000000000000000000000000000000000000000000",
       "6c5e5cbae4b05e149d0aca50bf7b4112acbbe6233ace9c8bd5bcedf34df9ce0b", 1},
      {"4ff97748221f954414f836d84e8e7e207786bcd20eb6704475ffca307e792c60",
       "2c7be86ab07488ba43e8e03d85a67625cfbf98c8544de4c877241b7aaafc7f63",
       "0100000000000000000000000000000000000000000000000000000000000000",
       "3ec65b03954ce7432525b9b3f4a9f5747f57b40903d1bf8892527366325fe036", 0},
      {"358fd25e4b84397d207e23cf3a75819bd6b2254cabc990b31ad63873cc38fc7c",
       "ca48045f790145a1eec3946dfd73747fdfffdb4238607e0a203f8ef5bef90e0e",
       "0100000000000000000000000000000000000000000000000000000000000000",
       "6c5e5cbae4b05e149d0aca50bf7b4112acbbe6233ace9c8bd5bcedf34df9ce0b", 0},
      {"358fd25e4b84397d207e23cf3a75819bd6b2254cabc990b31ad63873cc38fc7c",
       "ca48045f790145a1eec3946dfd73747fdfffdb4238607e0a203f8ef5bef90e0e",
       "0100000000000000000000000000000000000000000000000000000000000000",
       "6c5e5ffae4b05e149d0aca50bf7b4112acbbe6233ace9c8bd5bcedf34df9ce0b", 0},
  };

  struct ge25519_t p;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    curve25519_expand_reduce(p.x, fromhex(tests[i].x));
    curve25519_expand_reduce(p.y, fromhex(tests[i].y));
    curve25519_expand_reduce(p.z, fromhex(tests[i].z));
    curve25519_expand_reduce(p.t, fromhex(tests[i].t));
    ck_assert_int_eq(ge25519_check(&p), tests[i].r);
  }
}
END_TEST

START_TEST(test_xmr_ge25519_scalarmult_base_wrapper) {
  static const struct {
    char *sc;
    char *pt;
  } tests[] = {
      {
          "40be740e26bd1c84f5a8fec737c0ed30e87bd45adfcd91e320f8dfb68b1a870e",
          "b7a8b2f3dbfd41b38d20aec733a316dbfc2633503799cd36f38570cafc8ea887",
      },
      {
          "1b3746add992215d427e43a58354c11ff9e6dfa1c187250938f7f9334fa41d05",
          "e2a1bfbe38a9749fe6ede79d923b778fa4c89393473d633bec01fa68617d0828",
      },
      {
          "69af25c54090a9746d3f6043348452429ffd53c1530fa114fd0055b70d61020f",
          "6bf1783b0a7495d5f6c36605dca95e723ca120a306c255084787f09b12771124",
      },
      {
          "0000000000000000000000000000000000000000000000000000000000000000",
          "0100000000000000000000000000000000000000000000000000000000000000",
      },
      {
          "0100000000000000000000000000000000000000000000000000000000000000",
          "5866666666666666666666666666666666666666666666666666666666666666",
      },
      {
          "0800000000000000000000000000000000000000000000000000000000000000",
          "b4b937fca95b2f1e93e41e62fc3c78818ff38a66096fad6e7973e5c90006d321",
      },
      {
          "ffffffffffffffff000000000000000000000000000000000000000000000000",
          "e185757a3fdc6519a6e7bebd97aa52bdc999e4c87d5c3aad0d995763ab6c6985",
      },
  };

  ge25519 pt, pt2;
  bignum256modm sc;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(sc, fromhex(tests[i].sc), 32);
    ge25519_unpack_vartime(&pt, fromhex(tests[i].pt));
    ge25519_scalarmult_base_wrapper(&pt2, sc);
    ck_assert_int_eq(ge25519_eq(&pt, &pt2), 1);
  }
}
END_TEST

START_TEST(test_xmr_ge25519_scalarmult) {
  static const struct {
    char *sc;
    char *pt;
    char *pt2;
  } tests[] = {
      {
          "0000000000000000000000000000000000000000000000000000000000000000",
          "5cbb3b2784c16f0e7eb4f2a7f93288552bb24ec51c5e01504c1e6885cfbca6d0",
          "0100000000000000000000000000000000000000000000000000000000000000",
      },
      {
          "0100000000000000000000000000000000000000000000000000000000000000",
          "f39b6770008d069acb92eb95329dec2cb0054da024e437a1bdf1ae06527deff6",
          "f39b6770008d069acb92eb95329dec2cb0054da024e437a1bdf1ae06527deff6",
      },
      {
          "3930000000000000000000000000000000000000000000000000000000000000",
          "2835b3983e3cc01a640fd188bf6bbbafbf997a3344d800eed22e4e82a412941c",
          "2fe8b2dd0f23e02fca6989e170135584d684583c0a44f6a7d3ebd964685d36c7",
      },
      {
          "ffffffffffffffff000000000000000000000000000000000000000000000000",
          "bb8af7a53a8f1b477c810e833a84cdc789a6b81a6b6417be4f97ffd9ae0fe0b8",
          "3a5c9a7dacca9dd8827881f38c36aad7d402a5efc2cab58c7553b903876e1491",
      },
      {
          "864203a09e1c788a482685c739af07355ebb2c840b7de6af87eff5f19ee3b807",
          "d404a9bbf351e7320ea6d11cdeeccaf505f706731cb5e5d839b950edb7ba6286",
          "11e09c89e0be7663e0e2d4a01fb05d6a3fd84a78a6fa4fd7daaacf2d19311a38",
      },
      {
          "3e01f05920a238e33766814d10f0c3a3e975072399ad90a823d4808db1d85209",
          "52a2d35798a0ac209b8fa194fe398b869aba5f20d80ee3d8ca77759a8e0bae0d",
          "4256addc2f036150f3fdc0a7905f01285239d6dd4eecc4be8e3b134eef4639fe",
      },
      {
          "ad63d591716a9e89a024a074bc6ce661268d1bb3665f91e8b981f189b1a49507",
          "3928bde7a92e1341c3dfee35a66fa5639204f5b9747963278af430145028648d",
          "9c959003ba91004956df98800a5024d94031db5ac659675b26350657d93c34f9",
      },
  };

  ge25519 pt, pt2, pt3;
  bignum256modm sc;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(sc, fromhex(tests[i].sc), 32);
    ge25519_unpack_vartime(&pt, fromhex(tests[i].pt));
    ge25519_unpack_vartime(&pt2, fromhex(tests[i].pt2));
    ge25519_scalarmult(&pt3, &pt, sc);
    ck_assert_int_eq(ge25519_eq(&pt3, &pt2), 1);
  }
}
END_TEST

START_TEST(test_xmr_ge25519_ops) {
  int tests[] = {1, 2, 7, 8, 637, 9912, 12345};
  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    struct ge25519_t a, b, c, d;
    bignum256modm s1 = {0}, s2 = {0}, s3 = {0}, s4 = {0};

    set256_modm(s1, tests[i]);
    set256_modm(s2, 8 * tests[i]);
    set256_modm(s3, 8);
    set256_modm(s4, 2);

    ge25519_scalarmult_base_niels(&a, ge25519_niels_base_multiples, s1);
    ge25519_scalarmult_base_niels(&b, ge25519_niels_base_multiples, s2);
    ge25519_scalarmult(&c, &a, s4);
    ge25519_scalarmult(&c, &c, s4);
    ge25519_scalarmult(&c, &c, s4);
    ck_assert_int_eq(ge25519_eq(&c, &b), 1);
    ck_assert_int_eq(ge25519_eq(&a, &b), 0);

    ge25519_scalarmult_base_wrapper(&a, s1);
    ge25519_mul8(&b, &a);
    ge25519_scalarmult_base_wrapper(&c, s2);
    ck_assert_int_eq(ge25519_eq(&b, &c), 1);

    ge25519_scalarmult(&d, &a, s3);
    ck_assert_int_eq(ge25519_eq(&d, &c), 1);

    ge25519_copy(&a, &b);
    ge25519_neg_full(&b);
    ck_assert_int_eq(ge25519_eq(&b, &c), 0);

    ge25519_add(&c, &a, &b, 0);
    set256_modm(s2, 0);
    ge25519_scalarmult_base_wrapper(&a, s2);
    ck_assert_int_eq(ge25519_eq(&a, &c), 1);
  }
}
END_TEST

START_TEST(test_xmr_check_point) {
  static const struct {
    char *p;
    bool on;
  } tests[] = {
      {"001000a93e0e6937b4feaf079e418a028ca85459aa39ac3871b94076f88ca608",
       true},
      {"54863a0464c008acc99cffb179bc6cf34eb1bbdf6c29f7a070a7c6376ae30ab5",
       true},
      {"bebe3c84092c0f7a92704cafb16562cc45c47f45e84baec8d4bba3559d1c1808",
       true},
      {"00000000000000c60073ec000000000000ff0000000000000000000000000080",
       false},
      {"00000000000000004e0000000000000000000000000000000000000000000000",
       false},
      {"0000008b0000000000000000b200000000000000000000000000000000000080",
       false},
      {"a0953eebe2f676256c37af4f6f84f32d397aaf3b73606e96c5ddfcecbb1ceec8",
       false},
      {"a82cd837efee505ec8425769ea925bee869ec3c78a57708c64c2ef2bd6ad3b88",
       false},
      {"031c56cfc99758f6f025630e77c6dea0b853c3ab0bf6cf8c8dab03d1a4618178",
       false},
  };

  ge25519 tmp;
  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    int res = ge25519_unpack_negative_vartime(&tmp, fromhex(tests[i].p));
    ck_assert_int_eq(ge25519_check(&tmp), tests[i].on);
    ck_assert_int_eq(res, tests[i].on);
  }
}
END_TEST

START_TEST(test_xmr_h) {
  char *H = "8b655970153799af2aeadc9ff1add0ea6c7251d54154cfa92c173a0dd39c1f94";
  ge25519 H2, Z;
  ge25519_p1p1 P_11;
  ge25519_pniels P_ni;
  uint8_t buff[32] = {0};

  ge25519_pack(buff, &xmr_h);
  ck_assert_mem_eq(buff, fromhex(H), 32);

  int res = ge25519_unpack_vartime(&H2, buff);
  ck_assert_int_eq(res, 1);
  ck_assert_int_eq(ge25519_eq(&xmr_h, &xmr_h), 1);
  ck_assert_int_eq(ge25519_eq(&H2, &xmr_h), 1);

  res = ge25519_unpack_negative_vartime(&H2, buff);
  ck_assert_int_eq(res, 1);
  ck_assert_int_eq(ge25519_eq(&H2, &xmr_h), 0);
  ge25519_neg_full(&H2);
  ck_assert_int_eq(ge25519_eq(&H2, &xmr_h), 1);

  ge25519_full_to_pniels(&P_ni, &xmr_h);
  ge25519_pnielsadd_p1p1(&P_11, &H2, &P_ni, 1);
  ge25519_p1p1_to_full(&H2, &P_11);
  ge25519_set_neutral(&Z);
  ck_assert_int_eq(ge25519_eq(&Z, &H2), 1);
}
END_TEST

START_TEST(test_xmr_fast_hash) {
  uint8_t hash[32];
  char tests[][2][65] = {
      {"", "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470"},
      {"00",
       "bc36789e7a1e281436464229828f817d6612f7b477d66591ff96a9e064bcc98a"},
      {"000102",
       "f84a97f1f0a956e738abd85c2e0a5026f8874e3ec09c8f012159dfeeaab2b156"},
      {"000102030405",
       "51e8babe8b42352100dffa7f7b3843c95245d3d545c6cbf5052e80258ae80627"},
      {"000102030406",
       "74e7a0111ee2390dc68269a549a76dcfb553ca1260035eae982d669ff6494f32"},
      {"000102030407",
       "3a81c5d02a87786343f88414aae150a09f6933b1d3bb660d0a9ac54e12e5cd86"},
      {"259ef2aba8feb473cf39058a0fe30b9ff6d245b42b6826687ebd6b63128aff64",
       "7fb4d1c8e32f7414fe8c7b2774ec05bff6845e4278565d17f95559513a244da2"},
      {"44caa1c26187afe8dacc5d91cb8a51282334d9308a818fe4d3607275e2a61f05",
       "2998fe52f8b9883149babd9c546912c3edfbd3cd98896a0e57b1b5929fa5ff7b"},
  };

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    xmr_fast_hash(hash, fromhex(tests[i][0]), strlen(tests[i][0]) / 2);
    ck_assert_mem_eq(hash, fromhex(tests[i][1]), 32);
  }
}
END_TEST

START_TEST(test_xmr_hasher) {
  Hasher hasher;
  uint8_t hash[32];

  static const struct {
    char *chunk[3];
    char *hash;
  } tests[] = {
      {{"00", "01", "02"},
       "f84a97f1f0a956e738abd85c2e0a5026f8874e3ec09c8f012159dfeeaab2b156"},
      {{"001122334455667788", "00", ""},
       "72a228ee8d0d01c815f112ce315cfc215a0594abcec24162304ae0ffda139d9e"},
      {{"001000a93e0e6937b4feaf079e418a028ca85459aa39ac3871b94076f88ca608", "",
        "00112233445566"},
       "c3deafd96ff10cc190c6024548c344f6401cfe5151ab2fcd40df7cc501147e01"},
  };

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    xmr_hasher_init(&hasher);
    for (int j = 0; j < 3; j++) {
      xmr_hasher_update(&hasher, fromhex(tests[i].chunk[j]),
                        strlen(tests[i].chunk[j]) / 2);
    }
    xmr_hasher_final(&hasher, hash);
    ck_assert_mem_eq(hash, fromhex(tests[i].hash), 32);
  }
}
END_TEST

START_TEST(test_xmr_hash_to_scalar) {
  bignum256modm a1;
  unsigned char out[32];
  char tests[][2][65] = {
      {"", "4a078e76cd41a3d3b534b83dc6f2ea2de500b653ca82273b7bfad8045d85a400"},
      {"00",
       "5497c9b6a7059553835f85118dc089d66512f7b477d66591ff96a9e064bcc90a"},
      {"000102",
       "5727ca206dbafa2e099b022ed528f5bdf7874e3ec09c8f012159dfeeaab2b106"},
      {"000102030405",
       "7740cf04577c107153a50b3abe44859f5245d3d545c6cbf5052e80258ae80607"},
      {"000102030406",
       "ad6bbffaceb8020543ac82bcadb9d090b553ca1260035eae982d669ff6494f02"},
      {"000102030407",
       "d2e116e9576ee5a29011c8fcb41259f99e6933b1d3bb660d0a9ac54e12e5cd06"},
      {"259ef2aba8feb473cf39058a0fe30b9ff6d245b42b6826687ebd6b63128aff64",
       "3d6d3727dc50bca39e6ccfc9c12950eef5845e4278565d17f95559513a244d02"},
      {"44caa1c26187afe8dacc5d91cb8a51282334d9308a818fe4d3607275e2a61f05",
       "aecc45c83f0408c96c70f8273e94f930edfbd3cd98896a0e57b1b5929fa5ff0b"},
  };

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    xmr_hash_to_scalar(a1, fromhex(tests[i][0]), strlen(tests[i][0]) / 2);
    contract256_modm(out, a1);
    ck_assert_mem_eq(out, fromhex(tests[i][1]), 32);
  }
}
END_TEST

START_TEST(test_xmr_hash_to_ec) {
  ge25519 p1;
  unsigned char out[32];
  char tests[][2][65] = {
      {"", "d6d7d783ab18e1be65586adb7902a4175b737ef0b902875e1d1d5c5cf0478c0b"},
      {"00",
       "8e2fecb36320bc4e192e10ef54afc7c83fbeb0c38b7debd4fea51301f0bd4f3d"},
      {"000102",
       "73b233e2e75d81b9657a857e38e7ab2bc3600e5c56622b9fe4b976ff312220fa"},
      {"000102030405",
       "bebe3c84092c0f7a92704cafb16562cc45c47f45e84baec8d4bba3559d1c1808"},
      {"000102030406",
       "525567a6a40a94f2d916bc1efea234bbd3b9162403ec2faba871a90f8d0d487e"},
      {"000102030407",
       "99b1be2a92cbd22b24b48fb7a9daadd4d13a56915c4f6ed696f271ad5bdbc149"},
      {"42f6835bf83114a1f5f6076fe79bdfa0bd67c74b88f127d54572d3910dd09201",
       "54863a0464c008acc99cffb179bc6cf34eb1bbdf6c29f7a070a7c6376ae30ab5"},
      {"44caa1c26187afe8dacc5d91cb8a51282334d9308a818fe4d3607275e2a61f05",
       "001000a93e0e6937b4feaf079e418a028ca85459aa39ac3871b94076f88ca608"},
  };

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    xmr_hash_to_ec(&p1, fromhex(tests[i][0]), strlen(tests[i][0]) / 2);
    ge25519_pack(out, &p1);
    ck_assert_mem_eq(out, fromhex(tests[i][1]), 32);
  }
}
END_TEST

START_TEST(test_xmr_derivation_to_scalar) {
  static const struct {
    char *pt;
    uint32_t idx;
    char *sc;
  } tests[] = {
      {
          "c655b2d9d2670a1c9f26f7586b6d6b1ec5173b8b33bca64c3d305a42d66738b1",
          0,
          "ca7ce31b273dd1ac00dc3553e654fb66036804800e27c826bd2b78649243900b",
      },
      {
          "2b1dbd7a007dcc4d729fa8359705595599737fcef60afb36b379fe033095dca7",
          1,
          "60afd5a63b14845d3b92d16eac386713e4ff617fdc5c1a07c3212098c1f5610c",
      },
      {
          "a48ed3797225dab4b4316b5e40107b6bd63e5f4dc517ba602774d703576ec771",
          24,
          "fe81804091e50a5c2233faa6277360fbe1948ea15dddbae62c1d40bbd1918606",
      },
      {
          "fa27b5b39741f5341b4e89269e3a05ff7e76ec7739843872468fc4bec8475410",
          65537,
          "1ba36841f57aa8b799c4dd02b39d53e5fb7780d3f09f91a57a86dcb418d8d506",
      },
  };

  ge25519 pt;
  bignum256modm sc, sc2;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(sc, fromhex(tests[i].sc), 32);
    ge25519_unpack_vartime(&pt, fromhex(tests[i].pt));

    xmr_derivation_to_scalar(sc2, &pt, tests[i].idx);
    ck_assert_int_eq(eq256_modm(sc, sc2), 1);

    xmr_derivation_to_scalar(sc2, &pt, tests[i].idx + 1);
    ck_assert_int_eq(eq256_modm(sc, sc2), 0);
  }
}
END_TEST

START_TEST(test_xmr_generate_key_derivation) {
  static const struct {
    char *pt;
    char *sc;
    char *r;
  } tests[] = {
      {
          "38f94f27c8037aff025e365275ed1029fd636dda5f69e5f98fdcf92e0a28f31a",
          "8f1c73ee5327a43264a7b60b9e7882312b582f33e89846a8694dbf094bb3a90a",
          "1fbfe4dcc8c824c274649545f297fa320cd4c1689b1d0ff4887567c4d4a75649",
      },
      {
          "26785c3941a32f194228eb659c5ee305e63868896defc50ee6c4e0e92d1e246a",
          "dbbffec4686ba8ab25e2f1b04c0e7ae51c5143c91353bfb5998430ebe365a609",
          "cca34db8dd682ec164d8973b555253934596b77849ef7709d9321121c25aba02",
      },
      {
          "43505a8ce7248f70d3aae4f57fb59c254ce2b2a0cc2bcf50f2344e51d59b36b3",
          "19a802e35f6ff94efe96ec016effe04e635bbd9c1ce2612d5ba2ee4659456b06",
          "fc6c93a93f77ff89c18b9abf95b28ec8591ab97eee8e4afee93aa766a4bd3934",
      },
  };

  ge25519 pt, pt2, pt3;
  bignum256modm sc;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(sc, fromhex(tests[i].sc), 32);
    ge25519_unpack_vartime(&pt, fromhex(tests[i].pt));
    ge25519_unpack_vartime(&pt2, fromhex(tests[i].r));
    xmr_generate_key_derivation(&pt3, &pt, sc);
    ck_assert_int_eq(ge25519_eq(&pt3, &pt2), 1);
    ck_assert_int_eq(ge25519_eq(&pt3, &pt), 0);
  }
}
END_TEST

START_TEST(test_xmr_derive_private_key) {
  static const struct {
    char *pt;
    uint32_t idx;
    char *base;
    char *r;
  } tests[] = {
      {
          "0541d8f069e5e80a892e39bbf1944ef578008cf9ecf1d100760a05858c1b709e",
          0,
          "76967eeb0a3d181bb0b384be71c680a4287599f27b2ddbd07f8e06ab6f2c880e",
          "45728c5cb658e470790f124a01699d2126832b7e5c6b7760b6f11119b96ad603",
      },
      {
          "fc6e0bd785a84e62c9ac8a97e0e604a79494bc2cf7b3b38ef8af7791c87b5bb8",
          1,
          "32fbe149562b7ccb34bc4105b87b2a834024799336c8eea5e94df77f1ae9a807",
          "64508e83bbadf63f8ecfae4d9dcdd39a4ba23508a545e1a37026f0fa2539d601",
      },
      {
          "f6bd7a72dc9444dc7e09a0eb4d312d36fe173693d6405b132a5b090297a04ea9",
          65537,
          "333a8fcce6726457e4222a87b9b475c1fcf985f756c2029fcb39184c0a5c4804",
          "37c16a22da4c0082ebf4bf807403b169f75142a9bd8560ed45f3f9347218260e",
      },
  };

  ge25519 pt;
  bignum256modm base, res, res_exp;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(base, fromhex(tests[i].base), 32);
    expand256_modm(res_exp, fromhex(tests[i].r), 32);
    ge25519_unpack_vartime(&pt, fromhex(tests[i].pt));

    xmr_derive_private_key(res, &pt, tests[i].idx, base);
    ck_assert_int_eq(eq256_modm(res, res_exp), 1);
    ck_assert_int_eq(eq256_modm(res, base), 0);
  }
}
END_TEST

START_TEST(test_xmr_derive_public_key) {
  static const struct {
    char *pt;
    uint32_t idx;
    char *base;
    char *r;
  } tests[] = {
      {
          "653f03e7766d472826aa49793bc0cfde698e6745ae5e4217980ba307739f2ed9",
          0,
          "2a393f0858732970ac8dea003b17e1ce9371f0a045bd9b7af0d998262739f4cc",
          "f7a3db27c45f265f6a68a30137ca44289a6cf1a6db2cf482c59ebfb0142ad419",
      },
      {
          "338e93f61e6470a5cc71c07b8caedd1a9a28da037aab65c1ca5538501b012c81",
          1,
          "af3a1d39397d778731c4510110fd117dc02f756e390713d58f94a06203ce39eb",
          "779e2a043c881f06aba1952741fd753098615c4fafa8f62748467ab9bac43241",
      },
      {
          "7735e9476440927b89b18d7a1e0645b218a1a6d28c642aebb16c1dba0926d5e4",
          65537,
          "62c3eed062bd602f7f2164c69ad0b5a8eb3ea560c930f6b41abfc1c4839ea432",
          "6da4ebd29498d16c4e813abb3e328c83f9b01a7ba1da6e818071f8ec563626c8",
      },
  };

  ge25519 pt, base, res, res_exp;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    ge25519_unpack_vartime(&pt, fromhex(tests[i].pt));
    ge25519_unpack_vartime(&base, fromhex(tests[i].base));
    ge25519_unpack_vartime(&res_exp, fromhex(tests[i].r));

    xmr_derive_public_key(&res, &pt, tests[i].idx, &base);

    ck_assert_int_eq(ge25519_eq(&res, &res_exp), 1);
    ck_assert_int_eq(ge25519_eq(&res, &base), 0);
  }
}
END_TEST

START_TEST(test_xmr_add_keys2) {
  static const struct {
    char *a;
    char *b;
    char *B;
    char *r;
  } tests[] = {
      {
          "631238da9578d7cb8db16fc4322671bfcb251cc5228b060664800ec1895be608",
          "f9a73fca0be058415a148f9e2871be59e1fc7ae6f6193199125237e0d7c1630f",
          "ef5ca4fc90f330e825adcdc953da0b3becd853aa819219842790bb39775f2255",
          "06623fd0e7a3d787a4d224f6ca2fdab2dcd9d1221578515974b9c4dee65fdcf5",
      },
      {
          "dac2e629e5c75c312253b19d1d3a0a423158fdd9cdcf4c7a7bf2717d0b748602",
          "0483d98d750d4977b499cefd558a0a61580823a37da2b011501e24718e6c7f0a",
          "51fd3cd2f1a603ec7be3b35da9c105d91c4304e6a63facf48d7730712cedc0ee",
          "f7a5d645ba01a5b7ccbe9636d14422bb587fc529317b23761f0e39222b783b87",
      },
      {
          "817c4d2fd3e841d860bdab6b7ccf098f3e637eca468d0a3825c50b71f61d0e0c",
          "1f6c4795d7fb0d53b5775874ac4c0963607d2b7bd11a7c5d10735badc4a27207",
          "bef0e0ed09d602bbe1dd38358b5f8fca27fcad60a69440f104441c3fc68df9c7",
          "bc0fc824d74eca0e10eacd0bc2f3322e0bcb02a44ce53f2f5f1fc472f99be8d2",
      },
  };

  bignum256modm a, b;
  ge25519 B, res, res_exp;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(a, fromhex(tests[i].a), 32);
    expand256_modm(b, fromhex(tests[i].b), 32);
    ge25519_unpack_vartime(&B, fromhex(tests[i].B));
    ge25519_unpack_vartime(&res_exp, fromhex(tests[i].r));

    xmr_add_keys2(&res, a, b, &B);
    ck_assert_int_eq(ge25519_eq(&res, &res_exp), 1);
    ck_assert_int_eq(ge25519_eq(&res, &B), 0);

    xmr_add_keys2_vartime(&res, a, b, &B);
    ck_assert_int_eq(ge25519_eq(&res, &res_exp), 1);
    ck_assert_int_eq(ge25519_eq(&res, &B), 0);
  }
}
END_TEST

START_TEST(test_xmr_add_keys3) {
  static const struct {
    char *a;
    char *A;
    char *b;
    char *B;
    char *r;
  } tests[] = {
      {
          "7048b8c4603ae194c502fa458b0e11a4c7a330852bbef66b7c1d67e9f919f509",
          "9167c5b182758699baeb421e7f1200272fc775e4c7c7c183cc47261dccbb569f",
          "c2cb2bc0249fc7be8eb9b3bed7d37aa6f2c3f433abb3a4a00b13bed64b61f30b",
          "b3ec53b07a1be70ac8d0fa365b86f0d6d4cbf98641e7704b3d684558e2ea59ef",
          "4dc016d702d599bde5eaeb2bf0c2d0d3f6b9cede961bc539bcb369c3b3086358",
      },
      {
          "e9794a6652940474958936f07f3904d514228553247633cfb7ae8ffa9fa0f406",
          "0e51cea6df2f6f56a9935689364f0d295a7c89f51d40efb2518c17d1b9db792b",
          "c132e7be08afdd93984c52c6e1c596edc6b8fc8f1faed95f55e2f819ee806706",
          "1a0e03c6858f6cf1b43f4b8456c03144af553bbbd050e152834fd1615b577cb3",
          "088f19c6727f8704373d391a36c230395d386f69edb4151ecf8afcd27793fff5",
      },
      {
          "88920b0c96b15cc04e879f53a76f85f3c7a2a5f275b2772b5b74ee83372aea00",
          "e95731ab61a98fedcded475cf21b4ecf2ef9f1adecefba8fdc476a5bb1cf60f9",
          "c86026b66c1045fb69e4f24ff6c15d4fad4d565e646938a2ffb7db37ccb4100d",
          "d80cbf2986c12e4c7ebac1e55abbdfc4212c00aec8bc90c965becf863262a074",
          "047cebaeb3ec2132e7386ba52531b04070206ba1106565c0fbd7d7280694568a",
      },
  };

  bignum256modm a, b;
  ge25519 A, B, res, res_exp;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(a, fromhex(tests[i].a), 32);
    expand256_modm(b, fromhex(tests[i].b), 32);
    ge25519_unpack_vartime(&A, fromhex(tests[i].A));
    ge25519_unpack_vartime(&B, fromhex(tests[i].B));
    ge25519_unpack_vartime(&res_exp, fromhex(tests[i].r));

    xmr_add_keys3(&res, a, &A, b, &B);
    ck_assert_int_eq(ge25519_eq(&res, &res_exp), 1);
    ck_assert_int_eq(ge25519_eq(&res, &B), 0);

    xmr_add_keys3_vartime(&res, a, &A, b, &B);
    ck_assert_int_eq(ge25519_eq(&res, &res_exp), 1);
    ck_assert_int_eq(ge25519_eq(&res, &B), 0);
  }
}
END_TEST

START_TEST(test_xmr_get_subaddress_secret_key) {
  static const struct {
    uint32_t major, minor;
    char *m;
    char *r;
  } tests[] = {
      {
          0,
          0,
          "36fad9f7bff465c15a755f1482fb2ecc3a4e434303df906882234e42b5813207",
          "8a510a9fe1824b49abbae05958084f9c9098775f29e15427309177882471cf01",
      },
      {
          0,
          1,
          "36fad9f7bff465c15a755f1482fb2ecc3a4e434303df906882234e42b5813207",
          "2bbc9366c04abb0523e2b2d6e709670ffe6645bacedfee968d9c6bc8eefe9c0f",
      },
      {
          100,
          100,
          "36fad9f7bff465c15a755f1482fb2ecc3a4e434303df906882234e42b5813207",
          "c3837d41fedeaed126cf4fc1a5ea47b8b7f38f6a64aa534e3dd45a3c93f37600",
      },
  };

  bignum256modm m, res, res_exp;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(m, fromhex(tests[i].m), 32);
    expand256_modm(res_exp, fromhex(tests[i].r), 32);
    xmr_get_subaddress_secret_key(res, tests[i].major, tests[i].minor, m);

    ck_assert_int_eq(eq256_modm(res, res_exp), 1);
    ck_assert_int_eq(eq256_modm(res, m), 0);
  }
}
END_TEST

START_TEST(test_xmr_gen_c) {
  static const struct {
    char *a;
    uint64_t amount;
    char *r;
  } tests[] = {
      {
          "e3e6558c291bbb98aa691d068b67d59dc520afb23fdd51bf65283626fc2ad903",
          0,
          "ef19d73bdf3749240b80ee7695f53ad7c2fc2cf868a93209799f41212d099750",
      },
      {
          "6788c9579c377f3228680bd0e6d01b1ee0c763b35ed39d36fa2146cc2ee16e0e",
          1,
          "4913b9af4f2725d87a4404c22cf366597d1c1e6a1f510ae14081d8b7c5a9de77",
      },
      {
          "ad9e89d67012935540427c241756d6a9d260c5e134603c41d31e24f8651bef08",
          65537,
          "f005721da08f24e68314abed3ddfd94165e4be3813398fb126e3f366820b9c90",
      },
      {
          "fdbb70ff07be24d98de3bffa0a33756646497224318fb7fe136f0e7789d12607",
          0xffffffffffffffffULL,
          "a9c38927f299c5f14c98a1a9c9981e59c606ff597274b9b709e1356f12e1498c",
      },
  };

  bignum256modm a;
  ge25519 res, res_exp;

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    expand256_modm(a, fromhex(tests[i].a), 32);
    ge25519_unpack_vartime(&res_exp, fromhex(tests[i].r));
    xmr_gen_c(&res, a, tests[i].amount);

    ck_assert_int_eq(ge25519_eq(&res, &res_exp), 1);
  }
}
END_TEST

START_TEST(test_xmr_varint) {
  static const struct {
    uint64_t x;
    char *r;
  } tests[] = {
      {
          0,
          "00",
      },
      {
          24,
          "18",
      },
      {
          65535,
          "ffff03",
      },
      {
          65537,
          "818004",
      },
      {
          0x7fffffffULL,
          "ffffffff07",
      },
      {
          0xffffffffULL,
          "ffffffff0f",
      },
      {
          0xffffffffffffffffULL,
          "ffffffffffffffffff01",
      },
      {
          0xdeadc0deULL,
          "de81b7f50d",
      },
  };

  uint64_t val;
  unsigned char buff[64];

  for (size_t i = 0; i < (sizeof(tests) / sizeof(*tests)); i++) {
    int s1 = xmr_size_varint(tests[i].x);
    int written = 0;
    int read = 0;

    ck_assert_uint_eq((size_t)s1, strlen(tests[i].r) / 2);
    written = xmr_write_varint(buff, sizeof(buff), tests[i].x);
    ck_assert_int_eq(s1, written);
    ck_assert_mem_eq(buff, fromhex(tests[i].r), strlen(tests[i].r) / 2);

    read = xmr_read_varint(buff, sizeof(buff), &val);
    ck_assert_int_eq(read, written);
    ck_assert(tests[i].x == val);
  }
}
END_TEST
#endif
