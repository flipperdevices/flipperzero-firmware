/*!
    \ingroup MD4

    \brief This function initializes md4. This is automatically
    called by wc_Md4Hash.

    \return 0 Returned upon successfully initializing

    \param md4 pointer to the md4 structure to use for encryption

    _Example_
    \code
    md4 md4[1];
    if ((ret = wc_InitMd4(md4)) != 0) {
       WOLFSSL_MSG("wc_Initmd4 failed");
    }
    else {
       wc_Md4Update(md4, data, len);
       wc_Md4Final(md4, hash);
    }
    \endcode

    \sa wc_Md4Hash
    \sa wc_Md4Update
    \sa wc_Md4Final
*/
void wc_InitMd4(Md4*);

/*!
    \ingroup MD4

    \brief Can be called to continually hash the provided byte array
    of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param md4 pointer to the md4 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    md4 md4[1];
    byte data[] = { }; // Data to be hashed
    word32 len = sizeof(data);

    if ((ret = wc_InitMd4(md4)) != 0) {
       WOLFSSL_MSG("wc_Initmd4 failed");
    }
    else {
       wc_Md4Update(md4, data, len);
       wc_Md4Final(md4, hash);
    }
    \endcode

    \sa wc_Md4Hash
    \sa wc_Md4Final
    \sa wc_InitMd4
*/
void wc_Md4Update(Md4* md4, const byte* data, word32 len);

/*!
    \ingroup MD4

    \brief Finalizes hashing of data. Result is placed into hash.

    \return 0 Returned upon successfully finalizing.

    \param md4 pointer to the md4 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    md4 md4[1];
    if ((ret = wc_InitMd4(md4)) != 0) {
        WOLFSSL_MSG("wc_Initmd4 failed");
    }
    else {
        wc_Md4Update(md4, data, len);
        wc_Md4Final(md4, hash);
    }
    \endcode

    \sa wc_Md4Hash
    \sa wc_Md4Final
    \sa wc_InitMd4
*/
void wc_Md4Final(Md4* md4, byte* hash);
