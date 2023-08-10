/*!
    \ingroup ARC4
    \brief This function encrypts an input message from the buffer in, placing
    the ciphertext in the output buffer out, or decrypts a ciphertext from the
    buffer in, placing the plaintext in the output buffer out, using ARC4
    encryption. This function is used for both encryption and decryption.
    Before this method may be called, one must first initialize the ARC4
    structure using wc_Arc4SetKey.

    \return none

    \param arc4 pointer to the ARC4 structure used to process the message
    \param out pointer to the output buffer in which to store the
    processed message
    \param in pointer to the input buffer containing the message to process
    \param length length of the message to process

    _Example_
    \code
    Arc4 enc;
    byte key[] = { key to use for encryption };
    wc_Arc4SetKey(&enc, key, sizeof(key));

    byte plain[] = { plain text to encode };
    byte cipher[sizeof(plain)];
    byte decrypted[sizeof(plain)];
    // encrypt the plain into cipher
    wc_Arc4Process(&enc, cipher, plain, sizeof(plain));
    // decrypt the cipher
    wc_Arc4Process(&enc, decrypted, cipher, sizeof(cipher));
    \endcode

    \sa wc_Arc4SetKey
*/
int wc_Arc4Process(Arc4* arc4, byte* out, const byte* in, word32 length);

/*!
    \ingroup ARC4

    \brief This function sets the key for a ARC4 object, initializing it for
    use as a cipher. It should be called before using it for encryption
    with wc_Arc4Process.

    \return none

    \param arc4 pointer to an arc4 structure to be used for encryption
    \param key key with which to initialize the arc4 structure
    \param length length of the key used to initialize the arc4 structure

    _Example_
    \code
    Arc4 enc;
    byte key[] = { initialize with key to use for encryption };
    wc_Arc4SetKey(&enc, key, sizeof(key));
    \endcode

    \sa wc_Arc4Process
*/
int wc_Arc4SetKey(Arc4* arc4, const byte* key, word32 length);
