/*!
    \ingroup ECC 
    \brief  この関数は新しいECC_KEYを生成し、それをキーに格納します。
    \return 0  成功に戻りました。
    \return ECC_BAD_ARG_E  RNGまたはキーがNULLに評価された場合に返されます
    \return BAD_FUNC_ARG  指定されたキーサイズがサポートされているキーの正しい範囲にない場合に返されます。
    \return MEMORY_E  ECCキーの計算中にメモリの割り当てエラーがある場合に返されます。
    \return MP_INIT_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \param rng  キーを生成するための初期化されたRNGオブジェクトへのポインタ
    \param keysize  ECC_KEYの希望の長さ
    _Example_
    \code
    ecc_key key;
    wc_ecc_init(&key);
    WC_RNG rng;
    wc_InitRng(&rng);
    wc_ecc_make_key(&rng, 32, &key); // initialize 32 byte ecc key
    \endcode
    \sa wc_ecc_init
    \sa wc_ecc_shared_secret
*/

int wc_ecc_make_key(WC_RNG* rng, int keysize, ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は新しいECC_KEYを生成し、それをキーに格納します。
    \return 0  成功に戻りました。
    \return ECC_BAD_ARG_E  RNGまたはキーがNULLに評価された場合に返されます
    \return BAD_FUNC_ARG  指定されたキーサイズがサポートされているキーの正しい範囲にない場合に返されます。
    \return MEMORY_E  ECCキーの計算中にメモリの割り当てエラーがある場合に返されます。
    \return MP_INIT_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECCキーの計算中にエラーが発生した場合に返される可能性があります
    \param key  作成したキーを保存するためのポインタ。
    \param keysize  CavenIDに基づいて設定されたバイト単位で作成するキーのサイズ
    \param rng  鍵作成に使用されるRNG
    _Example_
    \code
    ecc_key key;
    int ret;
    WC_RNG rng;
    wc_ecc_init(&key);
    wc_InitRng(&rng);
    int curveId = ECC_SECP521R1;
    int keySize = wc_ecc_get_curve_size_from_id(curveId);
    ret = wc_ecc_make_key_ex(&rng, keySize, &key, curveId);
    if (ret != MP_OKAY) {
        // error handling
    }

    \endcode
    \sa wc_ecc_make_key
    \sa wc_ecc_get_curve_size_from_id
*/

int wc_ecc_make_key_ex(WC_RNG* rng, int keysize, ecc_key* key, int curve_id);

/*!
    \ingroup ECC 
    \brief  ECCキーの有効性を有効にします。
    \return MP_OKAY  成功、キーは大丈夫です。
    \return BAD_FUNC_ARG  キーがNULLの場合は返します。
    \return ECC_INF_E  WC_ECC_POINT_IS_AT_INFINITYが1を返す場合に返します。
    _Example_
    \code
    ecc_key key;
    WC_RNG rng;
    int check_result;
    wc_ecc_init(&key);
    wc_InitRng(&rng);
    wc_ecc_make_key(&rng, 32, &key);
    check_result = wc_ecc_check_key(&key);

    if (check_result == MP_OKAY)
    {
        // key check succeeded
    }
    else
    {
        // key check failed
    }
    \endcode
    \sa wc_ecc_point_is_at_infinity
*/

int wc_ecc_check_key(ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、使用された後にECC_KEYキーを解放します。
    _Example_
    \code
    // initialize key and perform ECC operations
    ...
    wc_ecc_key_free(&key);
    \endcode
    \sa wc_ecc_key_new
    \sa wc_ecc_init_ex
*/

void wc_ecc_key_free(ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、ローカル秘密鍵と受信した公開鍵を使用して新しい秘密鍵を生成します。この共有秘密鍵をバッファアウトに格納し、出力バッファに書き込まれたバイト数を保持するためにoutlentenを更新します。
    \return 0  共有秘密鍵の生成に成功したときに返されます
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLに評価された場合に返されます。
    \return ECC_BAD_ARG_E  引数として与えられた秘密鍵の種類がecc_privatekeyではない場合、またはパブリックキータイプ（ECC-> DPによって与えられた）が同等でない場合に返されます。
    \return MEMORY_E  新しいECCポイントを生成するエラーが発生した場合
    \return BUFFER_E  生成された共有秘密鍵が提供されたバッファーに格納するのに長すぎる場合に返されます
    \return MP_INIT_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_READ_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_CMP_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_INVMOD_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_EXPTMOD_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_MOD_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_MUL_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_ADD_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_MULMOD_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_TO_E  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \return MP_MEM  共有キーの計算中にエラーが発生した場合は返される可能性があります
    \param private_key  ローカル秘密鍵を含むECC_KEY構造体へのポインタ
    \param public_key  受信した公開鍵を含むECC_Key構造体へのポインタ
    \param out  生成された共有秘密鍵を保存する出力バッファへのポインタ
    _Example_
    \code
    ecc_key priv, pub;
    WC_RNG rng;
    byte secret[1024]; // can hold 1024 byte shared secret key
    word32 secretSz = sizeof(secret);
    int ret;

    wc_InitRng(&rng); // initialize rng
    wc_ecc_init(&priv); // initialize key
    wc_ecc_make_key(&rng, 32, &priv); // make public/private key pair
    // receive public key, and initialise into pub
    ret = wc_ecc_shared_secret(&priv, &pub, secret, &secretSz);
    // generate secret key
    if ( ret != 0 ) {
    	// error generating shared secret key
    }
    \endcode
    \sa wc_ecc_init
    \sa wc_ecc_make_key
*/

int wc_ecc_shared_secret(ecc_key* private_key, ecc_key* public_key, byte* out,
                      word32* outlen);

/*!
    \ingroup ECC 
    \brief  秘密鍵とパブリックポイントの間にECC共有秘密を作成します。
    \return MP_OKAY  成功を示します。
    \return BAD_FUNC_ARG  引数がNULLのときにエラーが返されます。
    \return ECC_BAD_ARG_E  private_key-> typeがecc_privatekeyまたはprivate_key-> idxが検証できない場合に返されました。
    \return BUFFER_E  outlenが小さすぎるとエラーが発生します。
    \return MEMORY_E  新しいポイントを作成するためのエラー。
    \return MP_VAL  初期化失敗が発生したときに可能です。
    \return MP_MEM  初期化失敗が発生したときに可能です。
    \param private_key  プライベートECCキー。
    \param point  使用するポイント（公開鍵）。
    \param out  共有秘密の出力先。ANSI X9.63からEC-DHに準拠しています。
    _Example_
    \code
    ecc_key key;
    ecc_point* point;
    byte shared_secret[];
    int secret_size;
    int result;

    point = wc_ecc_new_point();

    result = wc_ecc_shared_secret_ex(&key, point,
    &shared_secret, &secret_size);

    if (result != MP_OKAY)
    {
        // Handle error
    }
    \endcode
    \sa wc_ecc_verify_hash_ex
*/

int wc_ecc_shared_secret_ex(ecc_key* private_key, ecc_point* point,
                             byte* out, word32 *outlen);

/*!
    \ingroup ECC 
    \brief  この関数は、信頼性を保証するためにECC_KEYオブジェクトを使用してメッセージダイジェストに署名します。
    \return 0  メッセージダイジェストの署名を正常に生成したときに返されました
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLに評価された場合、または出力バッファが小さすぎて生成された署名を保存する場合は返されます。
    \return ECC_BAD_ARG_E  入力キーが秘密鍵ではない場合、またはECC OIDが無効な場合
    \return RNG_FAILURE_E  RNGが満足のいくキーを正常に生成できない場合に返されます。
    \return MP_INIT_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_READ_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_CMP_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_INVMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_EXPTMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MUL_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_ADD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MULMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_TO_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MEM  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \param in  サインするメッセージハッシュを含むバッファへのポインタ
    \param inlen  署名するメッセージの長さ
    \param out  生成された署名を保存するためのバッファ
    \param outlen  出力バッファの最大長。メッセージ署名の生成に成功したときに書き込まれたバイトを保存します
    _Example_
    \code
    ecc_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[512]; // will hold generated signature
    sigSz = sizeof(sig);
    byte digest[] = { // initialize with message hash };
    wc_InitRng(&rng); // initialize rng
    wc_ecc_init(&key); // initialize key
    wc_ecc_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ecc_sign_hash(digest, sizeof(digest), sig, &sigSz, &key);
    if ( ret != 0 ) {
	    // error generating message signature
    }
    \endcode
    \sa wc_ecc_verify_hash
*/

int wc_ecc_sign_hash(const byte* in, word32 inlen, byte* out, word32 *outlen,
                     WC_RNG* rng, ecc_key* key);

/*!
    \ingroup ECC 
    \brief  メッセージダイジェストに署名します。
    \return MP_OKAY  メッセージダイジェストの署名を正常に生成したときに返されました
    \return ECC_BAD_ARG_E  入力キーが秘密鍵ではない場合、またはECC IDXが無効な場合、またはいずれかの入力パラメータがNULLに評価されている場合、または出力バッファが小さすぎて生成された署名を保存するには小さすぎる場合
    \return RNG_FAILURE_E  RNGが満足のいくキーを正常に生成できない場合に返されます。
    \return MP_INIT_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_READ_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_CMP_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_INVMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_EXPTMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MUL_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_ADD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MULMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_TO_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MEM  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \param in  メッセージがサインにダイジェスト。
    \param inlen  ダイジェストの長さ。
    \param rng  WC_RNG構造体へのポインタ。
    \param key  プライベートECCキー。
    \param r  署名のRコンポーネントの宛先。
    _Example_
    \code
    ecc_key key;
    WC_RNG rng;
    int ret, sigSz;
    mp_int r; // destination for r component of signature.
    mp_int s; // destination for s component of signature.

    byte sig[512]; // will hold generated signature
    sigSz = sizeof(sig);
    byte digest[] = { initialize with message hash };
    wc_InitRng(&rng); // initialize rng
    wc_ecc_init(&key); // initialize key
    mp_init(&r); // initialize r component
    mp_init(&s); // initialize s component
    wc_ecc_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ecc_sign_hash_ex(digest, sizeof(digest), &rng, &key, &r, &s);

    if ( ret != MP_OKAY ) {
    	// error generating message signature
    }
    \endcode
    \sa wc_ecc_verify_hash_ex
*/

int wc_ecc_sign_hash_ex(const byte* in, word32 inlen, WC_RNG* rng,
                        ecc_key* key, mp_int *r, mp_int *s);

/*!
    \ingroup ECC 
    \brief  この関数は、真正性を確保するためにハッシュのECCシグネチャを検証します。答えを介して、有効な署名に対応する1、無効な署名に対応する0で答えを返します。
    \return 0  署名検証に正常に実行されたときに返されます。注：これは署名が検証されていることを意味するわけではありません。信頼性情報は代わりにSTATで格納されます
    \return BAD_FUNC_ARG  返された入力パラメータはNULLに評価されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return MP_INIT_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_READ_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_CMP_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_INVMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_EXPTMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MUL_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_ADD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MULMOD_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_TO_E  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \return MP_MEM  メッセージ署名の計算中にエラーが発生した場合に返される可能性があります。
    \param sig  確認する署名を含むバッファへのポインタ
    \param siglen  検証する署名の長さ
    \param hash  確認されたメッセージのハッシュを含むバッファへのポインタ
    \param hashlen  認証されたメッセージのハッシュの長さ
    \param stat  検証の結果へのポインタ。1メッセージが正常に認証されたことを示します
    _Example_
    \code
    ecc_key key;
    int ret, verified = 0;

    byte sig[1024] { initialize with received signature };
    byte digest[] = { initialize with message hash };
    // initialize key with received public key
    ret = wc_ecc_verify_hash(sig, sizeof(sig), digest,sizeof(digest),
    &verified, &key);
    if ( ret != 0 ) {
	    // error performing verification
    } else if ( verified == 0 ) {
	    // the signature is invalid
    }
    \endcode
    \sa wc_ecc_sign_hash
    \sa wc_ecc_verify_hash_ex
*/

int wc_ecc_verify_hash(const byte* sig, word32 siglen, const byte* hash,
                    word32 hashlen, int* stat, ecc_key* key);

/*!
    \ingroup ECC 
    \brief  ECC署名を確認してください。結果はstatに書き込まれます。1が有効で、0が無効です。注：有効なテストに戻り値を使用しないでください。statのみを使用してください。
    \return MP_OKAY  成功した場合（署名が無効であっても）
    \return ECC_BAD_ARG_E  引数がNULLの場合、またはkey-idxが無効な場合は返します。
    \return MEMORY_E  INTまたはポイントの割り当て中にエラーが発生しました。
    \param r  検証する署名Rコンポーネント
    \param s  検証するシグネチャSコンポーネント
    \param hash  署名されたハッシュ（メッセージダイジェスト）
    \param hashlen  ハッシュの長さ（オクテット）
    \param stat  署名の結果、1 ==有効、0 ==無効
    _Example_
    \code
    mp_int r;
    mp_int s;
    int stat;
    byte hash[] = { Some hash }
    ecc_key key;

    if(wc_ecc_verify_hash_ex(&r, &s, hash, hashlen, &stat, &key) == MP_OKAY)
    {
        // Check stat
    }
    \endcode
    \sa wc_ecc_verify_hash
*/

int wc_ecc_verify_hash_ex(mp_int *r, mp_int *s, const byte* hash,
                          word32 hashlen, int* stat, ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、メッセージ検証または鍵交渉で将来の使用のためにECC_KEYオブジェクトを初期化します。
    \return 0  ECC_Keyオブジェクトの初期化に成功したときに返されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    _Example_
    \code
    ecc_key key;
    wc_ecc_init(&key);
    \endcode
    \sa wc_ecc_make_key
    \sa wc_ecc_free
*/

int wc_ecc_init(ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、メッセージ検証または鍵交渉で将来の使用のためにECC_KEYオブジェクトを初期化します。
    \return 0  ECC_Keyオブジェクトの初期化に成功したときに返されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \param key  初期化するECC_Keyオブジェクトへのポインタ
    \param devId  非同期ハードウェアで使用するID
    _Example_
    \code
    ecc_key key;
    wc_ecc_init_ex(&key, heap, devId);
    \endcode
    \sa wc_ecc_make_key
    \sa wc_ecc_free
    \sa wc_ecc_init
*/

int wc_ecc_init_ex(ecc_key* key, void* heap, int devId);

/*!
    \ingroup ECC 
    \brief  この関数はユーザー定義ヒープを使用し、キー構造のスペースを割り当てます。
    \return 0  ECC_Keyオブジェクトの初期化に成功したときに返されます
    _Example_
    \code
    wc_ecc_key_new(&heap);
    \endcode
    \sa wc_ecc_make_key
    \sa wc_ecc_key_free
    \sa wc_ecc_init
*/

ecc_key* wc_ecc_key_new(void* heap);

/*!
    \ingroup ECC 
    \brief  この関数は、使用後にECC_KEYオブジェクトを解放します。
    \return int  integerがWolfSSLエラーまたは成功状況を示すことを返しました。
    _Example_
    \code
    // initialize key and perform secure exchanges
    ...
    wc_ecc_free(&key);
    \endcode
    \sa wc_ecc_init
*/

int wc_ecc_free(ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は固定小数点キャッシュを解放します。これはECCで使用でき、計算時間を高速化します。この機能を使用するには、FP_ECC（固定小数点ECC）を定義する必要があります。
    \return none  いいえ返します。
    _Example_
    \code
    ecc_key key;
    // initialize key and perform secure exchanges
    ...

    wc_ecc_fp_free();
    \endcode
    \sa wc_ecc_free
*/

void wc_ecc_fp_free(void);

/*!
    \ingroup ECC 
    \brief  ECC IDXが有効かどうかを確認します。
    \return 1  有効な場合は返品してください。
    \return 0  無効な場合は返します。
    _Example_
    \code
    ecc_key key;
    WC_RNG rng;
    int is_valid;
    wc_ecc_init(&key);
    wc_InitRng(&rng);
    wc_ecc_make_key(&rng, 32, &key);
    is_valid = wc_ecc_is_valid_idx(key.idx);
    if (is_valid == 1)
    {
        // idx is valid
    }
    else if (is_valid == 0)
    {
        // idx is not valid
    }
    \endcode
    \sa none
*/

int wc_ecc_is_valid_idx(int n);

/*!
    \ingroup ECC 
    \brief  新しいECCポイントを割り当てます。
    \return p  新しく割り当てられたポイント。
    \return NULL  エラー時にNULLを返します。
    _Example_
    \code
    ecc_point* point;
    point = wc_ecc_new_point();
    if (point == NULL)
    {
        // Handle point creation error
    }
    // Do stuff with point
    \endcode
    \sa wc_ecc_del_point
    \sa wc_ecc_cmp_point
    \sa wc_ecc_copy_point
*/

ecc_point* wc_ecc_new_point(void);

/*!
    \ingroup ECC 
    \brief  メモリからECCポイントを解放します。
    \return none  いいえ返します。
    _Example_
    \code
    ecc_point* point;
    point = wc_ecc_new_point();
    if (point == NULL)
    {
        // Handle point creation error
    }
    // Do stuff with point
    wc_ecc_del_point(point);
    \endcode
    \sa wc_ecc_new_point
    \sa wc_ecc_cmp_point
    \sa wc_ecc_copy_point
*/

void wc_ecc_del_point(ecc_point* p);

/*!
    \ingroup ECC 
    \brief  あるポイントの値を別のポイントにコピーします。
    \return ECC_BAD_ARG_E  PまたはRがNULLのときにスローされたエラー。
    \return MP_OKAY  ポイントが正常にコピーされました
    \return ret  内部関数からのエラー。になることができます...
    \param p  コピーするポイント。
    _Example_
    \code
    ecc_point* point;
    ecc_point* copied_point;
    int copy_return;

    point = wc_ecc_new_point();
    copy_return = wc_ecc_copy_point(point, copied_point);
    if (copy_return != MP_OKAY)
    {
        // Handle error
    }
    \endcode
    \sa wc_ecc_new_point
    \sa wc_ecc_cmp_point
    \sa wc_ecc_del_point
*/

int wc_ecc_copy_point(ecc_point* p, ecc_point *r);

/*!
    \ingroup ECC 
    \brief  ポイントの値を別のものと比較してください。
    \return BAD_FUNC_ARG  1つまたは両方の引数はnullです。
    \return MP_EQ  ポイントは同じです。
    \return ret  mp_ltまたはmp_gtのどちらかで、ポイントが等しくないことを意味します。
    \param a  比較する最初のポイント。
    _Example_
    \code
    ecc_point* point;
    ecc_point* point_to_compare;
    int cmp_result;

    point = wc_ecc_new_point();
    point_to_compare = wc_ecc_new_point();
    cmp_result = wc_ecc_cmp_point(point, point_to_compare);
    if (cmp_result == BAD_FUNC_ARG)
    {
        // arguments are invalid
    }
    else if (cmp_result == MP_EQ)
    {
        // Points are equal
    }
    else
    {
        // Points are not equal
    }
    \endcode
    \sa wc_ecc_new_point
    \sa wc_ecc_del_point
    \sa wc_ecc_copy_point
*/

int wc_ecc_cmp_point(ecc_point* a, ecc_point *b);

/*!
    \ingroup ECC 
    \brief  ポイントが無限大にあるかどうかを確認します。返品1が無限大である場合は0、そうでない場合は0、<0エラー時の0
    \return 1  Pは無限大です。
    \return 0  Pは無限大ではありません。
    \return <0  エラー。
    _Example_
    \code
    ecc_point* point;
    int is_infinity;
    point = wc_ecc_new_point();

    is_infinity = wc_ecc_point_is_at_infinity(point);
    if (is_infinity < 0)
    {
        // Handle error
    }
    else if (is_infinity == 0)
    {
        // Point is not at infinity
    }
    else if (is_infinity == 1)
    {
        // Point is at infinity
    }
    \endcode
    \sa wc_ecc_new_point
    \sa wc_ecc_del_point
    \sa wc_ecc_cmp_point
    \sa wc_ecc_copy_point
*/

int wc_ecc_point_is_at_infinity(ecc_point *p);

/*!
    \ingroup ECC 
    \brief  ECC固定点乗算を実行します。
    \return MP_OKAY  成功した操作で返します。
    \return MP_INIT_E  複数のPrecision Integer（MP_INT）ライブラリで使用するための整数を初期化するエラーがある場合に返されます。
    \param k  計量。
    \param G  乗算する基点。
    \param R  商品の目的地
    \param modulus  曲線の弾性率
    _Example_
    \code
    ecc_point* base;
    ecc_point* destination;
    // Initialize points
    base = wc_ecc_new_point();
    destination = wc_ecc_new_point();
    // Setup other arguments
    mp_int multiplicand;
    mp_int modulus;
    int map;
    \endcode
    \sa none
*/

int wc_ecc_mulmod(mp_int* k, ecc_point *G, ecc_point *R,
                  mp_int* a, mp_int* modulus, int map);

/*!
    \ingroup ECC 
    \brief  この関数はECCキーをECC_KEY構造体からエクスポートし、結果をOUTに格納します。キーはANSI X9.63フォーマットに保存されます。outlenの出力バッファに書き込まれたバイトを格納します。
    \return 0  ECC_KEYのエクスポートに正常に返されました
    \return LENGTH_ONLY_E  出力バッファがNULLに評価されている場合は返されますが、他の2つの入力パラメータは有効です。関数がキーを保存するのに必要な長さを返すだけであることを示します
    \return ECC_BAD_ARG_E  入力パラメータのいずれかがNULLの場合、またはキーがサポートされていない場合（無効なインデックスがあります）
    \return BUFFER_E  出力バッファが小さすぎてECCキーを保存する場合は返されます。出力バッファが小さすぎると、必要なサイズはoutlenに返されます。
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return MP_INIT_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \param key  エクスポートするECC_KEYオブジェクトへのポインタ
    \param out  ANSI X9.63フォーマット済みキーを保存するバッファへのポインタ
    _Example_
    \code
    int ret;
    byte buff[1024];
    word32 buffSz = sizeof(buff);

    ecc_key key;
    // initialize key, make key
    ret = wc_ecc_export_x963(&key, buff, &buffSz);
    if ( ret != 0) {
    	// error exporting key
    }
    \endcode
    \sa wc_ecc_export_x963_ex
    \sa wc_ecc_import_x963
*/

int wc_ecc_export_x963(ecc_key* key, byte* out, word32* outLen);

/*!
    \ingroup ECC 
    \brief  この関数はECCキーをECC_KEY構造体からエクスポートし、結果をOUTに格納します。キーはANSI X9.63フォーマットに保存されます。outlenの出力バッファに書き込まれたバイトを格納します。この関数は、圧縮されたパラメータを介して証明書を圧縮する追加のオプションを使用する。このパラメータがtrueの場合、キーはANSI X9.63圧縮形式で保存されます。
    \return 0  ECC_KEYのエクスポートに正常に返されました
    \return NOT_COMPILED_IN  hand_comp_keyがコンパイル時に有効になっていない場合は返されますが、キーは圧縮形式で要求されました
    \return LENGTH_ONLY_E  出力バッファがNULLに評価されている場合は返されますが、他の2つの入力パラメータは有効です。関数がキーを保存するのに必要な長さを返すだけであることを示します
    \return ECC_BAD_ARG_E  入力パラメータのいずれかがNULLの場合、またはキーがサポートされていない場合（無効なインデックスがあります）
    \return BUFFER_E  出力バッファが小さすぎてECCキーを保存する場合は返されます。出力バッファが小さすぎると、必要なサイズはoutlenに返されます。
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return MP_INIT_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \param key  エクスポートするECC_KEYオブジェクトへのポインタ
    \param out  ANSI X9.63フォーマット済みキーを保存するバッファへのポインタ
    \param outLen  出力バッファのサイズ。キーの保存に成功した場合は、出力バッファに書き込まれたバイトを保持します。
    _Example_
    \code
    int ret;
    byte buff[1024];
    word32 buffSz = sizeof(buff);
    ecc_key key;
    // initialize key, make key
    ret = wc_ecc_export_x963_ex(&key, buff, &buffSz, 1);
    if ( ret != 0) {
	    // error exporting key
    }
    \endcode
    \sa wc_ecc_export_x963
    \sa wc_ecc_import_x963
*/

int wc_ecc_export_x963_ex(ecc_key* key, byte* out, word32* outLen, int compressed);

/*!
    \ingroup ECC 
    \brief  この関数は、ANSI X9.63形式で保存されているキーを含むバッファからパブリックECCキーをインポートします。この関数は、圧縮キーがhand_comp_keyオプションを介してコンパイル時に有効になっている限り、圧縮キーと非圧縮キーの両方を処理します。
    \return 0  ECC_KEYのインポートに成功しました
    \return NOT_COMPILED_IN  hand_comp_keyがコンパイル時に有効になっていない場合は返されますが、キーは圧縮形式で保存されます。
    \return ECC_BAD_ARG_E  INまたはKEYがNULLに評価された場合、またはInlenが偶数の場合（X9.63規格によれば、キーは奇数でなければなりません）。
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return ASN_PARSE_E  ECCキーの解析中にエラーがある場合は返されます。ECCキーが有効なANSI X9.63フォーマットに格納されていないことを示すことがあります。
    \return IS_POINT_E  エクスポートされた公開鍵がECC曲線上の点ではない場合に返されます
    \return MP_INIT_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \param in  ANSI x9.63フォーマットされたECCキーを含むバッファへのポインタ
    \param inLen  入力バッファの長さ
    _Example_
    \code
    int ret;
    byte buff[] = { initialize with ANSI X9.63 formatted key };

    ecc_key pubKey;
    wc_ecc_init(&pubKey);

    ret = wc_ecc_import_x963(buff, sizeof(buff), &pubKey);
    if ( ret != 0) {
    	// error importing key
    }
    \endcode
    \sa wc_ecc_export_x963
    \sa wc_ecc_import_private_key
*/

int wc_ecc_import_x963(const byte* in, word32 inLen, ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、生の秘密鍵を含むバッファと、ANSI X9.63フォーマットされた公開鍵を含む2番目のバッファーからパブリック/プライベートECCキーのペアをインポートします。この関数は、圧縮キーがhand_comp_keyオプションを介してコンパイル時に有効になっている限り、圧縮キーと非圧縮キーの両方を処理します。
    \return 0  habe_comp_keyがコンパイル時に有効になっていない場合は、ecc_key not_compiled_inを正常にインポートしましたが、キーは圧縮形式で保存されます。
    \return ECC_BAD_ARG_E  INまたはKEYがNULLに評価された場合、またはInlenが偶数の場合（X9.63規格によれば、キーは奇数でなければなりません）。
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return ASN_PARSE_E  ECCキーの解析中にエラーがある場合は返されます。ECCキーが有効なANSI X9.63フォーマットに格納されていないことを示すことがあります。
    \return IS_POINT_E  エクスポートされた公開鍵がECC曲線上の点ではない場合に返されます
    \return MP_INIT_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \param priv  RAW秘密鍵を含むバッファへのポインタ
    \param privSz  秘密鍵バッファのサイズ
    \param pub  ANSI x9.63フォーマットされたECC公開鍵を含むバッファへのポインタ
    \param pubSz  公開鍵入力バッファの長さ
    _Example_
    \code
    int ret;
    byte pub[] = { initialize with ANSI X9.63 formatted key };
    byte priv[] = { initialize with the raw private key };

    ecc_key key;
    wc_ecc_init(&key);
    ret = wc_ecc_import_private_key(priv, sizeof(priv), pub, sizeof(pub),
    &key);
    if ( ret != 0) {
    	// error importing key
    }
    \endcode
    \sa wc_ecc_export_x963
    \sa wc_ecc_import_private_key
*/

int wc_ecc_import_private_key(const byte* priv, word32 privSz, const byte* pub,
                           word32 pubSz, ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、ECCシグネチャのR部分とS部分をDER符号化ECDSAシグネチャに変換します。この機能は、outlenでは、出力バッファに書き込まれた長さも記憶されています。
    \return 0  署名の変換に成功したことに戻りました
    \return ECC_BAD_ARG_E  いずれかの入力パラメータがNULLに評価された場合、または入力バッファがDERエンコードされたECDSAシグネチャを保持するのに十分な大きさでない場合に返されます。
    \return MP_INIT_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_READ_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_CMP_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_INVMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_EXPTMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MUL_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_ADD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MULMOD_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_TO_E  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \return MP_MEM  ECC_KEYの処理中にエラーが発生した場合に返される可能性があります
    \param r  署名のR部分を文字列として含むバッファへのポインタ
    \param s  シグネチャのS部分を含むバッファへのポインタ文字列としてのポインタ
    \param out  DERエンコードされたECDSAシグネチャを保存するバッファへのポインタ
    _Example_
    \code
    int ret;
    ecc_key key;
    // initialize key, generate R and S

    char r[] = { initialize with R };
    char s[] = { initialize with S };
    byte sig[wc_ecc_sig_size(key)];
    // signature size will be 2 * ECC key size + ~10 bytes for ASN.1 overhead
    word32 sigSz = sizeof(sig);
    ret = wc_ecc_rs_to_sig(r, s, sig, &sigSz);
    if ( ret != 0) {
    	// error converting parameters to signature
    }
    \endcode
    \sa wc_ecc_sign_hash
    \sa wc_ecc_sig_size
*/

int wc_ecc_rs_to_sig(const char* r, const char* s, byte* out, word32* outlen);

/*!
    \ingroup ECC 
    \brief  この関数は、ECC署名のRAW成分を持つECC_KEY構造体を埋めます。
    \return 0  ECC_Key構造体に正常にインポートされたときに返されます
    \return ECC_BAD_ARG_E  いずれかの入力値がNULLに評価された場合に返されます。
    \return MEMORY_E  ECC_Keyのパラメータを格納するためのエラーの初期化スペースがある場合に返されます。
    \return ASN_PARSE_E  入力カーベナデがECC_SETSで定義されていない場合
    \return MP_INIT_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_READ_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_CMP_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_INVMOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_EXPTMOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MUL_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_ADD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MULMOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_TO_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MEM  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \param key  塗りつぶすためのECC_KEY構造体へのポインタ
    \param qx  ASCII六角文字列として基点のXコンポーネントを含むバッファへのポインタ
    \param qy  ASCII六角文字列として基点のY成分を含むバッファへのポインタ
    \param d  ASCII hex文字列として秘密鍵を含むバッファへのポインタ
    _Example_
    \code
    int ret;
    ecc_key key;
    wc_ecc_init(&key);

    char qx[] = { initialize with x component of base point };
    char qy[] = { initialize with y component of base point };
    char d[]  = { initialize with private key };
    ret = wc_ecc_import_raw(&key,qx, qy, d, "ECC-256");
    if ( ret != 0) {
    	// error initializing key with given inputs
    }
    \endcode
    \sa wc_ecc_import_private_key
*/

int wc_ecc_import_raw(ecc_key* key, const char* qx, const char* qy,
                   const char* d, const char* curveName);

/*!
    \ingroup ECC 
    \brief  この関数は、ECC_KEY構造体から秘密鍵のみをエクスポートします。秘密鍵をバッファアウトに格納し、outlenにこのバッファに書き込まれたバイトを設定します。
    \return 0  秘密鍵のエクスポートに成功したときに返されます
    \return ECC_BAD_ARG_E  いずれかの入力値がNULLに評価された場合に返されます。
    \return MEMORY_E  ECC_Keyのパラメータを格納するためのエラーの初期化スペースがある場合に返されます。
    \return ASN_PARSE_E  入力カーベナデがECC_SETSで定義されていない場合
    \return MP_INIT_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_READ_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_CMP_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_INVMOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_EXPTMOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MUL_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_ADD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MULMOD_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_TO_E  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \return MP_MEM  入力パラメータの処理中にエラーが発生した場合に返される可能性があります。
    \param key  秘密鍵をエクスポートするECC_Key構造体へのポインタ
    \param out  秘密鍵を保存するバッファへのポインタ
    _Example_
    \code
    int ret;
    ecc_key key;
    // initialize key, make key

    char priv[ECC_KEY_SIZE];
    word32 privSz = sizeof(priv);
    ret = wc_ecc_export_private_only(&key, priv, &privSz);
    if ( ret != 0) {
    	// error exporting private key
    }
    \endcode
    \sa wc_ecc_import_private_key
*/

int wc_ecc_export_private_only(ecc_key* key, byte* out, word32* outLen);

/*!
    \ingroup ECC 
    \brief  DERへのエクスポートポイント。
    \return 0  成功に戻りました。
    \return ECC_BAD_ARG_E  curve_idxが0未満または無効である場合は返します。いつ来るのか
    \return LENGTH_ONLY_E  outlenは設定されていますが、他にはありません。
    \return BUFFER_E  outlennが1 + 2 *曲線サイズよりも小さい場合は返します。
    \return MEMORY_E  メモリの割り当てに問題がある場合は返します。
    \param curve_idx  ECC_SETSから使用される曲線のインデックス。
    \param point  Derへのエクスポートを指す。
    \param out  出力の目的地
    _Example_
    \code
    int curve_idx;
    ecc_point* point;
    byte out[];
    word32 outLen;
    wc_ecc_export_point_der(curve_idx, point, out, &outLen);
    \endcode
    \sa wc_ecc_import_point_der
*/

int wc_ecc_export_point_der(const int curve_idx, ecc_point* point,
                            byte* out, word32* outLen);

/*!
    \ingroup ECC 
    \brief  Derフォーマットからのインポートポイント。
    \return ECC_BAD_ARG_E  引数がnullの場合、またはInlenが偶数の場合は返します。
    \return MEMORY_E  エラー初期化がある場合に返します
    \return NOT_COMPILED_IN  habe_comp_keyが真実でない場合は返され、inは圧縮証明書です
    \return MP_OKAY  操作が成功しました。
    \param in  からのポイントをインポートするためのDer Buffer。
    \param inLen  DERバッファの長さ
    \param curve_idx  曲線のインデックス
    _Example_
    \code
    byte in[];
    word32 inLen;
    int curve_idx;
    ecc_point* point;
    wc_ecc_import_point_der(in, inLen, curve_idx, point);
    \endcode
    \sa wc_ecc_export_point_der
*/

int wc_ecc_import_point_der(byte* in, word32 inLen, const int curve_idx,
                            ecc_point* point);

/*!
    \ingroup ECC 
    \brief  この関数は、ecc_key構造体のキーサイズをオクテットで返します。
    \return Given  有効なキー、オクテットのキーサイズを返します
    \return 0  与えられたキーがNULLの場合に返されます
    _Example_
    \code
    int keySz;
    ecc_key key;
    // initialize key, make key
    keySz = wc_ecc_size(&key);
    if ( keySz == 0) {
    	// error determining key size
    }
    \endcode
    \sa wc_ecc_make_key
*/

int wc_ecc_size(ecc_key* key);

/*!
    \ingroup ECC 
    \brief  この関数は、次のようにして指定されたECCシグネチャの最悪の場合のサイズを返します。（KEYSZ * 2）+ SIG_HEADER_SZ + ECC_MAX_PAD_SZ。実際のシグネチャサイズは、WC_ECC_SIGN_HASHで計算できます。
    \return returns  最大署名サイズ（オクテット）
    _Example_
    \code
    int sigSz = wc_ecc_sig_size_calc(32);
    if ( sigSz == 0) {
    	// error determining sig size
    }
    \endcode
    \sa wc_ecc_sign_hash
    \sa wc_ecc_sig_size
*/

int wc_ecc_sig_size_calc(int sz);


/*!
    \ingroup ECC 
    \brief  この関数は、次のようにして指定されたECCシグネチャの最悪の場合のサイズを返します。（KEYSZ * 2）+ SIG_HEADER_SZ + ECC_MAX_PAD_SZ。実際のシグネチャサイズは、WC_ECC_SIGN_HASHで計算できます。
    \return Success  有効なキーを考えると、最大署名サイズをオクテットで返します。
    \return 0  与えられたキーがNULLの場合に返されます
    _Example_
    \code
    int sigSz;
    ecc_key key;
    // initialize key, make key

    sigSz = wc_ecc_sig_size(&key);
    if ( sigSz == 0) {
        // error determining sig size
    }
    \endcode
    \sa wc_ecc_sign_hash
    \sa wc_ecc_sig_size_calc
*/

int wc_ecc_sig_size(ecc_key* key);


/*!
    \ingroup ECC 
    \brief  この機能は、ECCとの安全なメッセージ交換を可能にするために、新しいECCコンテキストオブジェクトのスペースを割り当て、初期化します。
    \return Success  新しいECENCCTXオブジェクトの生成に成功した場合は、そのオブジェクトへのポインタを返します
    \return NULL  関数が新しいECENCCTXオブジェクトを生成できない場合に返されます
    \param flags  これがサーバーであるかクライアントのコンテキストオプションがあるかどうかを示します.req_resp_client、およびreq_resp_server
    _Example_
    \code
    ecEncCtx* ctx;
    WC_RNG rng;
    wc_InitRng(&rng);
    ctx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng);
    if(ctx == NULL) {
        // error generating new ecEncCtx object
    }
    \endcode
    \sa wc_ecc_encrypt
    \sa wc_ecc_encrypt_ex
    \sa wc_ecc_decrypt
*/

ecEncCtx* wc_ecc_ctx_new(int flags, WC_RNG* rng);

/*!
    \ingroup ECC 
    \brief  この関数は、メッセージの暗号化と復号化に使用されるECENCCTXオブジェクトを解放します。
    \return none  戻り値。
    _Example_
    \code
    ecEncCtx* ctx;
    WC_RNG rng;
    wc_InitRng(&rng);
    ctx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng);
    // do secure communication
    ...
    wc_ecc_ctx_free(&ctx);
    \endcode
    \sa wc_ecc_ctx_new
*/

void wc_ecc_ctx_free(ecEncCtx*);

/*!
    \ingroup ECC 
    \brief  この関数はECENCCTX構造をリセットして、新しいコンテキストオブジェクトを解放し、新しいコンテキストオブジェクトを割り当てます。
    \return 0  ecencctx構造が正常にリセットされた場合に返されます
    \return BAD_FUNC_ARG  RNGまたはCTXがNULLの場合に返されます
    \return RNG_FAILURE_E  ECCオブジェクトに新しい塩を生成するエラーがある場合
    \param ctx  リセットするECENCCTXオブジェクトへのポインタ
    _Example_
    \code
    ecEncCtx* ctx;
    WC_RNG rng;
    wc_InitRng(&rng);
    ctx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng);
    // do secure communication
    ...
    wc_ecc_ctx_reset(&ctx, &rng);
    // do more secure communication
    \endcode
    \sa wc_ecc_ctx_new
*/

int wc_ecc_ctx_reset(ecEncCtx* ctx, WC_RNG* rng);  /* reset for use again w/o alloc/free */

/*!
    \ingroup ECC 
    \brief  この関数は、wc_ecc_ctx_newの後にオプションで呼び出されることができます。暗号化、KDF、およびMACアルゴリズムをECENCENCCTXオブジェクトに設定します。
    \return 0  ECENCCTXオブジェクトの情報を正常に設定すると返されます。
    \return BAD_FUNC_ARG  指定されたecencctxオブジェクトがNULLの場合に返されます。
    \param ctx  情報を設定するECENCCTXへのポインタ
    \param encAlgo  使用する暗号化アルゴリズム
    \param kdfAlgo  使用するKDFアルゴリズム
    _Example_
    \code
    ecEncCtx* ctx;
    // initialize ctx
    if(wc_ecc_ctx_set_algo(&ctx, ecAES_128_CTR, ecHKDF_SHA256, ecHMAC_SHA256))) {
	    // error setting info
    }
    \endcode
    \sa wc_ecc_ctx_new
*/

int wc_ecc_ctx_set_algo(ecEncCtx* ctx, byte encAlgo, byte kdfAlgo,
    byte macAlgo);

/*!
    \ingroup ECC 
    \brief  この関数はECENCENCCTXオブジェクトの塩を返します。この関数は、ECENCCTXの状態がECSRV_INITまたはECCLI_INITの場合にのみ呼び出す必要があります。
    \return Success  成功すると、ECENCENTX塩を返します
    \return NULL  ecencctxオブジェクトがNULLの場合、またはECENCCTXの状態がECSRV_INITまたはECCLI_INITでない場合に返されます。後者の2つのケースでは、この機能はそれぞれECSRV_BAD_STATEまたはECCLI_BAD_STATEにECENCCTXの状態を設定します。
    _Example_
    \code
    ecEncCtx* ctx;
    WC_RNG rng;
    const byte* salt;
    wc_InitRng(&rng);
    ctx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng);
    salt = wc_ecc_ctx_get_own_salt(&ctx);
    if(salt == NULL) {
    	// error getting salt
    }
    \endcode
    \sa wc_ecc_ctx_new
    \sa wc_ecc_ctx_set_peer_salt
*/

const byte* wc_ecc_ctx_get_own_salt(ecEncCtx*);

/*!
    \ingroup ECC 
    \brief  この関数は、ECENCENCCTXオブジェクトのピアソルトを設定します。
    \return 0  ECENCCTXオブジェクトのピアソルトの設定に成功したときに返されます。
    \return BAD_FUNC_ARG  指定されたecencctxオブジェクトがnullまたは無効なプロトコルがある場合、または指定された塩がNULLの場合
    \return BAD_ENC_STATE_E  ecencctxの状態がECSRV_SALT_GETまたはECCLI_SALT_GETの場合に返されます。後者の2つのケースでは、この機能はそれぞれECSRV_BAD_STATEまたはECCLI_BAD_STATEにECENCCTXの状態を設定します。
    \param ctx  塩を設定するためのecencctxへのポインタ
    _Example_
    \code
    ecEncCtx* cliCtx, srvCtx;
    WC_RNG rng;
    const byte* cliSalt, srvSalt;
    int ret;

    wc_InitRng(&rng);
    cliCtx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng);
    srvCtx = wc_ecc_ctx_new(REQ_RESP_SERVER, &rng);

    cliSalt = wc_ecc_ctx_get_own_salt(&cliCtx);
    srvSalt = wc_ecc_ctx_get_own_salt(&srvCtx);
    ret = wc_ecc_ctx_set_peer_salt(&cliCtx, srvSalt);
    \endcode
    \sa wc_ecc_ctx_get_own_salt
*/

int wc_ecc_ctx_set_peer_salt(ecEncCtx* ctx, const byte* salt);

/*!
    \ingroup ECC 
    \brief  この関数は、wc_ecc_ctx_set_peer_saltの前後にオプションで呼び出されることができます。ECENCCTXオブジェクトのオプションの情報を設定します。
    \return 0  ECENCCTXオブジェクトの情報を正常に設定すると返されます。
    \return BAD_FUNC_ARG  与えられたECENCCTXオブジェクトがNULLの場合、入力情報はNULLまたはサイズが無効です。
    \param ctx  情報を設定するECENCCTXへのポインタ
    \param info  設定する情報を含むバッファへのポインタ
    _Example_
    \code
    ecEncCtx* ctx;
    byte info[] = { initialize with information };
    // initialize ctx, get salt,
    if(wc_ecc_ctx_set_info(&ctx, info, sizeof(info))) {
	    // error setting info
    }
    \endcode
    \sa wc_ecc_ctx_new
*/

int wc_ecc_ctx_set_info(ecEncCtx* ctx, const byte* info, int sz);

/*!
    \ingroup ECC 
    \brief  この関数は指定された入力メッセージをMSGからOUTに暗号化します。この関数はパラメータとしてオプションのCTXオブジェクトを取ります。提供されている場合、ECENCCTXのEncalgo、Kdfalgo、およびMacalgoに基づいて暗号化が進みます。CTXが指定されていない場合、処理はデフォルトのアルゴリズム、ECAES_128_CBC、ECHKDF_SHA256、ECHMAC_SHA256で完了します。この機能は、メッセージがCTXで指定された暗号化タイプに従って埋め込まれている必要があります。
    \return 0  入力メッセージの暗号化に成功したら返されます
    \return BAD_FUNC_ARG  PRIVKEY、PUBKEY、MSG、MSGSZ、OUT、OUTSZがNULLの場合、またはCTXオブジェクトがサポートされていない暗号化タイプを指定します。
    \return BAD_ENC_STATE_E  指定されたCTXオブジェクトが暗号化に適していない状態にある場合に返されます。
    \return BUFFER_E  指定された出力バッファが小さすぎて暗号化された暗号文を保存する場合に返されます
    \return MEMORY_E  共有秘密鍵のメモリの割り当て中にエラーがある場合に返されます
    \param privKey  暗号化に使用する秘密鍵を含むECC_KEYオブジェクトへのポインタ
    \param pubKey  コミュニケーションを希望するピアの公開鍵を含むECC_Keyオブジェクトへのポインタ
    \param msg  暗号化するメッセージを保持しているバッファへのポインタ
    \param msgSz  暗号化するバッファのサイズ
    \param out  暗号化された暗号文を保存するバッファへのポインタ
    \param outSz  OUTバッファ内の使用可能なサイズを含むWord32オブジェクトへのポインタ。メッセージの暗号化に成功したら、出力バッファに書き込まれたバイト数を保持します。
    _Example_
    \code
    byte msg[] = { initialize with msg to encrypt. Ensure padded to block size };
    byte out[sizeof(msg)];
    word32 outSz = sizeof(out);
    int ret;
    ecc_key cli, serv;
    // initialize cli with private key
    // initialize serv with received public key

    ecEncCtx* cliCtx, servCtx;
    // initialize cliCtx and servCtx
    // exchange salts
    ret = wc_ecc_encrypt(&cli, &serv, msg, sizeof(msg), out, &outSz, cliCtx);
    if(ret != 0) {
    	// error encrypting message
    }
    \endcode
    \sa wc_ecc_encrypt_ex
    \sa wc_ecc_decrypt
*/

int wc_ecc_encrypt(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
                word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx);

/*!
    \ingroup ECC 
    \brief  この関数は指定された入力メッセージをMSGからOUTに暗号化します。この関数はパラメータとしてオプションのCTXオブジェクトを取ります。提供されている場合、ECENCCTXのEncalgo、Kdfalgo、およびMacalgoに基づいて暗号化が進みます。CTXが指定されていない場合、処理はデフォルトのアルゴリズム、ECAES_128_CBC、ECHKDF_SHA256、ECHMAC_SHA256で完了します。この機能は、メッセージがCTXで指定された暗号化タイプに従って埋め込まれている必要があります。
    \return 0  入力メッセージの暗号化に成功したら返されます
    \return BAD_FUNC_ARG  PRIVKEY、PUBKEY、MSG、MSGSZ、OUT、OUTSZがNULLの場合、またはCTXオブジェクトがサポートされていない暗号化タイプを指定します。
    \return BAD_ENC_STATE_E  指定されたCTXオブジェクトが暗号化に適していない状態にある場合に返されます。
    \return BUFFER_E  指定された出力バッファが小さすぎて暗号化された暗号文を保存する場合に返されます
    \return MEMORY_E  共有秘密鍵のメモリの割り当て中にエラーがある場合に返されます
    \param privKey  暗号化に使用する秘密鍵を含むECC_KEYオブジェクトへのポインタ
    \param pubKey  コミュニケーションを希望するピアの公開鍵を含むECC_Keyオブジェクトへのポインタ
    \param msg  暗号化するメッセージを保持しているバッファへのポインタ
    \param msgSz  暗号化するバッファのサイズ
    \param out  暗号化された暗号文を保存するバッファへのポインタ
    \param outSz  OUTバッファ内の使用可能なサイズを含むWord32オブジェクトへのポインタ。メッセージの暗号化に成功したら、出力バッファに書き込まれたバイト数を保持します。
    \param ctx  オプション：使用するさまざまな暗号化アルゴリズムを指定するECENCCTXオブジェクトへのポインタ
    _Example_
    \code
    byte msg[] = { initialize with msg to encrypt. Ensure padded to block size };
    byte out[sizeof(msg)];
    word32 outSz = sizeof(out);
    int ret;
    ecc_key cli, serv;
    // initialize cli with private key
    // initialize serv with received public key

    ecEncCtx* cliCtx, servCtx;
    // initialize cliCtx and servCtx
    // exchange salts
    ret = wc_ecc_encrypt_ex(&cli, &serv, msg, sizeof(msg), out, &outSz, cliCtx,
        1);
    if(ret != 0) {
    	// error encrypting message
    }
    \endcode
    \sa wc_ecc_encrypt
    \sa wc_ecc_decrypt
*/

int wc_ecc_encrypt_ex(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
    word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx, int compressed);

/*!
    \ingroup ECC 
    \brief  この関数はMSGからOUTへの暗号文を復号化します。この関数はパラメータとしてオプションのCTXオブジェクトを取ります。提供されている場合、ECENCCTXのEncalgo、Kdfalgo、およびMacalgoに基づいて暗号化が進みます。CTXが指定されていない場合、処理はデフォルトのアルゴリズム、ECAES_128_CBC、ECHKDF_SHA256、ECHMAC_SHA256で完了します。この機能は、メッセージがCTXで指定された暗号化タイプに従って埋め込まれている必要があります。
    \return 0  入力メッセージの復号化に成功したときに返されます
    \return BAD_FUNC_ARG  PRIVKEY、PUBKEY、MSG、MSGSZ、OUT、OUTSZがNULLの場合、またはCTXオブジェクトがサポートされていない暗号化タイプを指定します。
    \return BAD_ENC_STATE_E  指定されたCTXオブジェクトが復号化に適していない状態にある場合に返されます。
    \return BUFFER_E  供給された出力バッファが小さすぎて復号化された平文を保存する場合は返されます。
    \return MEMORY_E  共有秘密鍵のメモリの割り当て中にエラーがある場合に返されます
    \param privKey  復号化に使用する秘密鍵を含むECC_Keyオブジェクトへのポインタ
    \param pubKey  コミュニケーションを希望するピアの公開鍵を含むECC_Keyオブジェクトへのポインタ
    \param msg  暗号文を復号化するためのバッファへのポインタ
    \param msgSz  復号化するバッファのサイズ
    \param out  復号化された平文を保存するバッファへのポインタ
    \param outSz  OUTバッファ内の使用可能なサイズを含むWord32オブジェクトへのポインタ。暗号文を正常に復号化すると、出力バッファに書き込まれたバイト数を保持します。
    _Example_
    \code
    byte cipher[] = { initialize with
    ciphertext to decrypt. Ensure padded to block size };
    byte plain[sizeof(cipher)];
    word32 plainSz = sizeof(plain);
    int ret;
    ecc_key cli, serv;
    // initialize cli with private key
    // initialize serv with received public key
    ecEncCtx* cliCtx, servCtx;
    // initialize cliCtx and servCtx
    // exchange salts
    ret = wc_ecc_decrypt(&cli, &serv, cipher, sizeof(cipher),
    plain, &plainSz, cliCtx);

    if(ret != 0) {
    	// error decrypting message
    }
    \endcode
    \sa wc_ecc_encrypt
    \sa wc_ecc_encrypt_ex
*/

int wc_ecc_decrypt(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
                word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx);


/*!
    \ingroup ECC 
    \brief  非ブロック操作のためのECCサポートを有効にします。次のビルドオプションを使用した単精度（SP）数学でサポートされています.WolfSSL_SP_SP_SMALL WOLFSSL_SP_NO_MALLOC WC_ECC_NONBLOCK
    \return 0  コールバックコンテキストを入力メッセージに正常に設定すると返されます。
    \param key  ECC_KEYオブジェクトへのポインタ
    _Example_
    \code
    int ret;
    ecc_key ecc;
    ecc_nb_ctx_t nb_ctx;

    ret = wc_ecc_init(&ecc);
    if (ret == 0) {
        ret = wc_ecc_set_nonblock(&ecc, &nb_ctx);
        if (ret == 0) {
            do {
                ret = wc_ecc_verify_hash_ex(
                    &r, &s,       // r/s as mp_int
                    hash, hashSz, // computed hash digest
                    &verify_res,  // verification result 1=success
                    &key
                );

                // TODO: Real-time work can be called here 
            } while (ret == FP_WOULDBLOCK);
        }
        wc_ecc_free(&key);
    }
    \endcode
*/
int wc_ecc_set_nonblock(ecc_key *key, ecc_nb_ctx_t* ctx);
