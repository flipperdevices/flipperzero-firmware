/*!
    \ingroup SHA

    \brief This function initializes SHA256. This is automatically
    called by wc_Sha256Hash.

    \return 0 Returned upon successfully initializing

    \param sha256 pointer to the sha256 structure to use for encryption

    _Example_
    \code
    Sha256 sha256[1];
    if ((ret = wc_InitSha256(sha256)) != 0) {
        WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
        wc_Sha256Update(sha256, data, len);
        wc_Sha256Final(sha256, hash);
    }
    \endcode

    \sa wc_Sha256Hash
    \sa wc_Sha256Update
    \sa wc_Sha256Final
*/
int wc_InitSha256(wc_Sha256*);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha256 pointer to the sha256 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    Sha256 sha256[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha256(sha256)) != 0) {
       WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
        wc_Sha256Update(sha256, data, len);
        wc_Sha256Final(sha256, hash);
    }
    \endcode

    \sa wc_Sha256Hash
    \sa wc_Sha256Final
    \sa wc_InitSha256
*/
int wc_Sha256Update(wc_Sha256* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha256 struct.

    \return 0 Returned upon successfully finalizing.

    \param sha256 pointer to the sha256 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha256 sha256[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha256(sha256)) != 0) {
       WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
       wc_Sha256Update(sha256, data, len);
       wc_Sha256Final(sha256, hash);
    }
    \endcode

    \sa wc_Sha256Hash
    \sa wc_Sha256GetHash
    \sa wc_InitSha256
*/
int wc_Sha256Final(wc_Sha256* sha256, byte* hash);

/*!
    \ingroup SHA

    \brief Resets the Sha256 structure.  Note: this is only supported
    if you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param sha256 Pointer to the sha256 structure to be freed.

    _Example_
    \code
    Sha256 sha256;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha256(&sha256)) != 0) {
        WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
        wc_Sha256Update(&sha256, data, len);
        wc_Sha256Final(&sha256, hash);
        wc_Sha256Free(&sha256);
    }
    \endcode

    \sa wc_InitSha256
    \sa wc_Sha256Update
    \sa wc_Sha256Final
*/
void wc_Sha256Free(wc_Sha256*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not
    reset state of sha256 struct.

    \return 0 Returned upon successfully finalizing.

    \param sha256 pointer to the sha256 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha256 sha256[1];
    if ((ret = wc_InitSha256(sha256)) != 0) {
       WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
       wc_Sha256Update(sha256, data, len);
       wc_Sha256GetHash(sha256, hash);
    }
    \endcode

    \sa wc_Sha256Hash
    \sa wc_Sha256Final
    \sa wc_InitSha256
*/
int wc_Sha256GetHash(wc_Sha256* sha256, byte* hash);

/*!
    \ingroup SHA

    \brief Used to initialize a Sha224 struct.

    \return 0 Success
    \return 1 Error returned because sha224 is null.

    \param sha224 Pointer to a Sha224 struct to initialize.

    _Example_
    \code
    Sha224 sha224;
    if(wc_InitSha224(&sha224) != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_Sha224Hash
    \sa wc_Sha224Update
    \sa wc_Sha224Final
*/
int wc_InitSha224(wc_Sha224*);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte array
    of length len.

    \return 0 Success
    \return 1 Error returned if function fails.
    \return BAD_FUNC_ARG Error returned if sha224 or data is null.

    \param sha224 Pointer to the Sha224 structure to use for encryption.
    \param data Data to be hashed.
    \param len Length of data to be hashed.

    _Example_
    \code
    Sha224 sha224;
    byte data[] = { /* Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha224(&sha224)) != 0) {
       WOLFSSL_MSG("wc_InitSha224 failed");
    }
    else {
      wc_Sha224Update(&sha224, data, len);
      wc_Sha224Final(&sha224, hash);
    }
    \endcode

    \sa wc_InitSha224
    \sa wc_Sha224Final
    \sa wc_Sha224Hash
*/
int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha224 struct.

    \return 0 Success
    \return <0 Error

    \param sha224 pointer to the sha224 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha224 sha224;
    byte data[] = { /* Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha224(&sha224)) != 0) {
        WOLFSSL_MSG("wc_InitSha224 failed");
    }
    else {
        wc_Sha224Update(&sha224, data, len);
        wc_Sha224Final(&sha224, hash);
    }
    \endcode

    \sa wc_InitSha224
    \sa wc_Sha224Hash
    \sa wc_Sha224Update
*/
int wc_Sha224Final(wc_Sha224* sha224, byte* hash);
