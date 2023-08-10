/*!
    \ingroup Curve448

    \brief This function generates a Curve448 key using the given random
    number generator, rng, of the size given (keysize), and stores it in
    the given curve448_key structure. It should be called after the key
    structure has been initialized through wc_curve448_init().

    \return 0 Returned on successfully generating the key and and storing
    it in the given curve448_key structure.
    \return ECC_BAD_ARG_E Returned if the input keysize does not correspond to
    the keysize for a curve448 key (56 bytes).
    \return RNG_FAILURE_E Returned if the rng internal status is not
    DRBG_OK or if there is in generating the next random block with rng.
    \return BAD_FUNC_ARG Returned if any of the input parameters passed in
    are NULL.

    \param [in] rng Pointer to the RNG object used to generate the ecc key.
    \param [in] keysize Size of the key to generate. Must be 56 bytes for
    curve448.
    \param [in,out] key Pointer to the curve448_key structure in which to
    store the generated key.

    _Example_
    \code
    int ret;

    curve448_key key;
    wc_curve448_init(&key); // initialize key
    WC_RNG rng;
    wc_InitRng(&rng); // initialize random number generator

    ret = wc_curve448_make_key(&rng, 56, &key);
    if (ret != 0) {
        // error making Curve448 key
    }
    \endcode

    \sa wc_curve448_init
*/

int wc_curve448_make_key(WC_RNG* rng, int keysize, curve448_key* key);

/*!
    \ingroup Curve448

    \brief This function computes a shared secret key given a secret private
    key and a received public key. It stores the generated secret key in the
    buffer out and assigns the variable of the secret key to outlen. Only
    supports big endian.

    \return 0 Returned on successfully computing a shared secret key
    \return BAD_FUNC_ARG Returned if any of the input parameters passed in
    are NULL

    \param [in] private_key Pointer to the curve448_key structure initialized
    with the user’s private key.
    \param [in] public_key Pointer to the  curve448_key structure containing
    the received public key.
    \param [out] out Pointer to a buffer in which to store the 56 byte computed
    secret key.
    \param [in,out] outlen Pointer in which to store the length written to the
    output buffer.

    _Example_
    \code
    int ret;

    byte sharedKey[56];
    word32 keySz;
    curve448_key privKey, pubKey;
    // initialize both keys

    ret = wc_curve448_shared_secret(&privKey, &pubKey, sharedKey, &keySz);
    if (ret != 0) {
        // error generating shared key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
    \sa wc_curve448_shared_secret_ex
*/

int wc_curve448_shared_secret(curve448_key* private_key,
                                curve448_key* public_key,
                                byte* out, word32* outlen);

/*!
    \ingroup Curve448

    \brief This function computes a shared secret key given a secret private
    key and a received public key. It stores the generated secret key in the
    buffer out and assigns the variable of the secret key to outlen. Supports
    both big and little endian.

    \return 0 Returned on successfully computing a shared secret key.
    \return BAD_FUNC_ARG Returned if any of the input parameters passed in
    are NULL.

    \param [in] private_key Pointer to the curve448_key structure initialized
    with the user’s private key.
    \param [in] public_key Pointer to the  curve448_key structure containing
    the received public key.
    \param [out] out Pointer to a buffer in which to store the 56 byte computed
    secret key.
    \param [in,out] outlen Pointer in which to store the length written to the
    output buffer.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set which
    form to use.

    _Example_
    \code
    int ret;

    byte sharedKey[56];
    word32 keySz;

    curve448_key privKey, pubKey;
    // initialize both keys

    ret = wc_curve448_shared_secret_ex(&privKey, &pubKey, sharedKey, &keySz,
            EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error generating shared key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
    \sa wc_curve448_shared_secret
*/

int wc_curve448_shared_secret_ex(curve448_key* private_key,
                                   curve448_key* public_key,
                                   byte* out, word32* outlen, int endian);

/*!
    \ingroup Curve448

    \brief This function initializes a Curve448 key. It should be called
    before generating a key for the structure.

    \return 0 Returned on successfully initializing the curve448_key structure.
    \return BAD_FUNC_ARG Returned when key is NULL.

    \param [in,out] key Pointer to the curve448_key structure to initialize.

    _Example_
    \code
    curve448_key key;
    wc_curve448_init(&key); // initialize key
    // make key and proceed to encryption
    \endcode

    \sa wc_curve448_make_key
*/

int wc_curve448_init(curve448_key* key);

/*!
    \ingroup Curve448

    \brief This function frees a Curve448 object.

    \param [in,out] key Pointer to the key object to free.

    _Example_
    \code
    curve448_key privKey;
    // initialize key, use it to generate shared secret key
    wc_curve448_free(&privKey);
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
*/

void wc_curve448_free(curve448_key* key);

/*!
    \ingroup Curve448

    \brief This function imports a curve448 private key only. (Big endian).

    \return 0 Returned on successfully importing private key.
    \return BAD_FUNC_ARG Returns if key or priv is null.
    \return ECC_BAD_ARG_E Returns if privSz is not equal to CURVE448_KEY_SIZE.

    \param [in] priv Pointer to a buffer containing the private key to import.
    \param [in] privSz Length of the private key to import.
    \param [in,out] key Pointer to the structure in which to store the imported
    key.

    _Example_
    \code
    int ret;

    byte priv[] = { Contents of private key };
    curve448_key key;
    wc_curve448_init(&key);

    ret = wc_curve448_import_private(priv, sizeof(priv), &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_curve448_import_private_ex
    \sa wc_curve448_size
*/

int wc_curve448_import_private(const byte* priv, word32 privSz,
                                 curve448_key* key);

/*!
    \ingroup Curve448

    \brief curve448 private key import only. (Big or Little endian).

    \return 0 Returned on successfully importing private key.
    \return BAD_FUNC_ARG Returns if key or priv is null.
    \return ECC_BAD_ARG_E Returns if privSz is not equal to CURVE448_KEY_SIZE.

    \param [in] priv Pointer to a buffer containing the private key to import.
    \param [in] privSz Length of the private key to import.
    \param [in,out] key Pointer to the structure in which to store the imported
    key.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to
    set which form to use.

    _Example_
    \code
    int ret;

    byte priv[] = { // Contents of private key };
    curve448_key key;
    wc_curve448_init(&key);

    ret = wc_curve448_import_private_ex(priv, sizeof(priv), &key,
            EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error importing key
    }

    \endcode

    \sa wc_curve448_import_private
    \sa wc_curve448_size
*/

int wc_curve448_import_private_ex(const byte* priv, word32 privSz,
                                    curve448_key* key, int endian);

/*!
    \ingroup Curve448

    \brief This function imports a public-private key pair into a
    curve448_key structure.  Big endian only.

    \return 0 Returned on importing into the curve448_key structure.
    \return BAD_FUNC_ARG Returns if any of the input parameters are null.
    \return ECC_BAD_ARG_E Returned if the input key’s key size does not match
    the public or private key sizes.

    \param [in] priv Pointer to a buffer containing the private key to import.
    \param [in] privSz Length of the private key to import.
    \param [in] pub Pointer to a buffer containing the public key to import.
    \param [in] pubSz Length of the public key to import.
    \param [in,out] key Pointer to the structure in which to store the imported
    keys

    _Example_
    \code
    int ret;

    byte priv[56];
    byte pub[56];
    // initialize with public and private keys
    curve448_key key;

    wc_curve448_init(&key);
    // initialize key

    ret = wc_curve448_import_private_raw(&priv, sizeof(priv), pub, sizeof(pub),
            &key);
    if (ret != 0) {
        // error importing keys
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
    \sa wc_curve448_import_public
    \sa wc_curve448_export_private_raw
*/

int wc_curve448_import_private_raw(const byte* priv, word32 privSz,
                            const byte* pub, word32 pubSz, curve448_key* key);

/*!
    \ingroup Curve448

    \brief This function imports a public-private key pair into a curve448_key structure.  Supports both big and little endian.

    \return 0 Returned on importing into the curve448_key structure.
    \return BAD_FUNC_ARG Returns if any of the input parameters are null.
    \return ECC_BAD_ARG_E Returned if the input key’s key size does not match
    the public or private key sizes.

    \param [in] priv Pointer to a buffer containing the private key to import.
    \param [in] privSz Length of the private key to import.
    \param [in] pub Pointer to a buffer containing the public key to import.
    \param [in] pubSz Length of the public key to import.
    \param [in,out] key Pointer to the structure in which to store the imported
    keys.
    \param [in]  endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set
    which form to use.

    _Example_
    \code
    int ret;

    byte priv[56];
    byte pub[56];
    // initialize with public and private keys
    curve448_key key;

    wc_curve448_init(&key);
    // initialize key

    ret = wc_curve448_import_private_raw_ex(&priv, sizeof(priv), pub,
            sizeof(pub), &key, EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error importing keys
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
    \sa wc_curve448_import_public
    \sa wc_curve448_export_private_raw
    \sa wc_curve448_import_private_raw
*/

int wc_curve448_import_private_raw_ex(const byte* priv, word32 privSz,
                                        const byte* pub, word32 pubSz,
                                        curve448_key* key, int endian);

/*!
    \ingroup Curve448

    \brief This function exports a private key from a curve448_key structure
    and stores it in the given out buffer. It also sets outLen to be the size
    of the exported key. Big Endian only.

    \return 0 Returned on successfully exporting the private key from the
    curve448_key structure.
    \return BAD_FUNC_ARG Returned if any input parameters are NULL.
    \return ECC_BAD_ARG_E Returned if wc_curve448_size() is not equal to key.

    \param [in] key Pointer to the structure from which to export the key.
    \param [out] out Pointer to the buffer in which to store the exported key.
    \param [in,out] outLen On in, is the size of the out in bytes.
    On out, will store the bytes written to the output buffer.

    _Example_
    \code
    int ret;
    byte priv[56];
    int privSz;

    curve448_key key;
    // initialize and make key

    ret = wc_curve448_export_private_raw(&key, priv, &privSz);
    if (ret != 0) {
        // error exporting key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
    \sa wc_curve448_import_private_raw
    \sa wc_curve448_export_private_raw_ex
*/

int wc_curve448_export_private_raw(curve448_key* key, byte* out,
                                     word32* outLen);

/*!
    \ingroup Curve448

    \brief This function exports a private key from a curve448_key structure
    and stores it in the given out buffer. It also sets outLen to be the size
    of the exported key.  Can specify whether it's big or little endian.

    \return 0 Returned on successfully exporting the private key from the
    curve448_key structure.
    \return BAD_FUNC_ARG Returned if any input parameters are NULL.
    \return ECC_BAD_ARG_E Returned if wc_curve448_size() is not equal to key.

    \param [in] key Pointer to the structure from which to export the key.
    \param [out] out Pointer to the buffer in which to store the exported key.
    \param [in,out] outLen On in, is the size of the out in bytes.
    On out, will store the bytes written to the output buffer.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set which
    form to use.

    _Example_
    \code
    int ret;

    byte priv[56];
    int privSz;
    curve448_key key;
    // initialize and make key
    ret = wc_curve448_export_private_raw_ex(&key, priv, &privSz,
            EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error exporting key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
    \sa wc_curve448_import_private_raw
    \sa wc_curve448_export_private_raw
    \sa wc_curve448_size
*/

int wc_curve448_export_private_raw_ex(curve448_key* key, byte* out,
                                        word32* outLen, int endian);

/*!
    \ingroup Curve448

    \brief This function imports a public key from the given in buffer and
    stores it in the  curve448_key structure.

    \return 0 Returned on successfully importing the public key into the
    curve448_key structure.
    \return ECC_BAD_ARG_E Returned if the inLen parameter does not match the key
    size of the key structure.
    \return BAD_FUNC_ARG Returned if any of the input parameters are NULL.

    \param [in] in Pointer to the buffer containing the public key to import.
    \param [in] inLen Length of the public key to import.
    \param [in,out] key Pointer to the curve448_key structure in which to store
    the key.

    _Example_
    \code
    int ret;

    byte pub[56];
    // initialize pub with public key

    curve448_key key;
    // initialize key

    ret = wc_curve448_import_public(pub,sizeof(pub), &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_export_public
    \sa wc_curve448_import_private_raw
    \sa wc_curve448_import_public_ex
    \sa wc_curve448_check_public
    \sa wc_curve448_size
*/

int wc_curve448_import_public(const byte* in, word32 inLen,
                                curve448_key* key);

/*!
    \ingroup Curve448

    \brief This function imports a public key from the given in buffer and
    stores it in the  curve448_key structure.

    \return 0 Returned on successfully importing the public key into the
    curve448_key structure.
    \return ECC_BAD_ARG_E Returned if the inLen parameter does not match the
    key size of the key structure.
    \return BAD_FUNC_ARG Returned if any of the input parameters are NULL.

    \param [in] in Pointer to the buffer containing the public key to import.
    \param [in] inLen Length of the public key to import.
    \param [in,out] key Pointer to the curve448_key structure in which to store
    the key.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set which
    form to use.

    _Example_
    \code
    int ret;

    byte pub[56];
    // initialize pub with public key
    curve448_key key;
    // initialize key

    ret = wc_curve448_import_public_ex(pub, sizeof(pub), &key,
            EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_export_public
    \sa wc_curve448_import_private_raw
    \sa wc_curve448_import_public
    \sa wc_curve448_check_public
    \sa wc_curve448_size
*/

int wc_curve448_import_public_ex(const byte* in, word32 inLen,
                                   curve448_key* key, int endian);

/*!
    \ingroup Curve448

    \brief This function checks that a public key buffer holds a valid
    Curve448 key value given the endian ordering.

    \return 0 Returned when the public key value is valid.
    \return ECC_BAD_ARG_E Returned if the public key value is not valid.
    \return BAD_FUNC_ARG Returned if any of the input parameters are NULL.

    \param [in] pub Pointer to the buffer containing the public key to check.
    \param [in] pubLen Length of the public key to check.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set which
    form to use.

    _Example_
    \code
    int ret;

    byte pub[] = { Contents of public key };

    ret = wc_curve448_check_public_ex(pub, sizeof(pub), EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error importing key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_import_public
    \sa wc_curve448_import_public_ex
    \sa wc_curve448_size
*/

int wc_curve448_check_public(const byte* pub, word32 pubSz, int endian);

/*!
    \ingroup Curve448

    \brief This function exports a public key from the given key structure and
    stores the result in the out buffer. Big endian only.

    \return 0 Returned on successfully exporting the public key from the
    curve448_key structure.
    \return ECC_BAD_ARG_E Returned if outLen is less than CURVE448_PUB_KEY_SIZE.
    \return BAD_FUNC_ARG Returned if any of the input parameters are NULL.

    \param [in] key Pointer to the curve448_key structure in from which to
    export the key.
    \param [out] out Pointer to the buffer in which to store the public key.
    \param [in,out] outLen On in, is the size of the out in bytes.
    On out, will store the bytes written to the output buffer.

    _Example_
    \code
    int ret;

    byte pub[56];
    int pubSz;

    curve448_key key;
    // initialize and make key

    ret = wc_curve448_export_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error exporting key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_export_private_raw
    \sa wc_curve448_import_public
*/

int wc_curve448_export_public(curve448_key* key, byte* out, word32* outLen);

/*!
    \ingroup Curve448

    \brief This function exports a public key from the given key structure and
    stores the result in the out buffer. Supports both big and little endian.

    \return 0 Returned on successfully exporting the public key from the
    curve448_key structure.
    \return ECC_BAD_ARG_E Returned if outLen is less than CURVE448_PUB_KEY_SIZE.
    \return BAD_FUNC_ARG Returned if any of the input parameters are NULL.

    \param [in] key Pointer to the curve448_key structure in from which to
    export the key.
    \param [out] out Pointer to the buffer in which to store the public key.
    \param [in,out] outLen On in, is the size of the out in bytes.
    On out, will store the bytes written to the output buffer.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set which
    form to use.

    _Example_
    \code
    int ret;

    byte pub[56];
    int pubSz;

    curve448_key key;
    // initialize and make key

    ret = wc_curve448_export_public_ex(&key, pub, &pubSz, EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error exporting key
    }
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_export_private_raw
    \sa wc_curve448_import_public
*/

int wc_curve448_export_public_ex(curve448_key* key, byte* out,
                                   word32* outLen, int endian);

/*!
    \ingroup Curve448

    \brief This function exports a key pair from the given key structure and
    stores the result in the out buffer. Big endian only.

    \return 0 Returned on successfully exporting the key pair from the
    curve448_key structure.
    \return BAD_FUNC_ARG Returned if any input parameters are NULL.
    \return ECC_BAD_ARG_E Returned if privSz is less than CURVE448_KEY_SIZE or
    pubSz is less than CURVE448_PUB_KEY_SIZE.

    \param [in] key Pointer to the curve448_key structure in from which to
    export the key pair.
    \param [out] priv Pointer to the buffer in which to store the private key.
    \param [in,out] privSz On in, is the size of the priv buffer in bytes.
    On out, will store the bytes written to the priv buffer.
    \param [out] pub Pointer to the buffer in which to store the public key.
    \param [in,out] pubSz On in, is the size of the pub buffer in bytes.
    On out, will store the bytes written to the pub buffer.

    _Example_
    \code
    int ret;

    byte pub[56];
    byte priv[56];
    int pubSz;
    int privSz;

    curve448_key key;
    // initialize and make key

    ret = wc_curve448_export_key_raw(&key, priv, &privSz, pub, &pubSz);
    if (ret != 0) {
        // error exporting key
    }
    \endcode

    \sa wc_curve448_export_key_raw_ex
    \sa wc_curve448_export_private_raw
*/

int wc_curve448_export_key_raw(curve448_key* key,
                                 byte* priv, word32 *privSz,
                                 byte* pub, word32 *pubSz);

/*!
    \ingroup Curve448

    \brief Export curve448 key pair.  Big or little endian.
    \brief This function exports a key pair from the given key structure and
    stores the result in the out buffer. Big or little endian.

    \return 0 Success
    \return BAD_FUNC_ARG Returned if any input parameters are NULL.
    \return ECC_BAD_ARG_E Returned if privSz is less than CURVE448_KEY_SIZE or
    pubSz is less than CURVE448_PUB_KEY_SIZE.

    \param [in] key Pointer to the curve448_key structure in from which to
    export the key pair.
    \param [out] priv Pointer to the buffer in which to store the private key.
    \param [in,out] privSz On in, is the size of the priv buffer in bytes.
    On out, will store the bytes written to the priv buffer.
    \param [out] pub Pointer to the buffer in which to store the public key.
    \param [in,out] pubSz On in, is the size of the pub buffer in bytes.
    On out, will store the bytes written to the pub buffer.
    \param [in] endian EC448_BIG_ENDIAN or EC448_LITTLE_ENDIAN to set which
    form to use.

    _Example_
    \code
    int ret;

    byte pub[56];
    byte priv[56];
    int pubSz;
    int privSz;

    curve448_key key;
    // initialize and make key

    ret = wc_curve448_export_key_raw_ex(&key,priv, &privSz, pub, &pubSz,
            EC448_BIG_ENDIAN);
    if (ret != 0) {
        // error exporting key
    }
    \endcode

    \sa wc_curve448_export_key_raw
    \sa wc_curve448_export_private_raw_ex
    \sa wc_curve448_export_public_ex
*/

int wc_curve448_export_key_raw_ex(curve448_key* key,
                                    byte* priv, word32 *privSz,
                                    byte* pub, word32 *pubSz,
                                    int endian);

/*!
    \ingroup Curve448

    \brief This function returns the key size of the given key structure.

    \return Success Given a valid, initialized curve448_key structure,
    returns the size of the key.
    \return 0 Returned if key is NULL.

    \param [in] key Pointer to the curve448_key structure in for which to
    determine the key size.

    _Example_
    \code
    int keySz;

    curve448_key key;
    // initialize and make key

    keySz = wc_curve448_size(&key);
    \endcode

    \sa wc_curve448_init
    \sa wc_curve448_make_key
*/

int wc_curve448_size(curve448_key* key);
