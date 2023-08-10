/*!
    \ingroup PSA 
    \brief  この関数は、与えられたコンテキストでのPSAサポートを可能にします。
    \param ctx  PSAサポートを有効にする必要があるWOLFSSL_CTXオブジェクトへのポインタ
    \return WOLFSSL_SUCCESS  成功した
    _Example_
    \code
    WOLFSSL_CTX *ctx;
    ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (!ctx)
        return NULL;
    ret = wolfSSL_CTX_psa_enable(ctx);
    if (ret != WOLFSSL_SUCCESS)
        printf("can't enable PSA on ctx");

    \endcode
    \sa wolfSSL_set_psa_ctx
*/
int wolfSSL_CTX_psa_enable(WOLFSSL_CTX *ctx);

/*!
    \ingroup PSA 
    \brief  与えられたSSLセッションのPSAコンテキストを設定する機能
    \param ssl  CTXが有効になるWolfSSLへのポインタ
    \param ctx  Struct PSA_SSL_CTXへのポインタ（SSLセッションに固有である必要があります）
    \return WOLFSSL_SUCCESS  成功した
    _Example_
    \code
    // Create new ssl session
    WOLFSSL *ssl;
    struct psa_ssl_ctx psa_ctx = { 0 };
    ssl = wolfSSL_new(ctx);
    if (!ssl)
        return NULL;
    // setup PSA context
    ret = wolfSSL_set_psa_ctx(ssl, ctx);
    \endcode
    \sa wolfSSL_psa_set_private_key_id
    \sa wolfSSL_psa_free_psa_ctx
*/
int wolfSSL_set_psa_ctx(WOLFSSL *ssl, struct psa_ssl_ctx *ctx);

/*!
    \ingroup PSA 
    \brief  この関数はPSAコンテキストによって使用されるリソースを解放します
    \sa wolfSSL_set_psa_ctx
*/
void wolfSSL_free_psa_ctx(struct psa_ssl_ctx *ctx);

/*!
    \ingroup PSA 
    \brief  この関数は、SSLセッションによって使用される秘密鍵を設定します
    \param ctx  構造体PSA_SSL_CTXへのポインタ
    _Example_
    \code
    // Create new ssl session
    WOLFSSL *ssl;
    struct psa_ssl_ctx psa_ctx = { 0 };
    psa_key_id_t key_id;

    // key provisioning already done
    get_private_key_id(&key_id);

    ssl = wolfSSL_new(ctx);
    if (!ssl)
        return NULL;

    wolfSSL_psa_set_private_key_id(&psa_ctx, key_id);
    wolfSSL_set_psa_ctx(ssl, ctx);
    \endcode
    \sa wolfSSL_set_psa_ctx
*/

int wolfSSL_psa_set_private_key_id(struct psa_ssl_ctx *ctx,
                                               psa_key_id_t id);
