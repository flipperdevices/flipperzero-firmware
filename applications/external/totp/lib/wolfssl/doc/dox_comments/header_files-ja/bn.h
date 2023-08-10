/*!
    \ingroup openSSL 
    \brief  この関数は、次の数学「R =（A ^ P）％M」を実行します。
    \return SSL_SUCCESS  数学操作をうまく実行します。
    \return SSL_FAILURE  エラーケースに遭遇した場合
    \param r  結果を保持するための構造。
    \param a  電力で上げられる値。
    \param p  によって上げる力。
    \param m  使用率
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
