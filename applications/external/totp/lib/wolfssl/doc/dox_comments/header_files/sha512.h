/*!
    \ingroup SHA

    \brief This function initializes SHA512. This is automatically called
    by wc_Sha512Hash.

    \return 0 Returned upon successfully initializing

    \param sha512 pointer to the sha512 structure to use for encryption

    _Example_
    \code
    Sha512 sha512[1];
    if ((ret = wc_InitSha512(sha512)) != 0) {
       WOLFSSL_MSG("wc_InitSha512 failed");
    }
    else {
       wc_Sha512Update(sha512, data, len);
       wc_Sha512Final(sha512, hash);
    }
    \endcode

    \sa wc_Sha512Hash
    \sa wc_Sha512Update
    \sa wc_Sha512Final
*/
int wc_InitSha512(wc_Sha512*);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte array
    of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha512 pointer to the sha512 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    Sha512 sha512[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha512(sha512)) != 0) {
       WOLFSSL_MSG("wc_InitSha512 failed");
    }
    else {
       wc_Sha512Update(sha512, data, len);
       wc_Sha512Final(sha512, hash);
    }
    \endcode

    \sa wc_Sha512Hash
    \sa wc_Sha512Final
    \sa wc_InitSha512
*/
int wc_Sha512Update(wc_Sha512* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.

    \return 0 Returned upon successfully finalizing the hash.

    \param sha512 pointer to the sha512 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha512 sha512[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha512(sha512)) != 0) {
        WOLFSSL_MSG("wc_InitSha512 failed");
    }
    else {
       wc_Sha512Update(sha512, data, len);
       wc_Sha512Final(sha512, hash);
    }
    \endcode

    \sa wc_Sha512Hash
    \sa wc_Sha512Final
    \sa wc_InitSha512
*/
int wc_Sha512Final(wc_Sha512* sha512, byte* hash);

/*!
    \ingroup SHA

    \brief This function initializes SHA384. This is automatically called
    by wc_Sha384Hash.

    \return 0 Returned upon successfully initializing

    \param sha384 pointer to the sha384 structure to use for encryption

    _Example_
    \code
    Sha384 sha384[1];
    if ((ret = wc_InitSha384(sha384)) != 0) {
       WOLFSSL_MSG("wc_InitSha384 failed");
    }
    else {
       wc_Sha384Update(sha384, data, len);
       wc_Sha384Final(sha384, hash);
    }
    \endcode

    \sa wc_Sha384Hash
    \sa wc_Sha384Update
    \sa wc_Sha384Final
*/
int wc_InitSha384(wc_Sha384*);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte array
    of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha384 pointer to the sha384 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    Sha384 sha384[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha384(sha384)) != 0) {
       WOLFSSL_MSG("wc_InitSha384 failed");
    }
    else {
       wc_Sha384Update(sha384, data, len);
       wc_Sha384Final(sha384, hash);
    }
    \endcode

    \sa wc_Sha384Hash
    \sa wc_Sha384Final
    \sa wc_InitSha384
*/
int wc_Sha384Update(wc_Sha384* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.

    \return 0 Returned upon successfully finalizing.

    \param sha384 pointer to the sha384 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha384 sha384[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha384(sha384)) != 0) {
       WOLFSSL_MSG("wc_InitSha384 failed");
    }
    else {
       wc_Sha384Update(sha384, data, len);
       wc_Sha384Final(sha384, hash);
    }
    \endcode

    \sa wc_Sha384Hash
    \sa wc_Sha384Final
    \sa wc_InitSha384
*/
int wc_Sha384Final(wc_Sha384* sha384, byte* hash);
