/*!
    \ingroup openSSL 
    \brief  それぞれのwolfssl_evp_cipherポインタのゲッター関数。最初にプログラム内でwolfssl_evp_init（）を1回呼び出す必要があります。wolfssl_des_ecbマクロは、wolfssl_evp_des_ede3_ecb（）に対して定義する必要があります。
    \return pointer  DES EDE3操作のためのwolfssl_evp_cipherポインタを返します。
    _Example_
    \code
    printf("block size des ede3 cbc = %d\n",
    wolfSSL_EVP_CIPHER_block_size(wolfSSL_EVP_des_ede3_cbc()));
    printf("block size des ede3 ecb = %d\n",
    wolfSSL_EVP_CIPHER_block_size(wolfSSL_EVP_des_ede3_ecb()));
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_init
*/
const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_ecb(void);

/*!
    \ingroup openSSL 
    \brief  それぞれのwolfssl_evp_cipherポインタのゲッター関数。最初にプログラム内でwolfssl_evp_init（）を1回呼び出す必要があります。wolfssl_des_ecbマクロは、wolfssl_evp_des_ecb（）に対して定義する必要があります。
    \return pointer  DES操作のためのwolfssl_evp_cipherポインタを返します。
    _Example_
    \code
    WOLFSSL_EVP_CIPHER* cipher;
    cipher = wolfSSL_EVP_des_cbc();
    …
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_init
*/
const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_cbc(void);

/*!
    \ingroup openSSL 
    \brief  wolfssl_evp_md_ctxを初期化する機能。この関数はwolfssl_engineがwolfssl_engineを使用しないため、wolfssl_evp_digestinit（）のラッパーです。
    \return SSL_SUCCESS  正常に設定されている場合。
    \return SSL_FAILURE  成功しなかった場合
    \param ctx  初期化する構造
    \param type  SHAなどのハッシュの種類。
    _Example_
    \code
    WOLFSSL_EVP_MD_CTX* md = NULL;
    wolfCrypt_Init();
    md = wolfSSL_EVP_MD_CTX_new();
    if (md == NULL) {
        printf("error setting md\n");
        return -1;
    }
    printf("cipher md init ret = %d\n", wolfSSL_EVP_DigestInit_ex(md,
    wolfSSL_EVP_sha1(), e));
    //free resources
    \endcode
    \sa wolfSSL_EVP_MD_CTX_new
    \sa wolfCrypt_Init
    \sa wolfSSL_EVP_MD_CTX_free
*/
int wolfSSL_EVP_DigestInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl);

/*!
    \ingroup openSSL 
    \brief  wolfssl_evp_cipher_ctxを初期化する機能。この関数はwolfssl_engineがwolfssl_engineを使用しないため、wolfssl_ciphinit（）のラッパーです。
    \return SSL_SUCCESS  正常に設定されている場合。
    \return SSL_FAILURE  成功しなかった場合
    \param ctx  初期化する構造
    \param type  AESなどの暗号化/復号化の種類。
    \param impl  使用するエンジン。wolfsslのn / aは、nullになることができます。
    \param key  設定するキー
    \param iv  アルゴリズムで必要な場合はIV。
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx = NULL;
    WOLFSSL_ENGINE* e = NULL;
    unsigned char key[16];
    unsigned char iv[12];
    wolfCrypt_Init();
    ctx = wolfSSL_EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("issue creating ctx\n");
        return -1;
    }

    printf("cipher init ex error ret = %d\n", wolfSSL_EVP_CipherInit_ex(NULL,
    EVP_aes_128_    cbc(), e, key, iv, 1));
    printf("cipher init ex success ret = %d\n", wolfSSL_EVP_CipherInit_ex(ctx,
    EVP_aes_128_c    bc(), e, key, iv, 1));
    // free resources
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
    \sa wolfCrypt_Init
    \sa wolfSSL_EVP_CIPHER_CTX_free
*/
int  wolfSSL_EVP_CipherInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv,
                                    int enc);

/*!
    \ingroup openSSL 
    \brief  wolfssl_evp_cipher_ctxを初期化する機能。WolfSSLはWOLFSSL_ENGINEを使用しないため、この関数はwolfssl_evp_ciphinit（）のラッパーです。暗号化フラグを暗号化するように設定します。
    \return SSL_SUCCESS  正常に設定されている場合。
    \return SSL_FAILURE  成功しなかった場合
    \param ctx  初期化する構造
    \param type  AESなどの暗号化の種類。
    \param impl  使用するエンジン。wolfsslのn / aは、nullになることができます。
    \param key  使用する鍵
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx = NULL;
    wolfCrypt_Init();
    ctx = wolfSSL_EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("error setting ctx\n");
        return -1;
    }
    printf("cipher ctx init ret = %d\n", wolfSSL_EVP_EncryptInit_ex(ctx,
    wolfSSL_EVP_aes_128_cbc(), e, key, iv));
    //free resources
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
    \sa wolfCrypt_Init
    \sa wolfSSL_EVP_CIPHER_CTX_free
*/
int  wolfSSL_EVP_EncryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv);

/*!
    \ingroup openSSL 
    \brief  wolfssl_evp_cipher_ctxを初期化する機能。WolfSSLはWOLFSSL_ENGINEを使用しないため、この関数はwolfssl_evp_ciphinit（）のラッパーです。暗号化フラグを復号化するように設定します。
    \return SSL_SUCCESS  正常に設定されている場合。
    \return SSL_FAILURE  成功しなかった場合
    \param ctx  初期化する構造
    \param type  AESなどの暗号化/復号化の種類。
    \param impl  使用するエンジン。wolfsslのn / aは、nullになることができます。
    \param key  設定するキー
    \param iv  アルゴリズムで必要な場合はIV。
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx = NULL;
    WOLFSSL_ENGINE* e = NULL;
    unsigned char key[16];
    unsigned char iv[12];

    wolfCrypt_Init();

    ctx = wolfSSL_EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("issue creating ctx\n");
        return -1;
    }

    printf("cipher init ex error ret = %d\n", wolfSSL_EVP_DecryptInit_ex(NULL,
    EVP_aes_128_    cbc(), e, key, iv, 1));
    printf("cipher init ex success ret = %d\n", wolfSSL_EVP_DecryptInit_ex(ctx,
    EVP_aes_128_c    bc(), e, key, iv, 1));
    // free resources
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
    \sa wolfCrypt_Init
    \sa wolfSSL_EVP_CIPHER_CTX_free
*/
int  wolfSSL_EVP_DecryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv);

/*!
    \ingroup openSSL 
    \brief  データを暗号化/復号化する機能。バッファ内では暗号化または復号化され、OUTバッファが結果を保持します。OUTORは暗号化/復号化された情報の長さになります。
    \return SSL_SUCCESS  成功した場合
    \return SSL_FAILURE  成功しなかった場合
    \param ctx  から暗号化の種類を取得するための構造。
    \param out  出力を保持するためのバッファ。
    \param outl  出力のサイズになるように調整しました。
    \param in  操作を実行するためのバッファー。
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx = NULL;
    unsigned char out[100];
    int outl;
    unsigned char in[100];
    int inl = 100;

    ctx = wolfSSL_EVP_CIPHER_CTX_new();
    // set up ctx
    ret = wolfSSL_EVP_CipherUpdate(ctx, out, outl, in, inl);
    // check ret value
    // buffer out holds outl bytes of data
    // free resources
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
    \sa wolfCrypt_Init
    \sa wolfSSL_EVP_CIPHER_CTX_free
*/
int wolfSSL_EVP_CipherUpdate(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl);

/*!
    \ingroup openSSL 
    \brief  この関数は、パディングを追加する最終暗号化操作を実行します。wolfssl_evp_ciph_no_paddingフラグがwolfssl_evp_cipher_ctx構造に設定されている場合、1が返され、暗号化/復号化は行われません。PADDING FLAGがSETIパディングを追加して暗号化すると、暗号化にCTXが設定されていると、復号化されたときにパディング値がチェックされます。
    \return 1  成功に戻りました。
    \return 0  失敗に遭遇した場合
    \param ctx  復号化/暗号化する構造。
    \param out  最後の復号化/暗号化のためのバッファ。
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx;
    int out1;
    unsigned char out[64];
    // create ctx
    wolfSSL_EVP_CipherFinal(ctx, out, &out1);
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
*/
int  wolfSSL_EVP_CipherFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);

/*!
    \ingroup openSSL 
    \brief  WolfSSL EVP_CIPHER_CTX構造キー長の設定機能
    \return SSL_SUCCESS  正常に設定されている場合。
    \return SSL_FAILURE  キーの長さを設定できなかった場合。
    \param ctx  キーの長さを設定する構造
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx;
    int keylen;
    // create ctx
    wolfSSL_EVP_CIPHER_CTX_set_key_length(ctx, keylen);
    \endcode
    \sa wolfSSL_EVP_CIPHER_flags
*/
int  wolfSSL_EVP_CIPHER_CTX_set_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                                     int keylen);

/*!
    \ingroup openSSL 
    \brief  これはCTXブロックサイズのGetter関数です。
    \return size  ctx-> block_sizeを返します。
    _Example_
    \code
    const WOLFSSL_CVP_CIPHER_CTX* ctx;
    //set up ctx
    printf(“block size = %d\n”, wolfSSL_EVP_CIPHER_CTX_block_size(ctx));
    \endcode
    \sa wolfSSL_EVP_CIPHER_block_size
*/
int wolfSSL_EVP_CIPHER_CTX_block_size(const WOLFSSL_EVP_CIPHER_CTX *ctx);

/*!
    \ingroup openSSL 
    \brief  これは暗号のブロックサイズのゲッター関数です。
    \return size  ブロックサイズを返します。
    _Example_
    \code
    printf(“block size = %d\n”,
    wolfSSL_EVP_CIPHER_block_size(wolfSSL_EVP_aes_256_ecb()));
    \endcode
    \sa wolfSSL_EVP_aes_256_ctr
*/
int wolfSSL_EVP_CIPHER_block_size(const WOLFSSL_EVP_CIPHER *cipher);

/*!
    \ingroup openSSL 
    \brief  WolfSSL evp_cipher_ctx構造の設定機能
    \return none  いいえ返します。
    \param ctx  フラグを設定する構造
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx;
    int flag;
    // create ctx
    wolfSSL_EVP_CIPHER_CTX_set_flags(ctx, flag);
    \endcode
    \sa wolfSSL_EVP_CIPHER_flags
    \sa wolfSSL_EVP_CIPHER_CTX_flags
*/
void wolfSSL_EVP_CIPHER_CTX_set_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags);

/*!
    \ingroup openSSL 
    \brief  WolfSSL evp_cipher_ctx構造のクリア機能
    \return none  いいえ返します。
    \param ctx  フラグをクリアするための構造
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx;
    int flag;
    // create ctx
    wolfSSL_EVP_CIPHER_CTX_clear_flags(ctx, flag);
    \endcode
    \sa wolfSSL_EVP_CIPHER_flags
    \sa wolfSSL_EVP_CIPHER_CTX_flags
*/
void wolfSSL_EVP_CIPHER_CTX_clear_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags);

/*!
    \ingroup openSSL 
    \brief  wolfssl_evp_cipher_ctx構造のためのセッター機能パディングを使用する。
    \return SSL_SUCCESS  正常に設定されている場合。
    \return BAD_FUNC_ARG  NULL引数が渡された場合。
    \param ctx  パディングフラグを設定する構造
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx;
    // create ctx
    wolfSSL_EVP_CIPHER_CTX_set_padding(ctx, 1);
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
*/
int  wolfSSL_EVP_CIPHER_CTX_set_padding(WOLFSSL_EVP_CIPHER_CTX *c, int pad);


/*!
    \ingroup openSSL 
    \brief  wolfssl_evp_cipher_ctx構造のゲッター関数廃止予定のV1.1.0
    \return unsigned  フラグ/モードの長い。
    _Example_
    \code
    WOLFSSL_EVP_CIPHER_CTX* ctx;
    unsigned long flags;
    ctx = wolfSSL_EVP_CIPHER_CTX_new()
    flags = wolfSSL_EVP_CIPHER_CTX_flags(ctx);
    \endcode
    \sa wolfSSL_EVP_CIPHER_CTX_new
    \sa wolfSSL_EVP_CIPHER_flags
*/
unsigned long wolfSSL_EVP_CIPHER_CTX_flags(const WOLFSSL_EVP_CIPHER_CTX *ctx);
