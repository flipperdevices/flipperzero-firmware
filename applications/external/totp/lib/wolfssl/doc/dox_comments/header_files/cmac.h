/*!
    \ingroup CMAC
    \brief Initialize the Cmac structure with defaults
    \return 0 on success
    \param cmac pointer to the Cmac structure
    \param key key pointer
    \param keySz size of the key pointer (16, 24 or 32)
    \param type Always WC_CMAC_AES = 1
    \param unused not used, exists for potential future use around compatibility

    _Example_
    \code
    Cmac cmac[1];
    ret = wc_InitCmac(cmac, key, keySz, WC_CMAC_AES, NULL);
    if (ret == 0) {
        ret = wc_CmacUpdate(cmac, in, inSz);
    }
    if (ret == 0) {
        ret = wc_CmacFinal(cmac, out, outSz);
    }
    \endcode

    \sa wc_InitCmac_ex
    \sa wc_CmacUpdate
    \sa wc_CmacFinal
*/
int wc_InitCmac(Cmac* cmac,
                const byte* key, word32 keySz,
                int type, void* unused);

/*!
    \ingroup CMAC
    \brief Initialize the Cmac structure with defaults
    \return 0 on success
    \param cmac pointer to the Cmac structure
    \param key key pointer
    \param keySz size of the key pointer (16, 24 or 32)
    \param type Always WC_CMAC_AES = 1
    \param unused not used, exists for potential future use around compatibility
    \param heap pointer to the heap hint used for dynamic allocation. Typically used with our static memory option. Can be NULL.
    \param devId ID to use with async hardware. Set to INVALID_DEVID if not using async hardware.

    _Example_
    \code
    Cmac cmac[1];
    ret = wc_InitCmac_ex(cmac, key, keySz, WC_CMAC_AES, NULL, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_CmacUpdate(cmac, in, inSz);
    }
    if (ret == 0) {
        ret = wc_CmacFinal(cmac, out, &outSz);
    }
    \endcode

    \sa wc_InitCmac_ex
    \sa wc_CmacUpdate
    \sa wc_CmacFinal
*/
int wc_InitCmac_ex(Cmac* cmac,
                const byte* key, word32 keySz,
                int type, void* unused, void* heap, int devId);

/*!
    \ingroup CMAC
    \brief Add Cipher-based Message Authentication Code input data
    \return 0 on success
    \param cmac pointer to the Cmac structure
    \param in input data to process
    \param inSz size of input data

    _Example_
    \code
    ret = wc_CmacUpdate(cmac, in, inSz);
    \endcode

    \sa wc_InitCmac
    \sa wc_CmacFinal
*/
int wc_CmacUpdate(Cmac* cmac,
                  const byte* in, word32 inSz);

/*!
    \ingroup CMAC
    \brief Generate the final result using Cipher-based Message Authentication Code
    \return 0 on success
    \param cmac pointer to the Cmac structure
    \param out pointer to return the result
    \param outSz pointer size of output (in/out)

    _Example_
    \code
    ret = wc_CmacFinal(cmac, out, &outSz);
    \endcode

    \sa wc_InitCmac
    \sa wc_CmacFinal
*/
int wc_CmacFinal(Cmac* cmac,
                 byte* out, word32* outSz);

/*!
    \ingroup CMAC
    \brief Single shot function for generating a CMAC
    \return 0 on success
    \param out pointer to return the result
    \param outSz pointer size of output (in/out)
    \param in input data to process
    \param inSz size of input data
    \param key key pointer
    \param keySz size of the key pointer (16, 24 or 32)

    _Example_
    \code
    ret = wc_AesCmacGenerate(mac, &macSz, msg, msgSz, key, keySz);
    \endcode

    \sa wc_AesCmacVerify
*/
int wc_AesCmacGenerate(byte* out, word32* outSz,
                       const byte* in, word32 inSz,
                       const byte* key, word32 keySz);

/*!
    \ingroup CMAC
    \brief Single shot function for validating a CMAC
    \return 0 on success
    \param check pointer to return the result
    \param checkSz size of checkout buffer
    \param in input data to process
    \param inSz size of input data
    \param key key pointer
    \param keySz size of the key pointer (16, 24 or 32)

    _Example_
    \code
    ret = wc_AesCmacVerify(mac, macSz, msg, msgSz, key, keySz);
    \endcode

    \sa wc_AesCmacGenerate
*/
int wc_AesCmacVerify(const byte* check, word32 checkSz,
                     const byte* in, word32 inSz,
                     const byte* key, word32 keySz);


/*!
    \ingroup CMAC
    \brief Only used with WOLFSSL_HASH_KEEP when hardware requires single-shot and the updates must be cached in memory
    \return 0 on success
    \param in input data to process
    \param inSz size of input data

    _Example_
    \code
    ret = wc_CMAC_Grow(cmac, in, inSz)
    \endcode
*/
int wc_CMAC_Grow(Cmac* cmac, const byte* in, int inSz);
