// https://github.com/input-output-hk/cardano-crypto/blob/master/tests/goldens/cardano/crypto/wallet/BIP39-128
START_TEST(test_ed25519_cardano_sign_vectors) {
  ed25519_public_key public_key;
  ed25519_secret_key secret_key;
  ed25519_secret_key secret_key_extension;
  ed25519_signature signature;

  static const char *vectors[] = {
      "6065a956b1b34145c4416fdc3ba3276801850e91a77a31a7be782463288aea5"
      "3",  // private key
      "60ba6e25b1a02157fb69c5d1d7b96c4619736e545447069a6a6f0ba90844bc8"
      "e",  // private key extension
      "64b20fa082b3143d6b5eed42c6ef63f99599d0888afe060620abc1b319935fe"
      "1",  // public key
      "45b1a75fe3119e13c6f60ab9ba674b42f946fdc558e07c83dfa0751c2eba69c7"
      "9331bd8a4a975662b23628a438a0eba76367e44c12ca91b39ec59063f860f10"
      "d",  // signature

      "e7d27516538403a53a8b041656a3f570909df641a0ab811fe7d87c9ba02a830"
      "c",  // private key
      "794a2c54ad8b525b781773c87d38cbf4197636bc427a9d551368286fe4c294a"
      "4",  // private key extension
      "95bb82ffd5707716bc65170ab4e8dafeed90fbe0ce9258713b7751e962d931d"
      "f",  // public key
      "f2c9171782e7df7665126ac545ae53b05964b0160536efdb545e2460dbbec2b1"
      "9ec6b338b8f1bf4dfee94360ed024b115e37b1d7e6f3f9ae4beb79539428560"
      "f",  // signature

      "9b5a3d9a4c60bcd49bb64b72c082b164314d0f61d842f2575fd1d4fb30a28a0"
      "c",  // private key
      "b093e376f41eb7bf80abcd0073a52455d25b5d21815bc758e5f6f81536aedeb"
      "b",  // private key extension
      "79fc8154554b97e4c56ef2f9dbb4c1421ff19509688931a1e964bda5dec0f19"
      "f",  // public key
      "2ba1439ae648a7e8da7c9ab1ee6da94fd4ebe37abd0978306e8fba2afa8f111a"
      "88a993dbf008bedae9167f4f68409e4c9ddaf02cba12418447b1848907ad800"
      "f",  // signature

      "52e0c98aa600cfdcd1ff28fcda5227ed87063f4a98547a78b771052cf102b40"
      "c",  // private key
      "6c18d9f8075b1a6a1833540607479bd58b7beb8a83d2bb01ca7ae02452a2580"
      "3",  // private key extension
      "dc907c7c06e6314eedd9e18c9f6c6f9cc4e205fb1c70da608234c319f1f7b0d"
      "6",  // public key
      "0cd34f84e0d2fcb1800bdb0e869b9041349955ced66aedbe6bda187ebe8d36a6"
      "2a05b39647e92fcc42aa7a7368174240afba08b8c81f981a22f942d6bd78160"
      "2",  // signature

      "11fd6462a3a92b35c22703f6f1c124ddcf36b7c2b09cc2784f320e1cfa12ec0"
      "4",  // private key
      "c2785803c61c46aeca192a1bb1b7b20a8c4cc7fa01db57fc5d1d8a547340235"
      "2",  // private key extension
      "839775a41876e328986aa26168958bba1176e67819b357eea84afceab8b1db7"
      "8",  // public key
      "e41f73db2f8d2896a687802b2be76b7cabb73dfbb4891494883a0cbd9bbb9e5f"
      "9d3e14d2d0b06c6674333508496db660936737c0efd9511514147dac79fa490"
      "5",  // signature

      "5b1e5cad02274ba461f4708d8598d3497faf8fe3e894a379573aa6ac3a03e50"
      "5",  // private key
      "ba179d2e3c67aabb486c48d16002b51ad32eab434c738a1550962313b07098c"
      "d",  // private key extension
      "75eb8d197ec8627c85af88e66aa1e49065dd8ac98ed8991db52ece01635dfb7"
      "6",  // public key
      "631015357cee3051116b4c2ff4d1c5beb13b6e5023635aa1eeb0563cadf0d4fb"
      "c10bd5e31b4a4220c67875558c41b5cc0328104ae39cc7ff20ff0c2bda59890"
      "6",  // signature

      "624b47150f58dfa44284fbc63c9f99b9b79f808c4955a461f0e2be44eb0be50"
      "d",  // private key
      "097aa006d694b165ef37cf23562e5967c96e49255d2f20faae478dee83aa5b0"
      "2",  // private key extension
      "0588589cd9b51dfc028cf225674069cbe52e0e70deb02dc45b79b26ee3548b0"
      "0",  // public key
      "1de1d275428ba9491a433cd473cd076c027f61e7a8b5391df9dea5cb4bc88d8a"
      "57b095906a30b13e68259851a8dd3f57b6f0ffa37a5d3ffc171240f2d404f90"
      "1",  // signature

      0,
      0,
  };

  const char **test_data;
  test_data = vectors;
  while (*test_data) {
    memcpy(secret_key, fromhex(*test_data), 32);
    MARK_SECRET_DATA(secret_key, sizeof(secret_key));

    memcpy(secret_key_extension, fromhex(*(test_data + 1)), 32);
    MARK_SECRET_DATA(secret_key_extension, sizeof(secret_key_extension));

    ed25519_publickey_ext(secret_key, public_key);
    UNMARK_SECRET_DATA(public_key, sizeof(public_key));

    ck_assert_mem_eq(public_key, fromhex(*(test_data + 2)), 32);

    const uint8_t *message = (const uint8_t *)"Hello World";
    ed25519_sign_ext(message, 11, secret_key, secret_key_extension, signature);
    UNMARK_SECRET_DATA(signature, sizeof(signature));

    ck_assert_mem_eq(signature, fromhex(*(test_data + 3)), 64);

    UNMARK_SECRET_DATA(secret_key, sizeof(secret_key));
    UNMARK_SECRET_DATA(secret_key_extension, sizeof(secret_key_extension));

    test_data += 4;
  }
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_1) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  ck_assert_mem_eq(
      cardano_secret,
      fromhex(
          "08a14df748e477a69d21c97c56db151fc19e2521f31dd0ac5360f269e5b6ea46"
          "daeb991f2d2128e2525415c56a07f4366baa26c1e48572a5e073934b6de35fbc"
          "affbc325d9027c0f2d9f925b1dcf6c12bf5c1dd08904474066a4f2c00db56173"),
      96);
  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "affbc325d9027c0f2d9f925b1dcf6c12bf5c1dd08904474066a4f2c00db56173"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "08a14df748e477a69d21c97c56db151fc19e2521f31dd0ac5360f269e5b6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "daeb991f2d2128e2525415c56a07f4366baa26c1e48572a5e073934b6de35fbc"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "9a1d04808b4c0682816961cf666e82a7fd35949658aba5354c517eccf12aacb4"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_2) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "104c6a0736e501c9bfe2966ba3773f5320495b19c3f2ed222234850af2ccd5b1"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "6064bf06b2e981d7c9792b1482eeecd40ec3cfa12143f4a1f149d48ce8b6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "64aa9a16331f14c981b769efcf96addcc4c6db44047fe7a7feae0be23d33bf54"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "c651c14a13c2311fc30a7acf244add1fdac3683e7ba89b4571e4cbcab509b915"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_3) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000001);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "da99870d7e69de2a76f255ba8c7ed22428c7e5b0a8df978753c707c95ec3d4ca"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "c85fa69f4a1891fd98d1d1fc5f0cf9b1d6e44b0e6906744ab23ea766edb6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "b4fc241feffe840b8a54a26ab447f5a5caa31032db3a8091fca14f38b86ed539"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "5a5b0c92530cd366f05cf072509c806f904262c259e79a0080bbd5ee35706bb1"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_4) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);
  hdnode_private_ckd(&node, 0x80000001);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "b40c44dfd9be08591b62be7f9991c85f812d8196927f3c824d9fcb17d275089e"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "d064dcf1449d9c3e47f5b422680343561989035bf2e4e23fc34cb61fedb6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "a3071959013af95aaecf78a7a2e1b9838bbbc4864d6a8a2295243782078345cd"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "aaaca5e7adc69a03ef1f5c017ed02879e8ca871df028461ed9bf19fb8fa15038"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_5) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);
  hdnode_private_ckd(&node, 0x80000001);
  hdnode_private_ckd(&node, 0x80000002);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "2593896baf92f6ab2c0f253787ab16be0244ba95e0d48ba09da1a7fd3f926c72"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "0811b6d5d6f7120cb05d4ce5453d6ce42825c2a8e53b6d370a6b05ccf4b6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "5bebf1eea68acd04932653d944b064b10baaf5886dd73c185cc285059bf93363"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "1c87a32c5babad2fe33e0586bdc523574c6126f8368bc76598e17ea46201f980"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_6) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);
  hdnode_private_ckd(&node, 0x80000001);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0x80000002);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "fe8c6c2ab1e30385513fcffb49dcfe3e7805260425ea76b3b72b9f5bbe3b3d40"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "6019b9f5ef6ca530b657bcdb500de5455db8d51afb951fa045b6fbb3f6b6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "466332cb097934b43008701e7e27044aa56c7859019e4eba18d91a3bea23dff7"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "0b8f04755481ced76b4e5795aaafdb3cbd757c10fe60e9c58f48cf29a7ec3575"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_7) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "ring crime symptom enough erupt lady behave ramp apart settle citizen "
      "junk",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 132);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);
  hdnode_private_ckd(&node, 0x80000001);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0xBB9ACA00);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "ff77c08d37471c1d4cedd3fae2642c009324d9712492efc74dedab09c9bf973c"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "488f34840bba516f7920f91676b8681d0dd833b4ce14468e0810b255f9b6ea46"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "01eccef768a79859f824a1d3c3e35e131184e2940c3fca9a4c9b307741f65363"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "148605be54585773b44ba87e79265149ae444c4cc37cb1f8db8c08482fba293b"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_8) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "found differ bulb shadow wrist blue bind vessel deposit tip pelican "
      "action surprise weapon check fiction muscle this",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 198);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);
  hdnode_private_ckd(&node, 0x80000001);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0xBB9ACA00);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "6fb22a4531ad79e828c4907c5fff3ecf686c16cb195f81243f1f0330173380e4"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "a0baa34e4e24f0500ed6e5e90ab41984b965b7464b0b28640528778dd8a6b854"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "170e0d3b65ba8d71f27a6db60d0ac26dcb16e52e08cc259db72066f206b258d5"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "3dae0c06d87db618d73ee808425898cdd882f9eb43bf139c6b3a4760551ee89f"),
      32);
}
END_TEST

START_TEST(test_bip32_cardano_hdnode_vector_9) {
  HDNode node;

  uint8_t mnemonic_bits[66];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];
  int mnemonic_bits_len = mnemonic_to_bits(
      "balance exotic ranch knife glory slow tape favorite yard gym awake "
      "ill exist useless parent aim pig stay effort into square gasp credit "
      "butter",
      mnemonic_bits);
  ck_assert_int_eq(mnemonic_bits_len, 264);
  secret_from_entropy_cardano_icarus((const uint8_t *)"", 0, mnemonic_bits,
                                     mnemonic_bits_len / 8, cardano_secret,
                                     NULL);
  hdnode_from_secret_cardano(cardano_secret, &node);

  hdnode_private_ckd(&node, 0x80000000);
  hdnode_private_ckd(&node, 0x80000001);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0x80000002);
  hdnode_private_ckd(&node, 0xBB9ACA00);

  ck_assert_mem_eq(
      node.chain_code,
      fromhex(
          "9b226add79f90086ea18b260da633089fe121db758aa31284ad1affaf3c9bb68"),
      32);
  ck_assert_mem_eq(
      node.private_key,
      fromhex(
          "38eb2a79486e516cb6658700503a3e2c870c03e9d1aec731f780aa6fb7f7de44"),
      32);
  ck_assert_mem_eq(
      node.private_key_extension,
      fromhex(
          "80d2c677638e5dbd4395cdec279bf2a42077f2797c9e887949d37cdb317fce6a"),
      32);
  ck_assert_int_eq(hdnode_fill_public_key(&node), 0);
  ck_assert_mem_eq(
      node.public_key + 1,
      fromhex(
          "115a365b2aad1d8eba7d379de518f1fa8553855110af24e5695011c32ce9a300"),
      32);
}
END_TEST

START_TEST(test_cardano_ledger_vector_1) {
  uint8_t seed[512 / 8];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];

  const char *mnemonic =
      "recall grace sport punch exhibit mad harbor stand obey "
      "short width stem awkward used stairs wool ugly "
      "trap season stove worth toward congress jaguar";

  mnemonic_to_seed(mnemonic, "", seed, NULL);
  const int res =
      secret_from_seed_cardano_ledger(seed, sizeof(seed), cardano_secret);
  ck_assert_int_eq(res, 1);
  ck_assert_mem_eq(
      cardano_secret,
      fromhex(
          "a08cf85b564ecf3b947d8d4321fb96d70ee7bb760877e371899b14e2ccf88658"
          "104b884682b57efd97decbb318a45c05a527b9cc5c2f64f7352935a049ceea60"
          "680d52308194ccef2a18e6812b452a5815fbd7f5babc083856919aaf668fe7e4"),
      CARDANO_SECRET_LENGTH);
}
END_TEST

START_TEST(test_cardano_ledger_vector_2) {
  uint8_t seed[512 / 8];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];

  const char *mnemonic =
      "correct cherry mammal bubble want mandate polar hazard "
      "crater better craft exotic choice fun tourist census "
      "gap lottery neglect address glow carry old business";

  mnemonic_to_seed(mnemonic, "", seed, NULL);
  const int res =
      secret_from_seed_cardano_ledger(seed, sizeof(seed), cardano_secret);
  ck_assert_int_eq(res, 1);
  ck_assert_mem_eq(
      cardano_secret,
      fromhex(
          "587c6774357ecbf840d4db6404ff7af016dace0400769751ad2abfc77b9a3844"
          "cc71702520ef1a4d1b68b91187787a9b8faab0a9bb6b160de541b6ee62469901"
          "fc0beda0975fe4763beabd83b7051a5fd5cbce5b88e82c4bbaca265014e524bd"),
      CARDANO_SECRET_LENGTH);
}
END_TEST

START_TEST(test_cardano_ledger_vector_3) {
  uint8_t seed[512 / 8];
  uint8_t cardano_secret[CARDANO_SECRET_LENGTH];

  const char *mnemonic =
      "abandon abandon abandon abandon abandon abandon abandon abandon "
      "abandon abandon abandon abandon abandon abandon abandon abandon "
      "abandon abandon abandon abandon abandon abandon abandon art";

  mnemonic_to_seed(mnemonic, "foo", seed, NULL);
  const int res =
      secret_from_seed_cardano_ledger(seed, sizeof(seed), cardano_secret);
  ck_assert_int_eq(res, 1);
  ck_assert_mem_eq(
      cardano_secret,
      fromhex(
          "f053a1e752de5c26197b60f032a4809f08bb3e5d90484fe42024be31efcba757"
          "8d914d3ff992e21652fee6a4d99f6091006938fac2c0c0f9d2de0ba64b754e92"
          "a4f3723f23472077aa4cd4dd8a8a175dba07ea1852dad1cf268c61a2679c3890"),
      CARDANO_SECRET_LENGTH);
}
END_TEST
