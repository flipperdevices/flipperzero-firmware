/*!
    \ingroup SHA

    \brief This function initializes SHA3-224. This is automatically
    called by wc_Sha3_224Hash.

    \return 0 Returned upon successfully initializing

    \param sha3 pointer to the sha3 structure to use for encryption

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_224(sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_224 failed");
    }
    else {
        wc_Sha3_224_Update(sha3, data, len);
        wc_Sha3_224_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_224Hash
    \sa wc_Sha3_224_Update
    \sa wc_Sha3_224_Final
*/
int wc_InitSha3_224(wc_Sha3* sha3, void* heap, int devId);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_224(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_224 failed");
    }
    else {
        wc_Sha3_224_Update(sha3, data, len);
        wc_Sha3_224_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_224Hash
    \sa wc_Sha3_224_Final
    \sa wc_InitSha3_224
*/
int wc_Sha3_224_Update(wc_Sha3* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha3 struct.

    \return 0 Returned upon successfully finalizing.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_224(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_224 failed");
    }
    else {
       wc_Sha3_224_Update(sha3, data, len);
       wc_Sha3_224_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_224Hash
    \sa wc_Sha3_224_GetHash
    \sa wc_InitSha3_224
*/
int wc_Sha3_224_Final(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Resets the wc_Sha3 structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param sha3 Pointer to the sha3 structure to be freed.

    _Example_
    \code
    wc_Sha3 sha3;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_224(&sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_224 failed");
    }
    else {
        wc_Sha3_224_Update(&sha3, data, len);
        wc_Sha3_224_Final(&sha3, hash);
        wc_Sha3_224_Free(&sha3);
    }
    \endcode

    \sa wc_InitSha3_224
    \sa wc_Sha3_224_Update
    \sa wc_Sha3_224_Final
*/
void wc_Sha3_224_Free(wc_Sha3*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of sha3 struct.

    \return 0 Returned upon successful copying of the hash.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_224(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_224 failed");
    }
    else {
       wc_Sha3_224_Update(sha3, data, len);
       wc_Sha3_224_GetHash(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_224Hash
    \sa wc_Sha3_224_Final
    \sa wc_InitSha3_224
    \sa wc_Sha3_224_Copy
*/
int wc_Sha3_224_GetHash(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Copy the state of the hash.

    \return 0 Returned upon successful copying.

    \param sha3 pointer to the sha3 structure to copy
    \param dst  pointer to the sha3 structure to copy into

    _Example_
    \code
    wc_Sha3 sha3[1];
    wc_Sha3 sha3_dup[1];
    if ((ret = wc_InitSha3_224(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_224 failed");
    }
    else {
       wc_Sha3_224_Update(sha3, data, len);
       wc_Sha3_224_Copy(sha3, sha3_dup);
    }
    \endcode

    \sa wc_Sha3_224Hash
    \sa wc_Sha3_224_Final
    \sa wc_InitSha3_224
    \sa wc_Sha3_224_GetHash
*/
int wc_Sha3_224_Copy(wc_Sha3* sha3, wc_Sha3* dst);

/*!
    \ingroup SHA

    \brief This function initializes SHA3-256. This is automatically
    called by wc_Sha3_256Hash.

    \return 0 Returned upon successfully initializing

    \param sha3 pointer to the sha3 structure to use for encryption

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_256(sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_256 failed");
    }
    else {
        wc_Sha3_256_Update(sha3, data, len);
        wc_Sha3_256_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_256Hash
    \sa wc_Sha3_256_Update
    \sa wc_Sha3_256_Final
*/
int wc_InitSha3_256(wc_Sha3* sha3, void* heap, int devId);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_256(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_256 failed");
    }
    else {
        wc_Sha3_256_Update(sha3, data, len);
        wc_Sha3_256_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_256Hash
    \sa wc_Sha3_256_Final
    \sa wc_InitSha3_256
*/
int wc_Sha3_256_Update(wc_Sha3* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha3 struct.

    \return 0 Returned upon successfully finalizing.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_256(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_256 failed");
    }
    else {
       wc_Sha3_256_Update(sha3, data, len);
       wc_Sha3_256_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_256Hash
    \sa wc_Sha3_256_GetHash
    \sa wc_InitSha3_256
*/
int wc_Sha3_256_Final(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Resets the wc_Sha3 structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param sha3 Pointer to the sha3 structure to be freed.

    _Example_
    \code
    wc_Sha3 sha3;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_256(&sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_256 failed");
    }
    else {
        wc_Sha3_256_Update(&sha3, data, len);
        wc_Sha3_256_Final(&sha3, hash);
        wc_Sha3_256_Free(&sha3);
    }
    \endcode

    \sa wc_InitSha3_256
    \sa wc_Sha3_256_Update
    \sa wc_Sha3_256_Final
*/
void wc_Sha3_256_Free(wc_Sha3*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of sha3 struct.

    \return 0 Returned upon successful copying of the hash.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_256(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_256 failed");
    }
    else {
       wc_Sha3_256_Update(sha3, data, len);
       wc_Sha3_256_GetHash(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_256Hash
    \sa wc_Sha3_256_Final
    \sa wc_InitSha3_256
    \sa wc_Sha3_256_Copy
*/
int wc_Sha3_256_GetHash(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Copy the state of the hash.

    \return 0 Returned upon successful copying.

    \param sha3 pointer to the sha3 structure to copy
    \param dst  pointer to the sha3 structure to copy into

    _Example_
    \code
    wc_Sha3 sha3[1];
    wc_Sha3 sha3_dup[1];
    if ((ret = wc_InitSha3_256(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_256 failed");
    }
    else {
       wc_Sha3_256_Update(sha3, data, len);
       wc_Sha3_256_Copy(sha3, sha3_dup);
    }
    \endcode

    \sa wc_Sha3_256Hash
    \sa wc_Sha3_256_Final
    \sa wc_InitSha3_256
    \sa wc_Sha3_256_GetHash
*/
int wc_Sha3_256_Copy(wc_Sha3* sha3, wc_Sha3* dst);

/*!
    \ingroup SHA

    \brief This function initializes SHA3-384. This is automatically
    called by wc_Sha3_384Hash.

    \return 0 Returned upon successfully initializing

    \param sha3 pointer to the sha3 structure to use for encryption

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_384(sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_384 failed");
    }
    else {
        wc_Sha3_384_Update(sha3, data, len);
        wc_Sha3_384_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_384Hash
    \sa wc_Sha3_384_Update
    \sa wc_Sha3_384_Final
*/
int wc_InitSha3_384(wc_Sha3* sha3, void* heap, int devId);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_384(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_384 failed");
    }
    else {
        wc_Sha3_384_Update(sha3, data, len);
        wc_Sha3_384_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_384Hash
    \sa wc_Sha3_384_Final
    \sa wc_InitSha3_384
*/
int wc_Sha3_384_Update(wc_Sha3* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha3 struct.

    \return 0 Returned upon successfully finalizing.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_384(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_384 failed");
    }
    else {
       wc_Sha3_384_Update(sha3, data, len);
       wc_Sha3_384_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_384Hash
    \sa wc_Sha3_384_GetHash
    \sa wc_InitSha3_384
*/
int wc_Sha3_384_Final(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Resets the wc_Sha3 structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param sha3 Pointer to the sha3 structure to be freed.

    _Example_
    \code
    wc_Sha3 sha3;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_384(&sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_384 failed");
    }
    else {
        wc_Sha3_384_Update(&sha3, data, len);
        wc_Sha3_384_Final(&sha3, hash);
        wc_Sha3_384_Free(&sha3);
    }
    \endcode

    \sa wc_InitSha3_384
    \sa wc_Sha3_384_Update
    \sa wc_Sha3_384_Final
*/
void wc_Sha3_384_Free(wc_Sha3*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of sha3 struct.

    \return 0 Returned upon successful copying of the hash.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_384(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_38384ailed");
    }
    else {
       wc_Sha3_384_Update(sha3, data, len);
       wc_Sha3_384_GetHash(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_384Hash
    \sa wc_Sha3_384_Final
    \sa wc_InitSha3_384
    \sa wc_Sha3_384_Copy
*/
int wc_Sha3_384_GetHash(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Copy the state of the hash.

    \return 0 Returned upon successful copying.

    \param sha3 pointer to the sha3 structure to copy
    \param dst  pointer to the sha3 structure to copy into

    _Example_
    \code
    wc_Sha3 sha3[1];
    wc_Sha3 sha3_dup[1];
    if ((ret = wc_InitSha3_384(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_384 failed");
    }
    else {
       wc_Sha3_384_Update(sha3, data, len);
       wc_Sha3_384_Copy(sha3, sha3_dup);
    }
    \endcode

    \sa wc_Sha3_384Hash
    \sa wc_Sha3_384_Final
    \sa wc_InitSha3_384
    \sa wc_Sha3_384_GetHash
*/
int wc_Sha3_384_Copy(wc_Sha3* sha3, wc_Sha3* dst);

/*!
    \ingroup SHA

    \brief This function initializes SHA3-512. This is automatically
    called by wc_Sha3_512Hash.

    \return 0 Returned upon successfully initializing

    \param sha3 pointer to the sha3 structure to use for encryption

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_512(sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_512 failed");
    }
    else {
        wc_Sha3_512_Update(sha3, data, len);
        wc_Sha3_512_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_512Hash
    \sa wc_Sha3_512_Update
    \sa wc_Sha3_512_Final
*/
int wc_InitSha3_512(wc_Sha3* sha3, void* heap, int devId);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_512(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_512 failed");
    }
    else {
        wc_Sha3_512_Update(sha3, data, len);
        wc_Sha3_512_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_512Hash
    \sa wc_Sha3_512_Final
    \sa wc_InitSha3_512
*/
int wc_Sha3_512_Update(wc_Sha3* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha3 struct.

    \return 0 Returned upon successfully finalizing.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_512(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_512 failed");
    }
    else {
       wc_Sha3_512_Update(sha3, data, len);
       wc_Sha3_512_Final(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_512Hash
    \sa wc_Sha3_512_GetHash
    \sa wc_InitSha3_512
*/
int wc_Sha3_512_Final(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Resets the wc_Sha3 structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param sha3 Pointer to the sha3 structure to be freed.

    _Example_
    \code
    wc_Sha3 sha3;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha3_512(&sha3, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitSha3_512 failed");
    }
    else {
        wc_Sha3_512_Update(&sha3, data, len);
        wc_Sha3_512_Final(&sha3, hash);
        wc_Sha3_512_Free(&sha3);
    }
    \endcode

    \sa wc_InitSha3_512
    \sa wc_Sha3_512_Update
    \sa wc_Sha3_512_Final
*/
void wc_Sha3_512_Free(wc_Sha3*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of sha3 struct.

    \return 0 Returned upon successful copying of the hash.

    \param sha3 pointer to the sha3 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Sha3 sha3[1];
    if ((ret = wc_InitSha3_512(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_512 failed");
    }
    else {
       wc_Sha3_512_Update(sha3, data, len);
       wc_Sha3_512_GetHash(sha3, hash);
    }
    \endcode

    \sa wc_Sha3_512Hash
    \sa wc_Sha3_512_Final
    \sa wc_InitSha3_512
    \sa wc_Sha3_512_Copy
*/
int wc_Sha3_512_GetHash(wc_Sha3* sha3, byte* hash);

/*!
    \ingroup SHA

    \brief Copy the state of the hash.

    \return 0 Returned upon successful copying.

    \param sha3 pointer to the sha3 structure to copy
    \param dst  pointer to the sha3 structure to copy into

    _Example_
    \code
    wc_Sha3 sha3[1];
    wc_Sha3 sha3_dup[1];
    if ((ret = wc_InitSha3_512(sha3, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitSha3_512 failed");
    }
    else {
       wc_Sha3_512_Update(sha3, data, len);
       wc_Sha3_512_Copy(sha3, sha3_dup);
    }
    \endcode

    \sa wc_Sha3_512Hash
    \sa wc_Sha3_512_Final
    \sa wc_InitSha3_512
    \sa wc_Sha3_512_GetHash
*/
int wc_Sha3_512_Copy(wc_Sha3* sha3, wc_Sha3* dst);

/*!
    \ingroup SHA

    \brief This function initializes SHAKE-128. This is automatically
    called by wc_Shake128Hash.

    \return 0 Returned upon successfully initializing

    \param shake pointer to the shake structure to use for encryption

    _Example_
    \code
    wc_Shake shake[1];
    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
        wc_Shake128_Update(shake, data, len);
        wc_Shake128_Final(shake, hash);
    }
    \endcode

    \sa wc_Shake128Hash
    \sa wc_Shake128_Update
    \sa wc_Shake128_Final
*/
int wc_InitShake128(wc_Shake* shake, void* heap, int devId);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param shake pointer to the shake structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
        wc_Shake128_Update(shake, data, len);
        wc_Shake128_Final(shake, hash);
    }
    \endcode

    \sa wc_Shake128Hash
    \sa wc_Shake128_Final
    \sa wc_InitShake128
*/
int wc_Shake128_Update(wc_Shake* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of shake struct.

    \return 0 Returned upon successfully finalizing.

    \param shake pointer to the shake structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
       wc_Shake128_Update(shake, data, len);
       wc_Shake128_Final(shake, hash);
    }
    \endcode

    \sa wc_Shake128Hash
    \sa wc_Shake128_GetHash
    \sa wc_InitShake128
*/
int wc_Shake128_Final(wc_Shake* shake, byte* hash);

/*!
    \ingroup SHA

    \brief Called to absorb the provided byte array of length len. Cannot
    be called incrementally.

    \return 0 Returned upon successfully absorbed the data.

    \param shake pointer to the shake structure to use for encryption
    \param data the data to be absorbed
    \param len length of data to be absorbed

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);
    byte out[2 * WC_SHA3_128_BLOCK_SIZE];
    int blocks = 2;

    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
       wc_Shake128_Absorb(shake, data, len);
       wc_Shake128_SqueezeBlocks(shake, out, blocks);
    }
    \endcode

    \sa wc_Shake128_SqueezeBlocks
    \sa wc_InitShake128
*/
int wc_Shake128_Absorb(wc_Shake* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Squeeze out more blocks of data. Result is placed into out. Can be
    called inrementally.

    \return 0 Returned upon successfully squeezing.

    \param shake pointer to the shake structure to use for encryption
    \param hash Byte array to hold output.
    \param blocks Number of blocks to squeeze out. Each block is
    WC_SHA3_128_BLOCK_SIZE bytes in length.

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);
    byte out[2 * WC_SHA3_128_BLOCK_SIZE];
    int blocks = 2;

    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
       wc_Shake128_Absorb(shake, data, len);
       wc_Shake128_SqueezeBlocks(shake, out, blocks);
    }
    \endcode

    \sa wc_Shake128_Absorb
    \sa wc_InitShake128
*/
int wc_Shake128_SqueezeBlocks(wc_Shake* shake, byte* out, word32 blockCnt);

/*!
    \ingroup SHA

    \brief Resets the wc_Shake structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param shake Pointer to the shake structure to be freed.

    _Example_
    \code
    wc_Shake shake;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitShake128(&shake, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
        wc_Shake128_Update(&shake, data, len);
        wc_Shake128_Final(&shake, hash);
        wc_Shake128_Free(&shake);
    }
    \endcode

    \sa wc_InitShake128
    \sa wc_Shake128_Update
    \sa wc_Shake128_Final
*/
void wc_Shake128_Free(wc_Shake*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of shake struct.

    \return 0 Returned upon successful copying of the hash.

    \param shake pointer to the shake structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Shake shake[1];
    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
       wc_Shake128_Update(shake, data, len);
       wc_Shake128_GetHash(shake, hash);
    }
    \endcode

    \sa wc_Shake128Hash
    \sa wc_Shake128_Final
    \sa wc_InitShake128
    \sa wc_Shake128_Copy
*/
int wc_Shake128_GetHash(wc_Shake* shake, byte* hash);

/*!
    \ingroup SHA

    \brief Copy the state of the hash.

    \return 0 Returned upon successful copying.

    \param shake pointer to the shake structure to copy
    \param dst  pointer to the shake structure to copy into

    _Example_
    \code
    wc_Shake shake[1];
    wc_Shake shake_dup[1];
    if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake128 failed");
    }
    else {
       wc_Shake128_Update(shake, data, len);
       wc_Shake128_Copy(shake, shake_dup);
    }
    \endcode

    \sa wc_Shake128Hash
    \sa wc_Shake128_Final
    \sa wc_InitShake128
    \sa wc_Shake128_GetHash
*/
int wc_Shake128_Copy(wc_Shake* shake, wc_Shake* dst);

/*!
    \ingroup SHA

    \brief This function initializes SHAKE-256. This is automatically
    called by wc_Shake256Hash.

    \return 0 Returned upon successfully initializing

    \param shake pointer to the shake structure to use for encryption

    _Example_
    \code
    wc_Shake shake[1];
    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
        wc_Shake256_Update(shake, data, len);
        wc_Shake256_Final(shake, hash);
    }
    \endcode

    \sa wc_Shake256Hash
    \sa wc_Shake256_Update
    \sa wc_Shake256_Final
*/
int wc_InitShake256(wc_Shake* shake, void* heap, int devId);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param shake pointer to the shake structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
        wc_Shake256_Update(shake, data, len);
        wc_Shake256_Final(shake, hash);
    }
    \endcode

    \sa wc_Shake256Hash
    \sa wc_Shake256_Final
    \sa wc_InitShake256
*/
int wc_Shake256_Update(wc_Shake* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of shake struct.

    \return 0 Returned upon successfully finalizing.

    \param shake pointer to the shake structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
       wc_Shake256_Update(shake, data, len);
       wc_Shake256_Final(shake, hash);
    }
    \endcode

    \sa wc_Shake256Hash
    \sa wc_Shake256_GetHash
    \sa wc_InitShake256
*/
int wc_Shake256_Final(wc_Shake* shake, byte* hash);

/*!
    \ingroup SHA

    \brief Called to absorb the provided byte array of length len. Cannot
    be called incrementally.

    \return 0 Returned upon successfully absorbed the data.

    \param shake pointer to the shake structure to use for encryption
    \param data the data to be absorbed
    \param len length of data to be absorbed

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);
    byte out[2 * WC_SHA3_256_BLOCK_SIZE];
    int blocks = 2;

    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
       wc_Shake256_Absorb(shake, data, len);
       wc_Shake256_SqueezeBlocks(shake, out, blocks);
    }
    \endcode

    \sa wc_Shake256_SqueezeBlocks
    \sa wc_InitShake256
*/
int wc_Shake256_Absorb(wc_Shake* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Squeeze out more blocks of data. Result is placed into out. Can be
    called incrementally.

    \return 0 Returned upon successfully squeezing.

    \param shake pointer to the shake structure to use for encryption
    \param hash Byte array to hold output.
    \param blocks Number of blocks to squeeze out. Each block is
    WC_SHA3_256_BLOCK_SIZE bytes in length.

    _Example_
    \code
    wc_Shake shake[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);
    byte out[2 * WC_SHA3_256_BLOCK_SIZE];
    int blocks = 2;

    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
       wc_Shake256_Absorb(shake, data, len);
       wc_Shake256_SqueezeBlocks(shake, out, blocks);
    }
    \endcode

    \sa wc_Shake256_Absorb
    \sa wc_InitShake256
*/
int wc_Shake256_SqueezeBlocks(wc_Shake* shake, byte* out, word32 blockCnt);

/*!
    \ingroup SHA

    \brief Resets the wc_Shake structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param shake Pointer to the shake structure to be freed.

    _Example_
    \code
    wc_Shake shake;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitShake256(&shake, NULL, INVALID_DEVID)) != 0) {
        WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
        wc_Shake256_Update(&shake, data, len);
        wc_Shake256_Final(&shake, hash);
        wc_Shake256_Free(&shake);
    }
    \endcode

    \sa wc_InitShake256
    \sa wc_Shake256_Update
    \sa wc_Shake256_Final
*/
void wc_Shake256_Free(wc_Shake*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of shake struct.

    \return 0 Returned upon successful copying of the hash.

    \param shake pointer to the shake structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    wc_Shake shake[1];
    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
       wc_Shake256_Update(shake, data, len);
       wc_Shake256_GetHash(shake, hash);
    }
    \endcode

    \sa wc_Shake256Hash
    \sa wc_Shake256_Final
    \sa wc_InitShake256
    \sa wc_Shake256_Copy
*/
int wc_Shake256_GetHash(wc_Shake* shake, byte* hash);

/*!
    \ingroup SHA

    \brief Copy the state of the hash.

    \return 0 Returned upon successful copying.

    \param shake pointer to the shake structure to copy
    \param dst  pointer to the shake structure to copy into

    _Example_
    \code
    wc_Shake shake[1];
    wc_Shake shake_dup[1];
    if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
       WOLFSSL_MSG("wc_InitShake256 failed");
    }
    else {
       wc_Shake256_Update(shake, data, len);
       wc_Shake256_Copy(shake, shake_dup);
    }
    \endcode

    \sa wc_Shake256Hash
    \sa wc_Shake256_Final
    \sa wc_InitShake256
    \sa wc_Shake256_GetHash
*/
int wc_Shake256_Copy(wc_Shake* shake, wc_Shake* dst);


