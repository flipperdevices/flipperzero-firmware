/*!
    \ingroup MD5

    \brief This function initializes md5. This is automatically
    called by wc_Md5Hash.

    \return 0 Returned upon successfully initializing.
    \return BAD_FUNC_ARG Returned if the Md5 structure is passed
    as a NULL value.

    \param md5 pointer to the md5 structure to use for encryption

    _Example_
    \code
    Md5 md5;
    byte* hash;
    if ((ret = wc_InitMd5(&md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       ret = wc_Md5Update(&md5, data, len);
       if (ret != 0) {
    	 // Md5 Update Failure Case.
       }
       ret = wc_Md5Final(&md5, hash);
      if (ret != 0) {
    	// Md5 Final Failure Case.
      }
    }
    \endcode

    \sa wc_Md5Hash
    \sa wc_Md5Update
    \sa wc_Md5Final
*/
int wc_InitMd5(wc_Md5*);

/*!
    \ingroup MD5

    \brief Can be called to continually hash the provided byte array of
    length len.

    \return 0 Returned upon successfully adding the data to the digest.
    \return BAD_FUNC_ARG Returned if the Md5 structure is NULL or if
    data is NULL and len is greater than zero. The function should
    not return an error if the data parameter is NULL and len is zero.

    \param md5 pointer to the md5 structure to use for encryption
    \param data the data to be hashed
    \param len length of data to be hashed

    _Example_
    \code
    Md5 md5;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitMd5(&md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       ret = wc_Md5Update(&md5, data, len);
       if (ret != 0) {
    	 // Md5 Update Error Case.
       }
       ret = wc_Md5Final(&md5, hash);
       if (ret != 0) {
    	// Md5 Final Error Case.
       }
    }
    \endcode

    \sa wc_Md5Hash
    \sa wc_Md5Final
    \sa wc_InitMd5
*/
int wc_Md5Update(wc_Md5* md5, const byte* data, word32 len);

/*!
    \ingroup MD5

    \brief Finalizes hashing of data. Result is placed into hash. Md5
    Struct is reset. Note: This function will also return the result
    of calling IntelQaSymMd5() in the case that HAVE_INTEL_QA is defined.

    \return 0 Returned upon successfully finalizing.
    \return BAD_FUNC_ARG Returned if the Md5 structure or hash pointer
    is passed in NULL.

    \param md5 pointer to the md5 structure to use for encryption
    \param hash Byte array to hold hash value.

    _Example_
    \code
    md5 md5[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitMd5(md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       ret = wc_Md5Update(md5, data, len);
       if (ret != 0) {
    	// Md5 Update Failure Case.
       }
      ret = wc_Md5Final(md5, hash);
       if (ret != 0) {
	    // Md5 Final Failure Case.
       }
    }
    \endcode

    \sa wc_Md5Hash
    \sa wc_InitMd5
    \sa wc_Md5GetHash
*/
int wc_Md5Final(wc_Md5* md5, byte* hash);

/*!
    \ingroup MD5

    \brief Resets the Md5 structure.  Note: this is only supported if
    you have WOLFSSL_TI_HASH defined.

    \return none No returns.

    \param md5 Pointer to the Md5 structure to be reset.

    _Example_
    \code
    Md5 md5;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitMd5(&md5)) != 0) {
        WOLFSSL_MSG("wc_InitMd5 failed");
    }
    else {
        wc_Md5Update(&md5, data, len);
        wc_Md5Final(&md5, hash);
        wc_Md5Free(&md5);
    }
    \endcode

    \sa wc_InitMd5
    \sa wc_Md5Update
    \sa wc_Md5Final
*/
void wc_Md5Free(wc_Md5*);

/*!
    \ingroup MD5

    \brief Gets hash data. Result is placed into hash.  Md5 struct
    is not reset.

    \return none No returns

    \param md5 pointer to the md5 structure to use for encryption.
    \param hash Byte array to hold hash value.

    _Example_
    \code
    md5 md5[1];
    if ((ret = wc_InitMd5(md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       wc_Md5Update(md5, data, len);
       wc_Md5GetHash(md5, hash);
    }
    \endcode

    \sa wc_Md5Hash
    \sa wc_Md5Final
    \sa wc_InitMd5
*/
int  wc_Md5GetHash(wc_Md5* md5, byte* hash);
