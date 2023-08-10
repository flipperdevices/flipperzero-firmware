/*!
    \ingroup openSSL 
    \brief  この関数は、PEM形式のwolfssl_bio構造体にキーを書き込みます。
    \return SSL_SUCCESS  成功すると。
    \return SSL_FAILURE  失敗すると。
    \param bio  wolfssl_bio構造体からPEMバッファを取得します。
    \param key  PEM形式に変換するためのキー。
    \param cipher  EVP暗号構造
    \param passwd  パスワード。
    \param len  パスワードの長さ
    \param cb  パスワードコールバック
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
