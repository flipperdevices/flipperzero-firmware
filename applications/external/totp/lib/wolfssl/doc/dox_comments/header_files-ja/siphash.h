
/*!
    \ingroup SipHash 
    \brief  この関数は、MacサイズのキーでSiphashを初期化します。
    \return 0  初期化に成功したときに返されます
    \return BAD_FUNC_ARG  SiphashまたはキーがNULLのときに返されます
    \return BAD_FUNC_ARG  OUTSZが8でも16でもない場合に返されます
    \param siphash  Macingに使用するサイプハッシュ構造へのポインタ
    \param key  16バイト配列へのポインタ
    _Example_
    \code
    SipHash siphash[1];
    unsigned char key[16] = { ... };
    byte macSz = 8; // 8 or 16

    if ((ret = wc_InitSipHash(siphash, key, macSz)) != 0) {
        WOLFSSL_MSG("wc_InitSipHash failed");
    }
    else if ((ret = wc_SipHashUpdate(siphash, data, len)) != 0) {
        WOLFSSL_MSG("wc_SipHashUpdate failed");
    }
    else if ((ret = wc_SipHashFinal(siphash, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHashFinal failed");
    }
    \endcode
    \sa wc_SipHash
    \sa wc_SipHashUpdate
    \sa wc_SipHashFinal
*/
int wc_InitSipHash(SipHash* siphash, const unsigned char* key,
    unsigned char outSz);

/*!
    \ingroup SipHash 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  Macにデータを追加したら、返されます
    \return BAD_FUNC_ARG  Siphashがnullのとき返されました
    \return BAD_FUNC_ARG  inneがnullのとき返され、Inszはゼロではありません
    \param siphash  Macingに使用するサイプハッシュ構造へのポインタ
    \param in  マイートするデータ
    _Example_
    \code
    SipHash siphash[1];
    byte data[] = { Data to be MACed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSipHash(siphash, key, macSz)) != 0) {
        WOLFSSL_MSG("wc_InitSipHash failed");
    }
    else if ((ret = wc_SipHashUpdate(siphash, data, len)) != 0) {
        WOLFSSL_MSG("wc_SipHashUpdate failed");
    }
    else if ((ret = wc_SipHashFinal(siphash, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHashFinal failed");
    }
    \endcode
    \sa wc_SipHash
    \sa wc_InitSipHash
    \sa wc_SipHashFinal
*/
int wc_SipHashUpdate(SipHash* siphash, const unsigned char* in,
    word32 inSz);

/*!
    \ingroup SipHash 
    \brief  データのMacingを確定します。結果が出入りする。
    \return 0  ファイナライズに成功したときに返されます。
    \return BAD_FUNC_ARG  SiphashのOUTがNULLのときに返されます
    \return BAD_FUNC_ARG  OUTSZが初期化された値と同じではない場合に返されます
    \param siphash  Macingに使用するサイプハッシュ構造へのポインタ
    \param out  MAC値を保持するためのバイト配列
    _Example_
    \code
    SipHash siphash[1];
    byte mac[8] = { ... }; // 8 or 16 bytes
    byte macSz = sizeof(mac);

    if ((ret = wc_InitSipHash(siphash, key, macSz)) != 0) {
        WOLFSSL_MSG("wc_InitSipHash failed");
    }
    else if ((ret = wc_SipHashUpdate(siphash, data, len)) != 0) {
        WOLFSSL_MSG("wc_SipHashUpdate failed");
    }
    else if ((ret = wc_SipHashFinal(siphash, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHashFinal failed");
    }
    \endcode
    \sa wc_SipHash
    \sa wc_InitSipHash
    \sa wc_SipHashUpdate
*/
int wc_SipHashFinal(SipHash* siphash, unsigned char* out,
    unsigned char outSz);

/*!
    \ingroup SipHash 
    \brief  この機能はSiphashを使用してデータを1ショットして、キーに基づいてMACを計算します。
    \return 0  Macingに成功したときに返されました
    \return BAD_FUNC_ARG  キーまたはOUTがNULLのときに返されます
    \return BAD_FUNC_ARG  inneがnullのとき返され、Inszはゼロではありません
    \return BAD_FUNC_ARG  OUTSZが8でも16でもない場合に返されます
    \param key  16バイト配列へのポインタ
    \param in  マイートするデータ
    \param inSz  マイクされるデータのサイズ
    \param out  MAC値を保持するためのバイト配列
    _Example_
    \code
    unsigned char key[16] = { ... };
    byte data[] = { Data to be MACed };
    word32 len = sizeof(data);
    byte mac[8] = { ... }; // 8 or 16 bytes
    byte macSz = sizeof(mac);

    if ((ret = wc_SipHash(key, data, len, mac, macSz)) != 0) {
        WOLFSSL_MSG("wc_SipHash failed");
    }
    \endcode
    \sa wc_InitSipHash
    \sa wc_SipHashUpdate
    \sa wc_SipHashFinal
*/
int wc_SipHash(const unsigned char* key, const unsigned char* in,
    word32 inSz, unsigned char* out, unsigned char outSz);

