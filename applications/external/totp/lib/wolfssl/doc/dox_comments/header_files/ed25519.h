/*!
    \ingroup ED25519

    \brief This function generates the Ed25519 public key from the private key.
    It stores the public key in the buffer pubKey, and sets the bytes
    written to this buffer in pubKeySz.

    \return 0 Returned upon successfully making the public key.
    \return BAD_FUNC_ARG Returned ifi key or pubKey evaluate to NULL, or if the
    specified key size is not 32 bytes (Ed25519 has 32 byte keys).
    \return MEMORY_E Returned if there is an error allocating memory
    during function execution.

    \param [in] key Pointer to the ed25519_key for which to generate a key.
    \param [out] out Pointer to the buffer in which to store the public key.
    \param [in,out] outLen Pointer to a word32 object with the size available
    in out. Set with the number of bytes written to out after successfully
    exporting the public key.

    _Example_
    \code
    int ret;

    ed25519_key key;
    byte priv[] = { initialize with 32 byte private key };
    byte pub[32];
    word32 pubSz = sizeof(pub);

    wc_ed25519_init(&key);
    wc_ed25519_import_private_only(priv, sizeof(priv), &key);
    ret = wc_ed25519_make_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error making public key
    }
    \endcode

    \sa wc_ed25519_init
    \sa wc_ed25519_import_private_only
    \sa wc_ed25519_make_key
*/

int wc_ed25519_make_public(ed25519_key* key, unsigned char* pubKey,
                           word32 pubKeySz);

/*!
    \ingroup ED25519

    \brief This function generates a new Ed25519 key and stores it in key.

    \return 0 Returned upon successfully making an ed25519_key.
    \return BAD_FUNC_ARG Returned if rng or key evaluate to NULL, or if the
    specified key size is not 32 bytes (Ed25519 has 32 byte keys).
    \return MEMORY_E Returned if there is an error allocating memory
    during function execution.

    \param [in] rng Pointer to an initialized RNG object with which to
    generate the key.
    \param [in] keysize Length of key to generate. Should always be 32 for
    Ed25519.
    \param [in,out] key Pointer to the ed25519_key for which to generate a key.

    _Example_
    \code
    int ret;

    WC_RNG rng;
    ed25519_key key;

    wc_InitRng(&rng);
    wc_ed25519_init(&key);
    wc_ed25519_make_key(&rng, 32, &key);
    if (ret != 0) {
        // error making key
    }
    \endcode

    \sa wc_ed25519_init
*/

int wc_ed25519_make_key(WC_RNG* rng, int keysize, ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function signs a message using an ed25519_key object
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
    \param [in] key Pointer to a private ed25519_key with which to generate the
    signature.

    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519_sign_msg(message, sizeof(message), sig, &sigSz, &key);
    if (ret != 0) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed25519ctx_sign_msg
    \sa wc_ed25519ph_sign_hash
    \sa wc_ed25519ph_sign_msg
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function signs a message using an ed25519_key object
    to guarantee authenticity. The context is part of the data signed.

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
    \param [in] key Pointer to a private ed25519_key with which to generate the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    message is being signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519ctx_sign_msg(message, sizeof(message), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed25519_sign_msg
    \sa wc_ed25519ph_sign_hash
    \sa wc_ed25519ph_sign_msg
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519ctx_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key,
                        const byte* context, byte contextLen);

/*!
    \ingroup ED25519

    \brief This function signs a message digest using an ed25519_key object
    to guarantee authenticity. The context is included as part of the data
    signed. The message is pre-hashed before signature calculation. The hash
    algorithm used to create message digest must be SHAKE-256.

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
    \param [in] key Pointer to a private ed25519_key with which to generate the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    message is being signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte hash[] = { initialize with SHA-512 hash of message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519ph_sign_hash(hash, sizeof(hash), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed25519_sign_msg
    \sa wc_ed25519ctx_sign_msg
    \sa wc_ed25519ph_sign_msg
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519ph_sign_hash(const byte* hash, word32 hashLen, byte* out,
                           word32 *outLen, ed25519_key* key,
                           const byte* context, byte contextLen);

/*!
    \ingroup ED25519

    \brief This function signs a message using an ed25519_key object
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
    \param [in] key Pointer to a private ed25519_key with which to generate the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    message is being signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519ph_sign_msg(message, sizeof(message), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode

    \sa wc_ed25519_sign_msg
    \sa wc_ed25519ctx_sign_msg
    \sa wc_ed25519ph_sign_hash
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519ph_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key,
                        const byte* context, byte contextLen);

/*!
    \ingroup ED25519

    \brief This function verifies the Ed25519 signature of a message to ensure
    authenticity. It returns the answer through res, with 1 corresponding to
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
    \param [out] res Pointer to the result of the verification. 1 indicates the
    message was successfully verified.
    \param [in] key Pointer to a public Ed25519 key with which to verify the
    signature.

    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    // initialize key with received public key
    ret = wc_ed25519_verify_msg(sig, sizeof(sig), msg, sizeof(msg), &verified,
            &key);
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed25519ctx_verify_msg
    \sa wc_ed25519ph_verify_hash
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                          word32 msgLen, int* ret, ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function verifies the Ed25519 signature of a message to ensure
    authenticity. The context is included as part of the data
    verified. It returns the answer through res, with 1 corresponding to
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
    \param [out] res Pointer to the result of the verification. 1 indicates the
    message was successfully verified.
    \param [in] key Pointer to a public Ed25519 key with which to verify the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    the message was signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed25519ctx_verify_msg(sig, sizeof(sig), msg, sizeof(msg),
            &verified, &key, );
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed25519_verify_msg
    \sa wc_ed25519ph_verify_hash
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519ctx_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                             word32 msgLen, int* ret, ed25519_key* key,
                             const byte* context, byte contextLen);

/*!
    \ingroup ED25519

    \brief This function verifies the Ed25519 signature of the digest of a
    message to ensure authenticity. The context is included as part of the data
    verified. The hash is the pre-hashed message before signature calculation.
    The hash algorithm used to create message digest must be SHA-512.
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
    \param [out] res Pointer to the result of the verification. 1 indicates the
    message was successfully verified.
    \param [in] key Pointer to a public Ed25519 key with which to verify the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    the message was signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte hash[] = { initialize with SHA-512 hash of message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed25519ph_verify_hash(sig, sizeof(sig), msg, sizeof(msg),
            &verified, &key, );
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed25519_verify_msg
    \sa wc_ed25519ctx_verify_msg
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519ph_verify_hash(const byte* sig, word32 siglen, const byte* hash,
                             word32 hashLen, int* ret, ed25519_key* key,
                             const byte* context, byte contextLen);

/*!
    \ingroup ED25519

    \brief This function verifies the Ed25519 signature of a message to ensure
    authenticity. The context is included as part of the data
    verified. The message is pre-hashed before verification. It returns the
    answer through res, with 1 corresponding to a valid signature, and 0
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
    \param [out] res Pointer to the result of the verification. 1 indicates the
    message was successfully verified.
    \param [in] key Pointer to a public Ed25519 key with which to verify the
    signature.
    \param [in] context Pointer to the buffer containing the context for which
    the message was signed.
    \param [in] contextLen Length of the context buffer.

    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed25519ctx_verify_msg(sig, sizeof(sig), msg, sizeof(msg),
            &verified, &key, );
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode

    \sa wc_ed25519_verify_msg
    \sa wc_ed25519ph_verify_hash
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519ph_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                            word32 msgLen, int* ret, ed25519_key* key,
                            const byte* context, byte contextLen);

/*!
    \ingroup ED25519

    \brief This function initializes an ed25519_key object for future use
    with message verification.

    \return 0 Returned upon successfully initializing the ed25519_key object.
    \return BAD_FUNC_ARG Returned if key is NULL.

    \param [in,out] key Pointer to the ed25519_key object to initialize.

    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);
    \endcode

    \sa wc_ed25519_make_key
    \sa wc_ed25519_free
*/

int wc_ed25519_init(ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function frees an Ed25519 object after it has been used.

    \param [in,out] key Pointer to the ed25519_key object to free

    _Example_
    \code
    ed25519_key key;
    // initialize key and perform secure exchanges
    ...
    wc_ed25519_free(&key);
    \endcode

    \sa wc_ed25519_init
*/

void wc_ed25519_free(ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function imports a public ed25519_key pair from a buffer
    containing the public key. This function will handle both compressed and
    uncompressed keys. The public key is checked that it matches the private
    key when one is present.

    \return 0 Returned on successfully importing the ed25519_key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or inLen is
    less than the size of an Ed25519 key.

    \param [in] in Pointer to the buffer containing the public key.
    \param [in] inLen Length of the buffer containing the public key.
    \param [in,out] key Pointer to the ed25519_key object in which to store the
    public key.

    _Example_
    \code
    int ret;
    byte pub[] = { initialize Ed25519 public key };

    ed_25519 key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_public(pub, sizeof(pub), &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed25519_import_public_ex
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_import_private_key_ex
    \sa wc_ed25519_export_public
*/

int wc_ed25519_import_public(const byte* in, word32 inLen, ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function imports a public ed25519_key pair from a buffer
    containing the public key. This function will handle both compressed and
    uncompressed keys. Check public key matches private key, when present,
    when not trusted.

    \return 0 Returned on successfully importing the ed25519_key.
    \return BAD_FUNC_ARG Returned if in or key evaluate to NULL, or inLen is
    less than the size of an Ed25519 key.

    \param [in] in Pointer to the buffer containing the public key.
    \param [in] inLen Length of the buffer containing the public key.
    \param [in,out] key Pointer to the ed25519_key object in which to store the
    public key.
    \param [in] trusted Public key data is trusted or not.

    _Example_
    \code
    int ret;
    byte pub[] = { initialize Ed25519 public key };

    ed_25519 key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_public_ex(pub, sizeof(pub), &key, 1);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_import_private_key_ex
    \sa wc_ed25519_export_public
*/

int wc_ed25519_import_public_ex(const byte* in, word32 inLen, ed25519_key* key,
    int trusted);

/*!
    \ingroup ED25519

    \brief This function imports an Ed25519 private key only from a
    buffer.

    \return 0 Returned on successfully importing the Ed25519 key.
    \return BAD_FUNC_ARG Returned if priv or key evaluate to NULL, or if
    privSz is not equal to ED25519_KEY_SIZE.

    \param [in] priv Pointer to the buffer containing the private key.
    \param [in] privSz Length of the private key.
    \param [in] pub Pointer to the buffer containing the public key.
    \param [in] pubSz Length of the public key.
    \param [in,out] key Pointer to the ed25519_key object in which to store the
    imported private key.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 32 byte private key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_private_only(priv, sizeof(priv), &key);
    if (ret != 0) {
        // error importing private key
    }
    \endcode

    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_public_ex
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_import_private_key_ex
    \sa wc_ed25519_export_private_only
*/

int wc_ed25519_import_private_only(const byte* priv, word32 privSz,
                                   ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function imports a public/private Ed25519 key pair from a
    pair of buffers. This function will handle both compressed and
    uncompressed keys. The public key is assumed to be untrusted and is
    checked against the private key.

    \return 0 Returned on successfully importing the ed25519_key.
    \return BAD_FUNC_ARG Returned if priv or key evaluate to NULL; or if
    either privSz is not equal to ED25519_KEY_SIZE nor ED25519_PRV_KEY_SIZE, or
    pubSz is less than ED25519_PUB_KEY_SIZE.

    \param [in] priv Pointer to the buffer containing the private key.
    \param [in] privSz Length of the private key.
    \param [in] pub Pointer to the buffer containing the public key.
    \param [in] pubSz Length of the public key.
    \param [in,out] key Pointer to the ed25519_key object in which to store the
    imported private/public key pair.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 32 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_private_key(priv, sizeof(priv), pub, sizeof(pub),
            &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_public_ex
    \sa wc_ed25519_import_private_only
    \sa wc_ed25519_import_private_key_ex
    \sa wc_ed25519_export_private
*/

int wc_ed25519_import_private_key(const byte* priv, word32 privSz,
                               const byte* pub, word32 pubSz, ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function imports a public/private Ed25519 key pair from a
    pair of buffers. This function will handle both compressed and
    uncompressed keys. The public is checked against private key if not trusted.

    \return 0 Returned on successfully importing the ed25519_key.
    \return BAD_FUNC_ARG Returned if priv or key evaluate to NULL; or if
    either privSz is not equal to ED25519_KEY_SIZE nor ED25519_PRV_KEY_SIZE, or
    pubSz is less than ED25519_PUB_KEY_SIZE.

    \param [in] priv Pointer to the buffer containing the private key.
    \param [in] privSz Length of the private key.
    \param [in] pub Pointer to the buffer containing the public key.
    \param [in] pubSz Length of the public key.
    \param [in,out] key Pointer to the ed25519_key object in which to store the
    imported private/public key pair.
    \param [in] trusted Public key data is trusted or not.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 32 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_private_key(priv, sizeof(priv), pub, sizeof(pub),
            &key, 1);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_public_ex
    \sa wc_ed25519_import_private_only
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_export_private
*/

int wc_ed25519_import_private_key_ex(const byte* priv, word32 privSz,
    const byte* pub, word32 pubSz, ed25519_key* key, int trusted);

/*!
    \ingroup ED25519

    \brief This function exports the private key from an ed25519_key
    structure. It stores the public key in the buffer out, and sets the bytes
    written to this buffer in outLen.

    \return 0 Returned upon successfully exporting the public key.
    \return BAD_FUNC_ARG Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough to
    store the private key. Upon returning this error, the function sets the
    size required in outLen.

    \param [in] key Pointer to an ed25519_key structure from which to export the
    public key.
    \param [out] out Pointer to the buffer in which to store the public key.
    \param [in,out] outLen Pointer to a word32 object with the size available
    in out. Set with the number of bytes written to out after successfully
    exporting the public key.

    _Example_
    \code
    int ret;
    ed25519_key key;
    // initialize key, make key

    char pub[32];
    word32 pubSz = sizeof(pub);

    ret = wc_ed25519_export_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error exporting public key
    }
    \endcode

    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_public_ex
    \sa wc_ed25519_export_private_only
*/

int wc_ed25519_export_public(ed25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED25519

    \brief This function exports only the private key from an ed25519_key
    structure. It stores the private key in the buffer out, and sets
    the bytes written to this buffer in outLen.

    \return 0 Returned upon successfully exporting the private key.
    \return BAD_FUNC_ARG Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough
    to store the private key.

    \param [in] key Pointer to an ed25519_key structure from which to export
    the private key.
    \param [out] out Pointer to the buffer in which to store the private key.
    \param [in,out] outLen Pointer to a word32 object with the size available in
    out. Set with the number of bytes written to out after successfully
    exporting the private key.

    _Example_
    \code
    int ret;
    ed25519_key key;
    // initialize key, make key

    char priv[32]; // 32 bytes because only private key
    word32 privSz = sizeof(priv);
    ret = wc_ed25519_export_private_only(&key, priv, &privSz);
    if (ret != 0) {
        // error exporting private key
    }
    \endcode

    \sa wc_ed25519_export_public
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_import_private_key_ex
*/

int wc_ed25519_export_private_only(ed25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED25519

    \brief This function exports the key pair from an ed25519_key
    structure. It stores the key pair in the buffer out, and sets
    the bytes written to this buffer in outLen.

    \return 0 Returned upon successfully exporting the key pair.
    \return BAD_FUNC_ARG Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough
    to store the key pair.

    \param [in] key Pointer to an ed25519_key structure from which to export
    the key pair.
    \param [out] out Pointer to the buffer in which to store the key pair.
    \param [in,out] outLen Pointer to a word32 object with the size available in
    out. Set with the number of bytes written to out after successfully
    exporting the key pair.

    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);

    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed25519_make_key(&rng, 32, &key); // initialize 32 byte Ed25519 key

    byte out[64]; // out needs to be a sufficient buffer size
    word32 outLen = sizeof(out);
    int key_size = wc_ed25519_export_private(&key, out, &outLen);
    if (key_size == BUFFER_E) {
        // Check size of out compared to outLen to see if function reset outLen
    }
    \endcode

    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_import_private_key_ex
    \sa wc_ed25519_export_private_only
*/

int wc_ed25519_export_private(ed25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED25519

    \brief This function exports the private and public key separately from an
    ed25519_key structure. It stores the private key in the buffer priv, and
    sets the bytes written to this buffer in privSz. It stores the public key
    in the buffer pub, and sets the bytes written to this buffer in pubSz.

    \return 0 Returned upon successfully exporting the key pair.
    \return BAD_FUNC_ARG Returned if any of the input values evaluate to NULL.
    \return BUFFER_E Returned if the buffer provided is not large enough
    to store the key pair.

    \param [in] key Pointer to an ed25519_key structure from which to export
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
    ed25519_key key;
    // initialize key, make key

    char pub[32];
    word32 pubSz = sizeof(pub);
    char priv[32];
    word32 privSz = sizeof(priv);

    ret = wc_ed25519_export_key(&key, priv, &pubSz, pub, &pubSz);
    if (ret != 0) {
        // error exporting public key
    }
    \endcode

    \sa wc_ed25519_export_private
    \sa wc_ed25519_export_public
*/

int wc_ed25519_export_key(ed25519_key* key,
                          byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz);

/*!
    \ingroup ED25519

    \brief This function checks the public key in ed25519_key structure matches
    the private key.

    \return 0 Returned if the private and public key matched.
    \return BAD_FUNC_ARG Returned if the given key is NULL.
    \return PUBLIC_KEY_E Returned if the no public key available or is invalid.

    \param [in] key Pointer to an ed25519_key structure holding a private and
    public key.

    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 57 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    wc_ed25519_import_private_key_ex(priv, sizeof(priv), pub, sizeof(pub), &key,
        1);
    ret = wc_ed25519_check_key(&key);
    if (ret != 0) {
        // error checking key
    }
    \endcode

    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_import_private_key_ex
*/

int wc_ed25519_check_key(ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function returns the size of an Ed25519 - 32 bytes.

    \return ED25519_KEY_SIZE The size of a valid private key (32 bytes).
    \return BAD_FUNC_ARG Returned if the given key is NULL.

    \param [in] key Pointer to an ed25519_key structure for which to get the
    key size.

    _Example_
    \code
    int keySz;
    ed25519_key key;
    // initialize key, make key
    keySz = wc_ed25519_size(&key);
    if (keySz == 0) {
        // error determining key size
    }
    \endcode

    \sa wc_ed25519_make_key
*/

int wc_ed25519_size(ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function returns the private key size (secret + public) in
    bytes.

    \return ED25519_PRV_KEY_SIZE The size of the private key (64 bytes).
    \return BAD_FUNC_ARG Returned if key argument is NULL.

    \param [in] key Pointer to an ed25519_key structure for which to get the
    key size.

    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);

    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed25519_make_key(&rng, 32, &key); // initialize 32 byte Ed25519 key
    int key_size = wc_ed25519_priv_size(&key);
    \endcode

    \sa wc_ed25519_pub_size
*/

int wc_ed25519_priv_size(ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function returns the compressed key size in bytes (public key).

    \return ED25519_PUB_KEY_SIZE The size of the compressed public key
    (32 bytes).
    \return BAD_FUNC_ARG Returns if key argument is NULL.

    \param [in] key Pointer to an ed25519_key structure for which to get the
    key size.

    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);
    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed25519_make_key(&rng, 32, &key); // initialize 32 byte Ed25519 key
    int key_size = wc_ed25519_pub_size(&key);
    \endcode

    \sa wc_ed25519_priv_size
*/

int wc_ed25519_pub_size(ed25519_key* key);

/*!
    \ingroup ED25519

    \brief This function returns the size of an Ed25519 signature (64 in bytes).

    \return ED25519_SIG_SIZE The size of an Ed25519 signature (64 bytes).
    \return BAD_FUNC_ARG Returns if key argument is NULL.

    \param [in] key Pointer to an ed25519_key structure for which to get the
    signature size.

    _Example_
    \code
    int sigSz;
    ed25519_key key;
    // initialize key, make key

    sigSz = wc_ed25519_sig_size(&key);
    if (sigSz == 0) {
        // error determining sig size
    }
    \endcode

    \sa wc_ed25519_sign_msg
*/

int wc_ed25519_sig_size(ed25519_key* key);
