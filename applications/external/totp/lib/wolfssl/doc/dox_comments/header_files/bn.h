/*!
    \ingroup openSSL

    \brief This function performs the following math “r = (a^p) % m”.

    \return SSL_SUCCESS On successfully performing math operation.
    \return SSL_FAILURE If an error case was encountered.

    \param r structure to hold result.
    \param a value to be raised by a power.
    \param p power to raise a by.
    \param m modulus to use.
    \param ctx currently not used with wolfSSL can be NULL.

    _Example_
    \code
    WOLFSSL_BIGNUM r,a,p,m;
    int ret;
    // set big number values
    ret  = wolfSSL_BN_mod_exp(r, a, p, m, NULL);
    // check ret value
    \endcode

    \sa wolfSSL_BN_new
    \sa wolfSSL_BN_free
*/
int wolfSSL_BN_mod_exp(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
        const WOLFSSL_BIGNUM *p, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx);
