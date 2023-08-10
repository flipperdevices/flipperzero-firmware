/*!
    \ingroup MD2

    \brief This function initializes md2. This is automatically
    called by wc_Md2Hash.

    \return 0 Returned upon successfully initializing

    \param md2 pointer to the md2 structure to use for encryption

    _Example_
    \code
    md2 md2[1];
    if ((ret = wc_InitMd2(md2)) != 0) {
       WOLFSSL_MSG("wc_Initmd2 failed");
    }
    else {
       wc_Md2Update(md2, data, len);
       wc_Md2Final(md2, hash);
    }
    \endcode

    \sa wc_Md2Hash
    \sa wc_Md2Update
    \sa wc_Md2Final
*/
void wc_InitMd2(Md2*);

/*!
    \ingroup MD2

    \brief Can be called to continually hash the provided byte
    array of length len.

    \return 0 Returned upon successfully adding the data to the digest.

    \param md2 pointer to the md2 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    md2 md2[1];
    byte data[] = { }; // Data to be hashed
    word32 len = sizeof(data);

    if ((ret = wc_InitMd2(md2)) != 0) {
       WOLFSSL_MSG("wc_Initmd2 failed");
    }
    else {
       wc_Md2Update(md2, data, len);
       wc_Md2Final(md2, hash);
    }
    \endcode

    \sa wc_Md2Hash
    \sa wc_Md2Final
    \sa wc_InitMd2
*/
void wc_Md2Update(Md2* md2, const byte* data, word32 len);

/*!
    \ingroup MD2

    \brief Finalizes hashing of data. Result is placed into hash.

    \return 0 Returned upon successfully finalizing.

    \param md2 pointer to the md2 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    md2 md2[1];
    byte data[] = { }; // Data to be hashed
    word32 len = sizeof(data);

    if ((ret = wc_InitMd2(md2)) != 0) {
       WOLFSSL_MSG("wc_Initmd2 failed");
    }
    else {
       wc_Md2Update(md2, data, len);
       wc_Md2Final(md2, hash);
    }
    \endcode

    \sa wc_Md2Hash
    \sa wc_Md2Final
    \sa wc_InitMd2
*/
void wc_Md2Final(Md2* md2, byte* hash);

/*!
    \ingroup MD2

    \brief Convenience function, handles all the hashing and places
    the result into hash.

    \return 0 Returned upon successfully hashing the data.
    \return Memory_E memory error, unable to allocate memory. This is only
    possible with the small stack option enabled.

    \param data the data to hash
    \param len the length of data
    \param hash Byte array to hold hash value.

    _Example_
    \code
    none
    \endcode

    \sa wc_Md2Hash
    \sa wc_Md2Final
    \sa wc_InitMd2
*/
int  wc_Md2Hash(const byte* data, word32 len, byte* hash);
