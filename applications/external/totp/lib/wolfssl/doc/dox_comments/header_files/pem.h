/*!
    \ingroup openSSL

    \brief This function writes a key into a WOLFSSL_BIO structure
    in PEM format.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE upon failure.

    \param bio WOLFSSL_BIO structure to get PEM buffer from.
    \param key key to convert to PEM format.
    \param cipher EVP cipher structure.
    \param passwd password.
    \param len length of password.
    \param cb password callback.
    \param arg optional argument.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    WOLFSSL_EVP_PKEY* key;
    int ret;
    // create bio and setup key
    ret = wolfSSL_PEM_write_bio_PrivateKey(bio, key, NULL, NULL, 0, NULL, NULL);
    //check ret value
    \endcode

    \sa wolfSSL_PEM_read_bio_X509_AUX
*/

int wolfSSL_PEM_write_bio_PrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EVP_PKEY* key,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        wc_pem_password_cb* cb, void* arg);
