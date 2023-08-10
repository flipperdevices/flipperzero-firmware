/*!
    \ingroup PSA
    \brief This function enables PSA support on the given context.

    \param ctx pointer to the WOLFSSL_CTX object on which the PSA support must be enabled
    \return WOLFSSL_SUCCESS on success
    \return BAD_FUNC_ARG if ctx == NULL

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

    \brief This function setup the PSA context for the given SSL session

    \param ssl pointer to the WOLFSSL where the ctx will be enabled
    \param ctx pointer to a struct psa_ssl_ctx (must be unique for a ssl session)

    \return WOLFSSL_SUCCESS on success
    \return BAD_FUNC_ARG if ssl or ctx are NULL

    This function setup the PSA context for the TLS callbacks to the given SSL
    session. At the end of the session, the resources used by the context
    should be freed using wolfSSL_free_psa_ctx().

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
    \brief This function releases the resources used by a PSA context

    \param ctx pointer to a struct psa_ssl_ctx

    \sa wolfSSL_set_psa_ctx
*/
void wolfSSL_free_psa_ctx(struct psa_ssl_ctx *ctx);

/*!
    \ingroup PSA
    \brief This function set the private key used by an SSL session

    \param ctx pointer to a struct psa_ssl_ctx
    \param id PSA id of the key to be used as private key

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
