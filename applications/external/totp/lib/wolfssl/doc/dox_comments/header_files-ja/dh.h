/*!
    \ingroup Diffie-Hellman 
    \brief  この関数は、Diffie-Hellman Exchangeプロトコルを使用して安全な秘密鍵を交渉するのに使用するためのDiffie-Hellmanキーを初期化します。
    \return none  いいえ返します。
    _Example_
    \code
    DhKey key;
    wc_InitDhKey(&key); // initialize DH key
    \endcode
    \sa wc_FreeDhKey
    \sa wc_DhGenerateKeyPair
*/
int wc_InitDhKey(DhKey* key);

/*!
    \ingroup Diffie-Hellman 
    \brief  この関数は、Diffie-Hellman Exchangeプロトコルを使用して安全な秘密鍵をネゴシエートするために使用された後にDiffie-Hellmanキーを解放します。
    \return none  いいえ返します。
    _Example_
    \code
    DhKey key;
    // initialize key, perform key exchange

    wc_FreeDhKey(&key); // free DH key to avoid memory leaks
    \endcode
    \sa wc_InitDhKey
*/
void wc_FreeDhKey(DhKey* key);

/*!
    \ingroup Diffie-Hellman 
    \brief  この関数はdiffie-hellmanパブリックパラメータに基づいてパブリック/秘密鍵ペアを生成し、PRIVSの秘密鍵とPubの公開鍵を格納します。初期化されたDiffie-Hellmanキーと初期化されたRNG構造を取ります。
    \return BAD_FUNC_ARG  この関数への入力の1つを解析するエラーがある場合に返されます
    \return RNG_FAILURE_E  RNGを使用して乱数を生成するエラーが発生した場合
    \return MP_INIT_E  公開鍵の生成中に数学ライブラリにエラーがある場合は返却される可能性があります
    \return MP_READ_E  公開鍵の生成中に数学ライブラリにエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  公開鍵の生成中に数学ライブラリにエラーがある場合は返却される可能性があります
    \return MP_TO_E  公開鍵の生成中に数学ライブラリにエラーがある場合は返却される可能性があります
    \param key  キーペアを生成するDHKEY構造体へのポインタ
    \param rng  キーを生成するための初期化された乱数発生器（RNG）へのポインタ
    \param priv  秘密鍵を保存するバッファへのポインタ
    \param privSz  PRIVに書かれた秘密鍵のサイズを保存します
    \param pub  公開鍵を保存するバッファへのポインタ
    _Example_
    \code
    DhKey key;
    int ret;
    byte priv[256];
    byte pub[256];
    word32 privSz, pubSz;

    wc_InitDhKey(&key); // initialize key
    // Set DH parameters using wc_DhSetKey or wc_DhKeyDecode
    WC_RNG rng;
    wc_InitRng(&rng); // initialize rng
    ret = wc_DhGenerateKeyPair(&key, &rng, priv, &privSz, pub, &pubSz);
    \endcode
    \sa wc_InitDhKey
    \sa wc_DhSetKey
    \sa wc_DhKeyDecode
*/
int wc_DhGenerateKeyPair(DhKey* key, WC_RNG* rng, byte* priv,
                                 word32* privSz, byte* pub, word32* pubSz);

/*!
    \ingroup Diffie-Hellman 
    \brief  この関数は、ローカル秘密鍵と受信した公開鍵に基づいて合意された秘密鍵を生成します。交換の両側で完了した場合、この関数は対称通信のための秘密鍵の合意を生成します。共有秘密鍵の生成に成功すると、書かれた秘密鍵のサイズは仲間に保存されます。
    \return 0  合意された秘密鍵の生成に成功しました
    \return MP_INIT_E  共有秘密鍵の生成中にエラーが発生した場合に返却される可能性があります
    \return MP_READ_E  共有秘密鍵の生成中にエラーが発生した場合に返却される可能性があります
    \return MP_EXPTMOD_E  共有秘密鍵の生成中にエラーが発生した場合に返却される可能性があります
    \return MP_TO_E  共有秘密鍵の生成中にエラーが発生した場合に返却される可能性があります
    \param key  共有キーを計算するために使用するDHKEY構造体へのポインタ
    \param agree  秘密キーを保存するバッファへのポインタ
    \param agreeSz  成功した後に秘密鍵のサイズを保持します
    \param priv  ローカル秘密鍵を含むバッファへのポインタ
    \param privSz  地元の秘密鍵のサイズ
    \param otherPub  受信した公開鍵を含むバッファへのポインタ
    _Example_
    \code
    DhKey key;
    int ret;
    byte priv[256];
    byte agree[256];
    word32 agreeSz;

    // initialize key, set key prime and base
    // wc_DhGenerateKeyPair -- store private key in priv
    byte pub[] = { // initialized with the received public key };
    ret = wc_DhAgree(&key, agree, &agreeSz, priv, sizeof(priv), pub,
    sizeof(pub));
    if ( ret != 0 ) {
    	// error generating shared key
    }
    \endcode
    \sa wc_DhGenerateKeyPair
*/
int wc_DhAgree(DhKey* key, byte* agree, word32* agreeSz,
                       const byte* priv, word32 privSz, const byte* otherPub,
                       word32 pubSz);

/*!
    \ingroup Diffie-Hellman 
    \brief  この機能は、DERフォーマットのキーを含む与えられた入力バッファからDiffie-Hellmanキーをデコードします。結果をDHKEY構造体に格納します。
    \return 0  入力キーの復号に成功したときに返されます
    \return ASN_PARSE_E  入力のシーケンスを解析したエラーがある場合に返されます
    \return ASN_DH_KEY_E  解析された入力から秘密鍵パラメータを読み取るエラーがある場合
    \param input  derフォーマットされたdiffie-hellmanキーを含むバッファへのポインタ
    \param inOutIdx  キーをデコードしている間に解析されたインデックスを保存する整数へのポインタ
    \param key  入力キーで初期化するためのDHKEY構造体へのポインタ
    _Example_
    \code
    DhKey key;
    word32 idx = 0;

    byte keyBuff[1024];
    // initialize with DER formatted key
    wc_DhKeyInit(&key);
    ret = wc_DhKeyDecode(keyBuff, &idx, &key, sizeof(keyBuff));

    if ( ret != 0 ) {
    	// error decoding key
    }
    \endcode
    \sa wc_DhSetKey
*/
int wc_DhKeyDecode(const byte* input, word32* inOutIdx, DhKey* key,
                           word32);

/*!
    \ingroup Diffie-Hellman 
    \brief  この関数は、入力秘密鍵パラメータを使用してDHKEY構造体のキーを設定します。WC_DHKEYDECODEとは異なり、この関数は入力キーがDERフォーマットでフォーマットされ、代わりにPARSED入力パラメータP（Prime）とG（Base）を受け入れる必要はありません。
    \return 0  鍵の設定に成功しました
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLに評価された場合に返されます。
    \return MP_INIT_E  ストレージのキーパラメータの初期化中にエラーがある場合に返されます。
    \return ASN_DH_KEY_E  DHキーパラメータPおよびGでエラーの読み取りがある場合は返されます
    \param key  キーを設定するDHKEY構造体へのポインタ
    \param p  キーで使用するためのプライムを含むバッファへのポインタ
    \param pSz  入力プライムの長さ
    \param g  キーで使用するためのベースを含むバッファへのポインタ
    _Example_
    \code
    DhKey key;

    byte p[] = { // initialize with prime };
    byte g[] = { // initialize with base };
    wc_DhKeyInit(&key);
    ret = wc_DhSetKey(key, p, sizeof(p), g, sizeof(g));

    if ( ret != 0 ) {
    	// error setting key
    }
    \endcode
    \sa wc_DhKeyDecode
*/
int wc_DhSetKey(DhKey* key, const byte* p, word32 pSz, const byte* g,
                        word32 gSz);

/*!
    \ingroup Diffie-Hellman 
    \brief  この関数は、与えられた入力バッファからDiffie-HellmanパラメータP（Prime）とG（ベース）をフォーマットされています。
    \return 0  DHパラメータの抽出に成功しました
    \return ASN_PARSE_E  DERフォーマットのDH証明書の解析中にエラーが発生した場合に返されます。
    \return BUFFER_E  解析されたパラメータを格納するためにPまたはGに不適切なスペースがある場合
    \param input  解析するDERフォーマットされたDifie-Hellman証明書を含むバッファへのポインタ
    \param inSz  入力バッファのサイズ
    \param p  解析されたプライムを保存するバッファへのポインタ
    \param pInOutSz  Pバッファ内の使用可能なサイズを含むWord32オブジェクトへのポインタ。関数呼び出しを完了した後にバッファに書き込まれたバイト数で上書きされます。
    \param g  解析されたベースを保存するバッファへのポインタ
    _Example_
    \code
    byte dhCert[] = { initialize with DER formatted certificate };
    byte p[MAX_DH_SIZE];
    byte g[MAX_DH_SIZE];
    word32 pSz = MAX_DH_SIZE;
    word32 gSz = MAX_DH_SIZE;

    ret = wc_DhParamsLoad(dhCert, sizeof(dhCert), p, &pSz, g, &gSz);
    if ( ret != 0 ) {
    	// error parsing inputs
    }
    \endcode
    \sa wc_DhSetKey
    \sa wc_DhKeyDecode
*/
int wc_DhParamsLoad(const byte* input, word32 inSz, byte* p,
                            word32* pInOutSz, byte* g, word32* gInOutSz);

/*!
    \ingroup Diffie-Hellman 
    \sa wc_Dh_ffdhe3072_Get
    \sa wc_Dh_ffdhe4096_Get
    \sa wc_Dh_ffdhe6144_Get
    \sa wc_Dh_ffdhe8192_Get
*/
const DhParams* wc_Dh_ffdhe2048_Get(void);

/*!
    \ingroup Diffie-Hellman 
    \sa wc_Dh_ffdhe2048_Get
    \sa wc_Dh_ffdhe4096_Get
    \sa wc_Dh_ffdhe6144_Get
    \sa wc_Dh_ffdhe8192_Get
*/
const DhParams* wc_Dh_ffdhe3072_Get(void);

/*!
    \ingroup Diffie-Hellman 
    \sa wc_Dh_ffdhe2048_Get
    \sa wc_Dh_ffdhe3072_Get
    \sa wc_Dh_ffdhe6144_Get
    \sa wc_Dh_ffdhe8192_Get
*/
const DhParams* wc_Dh_ffdhe4096_Get(void);

/*!
    \ingroup Diffie-Hellman 
    \sa wc_Dh_ffdhe2048_Get
    \sa wc_Dh_ffdhe3072_Get
    \sa wc_Dh_ffdhe4096_Get
    \sa wc_Dh_ffdhe8192_Get
*/
const DhParams* wc_Dh_ffdhe6144_Get(void);

/*!
    \ingroup Diffie-Hellman 
    \sa wc_Dh_ffdhe2048_Get
    \sa wc_Dh_ffdhe3072_Get
    \sa wc_Dh_ffdhe4096_Get
    \sa wc_Dh_ffdhe6144_Get
*/
const DhParams* wc_Dh_ffdhe8192_Get(void);

/*!
    \ingroup Diffie-Hellman 
*/
int wc_DhCheckKeyPair(DhKey* key, const byte* pub, word32 pubSz,
                        const byte* priv, word32 privSz);

/*!
    \ingroup Diffie-Hellman 
*/
int wc_DhCheckPrivKey(DhKey* key, const byte* priv, word32 pubSz);

/*!
*/
int wc_DhCheckPrivKey_ex(DhKey* key, const byte* priv, word32 pubSz,
                            const byte* prime, word32 primeSz);

/*!
*/
int wc_DhCheckPubKey(DhKey* key, const byte* pub, word32 pubSz);

/*!
*/
int wc_DhCheckPubKey_ex(DhKey* key, const byte* pub, word32 pubSz,
                            const byte* prime, word32 primeSz);

/*!
*/
int wc_DhExportParamsRaw(DhKey* dh, byte* p, word32* pSz,
                       byte* q, word32* qSz, byte* g, word32* gSz);

/*!
*/
int wc_DhGenerateParams(WC_RNG *rng, int modSz, DhKey *dh);

/*!
*/
int wc_DhSetCheckKey(DhKey* key, const byte* p, word32 pSz,
                        const byte* g, word32 gSz, const byte* q, word32 qSz,
                        int trusted, WC_RNG* rng);

/*!
*/
int wc_DhSetKey_ex(DhKey* key, const byte* p, word32 pSz,
                        const byte* g, word32 gSz, const byte* q, word32 qSz);

/*!
*/
int wc_FreeDhKey(DhKey* key);
