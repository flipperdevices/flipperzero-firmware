/*!
    \ingroup DSA 
    \brief  この関数は、デジタル署名アルゴリズム（DSA）を介した認証に使用するためにDSAKEYオブジェクトを初期化します。
    \return 0  成功に戻りました。
    \return BAD_FUNC_ARG  NULLキーが渡された場合に返されます。
    _Example_
    \code
    DsaKey key;
    int ret;
    ret = wc_InitDsaKey(&key); // initialize DSA key
    \endcode
    \sa wc_FreeDsaKey
*/
int wc_InitDsaKey(DsaKey* key);

/*!
    \ingroup DSA 
    \brief  この関数は、使用された後にdsakeyオブジェクトを解放します。
    \return none  いいえ返します。
    _Example_
    \code
    DsaKey key;
    // initialize key, use for authentication
    ...
    wc_FreeDsaKey(&key); // free DSA key
    \endcode
    \sa wc_FreeDsaKey
*/
void wc_FreeDsaKey(DsaKey* key);

/*!
    \ingroup DSA 
    \brief  この機能は入力ダイジェストに署名し、結果を出力バッファーに格納します。
    \return 0  入力ダイジェストに正常に署名したときに返されました
    \return MP_INIT_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_READ_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_CMP_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_INVMOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_EXPTMOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MUL_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_ADD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MULMOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_TO_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MEM  DSA署名の処理にエラーがある場合は返される可能性があります。
    \param digest  署名するハッシュへのポインタ
    \param out  署名を保存するバッファへのポインタ
    \param key  署名を生成するための初期化されたDsakey構造へのポインタ
    _Example_
    \code
    DsaKey key;
    // initialize DSA key, load private Key
    int ret;
    WC_RNG rng;
    wc_InitRng(&rng);
    byte hash[] = { // initialize with hash digest };
    byte signature[40]; // signature will be 40 bytes (320 bits)

    ret = wc_DsaSign(hash, signature, &key, &rng);
    if (ret != 0) {
	    // error generating DSA signature
    }
    \endcode
    \sa wc_DsaVerify
*/
int wc_DsaSign(const byte* digest, byte* out,
                           DsaKey* key, WC_RNG* rng);

/*!
    \ingroup DSA 
    \brief  この関数は、秘密鍵を考えると、ダイジェストの署名を検証します。回答パラメータでキーが正しく検証されているかどうか、正常な検証に対応する1、および失敗した検証に対応する0が格納されます。
    \return 0  検証要求の処理に成功したときに返されます。注：これは、署名が検証されていることを意味するわけではなく、関数が成功したというだけです。
    \return MP_INIT_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_READ_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_CMP_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_INVMOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_EXPTMOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MUL_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_ADD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MULMOD_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_TO_E  DSA署名の処理にエラーがある場合は返される可能性があります。
    \return MP_MEM  DSA署名の処理にエラーがある場合は返される可能性があります。
    \param digest  署名の主題を含むダイジェストへのポインタ
    \param sig  確認する署名を含むバッファへのポインタ
    \param key  署名を検証するための初期化されたDsakey構造へのポインタ
    _Example_
    \code
    DsaKey key;
    // initialize DSA key, load public Key

    int ret;
    int verified;
    byte hash[] = { // initialize with hash digest };
    byte signature[] = { // initialize with signature to verify };
    ret = wc_DsaVerify(hash, signature, &key, &verified);
    if (ret != 0) {
    	// error processing verify request
    } else if (answer == 0) {
    	// invalid signature
    }
    \endcode
    \sa wc_DsaSign
*/
int wc_DsaVerify(const byte* digest, const byte* sig,
                             DsaKey* key, int* answer);

/*!
    \ingroup DSA 
    \brief  この機能は、DSA公開鍵を含むDERフォーマットの証明書バッファを復号し、与えられたDSakey構造体にキーを格納します。また、入力読み取りの長さに応じてINOUTIDXパラメータを設定します。
    \return 0  dsakeyオブジェクトの公開鍵を正常に設定する
    \return ASN_PARSE_E  証明書バッファを読みながらエンコーディングにエラーがある場合
    \return ASN_DH_KEY_E  DSAパラメータの1つが誤ってフォーマットされている場合に返されます
    \param input  DERフォーマットDSA公開鍵を含むバッファへのポインタ
    \param inOutIdx  証明書の最後のインデックスを保存する整数へのポインタ
    \param key  公開鍵を保存するDsakey構造へのポインタ
    _Example_
    \code
    int ret, idx=0;

    DsaKey key;
    wc_InitDsaKey(&key);
    byte derBuff[] = { // DSA public key};
    ret = wc_DsaPublicKeyDecode(derBuff, &idx, &key, inSz);
    if (ret != 0) {
    	// error reading public key
    }
    \endcode
    \sa wc_InitDsaKey
    \sa wc_DsaPrivateKeyDecode
*/
int wc_DsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                      DsaKey* key, word32 inSz);

/*!
    \ingroup DSA 
    \brief  この機能は、DSA秘密鍵を含むDERフォーマットの証明書バッファをデコードし、指定されたDSakey構造体にキーを格納します。また、入力読み取りの長さに応じてINOUTIDXパラメータを設定します。
    \return 0  dsakeyオブジェクトの秘密鍵を正常に設定するに返されました
    \return ASN_PARSE_E  証明書バッファを読みながらエンコーディングにエラーがある場合
    \return ASN_DH_KEY_E  DSAパラメータの1つが誤ってフォーマットされている場合に返されます
    \param input  DERフォーマットDSA秘密鍵を含むバッファへのポインタ
    \param inOutIdx  証明書の最後のインデックスを保存する整数へのポインタ
    \param key  秘密鍵を保存するDSakey構造へのポインタ
    _Example_
    \code
    int ret, idx=0;

    DsaKey key;
    wc_InitDsaKey(&key);
    byte derBuff[] = { // DSA private key };
    ret = wc_DsaPrivateKeyDecode(derBuff, &idx, &key, inSz);
    if (ret != 0) {
    	// error reading private key
    }
    \endcode
    \sa wc_InitDsaKey
    \sa wc_DsaPublicKeyDecode
*/
int wc_DsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                       DsaKey* key, word32 inSz);

/*!
    \ingroup DSA 
    \brief  DSAKEYキーをDERフォーマット、出力への書き込み（Inlen）、書き込まれたバイトを返します。
    \return outLen  成功、書かれたバイト数
    \return BAD_FUNC_ARG  キーまたは出力はNULLまたはキー - >タイプがDSA_PRIVATEではありません。
    \return MEMORY_E  メモリの割り当て中にエラーが発生しました。
    \param key  変換するdsakey構造へのポインタ。
    \param output  変換キーの出力バッファへのポインタ。
    _Example_
    \code
    DsaKey key;
    WC_RNG rng;
    int derSz;
    int bufferSize = // Sufficient buffer size;
    byte der[bufferSize];

    wc_InitDsaKey(&key);
    wc_InitRng(&rng);
    wc_MakeDsaKey(&rng, &key);
    derSz = wc_DsaKeyToDer(&key, der, bufferSize);
    \endcode
    \sa wc_InitDsaKey
    \sa wc_FreeDsaKey
    \sa wc_MakeDsaKey
*/
int wc_DsaKeyToDer(DsaKey* key, byte* output, word32 inLen);

/*!
    \ingroup DSA 
    \brief  DSAキーを作成します。
    \return MP_OKAY  成功
    \return BAD_FUNC_ARG  RNGまたはDSAのどちらかがnullです。
    \return MEMORY_E  バッファにメモリを割り当てることができませんでした。
    \return MP_INIT_E  MP_INTの初期化エラー
    \param rng  WC_RNG構造体へのポインタ。
    _Example_
    \code
    WC_RNG rng;
    DsaKey dsa;
    wc_InitRng(&rng);
    wc_InitDsa(&dsa);
    if(wc_MakeDsaKey(&rng, &dsa) != 0)
    {
        // Error creating key
    }
    \endcode
    \sa wc_InitDsaKey
    \sa wc_FreeDsaKey
    \sa wc_DsaSign
*/
int wc_MakeDsaKey(WC_RNG *rng, DsaKey *dsa);

/*!
    \ingroup DSA 
    \brief  FIPS 186-4は、modulus_size値の有効な値を定義します（1024,160）（2048,256）（3072,256）
    \return 0  成功
    \return BAD_FUNC_ARG  RNGまたはDSAはNULLまたはMODULUS_SIZEが無効です。
    \return MEMORY_E  メモリを割り当てようとするエラーが発生しました。
    \param rng  WolfCrypt RNGへのポインタ。
    \param modulus_size  1024,2048、または3072は有効な値です。
    _Example_
    \code
    DsaKey key;
    WC_RNG rng;
    wc_InitDsaKey(&key);
    wc_InitRng(&rng);
    if(wc_MakeDsaParameters(&rng, 1024, &genKey) != 0)
    {
        // Handle error
    }
    \endcode
    \sa wc_MakeDsaKey
    \sa wc_DsaKeyToDer
    \sa wc_InitDsaKey
*/
int wc_MakeDsaParameters(WC_RNG *rng, int modulus_size, DsaKey *dsa);
