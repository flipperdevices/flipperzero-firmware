/*!
    \ingroup DSA

    \brief This function initializes a DsaKey object in order to use it for
    authentication via the Digital Signature Algorithm (DSA).

    \return 0 Returned on success.
    \return BAD_FUNC_ARG Returned if a NULL key is passed in.

    \param key pointer to the DsaKey structure to initialize

    _Example_
    \code
    DsaKey key;
    int ret;
    ret = wc_InitDsaKey(&key); // initialize DSA key
    \endcode

    \sa wc_FreeDsaKey
*/
int wc_InitDsaKey(DsaKey* key);

/*!
    \ingroup DSA

    \brief This function frees a DsaKey object after it has been used.

    \return none No returns.

    \param key pointer to the DsaKey structure to free

    _Example_
    \code
    DsaKey key;
    // initialize key, use for authentication
    ...
    wc_FreeDsaKey(&key); // free DSA key
    \endcode

    \sa wc_FreeDsaKey
*/
void wc_FreeDsaKey(DsaKey* key);

/*!
    \ingroup DSA

    \brief This function signs the input digest and stores the result in the
    output buffer, out.

    \return 0 Returned on successfully signing the input digest
    \return MP_INIT_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_READ_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_CMP_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_INVMOD_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_EXPTMOD_E may be returned if there is an error in processing
    the DSA signature.
    \return MP_MOD_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_MUL_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_ADD_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_MULMOD_E may be returned if there is an error in processing
    the DSA signature.
    \return MP_TO_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_MEM may be returned if there is an error in processing the
    DSA signature.

    \param digest pointer to the hash to sign
    \param out pointer to the buffer in which to store the signature
    \param key pointer to the initialized DsaKey structure with which to
    generate the signature
    \param rng pointer to an initialized RNG to use with the signature
    generation

    _Example_
    \code
    DsaKey key;
    // initialize DSA key, load private Key
    int ret;
    WC_RNG rng;
    wc_InitRng(&rng);
    byte hash[] = { // initialize with hash digest };
    byte signature[40]; // signature will be 40 bytes (320 bits)

    ret = wc_DsaSign(hash, signature, &key, &rng);
    if (ret != 0) {
	    // error generating DSA signature
    }
    \endcode

    \sa wc_DsaVerify
*/
int wc_DsaSign(const byte* digest, byte* out,
                           DsaKey* key, WC_RNG* rng);

/*!
    \ingroup DSA

    \brief This function verifies the signature of a digest, given a private
    key. It stores whether the key properly verifies in the answer parameter,
    with 1 corresponding to a successful verification, and 0 corresponding to
    failed verification.

    \return 0 Returned on successfully processing the verify request. Note:
    this does not mean that the signature is verified, only that the function
    succeeded
    \return MP_INIT_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_READ_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_CMP_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_INVMOD_E may be returned if there is an error in processing
    the DSA signature.
    \return MP_EXPTMOD_E may be returned if there is an error in processing
    the DSA signature.
    \return MP_MOD_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_MUL_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_ADD_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_MULMOD_E may be returned if there is an error in processing
    the DSA signature.
    \return MP_TO_E may be returned if there is an error in processing the
    DSA signature.
    \return MP_MEM may be returned if there is an error in processing the
    DSA signature.

    \param digest pointer to the digest containing the subject of the signature
    \param sig pointer to the buffer containing the signature to verify
    \param key pointer to the initialized DsaKey structure with which to
    verify the signature
    \param answer pointer to an integer which will store whether the
    verification was successful

    _Example_
    \code
    DsaKey key;
    // initialize DSA key, load public Key

    int ret;
    int verified;
    byte hash[] = { // initialize with hash digest };
    byte signature[] = { // initialize with signature to verify };
    ret = wc_DsaVerify(hash, signature, &key, &verified);
    if (ret != 0) {
    	// error processing verify request
    } else if (answer == 0) {
    	// invalid signature
    }
    \endcode

    \sa wc_DsaSign
*/
int wc_DsaVerify(const byte* digest, const byte* sig,
                             DsaKey* key, int* answer);

/*!
    \ingroup DSA

    \brief This function decodes a DER formatted certificate buffer containing
    a DSA public key, and stores the key in the given DsaKey structure. It
    also sets the inOutIdx parameter according to the length of the input read.

    \return 0 Returned on successfully setting the public key for the DsaKey
    object
    \return ASN_PARSE_E Returned if there is an error in the encoding while
    reading the certificate buffer
    \return ASN_DH_KEY_E Returned if one of the DSA parameters is incorrectly
    formatted

    \param input pointer to the buffer containing the DER formatted DSA
    public key
    \param inOutIdx pointer to an integer in which to store the final index
    of the certificate read
    \param key pointer to the DsaKey structure in which to store the public key
    \param inSz size of the input buffer

    _Example_
    \code
    int ret, idx=0;

    DsaKey key;
    wc_InitDsaKey(&key);
    byte derBuff[] = { // DSA public key};
    ret = wc_DsaPublicKeyDecode(derBuff, &idx, &key, inSz);
    if (ret != 0) {
    	// error reading public key
    }
    \endcode

    \sa wc_InitDsaKey
    \sa wc_DsaPrivateKeyDecode
*/
int wc_DsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                      DsaKey* key, word32 inSz);

/*!
    \ingroup DSA

    \brief This function decodes a DER formatted certificate buffer containing
    a DSA private key, and stores the key in the given DsaKey structure. It
    also sets the inOutIdx parameter according to the length of the input read.

    \return 0 Returned on successfully setting the private key for the DsaKey
    object
    \return ASN_PARSE_E Returned if there is an error in the encoding while
    reading the certificate buffer
    \return ASN_DH_KEY_E Returned if one of the DSA parameters is incorrectly
    formatted

    \param input pointer to the buffer containing the DER formatted DSA
    private key
    \param inOutIdx pointer to an integer in which to store the final index
    of the certificate read
    \param key pointer to the DsaKey structure in which to store the private
    key
    \param inSz size of the input buffer

    _Example_
    \code
    int ret, idx=0;

    DsaKey key;
    wc_InitDsaKey(&key);
    byte derBuff[] = { // DSA private key };
    ret = wc_DsaPrivateKeyDecode(derBuff, &idx, &key, inSz);
    if (ret != 0) {
    	// error reading private key
    }
    \endcode

    \sa wc_InitDsaKey
    \sa wc_DsaPublicKeyDecode
*/
int wc_DsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                       DsaKey* key, word32 inSz);

/*!
    \ingroup DSA

    \brief Convert DsaKey key to DER format, write to output (inLen),
    return bytes written.

    \return outLen Success, number of bytes written
    \return BAD_FUNC_ARG key or output are null or key->type is not
    DSA_PRIVATE.
    \return MEMORY_E Error allocating memory.

    \param key Pointer to DsaKey structure to convert.
    \param output Pointer to output buffer for converted key.
    \param inLen Length of key input.

    _Example_
    \code
    DsaKey key;
    WC_RNG rng;
    int derSz;
    int bufferSize = // Sufficient buffer size;
    byte der[bufferSize];

    wc_InitDsaKey(&key);
    wc_InitRng(&rng);
    wc_MakeDsaKey(&rng, &key);
    derSz = wc_DsaKeyToDer(&key, der, bufferSize);
    \endcode

    \sa wc_InitDsaKey
    \sa wc_FreeDsaKey
    \sa wc_MakeDsaKey
*/
int wc_DsaKeyToDer(DsaKey* key, byte* output, word32 inLen);

/*!
    \ingroup DSA

    \brief Create a DSA key.

    \return MP_OKAY Success
    \return BAD_FUNC_ARG Either rng or dsa is null.
    \return MEMORY_E Couldn't allocate memory for buffer.
    \return MP_INIT_E Error initializing mp_int

    \param rng Pointer to WC_RNG structure.
    \param dsa Pointer to DsaKey structure.

    _Example_
    \code
    WC_RNG rng;
    DsaKey dsa;
    wc_InitRng(&rng);
    wc_InitDsa(&dsa);
    if(wc_MakeDsaKey(&rng, &dsa) != 0)
    {
        // Error creating key
    }
    \endcode

    \sa wc_InitDsaKey
    \sa wc_FreeDsaKey
    \sa wc_DsaSign
*/
int wc_MakeDsaKey(WC_RNG *rng, DsaKey *dsa);

/*!
    \ingroup DSA

    \brief FIPS 186-4 defines valid for modulus_size values as
    (1024, 160) (2048, 256) (3072, 256)

    \return 0 Success
    \return BAD_FUNC_ARG rng or dsa is null or modulus_size is invalid.
    \return MEMORY_E Error attempting to allocate memory.

    \param rng pointer to wolfCrypt rng.
    \param modulus_size 1024, 2048, or 3072 are valid values.
    \param dsa Pointer to a DsaKey structure.

    _Example_
    \code
    DsaKey key;
    WC_RNG rng;
    wc_InitDsaKey(&key);
    wc_InitRng(&rng);
    if(wc_MakeDsaParameters(&rng, 1024, &genKey) != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_MakeDsaKey
    \sa wc_DsaKeyToDer
    \sa wc_InitDsaKey
*/
int wc_MakeDsaParameters(WC_RNG *rng, int modulus_size, DsaKey *dsa);
