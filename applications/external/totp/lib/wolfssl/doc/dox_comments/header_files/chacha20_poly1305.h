/*!
    \ingroup ChaCha20Poly1305

    \brief This function encrypts an input message, inPlaintext, using the
    ChaCha20 stream cipher, into the output buffer, outCiphertext. It
    also performs Poly-1305 authentication (on the cipher text), and
    stores the generated authentication tag in the output buffer, outAuthTag.

    \return 0 Returned upon successfully encrypting the message
    \return BAD_FUNC_ARG returned if there is an error during the encryption
    process

    \param inKey pointer to a buffer containing the 32 byte key to use
    for encryption
    \param inIv pointer to a buffer containing the 12 byte iv to use for
    encryption
    \param inAAD pointer to the buffer containing arbitrary length additional
    authenticated data (AAD)
    \param inAADLen length of the input AAD
    \param inPlaintext pointer to the buffer containing the plaintext to
    encrypt
    \param inPlaintextLen the length of the plain text to  encrypt
    \param outCiphertext pointer to the buffer in which to store the ciphertext
    \param outAuthTag pointer to a 16 byte wide buffer in which to store the
    authentication tag

    _Example_
    \code
    byte key[] = { // initialize 32 byte key };
    byte iv[]  = { // initialize 12 byte key };
    byte inAAD[] = { // initialize AAD };

    byte plain[] = { // initialize message to encrypt };
    byte cipher[sizeof(plain)];
    byte authTag[16];

    int ret = wc_ChaCha20Poly1305_Encrypt(key, iv, inAAD, sizeof(inAAD),
    plain, sizeof(plain), cipher, authTag);

    if(ret != 0) {
    	// error running encrypt
    }
    \endcode

    \sa wc_ChaCha20Poly1305_Decrypt
    \sa wc_ChaCha_*
    \sa wc_Poly1305*
*/

int wc_ChaCha20Poly1305_Encrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inPlaintext, const word32 inPlaintextLen,
                byte* outCiphertext,
                byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE]);

/*!
    \ingroup ChaCha20Poly1305

    \brief This function decrypts input ciphertext, inCiphertext, using the
    ChaCha20 stream cipher, into the output buffer, outPlaintext. It also
    performs Poly-1305 authentication, comparing the given inAuthTag to an
    authentication generated with the inAAD (arbitrary length additional
    authentication data). Note: If the generated authentication tag does
    not match the supplied authentication tag, the text is not decrypted.

    \return 0 Returned upon successfully decrypting the message
    \return BAD_FUNC_ARG Returned if any of the function arguments do not
    match what is expected
    \return MAC_CMP_FAILED_E Returned if the generated authentication tag
    does not match the supplied inAuthTag.

    \param inKey pointer to a buffer containing the 32 byte key to use for
    decryption
    \param inIv pointer to a buffer containing the 12 byte iv to use for
    decryption
    \param inAAD pointer to the buffer containing arbitrary length additional
    authenticated data (AAD)
    \param inAADLen length of the input AAD
    \param inCiphertext pointer to the buffer containing the ciphertext to
    decrypt
    \param outCiphertextLen the length of the ciphertext to decrypt
    \param inAuthTag pointer to the buffer containing the 16 byte digest
    for authentication
    \param outPlaintext pointer to the buffer in which to store the plaintext

    _Example_
    \code
    byte key[]   = { // initialize 32 byte key };
    byte iv[]    = { // initialize 12 byte key };
    byte inAAD[] = { // initialize AAD };

    byte cipher[]    = { // initialize with received ciphertext };
    byte authTag[16] = { // initialize with received authentication tag };

    byte plain[sizeof(cipher)];

    int ret = wc_ChaCha20Poly1305_Decrypt(key, iv, inAAD, sizeof(inAAD),
    cipher, sizeof(cipher), authTag, plain);

    if(ret == MAC_CMP_FAILED_E) {
    	// error during authentication
    } else if( ret != 0) {
    	// error with function arguments
    }
    \endcode

    \sa wc_ChaCha20Poly1305_Encrypt
    \sa wc_ChaCha_*
    \sa wc_Poly1305*
*/

int wc_ChaCha20Poly1305_Decrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inCiphertext, const word32 inCiphertextLen,
                const byte inAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE],
                byte* outPlaintext);
