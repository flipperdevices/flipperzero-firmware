/*!
    \ingroup HMAC 
    \brief  この関数はHMACオブジェクトを初期化し、その暗号化タイプ、キー、およびHMACの長さを設定します。
    \return 0  HMACオブジェクトの初期化に成功しました
    \return BAD_FUNC_ARG  入力タイプが無効な場合は返されます。有効なオプションは次のとおりです.MD5、SHA、SHA256、SHA384、SHA3-224、SHA3-256、SHA3-384、SHA3-512
    \return MEMORY_E  ハッシュに使用する構造体の割り当てメモリの割り当てがある場合
    \return HMAC_MIN_KEYLEN_E  FIPS実装を使用するときに返されることがあり、指定されたキー長は最小許容FIPS規格よりも短いです。
    \param hmac  初期化するHMACオブジェクトへのポインタ
    \param type  HMACオブジェクトを使用する暗号化方式を指定します。有効なオプションは次のとおりです.MD5、SHA、SHA256、SHA384、SHA3-224、SHA3-256、SHA3-384、SHA3-512
    \param key  HMACオブジェクトを初期化するキーを含むバッファへのポインタ
    _Example_
    \code
    Hmac hmac;
    byte key[] = { // initialize with key to use for encryption };
    if (wc_HmacSetKey(&hmac, MD5, key, sizeof(key)) != 0) {
    	// error initializing Hmac object
    }
    \endcode
    \sa wc_HmacUpdate
    \sa wc_HmacFinal
*/
int wc_HmacSetKey(Hmac* hmac, int type, const byte* key, word32 keySz);

/*!
    \ingroup HMAC 
    \brief  この関数は、HMACを使用して認証するメッセージを更新します。HMACオブジェクトがWC_HMACSETKEYで初期化された後に呼び出されるべきです。この関数は、ハッシュへのメッセージを更新するために複数回呼び出されることがあります。必要に応じてwc_hmacupdateを呼び出した後、最終認証済みメッセージタグを取得するためにwc_hmacfinalを呼び出す必要があります。
    \return 0  認証するメッセージの更新に成功しました
    \return MEMORY_E  ハッシュアルゴリズムで使用するためにメモリを割り当てるエラーがある場合
    \param hmac  メッセージを更新するHMACオブジェクトへのポインタ
    \param msg  追加するメッセージを含むバッファへのポインタ
    _Example_
    \code
    Hmac hmac;
    byte msg[] = { // initialize with message to authenticate };
    byte msg2[] = { // initialize with second half of message };
    // initialize hmac
    if( wc_HmacUpdate(&hmac, msg, sizeof(msg)) != 0) {
    	// error updating message
    }
    if( wc_HmacUpdate(&hmac, msg2, sizeof(msg)) != 0) {
    	// error updating with second message
    }
    \endcode
    \sa wc_HmacSetKey
    \sa wc_HmacFinal
*/
int wc_HmacUpdate(Hmac* hmac, const byte* in, word32 sz);

/*!
    \ingroup HMAC 
    \brief  この関数は、HMACオブジェクトのメッセージの最終ハッシュを計算します。
    \return 0  最後のハッシュの計算に成功した
    \return MEMORY_E  ハッシュアルゴリズムで使用するためにメモリを割り当てるエラーがある場合
    \param hmac  最終ハッシュを計算するHMACオブジェクトへのポインタ
    _Example_
    \code
    Hmac hmac;
    byte hash[MD5_DIGEST_SIZE];
    // initialize hmac with MD5 as type
    // wc_HmacUpdate() with messages

    if (wc_HmacFinal(&hmac, hash) != 0) {
    	// error computing hash
    }
    \endcode
    \sa wc_HmacSetKey
    \sa wc_HmacUpdate
*/
int wc_HmacFinal(Hmac* hmac, byte* out);

/*!
    \ingroup HMAC 
    \brief  この関数は、構成された暗号スイートに基づいて使用可能な最大のHMACダイジェストサイズを返します。
    \return Success  設定された暗号スイートに基づいて使用可能な最大のHMACダイジェストサイズを返します
    _Example_
    \code
    int maxDigestSz = wolfSSL_GetHmacMaxSize();
    \endcode
    \sa none
*/
int wolfSSL_GetHmacMaxSize(void);

/*!
    \ingroup HMAC 
    \brief  この関数は、HMACキー導出機能（HKDF）へのアクセスを提供します。HMACを利用して、任意のSALTとオプションの情報を派生したキーに変換します。0またはNULLが指定されている場合、ハッシュ型はデフォルトでMD5になります。
    \return 0  与えられた入力でキーの生成に成功したら返されます
    \return BAD_FUNC_ARG  無効なハッシュ型が引数として指定されている場合に返されます。有効な型は次のとおりです.MD5、SHA、SHA256、SHA384、SHA3-224、SHA3-256、SHA3-384、SHA3-512
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return HMAC_MIN_KEYLEN_E  FIPS実装を使用するときに返されることがあり、指定されたキー長は最小許容FIPS規格よりも短いです。
    \param type  HKDFに使用するハッシュタイプ。有効な型は次のとおりです.MD5、SHA、SHA256、SHA384、SHA3-224、SHA3-256、SHA3-384、SHA3-512
    \param inKey  KDFに使用するキーを含むバッファへのポインタ
    \param inKeySz  入力キーの長さ
    \param salt  任意の塩を含む緩衝液へのポインタ。塩を使用しない場合は代わりにNULLを使用してください
    \param saltSz  塩の長さ。塩を使用しない場合は0を使用してください
    \param info  オプションの追加情報を含むバッファへのポインタ。追加情報を追加していない場合はNULLを使用してください
    \param infoSz  追加情報の長さ追加情報を使用しない場合は0を使用してください
    \param out  派生キーを保存するバッファへのポインタ
    _Example_
    \code
    byte key[] = { // initialize with key };
    byte salt[] = { // initialize with salt };
    byte derivedKey[MAX_DIGEST_SIZE];

    int ret = wc_HKDF(SHA512, key, sizeof(key), salt, sizeof(salt),
    NULL, 0, derivedKey, sizeof(derivedKey));
    if ( ret != 0 ) {
	    // error generating derived key
    }
    \endcode
    \sa wc_HmacSetKey
*/
int wc_HKDF(int type, const byte* inKey, word32 inKeySz,
                    const byte* salt, word32 saltSz,
                    const byte* info, word32 infoSz,
                    byte* out, word32 outSz);
