/*!
    \ingroup Signature 
    \brief  この関数は、結果のシグネチャの最大サイズを返します。
    \return Returns  sig_type_e sig_typeがサポートされていない場合sig_typeが無効な場合はbad_func_argを返します。正の戻り値は、署名の最大サイズを示します。
    \param sig_type  wc_signature_type_eccまたはwc_signature_type_rsaなどの署名型列挙型値。
    \param key  ECC_KEYやRSAKEYなどのキー構造へのポインタ。
    _Example_
    \code
    // Get signature length
    enum wc_SignatureType sig_type = WC_SIGNATURE_TYPE_ECC;
    ecc_key eccKey;
    word32 sigLen;
    wc_ecc_init(&eccKey);
    sigLen = wc_SignatureGetSize(sig_type, &eccKey, sizeof(eccKey));
    if (sigLen > 0) {
    	// Success
    }
    \endcode
    \sa wc_HashGetDigestSize
    \sa wc_SignatureGenerate
    \sa wc_SignatureVerify
*/
int wc_SignatureGetSize(enum wc_SignatureType sig_type,
    const void* key, word32 key_len);

/*!
    \ingroup Signature 
    \brief  この関数は、データをハッシュし、結果のハッシュとキーを使用して署名を使用して署名を使用して署名を検証します。
    \return 0  成功
    \return SIG_TYPE_E  -231、署名タイプが有効/利用可能です
    \return BAD_FUNC_ARG  -173、関数の不良引数が提供されています
    \return BUFFER_E  -132、出力バッファが小さすぎたり入力が大きすぎたりします。
    \param hash_type  "wc_hash_type_sha256"などの "enum wc_hashtype"からのハッシュ型。
    \param sig_type  wc_signature_type_eccまたはwc_signature_type_rsaなどの署名型列挙型値。
    \param data  ハッシュへのデータを含むバッファへのポインタ。
    \param data_len  データバッファの長さ。
    \param sig  署名を出力するためのバッファへのポインタ。
    \param sig_len  シグネチャ出力バッファの長さ。
    \param key  ECC_KEYやRSAKEYなどのキー構造へのポインタ。
    _Example_
    \code
    int ret;
    ecc_key eccKey;

    // Import the public key
    wc_ecc_init(&eccKey);
    ret = wc_ecc_import_x963(eccPubKeyBuf, eccPubKeyLen, &eccKey);
    // Perform signature verification using public key
    ret = wc_SignatureVerify(
    WC_HASH_TYPE_SHA256, WC_SIGNATURE_TYPE_ECC,
    fileBuf, fileLen,
    sigBuf, sigLen,
    &eccKey, sizeof(eccKey));
    printf("Signature Verification: %s
    (%d)\n", (ret == 0) ? "Pass" : "Fail", ret);
    wc_ecc_free(&eccKey);
    \endcode
    \sa wc_SignatureGetSize
    \sa wc_SignatureGenerate
*/
int wc_SignatureVerify(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    const byte* sig, word32 sig_len,
    const void* key, word32 key_len);

/*!
    \ingroup Signature 
    \brief  この関数は、キーを使用してデータから署名を生成します。まずデータのハッシュを作成し、キーを使用してハッシュに署名します。
    \return 0  成功
    \return SIG_TYPE_E  -231、署名タイプが有効/利用可能です
    \return BAD_FUNC_ARG  -173、関数の不良引数が提供されています
    \return BUFFER_E  -132、出力バッファが小さすぎたり入力が大きすぎたりします。
    \param hash_type  "wc_hash_type_sha256"などの "enum wc_hashtype"からのハッシュ型。
    \param sig_type  wc_signature_type_eccまたはwc_signature_type_rsaなどの署名型列挙型値。
    \param data  ハッシュへのデータを含むバッファへのポインタ。
    \param data_len  データバッファの長さ。
    \param sig  署名を出力するためのバッファへのポインタ。
    \param sig_len  シグネチャ出力バッファの長さ。
    \param key  ECC_KEYやRSAKEYなどのキー構造へのポインタ。
    \param key_len  キー構造のサイズ
    _Example_
    \code
    int ret;
    WC_RNG rng;
    ecc_key eccKey;

    wc_InitRng(&rng);
    wc_ecc_init(&eccKey);

    // Generate key
    ret = wc_ecc_make_key(&rng, 32, &eccKey);

    // Get signature length and allocate buffer
    sigLen = wc_SignatureGetSize(sig_type, &eccKey, sizeof(eccKey));
    sigBuf = malloc(sigLen);

    // Perform signature verification using public key
    ret = wc_SignatureGenerate(
        WC_HASH_TYPE_SHA256, WC_SIGNATURE_TYPE_ECC,
        fileBuf, fileLen,
        sigBuf, &sigLen,
        &eccKey, sizeof(eccKey),
        &rng);
    printf("Signature Generation: %s
    (%d)\n", (ret == 0) ? "Pass" : "Fail", ret);

    free(sigBuf);
    wc_ecc_free(&eccKey);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_SignatureGetSize
    \sa wc_SignatureVerify
*/
int wc_SignatureGenerate(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    byte* sig, word32 *sig_len,
    const void* key, word32 key_len,
    WC_RNG* rng);
