
/*!
    \ingroup SipHash

    \brief This function initializes SipHash with a key for a MAC size.

    \return 0 Returned upon successfully initializing
    \return BAD_FUNC_ARG Returned when siphash or key is NULL
    \return BAD_FUNC_ARG Returned when outSz is neither 8 nor 16

    \param siphash pointer to the SipHash structure to use for MACing
    \param key pointer to the 16-byte array
    \param outSz number of bytes to output as MAC

    _Example_
    \code
    SipHash siphash[1];
    unsigned char key[16] = { ... };
    byte macSz = 8; // 8 or 16

    if ((ret = wc_InitSipHash(siphash, key, macSz)) != 0) {
        WOLFSSL_MSG("wc_InitSipHash failed");
    }
    else if ((ret = wc_SipHashUpdate(siphash, data, len)) != 0) {
        WOLFSSL_MSG("wc_SipHashUpdate failed");
    }
    else if ((ret = wc_SipHashFinal(siphash, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHashFinal failed");
    }
    \endcode

    \sa wc_SipHash
    \sa wc_SipHashUpdate
    \sa wc_SipHashFinal
*/
int wc_InitSipHash(SipHash* siphash, const unsigned char* key,
    unsigned char outSz);

/*!
    \ingroup SipHash

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the MAC
    \return BAD_FUNC_ARG Returned when siphash is NULL
    \return BAD_FUNC_ARG Returned when in is NULL and inSz is not zero

    \param siphash pointer to the SipHash structure to use for MACing
    \param in the data to be MACed
    \param inSz size of data to be MACed

    _Example_
    \code
    SipHash siphash[1];
    byte data[] = { Data to be MACed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSipHash(siphash, key, macSz)) != 0) {
        WOLFSSL_MSG("wc_InitSipHash failed");
    }
    else if ((ret = wc_SipHashUpdate(siphash, data, len)) != 0) {
        WOLFSSL_MSG("wc_SipHashUpdate failed");
    }
    else if ((ret = wc_SipHashFinal(siphash, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHashFinal failed");
    }
    \endcode

    \sa wc_SipHash
    \sa wc_InitSipHash
    \sa wc_SipHashFinal
*/
int wc_SipHashUpdate(SipHash* siphash, const unsigned char* in,
    word32 inSz);

/*!
    \ingroup SipHash

    \brief Finalizes MACing of data. Result is placed into out.

    \return 0 Returned upon successfully finalizing.
    \return BAD_FUNC_ARG Returned when siphash of out is NULL
    \return BAD_FUNC_ARG Returned when outSz is not the same as the initialized
    value

    \param siphash pointer to the SipHash structure to use for MACing
    \param out Byte array to hold MAC value
    \param outSz number of bytes to output as MAC

    _Example_
    \code
    SipHash siphash[1];
    byte mac[8] = { ... }; // 8 or 16 bytes
    byte macSz = sizeof(mac);

    if ((ret = wc_InitSipHash(siphash, key, macSz)) != 0) {
        WOLFSSL_MSG("wc_InitSipHash failed");
    }
    else if ((ret = wc_SipHashUpdate(siphash, data, len)) != 0) {
        WOLFSSL_MSG("wc_SipHashUpdate failed");
    }
    else if ((ret = wc_SipHashFinal(siphash, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHashFinal failed");
    }
    \endcode

    \sa wc_SipHash
    \sa wc_InitSipHash
    \sa wc_SipHashUpdate
*/
int wc_SipHashFinal(SipHash* siphash, unsigned char* out,
    unsigned char outSz);

/*!
    \ingroup SipHash

    \brief This function one-shots the data using SipHash to calculate a MAC
    based on the key.

    \return 0 Returned upon successfully MACing
    \return BAD_FUNC_ARG Returned when key or out is NULL
    \return BAD_FUNC_ARG Returned when in is NULL and inSz is not zero
    \return BAD_FUNC_ARG Returned when outSz is neither 8 nor 16

    \param key pointer to the 16-byte array
    \param in the data to be MACed
    \param inSz size of data to be MACed
    \param out Byte array to hold MAC value
    \param outSz number of bytes to output as MAC

    _Example_
    \code
    unsigned char key[16] = { ... };
    byte data[] = { Data to be MACed };
    word32 len = sizeof(data);
    byte mac[8] = { ... }; // 8 or 16 bytes
    byte macSz = sizeof(mac);

    if ((ret = wc_SipHash(key, data, len, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHash failed");
    }
    \endcode

    \sa wc_InitSipHash
    \sa wc_SipHashUpdate
    \sa wc_SipHashFinal
*/
int wc_SipHash(const unsigned char* key, const unsigned char* in,
    word32 inSz, unsigned char* out, unsigned char outSz);

