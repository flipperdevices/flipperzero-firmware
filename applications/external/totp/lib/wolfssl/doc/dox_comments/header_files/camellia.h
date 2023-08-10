/*!
    \ingroup Camellia

    \brief This function sets the key and initialization vector for a
    camellia object, initializing it for use as a cipher.

    \return 0 Returned upon successfully setting the key and initialization
    vector
    \return BAD_FUNC_ARG returned if there is an error processing one of
    the input arguments
    \return MEMORY_E returned if there is an error allocating memory with
    XMALLOC

    \param cam pointer to the camellia structure on which to set the key and iv
    \param key pointer to the buffer containing the 16, 24, or 32 byte key
    to use for encryption and decryption
    \param len length of the key passed in
    \param iv pointer to the buffer containing the 16 byte initialization
    vector for use with this camellia structure

    _Example_
    \code
    Camellia cam;
    byte key[32];
    // initialize key
    byte iv[16];
    // initialize iv
    if( wc_CamelliaSetKey(&cam, key, sizeof(key), iv) != 0) {
    	// error initializing camellia structure
    }
    \endcode

    \sa wc_CamelliaEncryptDirect
    \sa wc_CamelliaDecryptDirect
    \sa wc_CamelliaCbcEncrypt
    \sa wc_CamelliaCbcDecrypt
*/
int  wc_CamelliaSetKey(Camellia* cam,
                                   const byte* key, word32 len, const byte* iv);

/*!
    \ingroup Camellia

    \brief This function sets the initialization vector for a camellia object.

    \return 0 Returned upon successfully setting the key and initialization
    vector
    \return BAD_FUNC_ARG returned if there is an error processing one of the
    input arguments

    \param cam pointer to the camellia structure on which to set the iv
    \param iv pointer to the buffer containing the 16 byte initialization
    vector for use with this camellia structure

    _Example_
    \code
    Camellia cam;
    byte iv[16];
    // initialize iv
    if( wc_CamelliaSetIV(&cam, iv) != 0) {
	// error initializing camellia structure
    }
    \endcode

    \sa wc_CamelliaSetKey
*/
int  wc_CamelliaSetIV(Camellia* cam, const byte* iv);

/*!
    \ingroup Camellia

    \brief This function does a one-block encrypt using the provided camellia
    object. It parses the first 16 byte block from the buffer in and stores
    the encrypted result in the buffer out. Before using this function, one
    should initialize the camellia object using wc_CamelliaSetKey.

    \return none No returns.

    \param cam pointer to the camellia structure to use for encryption
    \param out pointer to the buffer in which to store the encrypted block
    \param in pointer to the buffer containing the plaintext block to encrypt

    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte plain[] = { // initialize with message to encrypt };
    byte cipher[16];

    wc_CamelliaEncryptDirect(&ca, cipher, plain);
    \endcode

    \sa wc_CamelliaDecryptDirect
*/
int  wc_CamelliaEncryptDirect(Camellia* cam, byte* out,
                                                                const byte* in);

/*!
    \ingroup Camellia

    \brief This function does a one-block decrypt using the provided camellia
    object. It parses the first 16 byte block from the buffer in, decrypts it,
    and stores the result in the buffer out. Before using this function, one
    should initialize the camellia object using wc_CamelliaSetKey.

    \return none No returns.

    \param cam pointer to the camellia structure to use for encryption
    \param out pointer to the buffer in which to store the decrypted
    plaintext block
    \param in pointer to the buffer containing the ciphertext block to decrypt

    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte cipher[] = { // initialize with encrypted message to decrypt };
    byte decrypted[16];

    wc_CamelliaDecryptDirect(&cam, decrypted, cipher);
    \endcode

    \sa wc_CamelliaEncryptDirect
*/
int  wc_CamelliaDecryptDirect(Camellia* cam, byte* out,
                                                                const byte* in);

/*!
    \ingroup Camellia

    \brief This function encrypts the plaintext from the buffer in and
    stores the output in the buffer out. It performs this encryption
    using Camellia with Cipher Block Chaining (CBC).

    \return none No returns.

    \param cam pointer to the camellia structure to use for encryption
    \param out pointer to the buffer in which to store the encrypted ciphertext
    \param in pointer to the buffer containing the plaintext to encrypt
    \param sz the size of the message to encrypt

    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte plain[] = { // initialize with encrypted message to decrypt };
    byte cipher[sizeof(plain)];

    wc_CamelliaCbcEncrypt(&cam, cipher, plain, sizeof(plain));
    \endcode

    \sa wc_CamelliaCbcDecrypt
*/
int wc_CamelliaCbcEncrypt(Camellia* cam,
                                          byte* out, const byte* in, word32 sz);

/*!
    \ingroup Camellia

    \brief This function decrypts the ciphertext from the buffer in and
    stores the output in the buffer out. It performs this decryption using
    Camellia with Cipher Block Chaining (CBC).

    \return none No returns.

    \param cam pointer to the camellia structure to use for encryption
    \param out pointer to the buffer in which to store the decrypted message
    \param in pointer to the buffer containing the encrypted ciphertext
    \param sz the size of the message to encrypt

    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte cipher[] = { // initialize with encrypted message to decrypt };
    byte decrypted[sizeof(cipher)];

    wc_CamelliaCbcDecrypt(&cam, decrypted, cipher, sizeof(cipher));
    \endcode

    \sa wc_CamelliaCbcEncrypt
*/
int wc_CamelliaCbcDecrypt(Camellia* cam,
                                          byte* out, const byte* in, word32 sz);
