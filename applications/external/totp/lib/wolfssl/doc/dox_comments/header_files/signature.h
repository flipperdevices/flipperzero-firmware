/*!
    \ingroup Signature

    \brief This function returns the maximum size of the resulting signature.

    \return Returns SIG_TYPE_E if sig_type is not supported. Returns
    BAD_FUNC_ARG if sig_type was invalid. A positive return value indicates
    the maximum size of a signature.

    \param sig_type A signature type enum value such as
    WC_SIGNATURE_TYPE_ECC or WC_SIGNATURE_TYPE_RSA.
    \param key Pointer to a key structure such as ecc_key or RsaKey.
    \param key_len Size of the key structure.

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

    \brief This function validates a signature by hashing the data and
    using the resulting hash and key to verify the signature.

    \return 0 Success
    \return SIG_TYPE_E -231, signature type not enabled/ available
    \return BAD_FUNC_ARG -173, bad function argument provided
    \return BUFFER_E -132, output buffer too small or input too large.

    \param hash_type A hash type from the “enum  wc_HashType” such as
    “WC_HASH_TYPE_SHA256”.
    \param sig_type A signature type enum value such as
    WC_SIGNATURE_TYPE_ECC or WC_SIGNATURE_TYPE_RSA.
    \param data Pointer to buffer containing the data to hash.
    \param data_len Length of the data buffer.
    \param sig Pointer to buffer to output signature.
    \param sig_len Length of the signature output buffer.
    \param key Pointer to a key structure such as ecc_key or RsaKey.
    \param key_len Size of the key structure.

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

    \brief This function generates a signature from the data using a
    key. It first creates a hash of the data then signs the hash using the key.

    \return 0 Success
    \return SIG_TYPE_E -231, signature type not enabled/ available
    \return BAD_FUNC_ARG -173, bad function argument provided
    \return BUFFER_E -132, output buffer too small or input too large.

    \param hash_type A hash type from the “enum  wc_HashType”
    such as “WC_HASH_TYPE_SHA256”.
    \param sig_type A signature type enum value such as
    WC_SIGNATURE_TYPE_ECC or WC_SIGNATURE_TYPE_RSA.
    \param data Pointer to buffer containing the data to hash.
    \param data_len Length of the data buffer.
    \param sig Pointer to buffer to output signature.
    \param sig_len Length of the signature output buffer.
    \param key Pointer to a key structure such as ecc_key or RsaKey.
    \param key_len Size of the key structure.
    \param rng Pointer to an initialized RNG structure.

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
