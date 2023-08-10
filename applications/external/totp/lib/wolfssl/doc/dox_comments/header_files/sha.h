/*!
    \ingroup SHA

    \brief This function initializes SHA. This is automatically called
    by wc_ShaHash.

    \return 0 Returned upon successfully initializing

    \param sha pointer to the sha structure to use for encryption

    _Example_
    \code
    Sha sha[1];
    if ((ret = wc_InitSha(sha)) != 0) {
       WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
       wc_ShaUpdate(sha, data, len);
       wc_ShaFinal(sha, hash);
    }
    \endcode

    \sa wc_ShaHash
    \sa wc_ShaUpdate
    \sa wc_ShaFinal
*/
int wc_InitSha(wc_Sha*);

/*!
    \ingroup SHA

    \brief Can be called to continually hash the provided byte array of
    length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param sha pointer to the sha structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    Sha sha[1];
    byte data[] = { // Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha(sha)) != 0) {
       WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
       wc_ShaUpdate(sha, data, len);
       wc_ShaFinal(sha, hash);
    }
    \endcode

    \sa wc_ShaHash
    \sa wc_ShaFinal
    \sa wc_InitSha
*/
int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len);

/*!
    \ingroup SHA

    \brief Finalizes hashing of data. Result is placed into hash.
    Resets state of sha struct.

    \return 0 Returned upon successfully finalizing.

    \param sha pointer to the sha structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha sha[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha(sha)) != 0) {
       WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
       wc_ShaUpdate(sha, data, len);
       wc_ShaFinal(sha, hash);
    }
    \endcode

    \sa wc_ShaHash
    \sa wc_InitSha
    \sa wc_ShaGetHash
*/
int wc_ShaFinal(wc_Sha* sha, byte* hash);

/*!
    \ingroup SHA

    \brief Used to clean up memory used by an initialized Sha struct.
    Note: this is only supported if you have WOLFSSL_TI_HASH defined.

    \return No returns.

    \param sha Pointer to the Sha struct to free.

    _Example_
    \code
    Sha sha;
    wc_InitSha(&sha);
    // Use sha
    wc_ShaFree(&sha);
    \endcode

    \sa wc_InitSha
    \sa wc_ShaUpdate
    \sa wc_ShaFinal
*/
void wc_ShaFree(wc_Sha*);

/*!
    \ingroup SHA

    \brief Gets hash data. Result is placed into hash.  Does not reset state
    of sha struct.

    \return 0 Returned upon successfully finalizing.

    \param sha pointer to the sha structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    Sha sha[1];
    if ((ret = wc_InitSha(sha)) != 0) {
    WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
        wc_ShaUpdate(sha, data, len);
        wc_ShaGetHash(sha, hash);
    }
    \endcode

    \sa wc_ShaHash
    \sa wc_ShaFinal
    \sa wc_InitSha
*/
int wc_ShaGetHash(wc_Sha* sha, byte* hash);
