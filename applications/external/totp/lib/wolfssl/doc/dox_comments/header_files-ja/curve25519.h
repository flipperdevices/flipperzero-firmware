/*!
    \ingroup Curve25519 
    \brief  この関数は、与えられたサイズ（Keysize）の指定された乱数発生器RNGを使用してCurve25519キーを生成し、それを指定されたCurve25519_Key構造体に格納します。キー構造がWC_CURVE25519_INIT（）を介して初期化された後に呼び出されるべきです。
    \return 0  キーの生成に成功し、それを指定されたCurve25519_Key構造体に格納します。
    \return ECC_BAD_ARG_E  入力キーサイズがCurve25519キー（32バイト）のキーシェイズに対応していない場合は返されます。
    \return RNG_FAILURE_E  RNGの内部ステータスがDRBG_OKでない場合、またはRNGを使用して次のランダムブロックを生成する場合に返されます。
    \return BAD_FUNC_ARG  渡された入力パラメータのいずれかがNULLの場合に返されます。
    \param [in]  RNG ECCキーの生成に使用されるRNGオブジェクトへのポインタ。
    \param [in]  キーサイズ生成キーのサイズ。Curve25519の32バイトでなければなりません。
    _Example_
    \code
    int ret;

    curve25519_key key;
    wc_curve25519_init(&key); // initialize key
    WC_RNG rng;
    wc_InitRng(&rng); // initialize random number generator

    ret = wc_curve25519_make_key(&rng, 32, &key);
    if (ret != 0) {
        // error making Curve25519 key
    }
    \endcode
    \sa wc_curve25519_init
*/

int wc_curve25519_make_key(WC_RNG* rng, int keysize, curve25519_key* key);

/*!
    \ingroup Curve25519 
    \brief  この関数は、秘密の秘密鍵と受信した公開鍵を考えると、共有秘密鍵を計算します。生成された秘密鍵をバッファアウトに保存し、ounlentの秘密鍵の変数を割り当てます。ビッグエンディアンのみをサポートします。
    \return 0  共有秘密鍵を正常に計算したときに返されました。
    \return BAD_FUNC_ARG  渡された入力パラメータのいずれかがNULLの場合に返されます。
    \return ECC_BAD_ARG_E  公開鍵の最初のビットが設定されている場合は、実装の指紋を避けるために返されます。
    \param [in]  Private_Key Curve25519_Key構造体の秘密鍵で初期化されました。
    \param [in]  public_key受信した公開鍵を含むCurve25519_Key構造体へのポインタ。
    \param [out]  32バイト計算された秘密鍵を格納するバッファへのポインタ。
    _Example_
    \code
    int ret;

    byte sharedKey[32];
    word32 keySz;
    curve25519_key privKey, pubKey;
    // initialize both keys

    ret = wc_curve25519_shared_secret(&privKey, &pubKey, sharedKey, &keySz);
    if (ret != 0) {
        // error generating shared key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
    \sa wc_curve25519_shared_secret_ex
*/

int wc_curve25519_shared_secret(curve25519_key* private_key,
                                curve25519_key* public_key,
                                byte* out, word32* outlen);

/*!
    \ingroup Curve25519 
    \brief  この関数は、秘密の秘密鍵と受信した公開鍵を考えると、共有秘密鍵を計算します。生成された秘密鍵をバッファアウトに保存し、ounlentの秘密鍵の変数を割り当てます。大きくてリトルエンディアンの両方をサポートします。
    \return 0  共有秘密鍵を正常に計算したときに返されました。
    \return BAD_FUNC_ARG  渡された入力パラメータのいずれかがNULLの場合に返されます。
    \return ECC_BAD_ARG_E  公開鍵の最初のビットが設定されている場合は、実装の指紋を避けるために返されます。
    \param [in]  Private_Key Curve25519_Key構造体の秘密鍵で初期化されました。
    \param [in]  public_key受信した公開鍵を含むCurve25519_Key構造体へのポインタ。
    \param [out]  32バイト計算された秘密鍵を格納するバッファへのポインタ。
    \param pin,out]  出力バッファに書き込まれた長さを記憶するポインタの概要。
    _Example_
    \code
    int ret;

    byte sharedKey[32];
    word32 keySz;

    curve25519_key privKey, pubKey;
    // initialize both keys

    ret = wc_curve25519_shared_secret_ex(&privKey, &pubKey, sharedKey, &keySz,
             EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error generating shared key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
    \sa wc_curve25519_shared_secret
*/

int wc_curve25519_shared_secret_ex(curve25519_key* private_key,
                                   curve25519_key* public_key,
                                   byte* out, word32* outlen, int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数はCurve25519キーを初期化します。構造のキーを生成する前に呼び出されるべきです。
    \return 0  Curve25519_Key構造体の初期化に成功しました。
    \return BAD_FUNC_ARG  キーがNULLのときに返されます。
    _Example_
    \code
    curve25519_key key;
    wc_curve25519_init(&key); // initialize key
    // make key and proceed to encryption
    \endcode
    \sa wc_curve25519_make_key
*/

int wc_curve25519_init(curve25519_key* key);

/*!
    \ingroup Curve25519 
    \brief  この関数はCurve25519オブジェクトを解放します。
    _Example_
    \code
    curve25519_key privKey;
    // initialize key, use it to generate shared secret key
    wc_curve25519_free(&privKey);
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
*/

void wc_curve25519_free(curve25519_key* key);

/*!
    \ingroup Curve25519 
    \brief  この関数はCurve25519秘密鍵のみをインポートします。（ビッグエンディアン）。
    \return 0  秘密鍵のインポートに成功しました。
    \return BAD_FUNC_ARG  キーまたはPRIVがNULLの場合は返します。
    \return ECC_BAD_ARG_E  PRIVSZがcurve25519_KEY_SIZEと等しくない場合は返します。
    \param [in]  インポートする秘密鍵を含むバッファへのポイント。
    \param [in]  インポートする秘密鍵のPrivsz長。
    _Example_
    \code
    int ret;

    byte priv[] = { Contents of private key };
    curve25519_key key;
    wc_curve25519_init(&key);

    ret = wc_curve25519_import_private(priv, sizeof(priv), &key);
    if (ret != 0) {
        // error importing keys
    }
    \endcode
    \sa wc_curve25519_import_private_ex
    \sa wc_curve25519_size
*/

int wc_curve25519_import_private(const byte* priv, word32 privSz,
                                 curve25519_key* key);

/*!
    \ingroup Curve25519 
    \brief  CURVE25519秘密鍵のインポートのみ。（大きなエンディアン）。
    \return 0  秘密鍵のインポートに成功しました。
    \return BAD_FUNC_ARG  キーまたはPRIVがNULLの場合は返します。
    \return ECC_BAD_ARG_E  PRIVSZがcurve25519_KEY_SIZEと等しくない場合は返します。
    \param [in]  インポートする秘密鍵を含むバッファへのポイント。
    \param [in]  インポートする秘密鍵のPrivsz長。
    \param [in,out]  インポートされたキーを保存する構造へのキーポインタ。
    _Example_
    \code
    int ret;

    byte priv[] = { // Contents of private key };
    curve25519_key key;
    wc_curve25519_init(&key);

    ret = wc_curve25519_import_private_ex(priv, sizeof(priv), &key,
            EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error importing keys
    }

    \endcode
    \sa wc_curve25519_import_private
    \sa wc_curve25519_size
*/

int wc_curve25519_import_private_ex(const byte* priv, word32 privSz,
                                    curve25519_key* key, int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数は、パブリック秘密鍵ペアをCurve25519_Key構造体にインポートします。ビッグエンディアンのみ。
    \return 0  Curve25519_Key構造体へのインポートに返されます
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返します。
    \return ECC_BAD_ARG_E  入力キーのキーサイズがPublicキーサイズまたは秘密鍵サイズと一致しない場合に返されます。
    \param [in]  インポートする秘密鍵を含むバッファへのポイント。
    \param [in]  インポートする秘密鍵のPrivsz長。
    \param [in]  パブリックキーをインポートするバッファへのPub。
    \param [in]  インポートする公開鍵のPubsz長さ。
    _Example_
    \code
    int ret;

    byte priv[32];
    byte pub[32];
    // initialize with public and private keys
    curve25519_key key;

    wc_curve25519_init(&key);
    // initialize key

    ret = wc_curve25519_import_private_raw(&priv, sizeof(priv), pub,
            sizeof(pub), &key);
    if (ret != 0) {
        // error importing keys
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
    \sa wc_curve25519_import_public
    \sa wc_curve25519_export_private_raw
*/

int wc_curve25519_import_private_raw(const byte* priv, word32 privSz,
                            const byte* pub, word32 pubSz, curve25519_key* key);

/*!
    \ingroup Curve25519 
    \brief  この関数は、パブリック秘密鍵ペアをCurve25519_Key構造体にインポートします。大きくてリトルエンディアンの両方をサポートします。
    \return 0  Curve25519_Key構造体へのインポートに返されます
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返します。
    \return ECC_BAD_ARG_E  戻されたIFまたは入力キーのキーサイズがパブリックキーサイズまたは秘密鍵サイズと一致しない場合
    \param [in]  インポートする秘密鍵を含むバッファへのポイント。
    \param [in]  インポートする秘密鍵のPrivsz長。
    \param [in]  パブリックキーをインポートするバッファへのPub。
    \param [in]  インポートする公開鍵のPubsz長さ。
    \param [in,out]  インポートされたキーを保存する構造へのキーポインタ。
    _Example_
    \code
    int ret;
    byte priv[32];
    byte pub[32];
    // initialize with public and private keys
    curve25519_key key;

    wc_curve25519_init(&key);
    // initialize key

    ret = wc_curve25519_import_private_raw_ex(&priv, sizeof(priv), pub,
            sizeof(pub), &key, EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error importing keys
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
    \sa wc_curve25519_import_public
    \sa wc_curve25519_export_private_raw
    \sa wc_curve25519_import_private_raw
*/

int wc_curve25519_import_private_raw_ex(const byte* priv, word32 privSz,
                                        const byte* pub, word32 pubSz,
                                        curve25519_key* key, int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数はCurve25519_Key構造体から秘密鍵をエクスポートし、それを指定されたアウトバッファに格納します。また、エクスポートされたキーのサイズになるように概要を設定します。ビッグエンディアンのみ。
    \return 0  Curve25519_Key構造体から秘密鍵を正常にエクスポートしました。
    \return BAD_FUNC_ARG  入力パラメータがNULLの場合に返されます。
    \return ECC_BAD_ARG_E  WC_CURVE25519_SIZE（）がキーと等しくない場合に返されます。
    \param [in]  キーをエクスポートする構造へのキーポインタ。
    \param [out]  エクスポートされたキーを保存するバッファへのポインタ。
    _Example_
    \code
    int ret;
    byte priv[32];
    int privSz;

    curve25519_key key;
    // initialize and make key

    ret = wc_curve25519_export_private_raw(&key, priv, &privSz);
    if (ret != 0) {
        // error exporting key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
    \sa wc_curve25519_import_private_raw
    \sa wc_curve25519_export_private_raw_ex
*/

int wc_curve25519_export_private_raw(curve25519_key* key, byte* out,
                                     word32* outLen);

/*!
    \ingroup Curve25519 
    \brief  この関数はCurve25519_Key構造体から秘密鍵をエクスポートし、それを指定されたアウトバッファに格納します。また、エクスポートされたキーのサイズになるように概要を設定します。それが大きいかリトルエンディアンかを指定できます。
    \return 0  Curve25519_Key構造体から秘密鍵を正常にエクスポートしました。
    \return BAD_FUNC_ARG  入力パラメータがNULLの場合に返されます。
    \return ECC_BAD_ARG_E  WC_CURVE25519_SIZE（）がキーと等しくない場合に返されます。
    \param [in]  キーをエクスポートする構造へのキーポインタ。
    \param [out]  エクスポートされたキーを保存するバッファへのポインタ。
    \param [in,out]  INに照会は、バイト数のサイズです。ON OUTでは、出力バッファに書き込まれたバイトを保存します。
    _Example_
    \code
    int ret;

    byte priv[32];
    int privSz;
    curve25519_key key;
    // initialize and make key
    ret = wc_curve25519_export_private_raw_ex(&key, priv, &privSz,
            EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error exporting key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
    \sa wc_curve25519_import_private_raw
    \sa wc_curve25519_export_private_raw
    \sa wc_curve25519_size
*/

int wc_curve25519_export_private_raw_ex(curve25519_key* key, byte* out,
                                        word32* outLen, int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数は、指定されたバッファから公開鍵をインポートし、それをCurve25519_Key構造体に格納します。
    \return 0  公開鍵をCurve25519_Key構造体に正常にインポートしました。
    \return ECC_BAD_ARG_E  InLenパラメータがキー構造のキーサイズと一致しない場合に返されます。
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返されます。
    \param [in]  インポートする公開鍵を含むバッファへのポインタ。
    \param [in]  インポートする公開鍵のインレル長。
    _Example_
    \code
    int ret;

    byte pub[32];
    // initialize pub with public key

    curve25519_key key;
    // initialize key

    ret = wc_curve25519_import_public(pub,sizeof(pub), &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_export_public
    \sa wc_curve25519_import_private_raw
    \sa wc_curve25519_import_public_ex
    \sa wc_curve25519_check_public
    \sa wc_curve25519_size
*/

int wc_curve25519_import_public(const byte* in, word32 inLen,
                                curve25519_key* key);

/*!
    \ingroup Curve25519 
    \brief  この関数は、指定されたバッファから公開鍵をインポートし、それをCurve25519_Key構造体に格納します。
    \return 0  公開鍵をCurve25519_Key構造体に正常にインポートしました。
    \return ECC_BAD_ARG_E  InLenパラメータがキー構造のキーサイズと一致しない場合に返されます。
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返されます。
    \param [in]  インポートする公開鍵を含むバッファへのポインタ。
    \param [in]  インポートする公開鍵のインレル長。
    \param [in,out]  キーを保存するカーブ25519キー構造へのキーポインタ。
    _Example_
    \code
    int ret;

    byte pub[32];
    // initialize pub with public key
    curve25519_key key;
    // initialize key

    ret = wc_curve25519_import_public_ex(pub, sizeof(pub), &key,
            EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error importing key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_export_public
    \sa wc_curve25519_import_private_raw
    \sa wc_curve25519_import_public
    \sa wc_curve25519_check_public
    \sa wc_curve25519_size
*/

int wc_curve25519_import_public_ex(const byte* in, word32 inLen,
                                   curve25519_key* key, int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数は、公開鍵バッファがエンディアン注文を与えられた有効なCurve2519キー値を保持していることを確認します。
    \return 0  公開鍵の値が有効なときに返されます。
    \return ECC_BAD_ARG_E  公開鍵の値が無効な場合は返されます。
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返されます。
    \param [in]  チェックするための公開鍵を含むバッファへのPubポインタ。
    \param [in]  チェックするための公開鍵の長さを掲載します。
    _Example_
    \code
    int ret;

    byte pub[] = { Contents of public key };

    ret = wc_curve25519_check_public_ex(pub, sizeof(pub), EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error importing key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_import_public
    \sa wc_curve25519_import_public_ex
    \sa wc_curve25519_size
*/

int wc_curve25519_check_public(const byte* pub, word32 pubSz, int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数は指定されたキー構造から公開鍵をエクスポートし、結果をアウトバッファに格納します。ビッグエンディアンのみ。
    \return 0  Curve25519_Key構造体から公開鍵を正常にエクスポートする上で返されます。
    \return ECC_BAD_ARG_E  outlenがcurve25519_pub_key_sizeより小さい場合に返されます。
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返されます。
    \param [in]  キーをエクスポートするCurve25519_Key構造体へのキーポインタ。
    \param [out]  公開鍵を保存するバッファへのポインタ。
    _Example_
    \code
    int ret;

    byte pub[32];
    int pubSz;

    curve25519_key key;
    // initialize and make key
    ret = wc_curve25519_export_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error exporting key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_export_private_raw
    \sa wc_curve25519_import_public
*/

int wc_curve25519_export_public(curve25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup Curve25519 
    \brief  この関数は指定されたキー構造から公開鍵をエクスポートし、結果をアウトバッファに格納します。大きくてリトルエンディアンの両方をサポートします。
    \return 0  Curve25519_Key構造体から公開鍵を正常にエクスポートする上で返されます。
    \return ECC_BAD_ARG_E  outlenがcurve25519_pub_key_sizeより小さい場合に返されます。
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLの場合に返されます。
    \param [in]  キーをエクスポートするCurve25519_Key構造体へのキーポインタ。
    \param [out]  公開鍵を保存するバッファへのポインタ。
    \param [in,out]  INに照会は、バイト数のサイズです。ON OUTでは、出力バッファに書き込まれたバイトを保存します。
    _Example_
    \code
    int ret;

    byte pub[32];
    int pubSz;

    curve25519_key key;
    // initialize and make key

    ret = wc_curve25519_export_public_ex(&key, pub, &pubSz, EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error exporting key
    }
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_export_private_raw
    \sa wc_curve25519_import_public
*/

int wc_curve25519_export_public_ex(curve25519_key* key, byte* out,
                                   word32* outLen, int endian);

/*!
    \ingroup Curve25519 
    \brief  Export Curve25519キーペア。ビッグエンディアンのみ。
    \return 0  Curve25519_Key構造体からキーペアのエクスポートに成功しました。
    \return BAD_FUNC_ARG  入力パラメータがNULLの場合に返されます。
    \return ECC_BAD_ARG_E  PRIVSZがCURUV25519_SEY_SIZEまたはPUBSZよりも小さい場合は、PUBSZがCURUG25519_PUB_KEY_SIZEよりも小さい場合に返されます。
    \param [in]  キーペアをエクスポートするCURUN448_KEY構造体へのキーポインタ。
    \param [out]  秘密鍵を保存するバッファへのPRIVポインタ。
    \param [in,out]  PRIVSZ ON INは、PRIVバッファのサイズをバイト単位で）です。ON OUTは、PRIVバッファに書き込まれたバイトを保存します。
    \param [out]  パブリックキーを保存するバッファへのPub。
    _Example_
    \code
    int ret;

    byte pub[32];
    byte priv[32];
    int pubSz;
    int privSz;

    curve25519_key key;
    // initialize and make key

    ret = wc_curve25519_export_key_raw(&key, priv, &privSz, pub, &pubSz);
    if (ret != 0) {
        // error exporting key
    }
    \endcode
    \sa wc_curve25519_export_key_raw_ex
    \sa wc_curve25519_export_private_raw
*/

int wc_curve25519_export_key_raw(curve25519_key* key,
                                 byte* priv, word32 *privSz,
                                 byte* pub, word32 *pubSz);

/*!
    \ingroup Curve25519 
    \brief  Export Curve25519キーペア。大きいまたはリトルエンディアン。
    \return 0  Curve25519_Key構造体からキーペアのエクスポートに成功しました。
    \return BAD_FUNC_ARG  入力パラメータがNULLの場合に返されます。
    \return ECC_BAD_ARG_E  PRIVSZがCURUV25519_SEY_SIZEまたはPUBSZよりも小さい場合は、PUBSZがCURUG25519_PUB_KEY_SIZEよりも小さい場合に返されます。
    \param [in]  キーペアをエクスポートするCURUN448_KEY構造体へのキーポインタ。
    \param [out]  秘密鍵を保存するバッファへのPRIVポインタ。
    \param [in,out]  PRIVSZ ON INは、PRIVバッファのサイズをバイト単位で）です。ON OUTは、PRIVバッファに書き込まれたバイトを保存します。
    \param [out]  パブリックキーを保存するバッファへのPub。
    \param [in,out]  PUBSZ ON INは、パブバッファのサイズをバイト単位で）です。ON OUTでは、PUBバッファに書き込まれたバイトを保存します。
    _Example_
    \code
    int ret;

    byte pub[32];
    byte priv[32];
    int pubSz;
    int privSz;

    curve25519_key key;
    // initialize and make key

    ret = wc_curve25519_export_key_raw_ex(&key,priv, &privSz, pub, &pubSz,
            EC25519_BIG_ENDIAN);
    if (ret != 0) {
        // error exporting key
    }
    \endcode
    \sa wc_curve25519_export_key_raw
    \sa wc_curve25519_export_private_raw_ex
    \sa wc_curve25519_export_public_ex
*/

int wc_curve25519_export_key_raw_ex(curve25519_key* key,
                                    byte* priv, word32 *privSz,
                                    byte* pub, word32 *pubSz,
                                    int endian);

/*!
    \ingroup Curve25519 
    \brief  この関数は与えられたキー構造のキーサイズを返します。
    \return Success  有効な初期化されたCurve25519_Key構造体を考慮すると、キーのサイズを返します。
    \return 0  キーがNULLの場合は返されます
    _Example_
    \code
    int keySz;

    curve25519_key key;
    // initialize and make key

    keySz = wc_curve25519_size(&key);
    \endcode
    \sa wc_curve25519_init
    \sa wc_curve25519_make_key
*/

int wc_curve25519_size(curve25519_key* key);
