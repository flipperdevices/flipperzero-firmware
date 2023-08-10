/*!
    \ingroup ED448

    \brief This function generates the Ed448 public key from the private key.
    It stores the public key in the buffer pubKey, and sets the bytes
    written to this buffer in pubKeySz.

    \return 0 Returned upon successfully making the public key.
    \return BAD_FUNC_ARG Returned ifi key or pubKey evaluate to NULL, or if the
    specified key size is not 57 bytes (Ed448 has 57 byte keys).
    \return MEMORY_E Returned if there is an error allocating memory
    during function execution.

    \param [in] key Pointer to the ed448_key for which to generate a key.
    \param [out] out Pointer to the buffer in which to store the public key.
    \param [in,out] outLen Pointer to a word32 object with the size available
    in out. Set with the number of bytes written to out after successfully
    exporting the public key.

    _Example_
    \code
    int ret;

    ed448_key key;
    byte priv[] = { initialize with 57 byte private key };
    byte pub[57];
    word32 pubSz = sizeof(pub);

    wc_ed448_init(&key);
    wc_ed448_import_private_only(priv, sizeof(priv), &key);
    ret = wc_ed448_make_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error making public key
    }
    \endcode

    \sa wc_ed448_init
    \sa wc_ed448_import_private_only
    \sa wc_ed448_make_key
*/

int wc_ed448_make_public(ed448_key* key, unsigned char* pubKey,
                         word32 pubKeySz);

/*!
    \ingroup ED448

    \brief This function generates a new Ed448 key and stores it in key.

    \return 0 Returned upon successfully making an ed448_key.
    \return BAD_FUNC_ARG Returned if rng or key evaluate to NULL, or if the
    specified key size is not 57 bytes (Ed448 has 57 byte keys).
    \return MEMORY_E Returned if there is an error allocating memory
    during function execution.

    \param [in] rng Pointer to an initialized RNG object with which to
    generate the key.
    \param [in] keysize Length of key to generate. Should always be 57 for
    Ed448.
    \param [in,out] key Pointer to the ed448_key for which to generate a key.

    _Example_
    \code
    int ret;

    WC_RNG rng;
    ed448_key key;

    wc_InitRng(&rng);
    wc_ed448_init(&key);
    ret = wc_ed448_make_key(&rng, 57, &key);
    if (ret != 0) {
        // error making key
    }
    \endcode

    \sa wc_ed448_init
*/

int wc_ed448_make_key(WC_RNG* rng, int keysize, ed448_key* key);

/*!
    \ingroup ED448

    \brief This function signs a message using an ed448_key object
    to guarantee authenticity.

    \return 0 Returned upon successfully generating a signature for the
    message.
    \return BAD_FUNC_ARG Returned if any of the input parameters evaluate to
    NULL, or if the output buffer is too small to store the generated signature.
    \return MEMORY_E Returned if there is an error allocating memory during
    function execution.

    \param [in] in Pointer to the buffer containing the message to sign.
    \param [in] inlen Length of the message to sign.
    \param [out] out Buffer in which to store the generated signature.
    \param [in,out] outlen Maximum length of the output buffer. Will store the
    bytes written to out upon successfully generating a message signature.
    \param [in] key Pointer to a private ed448_key with which to generate the
    signature.

    _Example_
    \code
    ed448_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[114]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };

    wc_InitRng(&rng); // initialize rng
    wc_ed448_init(&key); // initialize key
    wc_ed448_make_key(&rng, 57, &key); // make public/private key pair
    ret = wc_ed448_sign_msg(message, sizeof(message), sig, &sigSz, &key);
    if (ret != 0 ) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed448ph_sign_hash
    \sa wc_ed448ph_sign_msg
    \sa wc_ed448_verify_msg
*/

int wc_ed448_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed448_key* key);

/*!
    \ingroup ED448

    \brief This function signs a message digest using an ed448_key object
    to guarantee authenticity. The context is included as part of the data
    signed. The hash is the pre-hashed message before signature calculation.
    The hash algorithm used to create message digest must be SHAKE-256.

    \return 0 Returned upon successfully generating a signature for the
    message digest.
    \return BAD_FUNC_ARG Returned any of the input parameters evaluate to
    NULL, or if the output buffer is too small to store the generated signature.
    \return MEMORY_E Returned if there is an error allocating memory during
    function execution.

    \param [in] hash Pointer to the buffer containing the hash of the message
    to sign.
    \param [in] hashLen Length of the hash of the message to sign.
    \param [out] out Buffer in which to store the generated signature.
    \param [in,out] outlen Maximum length of the output buffer. Will store the
    bytes written to out upon successfully generating a message signature.
    \param [in] key Pointer to a private ed448_key with which to generate the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    message is being signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed448_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[114]; // will hold generated signature
    sigSz = sizeof(sig);
    byte hash[] = { initialize with SHAKE-256 hash of message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed448_init(&key); // initialize key
    wc_ed448_make_key(&rng, 57, &key); // make public/private key pair
    ret = wc_ed448ph_sign_hash(hash, sizeof(hash), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed448_sign_msg
    \sa wc_ed448ph_sign_msg
    \sa wc_ed448ph_verify_hash
*/

int wc_ed448ph_sign_hash(const byte* hash, word32 hashLen, byte* out,
                         word32 *outLen, ed448_key* key,
                         const byte* context, byte contextLen);

/*!
    \ingroup ED448

    \brief This function signs a message using an ed448_key object
    to guarantee authenticity. The context is included as part of the data
    signed. The message is pre-hashed before signature calculation.

    \return 0 Returned upon successfully generating a signature for the
    message.
    \return BAD_FUNC_ARG Returned any of the input parameters evaluate to
    NULL, or if the output buffer is too small to store the generated signature.
    \return MEMORY_E Returned if there is an error allocating memory during
    function execution.

    \param [in] in Pointer to the buffer containing the message to sign.
    \param [in] inlen Length of the message to sign.
    \param [out] out Buffer in which to store the generated signature.
    \param [in,out] outlen Maximum length of the output buffer. Will store the
    bytes written to out upon successfully generating a message signature.
    \param [in] key Pointer to a private ed448_key with which to generate the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    message is being signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed448_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[114]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed448_init(&key); // initialize key
    wc_ed448_make_key(&rng, 57, &key); // make public/private key pair
    ret = wc_ed448ph_sign_msg(message, sizeof(message), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed448_sign_msg
    \sa wc_ed448ph_sign_hash
    \sa wc_ed448ph_verify_msg
*/

int wc_ed448ph_sign_msg(const byte* in, word32 inLen, byte* out,
                        word32 *outLen, ed448_key* key, const byte* context,
                        byte contextLen);

/*!
    \ingroup ED448

    \brief This function verifies the Ed448 signature of a message to ensure
    authenticity. The context is included as part of the data
    verified. The answer is returned through res, with 1 corresponding to
    a valid signature, and 0 corresponding to an invalid signature.

    \return 0 Returned upon successfully performing the signature
    verification and authentication.
    \return BAD_FUNC_ARG Returned if any of the input parameters evaluate to
    NULL, or if the siglen does not match the actual length of a signature.
    \return SIG_VERIFY_E Returned if verification completes, but the signature
    generated does not match the signature provided.

    \param [in] sig Pointer to the buffer containing the signature to verify.
    \param [in] siglen Length of the signature to verify.
    \param [in] msg Pointer to the buffer containing the message to verify.
    \param [in] msgLen Length of the message to verify.
    \param [in] key Pointer to a public Ed448 key with which to verify the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    the message was signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed448_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed448_verify_msg(sig, sizeof(sig), msg, sizeof(msg), &verified,
            &key, context, sizeof(context));
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed448ph_verify_hash
    \sa wc_ed448ph_verify_msg
    \sa wc_ed448_sign_msg
*/

int wc_ed448_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                          word32 msgLen, int* res, ed448_key* key,
                          const byte* context, byte contextLen);

/*!
    \ingroup ED448

    \brief This function verifies the Ed448 signature of the digest of a message
    to ensure authenticity. The context is included as part of the data
    verified. The hash is the pre-hashed message before signature calculation.
    The hash algorithm used to create message digest must be SHAKE-256.
    The answer is returned through res, with 1 corresponding to a valid
    signature, and 0 corresponding to an invalid signature.

    \return 0 Returned upon successfully performing the signature
    verification and authentication.
    \return BAD_FUNC_ARG Returned if any of the input parameters evaluate to
    NULL, or if the siglen does not match the actual length of a signature.
    \return SIG_VERIFY_E Returned if verification completes, but the signature
    generated does not match the signature provided.

    \param [in] sig Pointer to the buffer containing the signature to verify.
    \param [in] siglen Length of the signature to verify.
    \param [in] hash Pointer to the buffer containing the hash of the message
    to verify.
    \param [in] hashLen Length of the hash to verify.
    \param [in] key Pointer to a public Ed448 key with which to verify the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    the message was signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed448_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte hash[] = { initialize with SHAKE-256 hash of message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed448ph_verify_hash(sig, sizeof(sig), hash, sizeof(hash),
            &verified, &key, context, sizeof(context));
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed448_verify_msg
    \sa wc_ed448ph_verify_msg
    \sa wc_ed448ph_sign_hash
*/

int wc_ed448ph_verify_hash(const byte* sig, word32 siglen, const byte* hash,
                          word32 hashlen, int* res, ed448_key* key,
                          const byte* context, byte contextLen);

/*!
    \ingroup ED448

    \brief This function verifies the Ed448 signature of a message to ensure
    authenticity. The context is included as part of the data
    verified. The message is pre-hashed before verification. The answer is
    returned through res, with 1 corresponding to a valid signature, and 0
    corresponding to an invalid signature.

    \return 0 Returned upon successfully performing the signature
    verification and authentication.
    \return BAD_FUNC_ARG Returned if any of the input parameters evaluate to
    NULL, or if the siglen does not match the actual length of a signature.
    \return SIG_VERIFY_E Returned if verification completes, but the signature
    generated does not match the signature provided.

    \param [in] sig Pointer to the buffer containing the signature to verify.
    \param [in] siglen Length of the signature to verify.
    \param [in] msg Pointer to the buffer containing the message to verify.
    \param [in] msgLen Length of the message to verify.
    \param [in] key Pointer to a public Ed448 key with which to verify the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    the message was signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed448_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed448ph_verify_msg(sig, sizeof(sig), msg, sizeof(msg), &verified,
            &key, context, sizeof(context));
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed448_verify_msg
    \sa wc_ed448ph_verify_hash
    \sa wc_ed448ph_sign_msg
*/

int wc_ed448ph_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                          word32 msgLen, int* res, ed448_key* key,
                          const byte* context, byte contextLen);

/*!
    \ingroup ED448

    \brief This function initializes an ed448_key object for future use
    with message verification.

    \return 0 Returned upon successfully initializing the ed448_key object.
    \return BAD_FUNC_ARG Returned if key is NULL.

    \param [in,out] key Pointer to the ed448_key object to initialize.

    _Example_
    \code
    ed448_key key;
    wc_ed448_init(&key);
    \endcode

    \sa wc_ed448_make_key
    \sa wc_ed448_free
*/

int wc_ed448_init(ed448_key* key);

/*!
    \ingroup ED448

    \brief This function frees an Ed448 object after it has been used.

    \param [in,out] key Pointer to the ed448_key object to free

    _Example_
    \code
    ed448_key key;
    // initialize key and perform secure exchanges
    ...
    wc_ed448_free(&key);
    \endcode

    \sa wc_ed448_init
*/

void wc_ed448_free(ed448_key* key);

/*!
    \ingroup ED448

    \brief This function imports a public ed448_key pair from a buffer
    containing the public key. This function will handle both compressed and
    uncompressed keys. The public key is checked that it matches the private
    key when one is present.

    \return 0 Returned on successfully importing the ed448_key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or inLen is
    less than the size of an Ed448 key.

    \param [in] in Pointer to the buffer containing the public key.
    \param [in] inLen Length of the buffer containing the public key.
    \param [in,out] key Pointer to the ed448_key object in which to store the
    public key.

    _Example_
    \code
    int ret;
    byte pub[] = { initialize Ed448 public key };

    ed_448 key;
    wc_ed448_init_key(&key);
    ret = wc_ed448_import_public(pub, sizeof(pub), &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed448_import_public_ex
    \sa wc_ed448_import_private_key
    \sa wc_ed448_import_private_key_ex
    \sa wc_ed448_export_public
*/

int wc_ed448_import_public(const byte* in, word32 inLen, ed448_key* key);

/*!
    \ingroup ED448

    \brief This function imports a public ed448_key pair from a buffer
    containing the public key. This function will handle both compressed and
    uncompressed keys. Check public key matches private key, when present,
    when not trusted.

    \return 0 Returned on successfully importing the ed448_key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or inLen is
    less than the size of an Ed448 key.

    \param [in] in Pointer to the buffer containing the public key.
    \param [in] inLen Length of the buffer containing the public key.
    \param [in,out] key Pointer to the ed448_key object in which to store the
    public key.
    \param [in] trusted Public key data is trusted or not.

    _Example_
    \code
    int ret;
    byte pub[] = { initialize Ed448 public key };

    ed_448 key;
    wc_ed448_init_key(&key);
    ret = wc_ed448_import_public_ex(pub, sizeof(pub), &key, 1);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed448_import_public
    \sa wc_ed448_import_private_key
    \sa wc_ed448_import_private_key_ex
    \sa wc_ed448_export_public
*/

int wc_ed448_import_public_ex(const byte* in, word32 inLen, ed448_key* key,
    int trusted);

/*!
    \ingroup ED448

    \brief This function imports an Ed448 private key only from a
    buffer.

    \return 0 Returned on successfully importing the Ed448 private key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or if
    privSz is less than ED448_KEY_SIZE.

    \param [in] priv Pointer to the buffer containing the private key.
    \param [in] privSz Length of the private key.
    \param [in,out] key Pointer to the ed448_key object in which to store the
    imported private key.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 57 byte private key };

    ed448_key key;
    wc_ed448_init_key(&key);
    ret = wc_ed448_import_private_only(priv, sizeof(priv), &key);
    if (ret != 0) {
        // error importing private key
    }
    \endcode

    \sa wc_ed448_import_public
    \sa wc_ed448_import_public_ex
    \sa wc_ed448_import_private_key
    \sa wc_ed448_import_private_key_ex
    \sa wc_ed448_export_private_only
*/

int wc_ed448_import_private_only(const byte* priv, word32 privSz,
                                 ed448_key* key);

/*!
    \ingroup ED448

    \brief This function imports a public/private Ed448 key pair from a
    pair of buffers. This function will handle both compressed and
    uncompressed keys.

    \return 0 Returned on successfully importing the Ed448 key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or if
    either privSz is less than ED448_KEY_SIZE or pubSz is less than
    ED448_PUB_KEY_SIZE.

    \param [in] priv Pointer to the buffer containing the private key.
    \param [in] privSz Length of the private key.
    \param [in] pub Pointer to the buffer containing the public key.
    \param [in] pubSz Length of the public key.
    \param [in,out] key Pointer to the ed448_key object in which to store the
    imported private/public key pair.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 57 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed448_key key;
    wc_ed448_init_key(&key);
    ret = wc_ed448_import_private_key(priv, sizeof(priv), pub, sizeof(pub),
            &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed448_import_public
    \sa wc_ed448_import_public_ex
    \sa wc_ed448_import_private_only
    \sa wc_ed448_import_private_key_ex
    \sa wc_ed448_export_private
*/

int wc_ed448_import_private_key(const byte* priv, word32 privSz,
                               const byte* pub, word32 pubSz, ed448_key* key);

/*!
    \ingroup ED448

    \brief This function imports a public/private Ed448 key pair from a
    pair of buffers. This function will handle both compressed and
    uncompressed keys. The public is checked against private key if not trusted.

    \return 0 Returned on successfully importing the Ed448 key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or if
    either privSz is less than ED448_KEY_SIZE or pubSz is less than
    ED448_PUB_KEY_SIZE.

    \param [in] priv Pointer to the buffer containing the private key.
    \param [in] privSz Length of the private key.
    \param [in] pub Pointer to the buffer containing the public key.
    \param [in] pubSz Length of the public key.
    \param [in,out] key Pointer to the ed448_key object in which to store the
    imported private/public key pair.
    \param [in] trusted Public key data is trusted or not.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 57 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed448_key key;
    wc_ed448_init_key(&key);
    ret = wc_ed448_import_private_key_ex(priv, sizeof(priv), pub, sizeof(pub),
            &key, 1);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed448_import_public
    \sa wc_ed448_import_public_ex
    \sa wc_ed448_import_private_only
    \sa wc_ed448_import_private_key
    \sa wc_ed448_export_private
*/

int wc_ed448_import_private_key_ex(const byte* priv, word32 privSz,
    const byte* pub, word32 pubSz, ed448_key* key, int trusted);

/*!
    \ingroup ED448

    \brief This function exports the private key from an ed448_key
    structure. It stores the public key in the buffer out, and sets the bytes
    written to this buffer in outLen.

    \return 0 Returned upon successfully exporting the public key.
    \return BAD_FUNC_ARG Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough to
    store the private key. Upon returning this error, the function sets the
    size required in outLen.

    \param [in] key Pointer to an ed448_key structure from which to export the
    public key.
    \param [out] out Pointer to the buffer in which to store the public key.
    \param [in,out] outLen Pointer to a word32 object with the size available
    in out. Set with the number of bytes written to out after successfully
    exporting the public key.

    _Example_
    \code
    int ret;
    ed448_key key;
    // initialize key, make key

    char pub[57];
    word32 pubSz = sizeof(pub);

    ret = wc_ed448_export_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error exporting public key
    }
    \endcode

    \sa wc_ed448_import_public
    \sa wc_ed448_import_public_ex
    \sa wc_ed448_export_private_only
*/

int wc_ed448_export_public(ed448_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED448

    \brief This function exports only the private key from an ed448_key
    structure. It stores the private key in the buffer out, and sets
    the bytes written to this buffer in outLen.

    \return 0 Returned upon successfully exporting the private key.
    \return ECC_BAD_ARG_E Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough
    to store the private key.

    \param [in] key Pointer to an ed448_key structure from which to export
    the private key.
    \param [out] out Pointer to the buffer in which to store the private key.
    \param [in,out] outLen Pointer to a word32 object with the size available in
    out. Set with the number of bytes written to out after successfully
    exporting the private key.

    _Example_
    \code
    int ret;
    ed448_key key;
    // initialize key, make key

    char priv[57]; // 57 bytes because only private key
    word32 privSz = sizeof(priv);
    ret = wc_ed448_export_private_only(&key, priv, &privSz);
    if (ret != 0) {
        // error exporting private key
    }
    \endcode

    \sa wc_ed448_export_public
    \sa wc_ed448_import_private_key
    \sa wc_ed448_import_private_key_ex
*/

int wc_ed448_export_private_only(ed448_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED448

    \brief This function exports the key pair from an ed448_key
    structure. It stores the key pair in the buffer out, and sets
    the bytes written to this buffer in outLen.

    \return 0 Returned upon successfully exporting the key pair.
    \return ECC_BAD_ARG_E Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough
    to store the key pair.

    \param [in] key Pointer to an ed448_key structure from which to export
    the key pair.
    \param [out] out Pointer to the buffer in which to store the key pair.
    \param [in,out] outLen Pointer to a word32 object with the size available in
    out. Set with the number of bytes written to out after successfully
    exporting the key pair.

    _Example_
    \code
    ed448_key key;
    wc_ed448_init(&key);

    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed448_make_key(&rng, 57, &key); // initialize 57 byte Ed448 key

    byte out[114]; // out needs to be a sufficient buffer size
    word32 outLen = sizeof(out);
    int key_size = wc_ed448_export_private(&key, out, &outLen);
    if (key_size == BUFFER_E) {
        // Check size of out compared to outLen to see if function reset outLen
    }
    \endcode

    \sa wc_ed448_import_private
    \sa wc_ed448_export_private_only
*/

int wc_ed448_export_private(ed448_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED448

    \brief This function exports the private and public key separately from an
    ed448_key structure. It stores the private key in the buffer priv, and sets
    the bytes written to this buffer in privSz. It stores the public key in the
    buffer pub, and sets the bytes written to this buffer in pubSz.

    \return 0 Returned upon successfully exporting the key pair.
    \return ECC_BAD_ARG_E Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough
    to store the key pair.

    \param [in] key Pointer to an ed448_key structure from which to export
    the key pair.
    \param [out] priv Pointer to the buffer in which to store the private key.
    \param [in,out] privSz Pointer to a word32 object with the size available in
    out. Set with the number of bytes written to out after successfully
    exporting the private key.
    \param [out] pub Pointer to the buffer in which to store the public key.
    \param [in,out] pubSz Pointer to a word32 object with the size available in
    out. Set with the number of bytes written to out after successfully
    exporting the public key.

    _Example_
    \code
    int ret;
    ed448_key key;
    // initialize key, make key

    char pub[57];
    word32 pubSz = sizeof(pub);
    char priv[57];
    word32 privSz = sizeof(priv);

    ret = wc_ed448_export_key(&key, priv, &pubSz, pub, &pubSz);
    if (ret != 0) {
        // error exporting private and public key
    }
    \endcode

    \sa wc_ed448_export_private
    \sa wc_ed448_export_public
*/

int wc_ed448_export_key(ed448_key* key,
                          byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz);

/*!
    \ingroup ED448

    \brief This function checks the public key in ed448_key structure matches
    the private key.

    \return 0 Returned if the private and public key matched.
    \return BAD_FUNC_ARGS Returned if the given key is NULL.

    \param [in] key Pointer to an ed448_key structure holding a private and
    public key.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 57 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed448_key key;
    wc_ed448_init_key(&key);
    wc_ed448_import_private_key_ex(priv, sizeof(priv), pub, sizeof(pub), &key,
        1);
    ret = wc_ed448_check_key(&key);
    if (ret != 0) {
        // error checking key
    }
    \endcode

    \sa wc_ed448_import_private_key
    \sa wc_ed448_import_private_key_ex
*/

int wc_ed448_check_key(ed448_key* key);


/*!
    \ingroup ED448

    \brief This function returns the size of an Ed448 private key - 57 bytes.

    \return ED448_KEY_SIZE The size of a valid private key (57 bytes).
    \return BAD_FUNC_ARGS Returned if the given key is NULL.

    \param [in] key Pointer to an ed448_key structure for which to get the
    key size.

    _Example_
    \code
    int keySz;
    ed448_key key;
    // initialize key, make key
    keySz = wc_ed448_size(&key);
    if (keySz == 0) {
        // error determining key size
    }
    \endcode

    \sa wc_ed448_make_key
*/

int wc_ed448_size(ed448_key* key);

/*!
    \ingroup ED448

    \brief This function returns the private key size (secret + public) in
    bytes.

    \return ED448_PRV_KEY_SIZE The size of the private key (114 bytes).
    \return BAD_FUNC_ARG Returns if key argument is NULL.

    \param [in] key Pointer to an ed448_key structure for which to get the
    key size.

    _Example_
    \code
    ed448_key key;
    wc_ed448_init(&key);

    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed448_make_key(&rng, 57, &key); // initialize 57 byte Ed448 key
    int key_size = wc_ed448_priv_size(&key);
    \endcode

    \sa wc_ed448_pub_size
*/

int wc_ed448_priv_size(ed448_key* key);

/*!
    \ingroup ED448

    \brief This function returns the compressed key size in bytes (public key).

    \return ED448_PUB_KEY_SIZE The size of the compressed public key (57 bytes).
    \return BAD_FUNC_ARG Returns if key argument is NULL.

    \param [in] key Pointer to an ed448_key structure for which to get the
    key size.

    _Example_
    \code
    ed448_key key;
    wc_ed448_init(&key);
    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed448_make_key(&rng, 57, &key); // initialize 57 byte Ed448 key
    int key_size = wc_ed448_pub_size(&key);
    \endcode

    \sa wc_ed448_priv_size
*/

int wc_ed448_pub_size(ed448_key* key);

/*!
    \ingroup ED448

    \brief This function returns the size of an Ed448 signature (114 in bytes).

    \return ED448_SIG_SIZE The size of an Ed448 signature (114 bytes).
    \return BAD_FUNC_ARG Returns if key argument is NULL.

    \param [in] key Pointer to an ed448_key structure for which to get the
    signature size.

    _Example_
    \code
    int sigSz;
    ed448_key key;
    // initialize key, make key

    sigSz = wc_ed448_sig_size(&key);
    if (sigSz == 0) {
        // error determining sig size
    }
    \endcode

    \sa wc_ed448_sign_msg
*/

int wc_ed448_sig_size(ed448_key* key);
