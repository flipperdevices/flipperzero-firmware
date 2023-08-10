/*!
    \ingroup AES
    \brief This function initializes an AES structure by setting the key and
    then setting the initialization vector.

    \return 0 On successfully setting key and initialization vector.
    \return BAD_FUNC_ARG Returned if key length is invalid.

    \param aes pointer to the AES structure to modify
    \param key 16, 24, or 32 byte secret key for encryption and decryption
    \param len length of the key passed in
    \param iv pointer to the initialization vector used to initialize the key
    \param dir Cipher direction. Set AES_ENCRYPTION to encrypt,  or
    AES_DECRYPTION to decrypt. Direction for some modes (CFB and CTR) is
    always AES_ENCRYPTION.

    _Example_
    \code
    Aes enc;
    int ret = 0;
    byte key[] = { some 16, 24 or 32 byte key };
    byte iv[]  = { some 16 byte iv };
    if (ret = wc_AesSetKey(&enc, key, AES_BLOCK_SIZE, iv,
    AES_ENCRYPTION) != 0) {
	// failed to set aes key
    }
    \endcode

    \sa wc_AesSetKeyDirect
    \sa wc_AesSetIV
*/
int  wc_AesSetKey(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir);

/*!
    \ingroup AES
    \brief This function sets the initialization vector for a
    particular AES object. The AES object should be initialized before
    calling this function.

    \return 0 On successfully setting initialization vector.
    \return BAD_FUNC_ARG Returned if AES pointer is NULL.

    \param aes pointer to the AES structure on which to set the
    initialization vector
    \param iv initialization vector used to initialize the AES structure.
    If the value is NULL, the default action initializes the iv to 0.

    _Example_
    \code
    Aes enc;
    // set enc key
    byte iv[]  = { some 16 byte iv };
    if (ret = wc_AesSetIV(&enc, iv) != 0) {
	// failed to set aes iv
    }
    \endcode

    \sa wc_AesSetKeyDirect
    \sa wc_AesSetKey
*/
int  wc_AesSetIV(Aes* aes, const byte* iv);

/*!
    \ingroup AES
    \brief Encrypts a plaintext message from the input buffer in, and places
    the resulting cipher text in the output buffer out using cipher block
    chaining with AES. This function requires that the AES object has been
    initialized by calling AesSetKey before a message is able to be encrypted.
    This function assumes that the input message is AES block length aligned,
    and expects the input length to be a multiple of the block length, which
    will optionally be checked and enforced if WOLFSSL_AES_CBC_LENGTH_CHECKS
    is defined in the build configuration.  In order to assure block-multiple
    input, PKCS#7 style padding should be added beforehand. This differs from
    the OpenSSL AES-CBC methods which add the padding for you. To make the
    wolfSSL and corresponding OpenSSL functions interoperate, one should specify
    the -nopad option in the OpenSSL command line function so that it behaves
    like the wolfSSL AesCbcEncrypt method and does not add extra padding
    during encryption.

    \return 0 On successfully encrypting message.
    \return BAD_ALIGN_E: may be returned on block align error
    \return BAD_LENGTH_E will be returned if the input length isn't a
    multiple of the AES block length, when the library is built with
    WOLFSSL_AES_CBC_LENGTH_CHECKS.

    \param aes pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the ciphertext
    of the encrypted message
    \param in pointer to the input buffer containing message to be encrypted
    \param sz size of input message

    _Example_
    \code
    Aes enc;
    int ret = 0;
    // initialize enc with AesSetKey, using direction AES_ENCRYPTION
    byte msg[AES_BLOCK_SIZE * n]; // multiple of 16 bytes
    // fill msg with data
    byte cipher[AES_BLOCK_SIZE * n]; // Some multiple of 16 bytes
    if ((ret = wc_AesCbcEncrypt(&enc, cipher, message, sizeof(msg))) != 0 ) {
	// block align error
    }
    \endcode

    \sa wc_AesSetKey
    \sa wc_AesSetIV
    \sa wc_AesCbcDecrypt
*/
int  wc_AesCbcEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);

/*!
    \ingroup AES
    \brief Decrypts a cipher from the input buffer in, and places the
    resulting plain text in the output buffer out using cipher block chaining
    with AES. This function requires that the AES structure has been
    initialized by calling AesSetKey before a message is able to be decrypted.
    This function assumes that the original message was AES block length
    aligned, and expects the input length to be a multiple of the block length,
    which will optionally be checked and enforced if
    WOLFSSL_AES_CBC_LENGTH_CHECKS is defined in the build configuration.
    This differs from the OpenSSL AES-CBC methods, which add PKCS#7 padding
    automatically, and so do not require block-multiple input. To make the
    wolfSSL function and equivalent OpenSSL functions interoperate, one
    should specify the -nopad option in the OpenSSL command line function
    so that it behaves like the wolfSSL AesCbcEncrypt method and does not
    create errors during decryption.

    \return 0 On successfully decrypting message.
    \return BAD_ALIGN_E may be returned on block align error.
    \return BAD_LENGTH_E will be returned if the input length isn't a
    multiple of the AES block length, when the library is built with
    WOLFSSL_AES_CBC_LENGTH_CHECKS.

    \param aes pointer to the AES object used to decrypt data.
    \param out pointer to the output buffer in which to store the plain text
    of the decrypted message.
    size must be a multiple of AES_BLOCK_LENGTH, padded if necessary
    \param in pointer to the input buffer containing cipher text to be
    decrypted.
    size must be a multiple of AES_BLOCK_LENGTH, padded if necessary
    \param sz size of input message.

    _Example_
    \code
    Aes dec;
    int ret = 0;
    // initialize dec with AesSetKey, using direction AES_DECRYPTION
    byte cipher[AES_BLOCK_SIZE * n]; // some multiple of 16 bytes
    // fill cipher with cipher text
    byte plain [AES_BLOCK_SIZE * n];
    if ((ret = wc_AesCbcDecrypt(&dec, plain, cipher, sizeof(cipher))) != 0 ) {
	// block align error
    }
    \endcode

    \sa wc_AesSetKey
    \sa wc_AesCbcEncrypt
*/
int  wc_AesCbcDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);

/*!
    \ingroup AES
    \brief Encrypts/Decrypts a message from the input buffer in, and places
    the resulting cipher text in the output buffer out using CTR mode with
    AES. This function is only enabled if WOLFSSL_AES_COUNTER is enabled at
    compile time. The AES structure should be initialized through AesSetKey
    before calling this function. Note that this function is used for both
    decryption and encryption. _NOTE:_ Regarding using same API for encryption
    and decryption. User should differentiate between Aes structures
    for encrypt/decrypt.

    \return int integer values corresponding to wolfSSL error or success
    status

    \param aes pointer to the AES object used to decrypt data
    \param out pointer to the output buffer in which to store the cipher
    text of the encrypted message
    size must be a multiple of AES_BLOCK_LENGTH, padded if necessary
    \param in pointer to the input buffer containing plain text to be encrypted
    size must be a multiple of AES_BLOCK_LENGTH, padded if necessary
    \param sz size of the input plain text

    _Example_
    \code
    Aes enc;
    Aes dec;
    // initialize enc and dec with AesSetKeyDirect, using direction
    AES_ENCRYPTION
    // since the underlying API only calls Encrypt and by default calling
    encrypt on
    // a cipher results in a decryption of the cipher

    byte msg[AES_BLOCK_SIZE * n]; //n being a positive integer making msg
    some multiple of 16 bytes
    // fill plain with message text
    byte cipher[AES_BLOCK_SIZE * n];
    byte decrypted[AES_BLOCK_SIZE * n];
    wc_AesCtrEncrypt(&enc, cipher, msg, sizeof(msg)); // encrypt plain
    wc_AesCtrEncrypt(&dec, decrypted, cipher, sizeof(cipher));
    // decrypt cipher text
    \endcode

    \sa wc_AesSetKey
*/
int wc_AesCtrEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz);

/*!
    \ingroup AES
    \brief This function is a one-block encrypt of the input block, in, into
    the output block, out. It uses the key of the provided AES structure, which
    should be initialized with wc_AesSetKey before calling this function.
    wc_AesSetKey should have been called with the iv set to NULL. This is only
    enabled if the configure option WOLFSSL_AES_DIRECT is enabled. __Warning:__
    In nearly all use cases ECB mode is considered to be less secure. Please
    avoid using ECB API’s directly whenever possible.

    \return int integer values corresponding to wolfSSL error or success
    status

    \param aes pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the cipher
    text of the encrypted message
    \param in pointer to the input buffer containing plain text to be encrypted

    _Example_
    \code
    Aes enc;
    // initialize enc with AesSetKey, using direction AES_ENCRYPTION
    byte msg [AES_BLOCK_SIZE]; // 16 bytes
    // initialize msg with plain text to encrypt
    byte cipher[AES_BLOCK_SIZE];
    wc_AesEncryptDirect(&enc, cipher, msg);
    \endcode

    \sa wc_AesDecryptDirect
    \sa wc_AesSetKeyDirect
*/
int wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in);

/*!
    \ingroup AES
    \brief This function is a one-block decrypt of the input block, in, into
    the output block, out. It uses the key of the provided AES structure, which
    should be initialized with wc_AesSetKey before calling this function.
    wc_AesSetKey should have been called with the iv set to NULL. This is only
    enabled if the configure option WOLFSSL_AES_DIRECT is enabled. __Warning:__
    In nearly all use cases ECB mode is considered to be less secure. Please
    avoid using ECB API’s directly whenever possible.

    \return int integer values corresponding to wolfSSL error or success
    status

    \param aes pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the plain
    text of the decrypted cipher text
    \param in pointer to the input buffer containing cipher text to be
    decrypted

    _Example_
    \code
    Aes dec;
    // initialize enc with AesSetKey, using direction AES_DECRYPTION
    byte cipher [AES_BLOCK_SIZE]; // 16 bytes
    // initialize cipher with cipher text to decrypt
    byte msg[AES_BLOCK_SIZE];
    wc_AesDecryptDirect(&dec, msg, cipher);
    \endcode

    \sa wc_AesEncryptDirect
    \sa wc_AesSetKeyDirect
 */
int wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in);

/*!
    \ingroup AES
    \brief This function is used to set the AES keys for CTR mode with AES.
    It initializes an AES object with the given key, iv
    (initialization vector), and encryption dir (direction). It is only
    enabled if the configure option WOLFSSL_AES_DIRECT is enabled.
    Currently wc_AesSetKeyDirect uses wc_AesSetKey internally. __Warning:__ In
    nearly all use cases ECB mode is considered to be less secure. Please avoid
    using ECB API’s directly whenever possible

    \return 0 On successfully setting the key.
    \return BAD_FUNC_ARG Returned if the given key is an invalid length.

    \param aes pointer to the AES object used to encrypt data
    \param key 16, 24, or 32 byte secret key for encryption and decryption
    \param len length of the key passed in
    \param iv initialization vector used to initialize the key
    \param dir Cipher direction. Set AES_ENCRYPTION to encrypt,  or
    AES_DECRYPTION to decrypt. (See enum in wolfssl/wolfcrypt/aes.h)
    (NOTE: If using wc_AesSetKeyDirect with Aes Counter mode (Stream cipher)
    only use AES_ENCRYPTION for both encrypting and decrypting)

    _Example_
    \code
    Aes enc;
    int ret = 0;
    byte key[] = { some 16, 24, or 32 byte key };
    byte iv[]  = { some 16 byte iv };
    if (ret = wc_AesSetKeyDirect(&enc, key, sizeof(key), iv,
    AES_ENCRYPTION) != 0) {
	// failed to set aes key
    }
    \endcode

    \sa wc_AesEncryptDirect
    \sa wc_AesDecryptDirect
    \sa wc_AesSetKey
*/
int  wc_AesSetKeyDirect(Aes* aes, const byte* key, word32 len,
                                const byte* iv, int dir);

/*!
    \ingroup AES
    \brief This function is used to set the key for AES GCM
    (Galois/Counter Mode). It initializes an AES object with the
    given key. It is only enabled if the configure option
    HAVE_AESGCM is enabled at compile time.

    \return 0 On successfully setting the key.
    \return BAD_FUNC_ARG Returned if the given key is an invalid length.

    \param aes pointer to the AES object used to encrypt data
    \param key 16, 24, or 32 byte secret key for encryption and decryption
    \param len length of the key passed in

    _Example_
    \code
    Aes enc;
    int ret = 0;
    byte key[] = { some 16, 24,32 byte key };
    if (ret = wc_AesGcmSetKey(&enc, key, sizeof(key)) != 0) {
	// failed to set aes key
    }
    \endcode

    \sa wc_AesGcmEncrypt
    \sa wc_AesGcmDecrypt
*/
int  wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len);

/*!
    \ingroup AES
    \brief This function encrypts the input message, held in the buffer in,
    and stores the resulting cipher text in the output buffer out. It
    requires a new iv (initialization vector) for each call to encrypt.
    It also encodes the input authentication vector, authIn, into the
    authentication tag, authTag.

    \return 0 On successfully encrypting the input message

    \param aes - pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the cipher text
    size must match in's size (sz)
    \param in pointer to the input buffer holding the message to encrypt
    size must be a multiple of AES_BLOCK_LENGTH, padded if necessary
    \param sz length of the input message to encrypt
    \param iv pointer to the buffer containing the initialization vector
    \param ivSz length of the initialization vector
    \param authTag pointer to the buffer in which to store the
    authentication tag
    \param authTagSz length of the desired authentication tag
    \param authIn pointer to the buffer containing the input
    authentication vector
    \param authInSz length of the input authentication vector

    _Example_
    \code
    Aes enc;
    // initialize aes structure by calling wc_AesGcmSetKey

    byte plain[AES_BLOCK_LENGTH * n]; //n being a positive integer
    making plain some multiple of 16 bytes
    // initialize plain with msg to encrypt
    byte cipher[sizeof(plain)];
    byte iv[] = // some 16 byte iv
    byte authTag[AUTH_TAG_LENGTH];
    byte authIn[] = // Authentication Vector

    wc_AesGcmEncrypt(&enc, cipher, plain, sizeof(cipher), iv, sizeof(iv),
			authTag, sizeof(authTag), authIn, sizeof(authIn));
    \endcode

    \sa wc_AesGcmSetKey
    \sa wc_AesGcmDecrypt
*/
int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES
    \brief This function decrypts the input cipher text, held in the buffer
    in, and stores the resulting message text in the output buffer out.
    It also checks the input authentication vector, authIn, against the
    supplied authentication tag, authTag.

    \return 0 On successfully decrypting the input message
    \return AES_GCM_AUTH_E If the authentication tag does not match the
    supplied authentication code vector, authTag.

    \param aes pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the message text
    size must match in's size (sz)
    \param in pointer to the input buffer holding the cipher text to decrypt
    size must be a multiple of AES_BLOCK_LENGTH, padded if necessary
    \param sz length of the cipher text to decrypt
    \param iv pointer to the buffer containing the initialization vector
    \param ivSz length of the initialization vector
    \param authTag pointer to the buffer containing the authentication tag
    \param authTagSz length of the desired authentication tag
    \param authIn pointer to the buffer containing the input
    authentication vector
    \param authInSz length of the input authentication vector

    _Example_
    \code
    Aes enc; //can use the same struct as was passed to wc_AesGcmEncrypt
    // initialize aes structure by calling wc_AesGcmSetKey if not already done

    byte cipher[AES_BLOCK_LENGTH * n]; //n being a positive integer
    making cipher some multiple of 16 bytes
    // initialize cipher with cipher text to decrypt
    byte output[sizeof(cipher)];
    byte iv[] = // some 16 byte iv
    byte authTag[AUTH_TAG_LENGTH];
    byte authIn[] = // Authentication Vector

    wc_AesGcmDecrypt(&enc, output, cipher, sizeof(cipher), iv, sizeof(iv),
			authTag, sizeof(authTag), authIn, sizeof(authIn));
    \endcode

    \sa wc_AesGcmSetKey
    \sa wc_AesGcmEncrypt
*/
int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES
    \brief This function initializes and sets the key for a GMAC object
    to be used for Galois Message Authentication.

    \return 0 On successfully setting the key
    \return BAD_FUNC_ARG Returned if key length is invalid.

    \param gmac pointer to the gmac object used for authentication
    \param key 16, 24, or 32 byte secret key for authentication
    \param len length of the key

    _Example_
    \code
    Gmac gmac;
    key[] = { some 16, 24, or 32 byte length key };
    wc_AesInit(gmac.aes, HEAP_HINT, INVALID_DEVID); // Make sure devId updated
    wc_GmacSetKey(&gmac, key, sizeof(key));
    \endcode

    \sa wc_GmacUpdate
    \sa wc_AesInit
*/
int wc_GmacSetKey(Gmac* gmac, const byte* key, word32 len);

/*!
    \ingroup AES
    \brief This function generates the Gmac hash of the authIn input and
    stores the result in the authTag buffer. After running wc_GmacUpdate,
    one should compare the generated authTag to a known authentication tag
    to verify the authenticity of a message.

    \return 0 On successfully computing the Gmac hash.

    \param gmac pointer to the gmac object used for authentication
    \param iv initialization vector used for the hash
    \param ivSz size of the initialization vector used
    \param authIn pointer to the buffer containing the authentication
    vector to verify
    \param authInSz size of the authentication vector
    \param authTag pointer to the output buffer in which to store the Gmac hash
    \param authTagSz the size of the output buffer used to store the Gmac hash

    _Example_
    \code
    Gmac gmac;
    key[] = { some 16, 24, or 32 byte length key };
    iv[] = { some 16 byte length iv };

    wc_AesInit(gmac.aes, HEAP_HINT, INVALID_DEVID); // Make sure devId updated
    wc_GmacSetKey(&gmac, key, sizeof(key));
    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE]; // will store authentication code

    wc_GmacUpdate(&gmac, iv, sizeof(iv), authIn, sizeof(authIn), tag,
    sizeof(tag));
    \endcode

    \sa wc_GmacSetKey
    \sa wc_AesInit
*/
int wc_GmacUpdate(Gmac* gmac, const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz);

/*!
    \ingroup AES
    \brief This function sets the key for an AES object using CCM
    (Counter with CBC-MAC). It takes a pointer to an AES structure and
    initializes it with supplied key.

    \return none

    \param aes aes structure in which to store the supplied key
    \param key 16, 24, or 32 byte secret key for encryption and decryption
    \param keySz size of the supplied key

    _Example_
    \code
    Aes enc;
    key[] = { some 16, 24, or 32 byte length key };

    wc_AesCcmSetKey(&aes, key, sizeof(key));
    \endcode

    \sa wc_AesCcmEncrypt
    \sa wc_AesCcmDecrypt
*/
int  wc_AesCcmSetKey(Aes* aes, const byte* key, word32 keySz);

/*!
    \ingroup AES

    \brief This function encrypts the input message, in, into the output
    buffer, out, using CCM (Counter with CBC-MAC). It subsequently
    calculates and stores the authorization tag, authTag, from the
    authIn input.

    \return none

    \param aes pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the cipher text
    \param in pointer to the input buffer holding the message to encrypt
    \param sz length of the input message to encrypt
    \param nonce pointer to the buffer containing the nonce
    (number only used once)
    \param nonceSz length of the nonce
    \param authTag pointer to the buffer in which to store the
    authentication tag
    \param authTagSz length of the desired authentication tag
    \param authIn pointer to the buffer containing the input
    authentication vector
    \param authInSz length of the input authentication vector

    _Example_
    \code
    Aes enc;
    // initialize enc with wc_AesCcmSetKey

    nonce[] = { initialize nonce };
    plain[] = { some plain text message };
    cipher[sizeof(plain)];

    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE]; // will store authentication code

    wc_AesCcmEncrypt(&enc, cipher, plain, sizeof(plain), nonce, sizeof(nonce),
			tag, sizeof(tag), authIn, sizeof(authIn));
    \endcode

    \sa wc_AesCcmSetKey
    \sa wc_AesCcmDecrypt
*/
int  wc_AesCcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES

    \brief This function decrypts the input cipher text, in, into
    the output buffer, out, using CCM (Counter with CBC-MAC). It
    subsequently calculates the authorization tag, authTag, from the
    authIn input. If the authorization tag is invalid, it sets the
    output buffer to zero and returns the error: AES_CCM_AUTH_E.

    \return 0 On successfully decrypting the input message
    \return AES_CCM_AUTH_E If the authentication tag does not match the
    supplied authentication code vector, authTag.

    \param aes pointer to the AES object used to encrypt data
    \param out pointer to the output buffer in which to store the cipher text
    \param in pointer to the input buffer holding the message to encrypt
    \param sz length of the input cipher text to decrypt
    \param nonce pointer to the buffer containing the nonce
    (number only used once)
    \param nonceSz length of the nonce
    \param authTag pointer to the buffer in which to store the
    authentication tag
    \param authTagSz length of the desired authentication tag
    \param authIn pointer to the buffer containing the input
    authentication vector
    \param authInSz length of the input authentication vector

    _Example_
    \code
    Aes dec;
    // initialize dec with wc_AesCcmSetKey

    nonce[] = { initialize nonce };
    cipher[] = { encrypted message };
    plain[sizeof(cipher)];

    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE] = { authentication tag received for verification };

    int return = wc_AesCcmDecrypt(&dec, plain, cipher, sizeof(cipher),
    nonce, sizeof(nonce),tag, sizeof(tag), authIn, sizeof(authIn));
    if(return != 0) {
	// decrypt error, invalid authentication code
    }
    \endcode

    \sa wc_AesCcmSetKey
    \sa wc_AesCcmEncrypt
*/
int  wc_AesCcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES

    \brief This is to help with setting keys to correct encrypt or
    decrypt type. It is up to user to call wc_AesXtsFree on aes key when done.

    \return 0 Success

    \param aes   AES keys for encrypt/decrypt process
    \param key   buffer holding aes key | tweak key
    \param len   length of key buffer in bytes. Should be twice that of
    key size.
                 i.e. 32 for a 16 byte key.
    \param dir   direction, either AES_ENCRYPTION or AES_DECRYPTION
    \param heap  heap hint to use for memory. Can be NULL
    \param devId id to use with async crypto. Can be 0

    _Example_
    \code
    XtsAes aes;

    if(wc_AesXtsSetKey(&aes, key, sizeof(key), AES_ENCRYPTION, NULL, 0) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsFree
*/
int wc_AesXtsSetKey(XtsAes* aes, const byte* key,
         word32 len, int dir, void* heap, int devId);

/*!
    \ingroup AES

    \brief Same process as wc_AesXtsEncrypt but uses a word64 type as the tweak
           value instead of a byte array. This just converts the word64 to a
           byte array and calls wc_AesXtsEncrypt.

    \return 0 Success

    \param aes    AES keys to use for block encrypt/decrypt
    \param out    output buffer to hold cipher text
    \param in     input plain text buffer to encrypt
    \param sz     size of both out and in buffers
    \param sector value to use for tweak

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    word64 s = VALUE;

    //set up keys with AES_ENCRYPTION as dir

    if(wc_AesXtsEncryptSector(&aes, cipher, plain, SIZE, s) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsEncryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

/*!
    \ingroup AES

    \brief Same process as wc_AesXtsDecrypt but uses a word64 type as the tweak
           value instead of a byte array. This just converts the word64 to a
           byte array.

    \return 0 Success

    \param aes    AES keys to use for block encrypt/decrypt
    \param out    output buffer to hold plain text
    \param in     input cipher text buffer to decrypt
    \param sz     size of both out and in buffers
    \param sector value to use for tweak

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    word64 s = VALUE;

    //set up aes key with AES_DECRYPTION as dir and tweak with AES_ENCRYPTION

    if(wc_AesXtsDecryptSector(&aes, plain, cipher, SIZE, s) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsDecryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

/*!
    \ingroup AES

    \brief AES with XTS mode. (XTS) XEX encryption with Tweak and cipher text
           Stealing.

    \return 0 Success

    \param aes   AES keys to use for block encrypt/decrypt
    \param out   output buffer to hold cipher text
    \param in    input plain text buffer to encrypt
    \param sz    size of both out and in buffers
    \param i     value to use for tweak
    \param iSz   size of i buffer, should always be AES_BLOCK_SIZE but having
                 this input adds a sanity check on how the user calls the
                 function.

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    unsigned char i[AES_BLOCK_SIZE];

    //set up key with AES_ENCRYPTION as dir

    if(wc_AesXtsEncrypt(&aes, cipher, plain, SIZE, i, sizeof(i)) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsEncrypt(XtsAes* aes, byte* out,
         const byte* in, word32 sz, const byte* i, word32 iSz);

/*!
    \ingroup AES

    \brief Same process as encryption but Aes key is AES_DECRYPTION type.

    \return 0 Success

    \param aes   AES keys to use for block encrypt/decrypt
    \param out   output buffer to hold plain text
    \param in    input cipher text buffer to decrypt
    \param sz    size of both out and in buffers
    \param i     value to use for tweak
    \param iSz   size of i buffer, should always be AES_BLOCK_SIZE but having
                 this input adds a sanity check on how the user calls the
                 function.

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    unsigned char i[AES_BLOCK_SIZE];

    //set up key with AES_DECRYPTION as dir and tweak with AES_ENCRYPTION

    if(wc_AesXtsDecrypt(&aes, plain, cipher, SIZE, i, sizeof(i)) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsDecrypt(XtsAes* aes, byte* out,
        const byte* in, word32 sz, const byte* i, word32 iSz);

/*!
    \ingroup AES

    \brief This is to free up any resources used by the XtsAes structure

    \return 0 Success

    \param aes AES keys to free

    _Example_
    \code
    XtsAes aes;

    if(wc_AesXtsSetKey(&aes, key, sizeof(key), AES_ENCRYPTION, NULL, 0) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
*/
int wc_AesXtsFree(XtsAes* aes);


/*!
    \ingroup AES
    \brief Initialize Aes structure. Sets heap hint to be used and ID for use
    with async hardware. It is up to the user to call wc_AesFree on the Aes
    structure when done.
    \return 0 Success

    \param aes aes structure in to initialize
    \param heap heap hint to use for malloc / free if needed
    \param devId ID to use with async hardware

    _Example_
    \code
    Aes enc;
    void* hint = NULL;
    int devId = INVALID_DEVID; //if not using async INVALID_DEVID is default

    //heap hint could be set here if used

    wc_AesInit(&enc, hint, devId);
    \endcode

    \sa wc_AesSetKey
    \sa wc_AesSetIV
    \sa wc_AesFree
*/
int  wc_AesInit(Aes* aes, void* heap, int devId);

/*!
    \ingroup AES
    \brief free resources associated with the Aes structure when applicable.
    Internally may sometimes be a no-op but still recommended to call in all
    cases as a general best-practice (IE if application code is ported for use
    on new environments where the call is applicable).
    \return no return (void function)

    \param aes aes structure in to free

    _Example_
    \code
    Aes enc;
    void* hint = NULL;
    int devId = INVALID_DEVID; //if not using async INVALID_DEVID is default

    //heap hint could be set here if used

    wc_AesInit(&enc, hint, devId);
    // ... do some interesting things ...
    wc_AesFree(&enc);
    \endcode

    \sa wc_AesInit
*/
int  wc_AesFree(Aes* aes);

/*!
    \ingroup AES

    \brief AES with CFB mode.

    \return 0 Success and negative error values on failure

    \param aes   AES keys to use for block encrypt/decrypt
    \param out   output buffer to hold cipher text must be at least as large
    as inputbuffer)
    \param in    input plain text buffer to encrypt
    \param sz    size of input buffer

    _Example_
    \code
    Aes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];

    //set up key with AES_ENCRYPTION as dir for both encrypt and decrypt

    if(wc_AesCfbEncrypt(&aes, cipher, plain, SIZE) != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_AesCfbDecrypt
    \sa wc_AesSetKey
*/
int wc_AesCfbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz);

/*!
    \ingroup AES

    \brief AES with CFB mode.

    \return 0 Success and negative error values on failure

    \param aes   AES keys to use for block encrypt/decrypt
    \param out   output buffer to hold decrypted text must be at least as large
    as inputbuffer)
    \param in    input buffer to decrypt
    \param sz    size of input buffer

    _Example_
    \code
    Aes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];

    //set up key with AES_ENCRYPTION as dir for both encrypt and decrypt

    if(wc_AesCfbDecrypt(&aes, plain, cipher, SIZE) != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_AesCfbEncrypt
    \sa wc_AesSetKey
*/
int wc_AesCfbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz);

/*!
    \ingroup AES

    \brief This function performs SIV (synthetic initialization vector)
    encryption as described in RFC 5297.

    \return 0 On successful encryption.
    \return BAD_FUNC_ARG If key, SIV, or output buffer are NULL. Also returned
    if the key size isn't 32, 48, or 64 bytes.
    \return Other Other negative error values returned if AES or CMAC operations
    fail.

    \param key Byte buffer containing the key to use.
    \param keySz Length of the key buffer in bytes.
    \param assoc Additional, authenticated associated data (AD).
    \param assocSz Length of AD buffer in bytes.
    \param nonce A number used once. Used by the algorithm in the same manner as
    the AD.
    \param nonceSz Length of nonce buffer in bytes.
    \param in Plaintext buffer to encrypt.
    \param inSz Length of plaintext buffer.
    \param siv The SIV output by S2V (see RFC 5297 2.4).
    \param out Buffer to hold the ciphertext. Should be the same length as the
    plaintext buffer.

    _Example_
    \code
    byte key[] = { some 32, 48, or 64 byte key };
    byte assoc[] = {0x01, 0x2, 0x3};
    byte nonce[] = {0x04, 0x5, 0x6};
    byte plainText[] = {0xDE, 0xAD, 0xBE, 0xEF};
    byte siv[AES_BLOCK_SIZE];
    byte cipherText[sizeof(plainText)];
    if (wc_AesSivEncrypt(key, sizeof(key), assoc, sizeof(assoc), nonce,
        sizeof(nonce), plainText, sizeof(plainText), siv, cipherText) != 0) {
        // failed to encrypt
    }
    \endcode

    \sa wc_AesSivDecrypt
*/


int wc_AesSivEncrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out);

/*!
    \ingroup AES
    \brief This function performs SIV (synthetic initialization vector)
    decryption as described in RFC 5297.

    \return 0 On successful decryption.
    \return BAD_FUNC_ARG If key, SIV, or output buffer are NULL. Also returned
    if the key size isn't 32, 48, or 64 bytes.
    \return AES_SIV_AUTH_E If the SIV derived by S2V doesn't match the input
    SIV (see RFC 5297 2.7). 
    \return Other Other negative error values returned if AES or CMAC operations
    fail.

    \param key Byte buffer containing the key to use.
    \param keySz Length of the key buffer in bytes.
    \param assoc Additional, authenticated associated data (AD).
    \param assocSz Length of AD buffer in bytes.
    \param nonce A number used once. Used by the underlying algorithm in the
    same manner as the AD.
    \param nonceSz Length of nonce buffer in bytes.
    \param in Ciphertext buffer to decrypt.
    \param inSz Length of ciphertext buffer.
    \param siv The SIV that accompanies the ciphertext (see RFC 5297 2.4).
    \param out Buffer to hold the decrypted plaintext. Should be the same length
    as the ciphertext buffer.

    _Example_
    \code
    byte key[] = { some 32, 48, or 64 byte key };
    byte assoc[] = {0x01, 0x2, 0x3};
    byte nonce[] = {0x04, 0x5, 0x6};
    byte cipherText[] = {0xDE, 0xAD, 0xBE, 0xEF};
    byte siv[AES_BLOCK_SIZE] = { the SIV that came with the ciphertext };
    byte plainText[sizeof(cipherText)];
    if (wc_AesSivDecrypt(key, sizeof(key), assoc, sizeof(assoc), nonce,
        sizeof(nonce), cipherText, sizeof(cipherText), siv, plainText) != 0) {
        // failed to decrypt
    }
    \endcode

    \sa wc_AesSivEncrypt
*/

int wc_AesSivDecrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out);
