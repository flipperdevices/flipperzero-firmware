/*!
    \ingroup ED25519 
    \brief  この関数は秘密鍵からED25519公開鍵を生成します。公開鍵をバッファPubkeyに格納し、Pubkeyszでこのバッファに書き込まれたバイトを設定します。
    \return 0  公開鍵の作成に成功したときに返されます。
    \return BAD_FUNC_ARG  IFIキーまたはPubKeyがNULLに評価された場合、または指定されたキーサイズが32バイトではない場合（ED25519に32バイトのキーがあります）。
    \return MEMORY_E  関数の実行中にメモリを割り当てるエラーがある場合に返されます。
    \param [in]  キーを生成するED25519_Keyへのキーポインタ。
    \param [out]  公開鍵を保存するバッファへのポインタ。
    _Example_
    \code
    int ret;

    ed25519_key key;
    byte priv[] = { initialize with 32 byte private key };
    byte pub[32];
    word32 pubSz = sizeof(pub);

    wc_ed25519_init(&key);
    wc_ed25519_import_private_only(priv, sizeof(priv), &key);
    ret = wc_ed25519_make_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error making public key
    }
    \endcode
    \sa wc_ed25519_init
    \sa wc_ed25519_import_private_only
    \sa wc_ed25519_make_key
*/

int wc_ed25519_make_public(ed25519_key* key, unsigned char* pubKey,
                           word32 pubKeySz);

/*!
    \ingroup ED25519 
    \brief  この関数は新しいED25519キーを生成し、それをキーに格納します。
    \return 0  ED25519_KEYを正常に行うと返されます。
    \return BAD_FUNC_ARG  RNGまたはKEYがNULLに評価された場合、または指定されたキーサイズが32バイトではない場合（ED25519に32バイトのキーがあります）。
    \return MEMORY_E  関数の実行中にメモリを割り当てるエラーがある場合に返されます。
    \param [in]  RNGキーを生成する初期化されたRNGオブジェクトへのポインタ。
    \param [in]  keysize keyの長さを生成します。ED25519の場合は常に32になります。
    _Example_
    \code
    int ret;

    WC_RNG rng;
    ed25519_key key;

    wc_InitRng(&rng);
    wc_ed25519_init(&key);
    wc_ed25519_make_key(&rng, 32, &key);
    if (ret != 0) {
        // error making key
    }
    \endcode
    \sa wc_ed25519_init
*/

int wc_ed25519_make_key(WC_RNG* rng, int keysize, ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_Keyオブジェクトを使用して認証を保証するメッセージに署名します。
    \return 0  メッセージの署名を正常に生成すると返されます。
    \return BAD_FUNC_ARG  入力パラメータのいずれかがNULLに評価された場合、または出力バッファが小さすぎて生成された署名を保存する場合は返されます。
    \return MEMORY_E  関数の実行中にメモリを割り当てるエラーがある場合に返されます。
    \param [in]  署名するメッセージを含むバッファへのポインタ。
    \param [in]  署名するメッセージのインレル長。
    \param [out]  生成された署名を格納するためのバッファー。
    \param [in,out]  出力バッファの最大長の範囲内。メッセージ署名の生成に成功したときに、書き込まれたバイトを保存します。
    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519_sign_msg(message, sizeof(message), sig, &sigSz, &key);
    if (ret != 0) {
        // error generating message signature
    }
    \endcode
    \sa wc_ed25519ctx_sign_msg
    \sa wc_ed25519ph_sign_hash
    \sa wc_ed25519ph_sign_msg
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_Keyオブジェクトを使用して認証を保証するメッセージに署名します。コンテキストは署名されたデータの一部です。
    \return 0  メッセージの署名を正常に生成すると返されます。
    \return BAD_FUNC_ARG  返された入力パラメータはNULLに評価されます。出力バッファが小さすぎて生成された署名を保存するには小さすぎます。
    \return MEMORY_E  関数の実行中にメモリを割り当てるエラーがある場合に返されます。
    \param [in]  署名するメッセージを含むバッファへのポインタ。
    \param [in]  署名するメッセージのインレル長。
    \param [out]  生成された署名を格納するためのバッファー。
    \param [in,out]  出力バッファの最大長の範囲内。メッセージ署名の生成に成功したときに、書き込まれたバイトを保存します。
    \param [in]  署名を生成するプライベートED25519_KEYへのキーポインタ。
    \param [in]  メッセージが署名されているコンテキストを含むバッファへのコンテキストポインタ。
    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519ctx_sign_msg(message, sizeof(message), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode
    \sa wc_ed25519_sign_msg
    \sa wc_ed25519ph_sign_hash
    \sa wc_ed25519ph_sign_msg
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519ctx_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key,
                        const byte* context, byte contextLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_Keyオブジェクトを使用してメッセージダイジェストに署名して信頼性を保証します。コンテキストは署名されたデータの一部として含まれています。署名計算の前にメッセージは事前にハッシュされています。メッセージダイジェストを作成するために使用されるハッシュアルゴリズムはShake-256でなければなりません。
    \return 0  メッセージダイジェストの署名を正常に生成すると返されます。
    \return BAD_FUNC_ARG  返された入力パラメータはNULLに評価されます。出力バッファが小さすぎて生成された署名を保存するには小さすぎます。
    \return MEMORY_E  関数の実行中にメモリを割り当てるエラーがある場合に返されます。
    \param [in]  サインへのメッセージのハッシュを含むバッファへのハッシュポインタ。
    \param [in]  サインへのメッセージのハッシュのハッシュの長さ。
    \param [out]  生成された署名を格納するためのバッファー。
    \param [in,out]  出力バッファの最大長の範囲内。メッセージ署名の生成に成功したときに、書き込まれたバイトを保存します。
    \param [in]  署名を生成するプライベートED25519_KEYへのキーポインタ。
    \param [in]  メッセージが署名されているコンテキストを含むバッファへのコンテキストポインタ。
    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte hash[] = { initialize with SHA-512 hash of message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519ph_sign_hash(hash, sizeof(hash), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode
    \sa wc_ed25519_sign_msg
    \sa wc_ed25519ctx_sign_msg
    \sa wc_ed25519ph_sign_msg
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519ph_sign_hash(const byte* hash, word32 hashLen, byte* out,
                           word32 *outLen, ed25519_key* key,
                           const byte* context, byte contextLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_Keyオブジェクトを使用して認証を保証するメッセージに署名します。コンテキストは署名されたデータの一部として含まれています。署名計算の前にメッセージは事前にハッシュされています。
    \return 0  メッセージの署名を正常に生成すると返されます。
    \return BAD_FUNC_ARG  返された入力パラメータはNULLに評価されます。出力バッファが小さすぎて生成された署名を保存するには小さすぎます。
    \return MEMORY_E  関数の実行中にメモリを割り当てるエラーがある場合に返されます。
    \param [in]  署名するメッセージを含むバッファへのポインタ。
    \param [in]  署名するメッセージのインレル長。
    \param [out]  生成された署名を格納するためのバッファー。
    \param [in,out]  出力バッファの最大長の範囲内。メッセージ署名の生成に成功したときに、書き込まれたバイトを保存します。
    \param [in]  署名を生成するプライベートED25519_KEYへのキーポインタ。
    \param [in]  メッセージが署名されているコンテキストを含むバッファへのコンテキストポインタ。
    _Example_
    \code
    ed25519_key key;
    WC_RNG rng;
    int ret, sigSz;

    byte sig[64]; // will hold generated signature
    sigSz = sizeof(sig);
    byte message[] = { initialize with message };
    byte context[] = { initialize with context of signing };

    wc_InitRng(&rng); // initialize rng
    wc_ed25519_init(&key); // initialize key
    wc_ed25519_make_key(&rng, 32, &key); // make public/private key pair
    ret = wc_ed25519ph_sign_msg(message, sizeof(message), sig, &sigSz, &key,
            context, sizeof(context));
    if (ret != 0) {
        // error generating message signature
    }
    \endcode
    \sa wc_ed25519_sign_msg
    \sa wc_ed25519ctx_sign_msg
    \sa wc_ed25519ph_sign_hash
    \sa wc_ed25519_verify_msg
*/

int wc_ed25519ph_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outlen, ed25519_key* key,
                        const byte* context, byte contextLen);

/*!
    \ingroup ED25519 
    \brief  この関数はメッセージのED25519署名を確認して信頼性を確保します。RESを介して答えを返し、有効な署名に対応する1、無効な署名に対応する0を返します。
    \return 0  署名検証と認証を正常に実行したときに返されます。
    \return BAD_FUNC_ARG  いずれかの入力パラメータがNULLに評価された場合、またはSIGLENが署名の実際の長さと一致しない場合に返されます。
    \return SIG_VERIFY_E  検証が完了した場合は返されますが、生成された署名は提供された署名と一致しません。
    \param [in]  検証するシグネチャを含むバッファへのSIGポインタ。
    \param [in]  検証するシグネチャのシグレンの長さ。
    \param [in]  メッセージを含むバッファへのMSGポインタを確認する。
    \param [in]  検証するメッセージのMSGlen長。
    \param [out]  検証の結果へのRESポインタ。1メッセージが正常に検証されたことを示します。
    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    // initialize key with received public key
    ret = wc_ed25519_verify_msg(sig, sizeof(sig), msg, sizeof(msg), &verified,
            &key);
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode
    \sa wc_ed25519ctx_verify_msg
    \sa wc_ed25519ph_verify_hash
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                          word32 msgLen, int* ret, ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数はメッセージのED25519署名を確認して信頼性を確保します。文脈はデータ検証済みの一部として含まれています。RESを介して答えを返し、有効な署名に対応する1、無効な署名に対応する0を返します。
    \return 0  署名検証と認証を正常に実行したときに返されます。
    \return BAD_FUNC_ARG  いずれかの入力パラメータがNULLに評価された場合、またはSIGLENが署名の実際の長さと一致しない場合に返されます。
    \return SIG_VERIFY_E  検証が完了した場合は返されますが、生成された署名は提供された署名と一致しません。
    \param [in]  検証するシグネチャを含むバッファへのSIGポインタ。
    \param [in]  検証するシグネチャのシグレンの長さ。
    \param [in]  メッセージを含むバッファへのMSGポインタを確認する。
    \param [in]  検証するメッセージのMSGlen長。
    \param [out]  検証の結果へのRESポインタ。1メッセージが正常に検証されたことを示します。
    \param [in]  署名を検証するためのPublic ED25519キーへのキーポインタ。
    \param [in]  メッセージが署名されたコンテキストを含むバッファへのコンテキストポインタ。
    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed25519ctx_verify_msg(sig, sizeof(sig), msg, sizeof(msg),
            &verified, &key, );
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode
    \sa wc_ed25519_verify_msg
    \sa wc_ed25519ph_verify_hash
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519ctx_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                             word32 msgLen, int* ret, ed25519_key* key,
                             const byte* context, byte contextLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、メッセージのダイジェストのED25519シグネチャを確認して、信頼性を確保します。文脈はデータ検証済みの一部として含まれています。ハッシュは、署名計算前のプリハッシュメッセージです。メッセージダイジェストを作成するために使用されるハッシュアルゴリズムはSHA-512でなければなりません。答えはRESを介して返され、有効な署名に対応する1、無効な署名に対応する0を返します。
    \return 0  署名検証と認証を正常に実行したときに返されます。
    \return BAD_FUNC_ARG  いずれかの入力パラメータがNULLに評価された場合、またはSIGLENが署名の実際の長さと一致しない場合に返されます。
    \return SIG_VERIFY_E  検証が完了した場合は返されますが、生成された署名は提供された署名と一致しません。
    \param [in]  検証するシグネチャを含むバッファへのSIGポインタ。
    \param [in]  検証するシグネチャのシグレンの長さ。
    \param [in]  検証するメッセージのハッシュを含むバッファへのハッシュポインタ。
    \param [in]  検証するハッシュのハッシュレン長。
    \param [out]  検証の結果へのRESポインタ。1メッセージが正常に検証されたことを示します。
    \param [in]  署名を検証するためのPublic ED25519キーへのキーポインタ。
    \param [in]  メッセージが署名されたコンテキストを含むバッファへのコンテキストポインタ。
    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte hash[] = { initialize with SHA-512 hash of message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed25519ph_verify_hash(sig, sizeof(sig), msg, sizeof(msg),
            &verified, &key, );
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode
    \sa wc_ed25519_verify_msg
    \sa wc_ed25519ctx_verify_msg
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519ph_verify_hash(const byte* sig, word32 siglen, const byte* hash,
                             word32 hashLen, int* ret, ed25519_key* key,
                             const byte* context, byte contextLen);

/*!
    \ingroup ED25519 
    \brief  この関数はメッセージのED25519署名を確認して信頼性を確保します。文脈はデータ検証済みの一部として含まれています。検証前にメッセージがプリハッシュされています。RESを介して答えを返し、有効な署名に対応する1、無効な署名に対応する0を返します。
    \return 0  署名検証と認証を正常に実行したときに返されます。
    \return BAD_FUNC_ARG  いずれかの入力パラメータがNULLに評価された場合、またはSIGLENが署名の実際の長さと一致しない場合に返されます。
    \return SIG_VERIFY_E  検証が完了した場合は返されますが、生成された署名は提供された署名と一致しません。
    \param [in]  検証するシグネチャを含むバッファへのSIGポインタ。
    \param [in]  検証するシグネチャのシグレンの長さ。
    \param [in]  メッセージを含むバッファへのMSGポインタを確認する。
    \param [in]  検証するメッセージのMSGlen長。
    \param [out]  検証の結果へのRESポインタ。1メッセージが正常に検証されたことを示します。
    \param [in]  署名を検証するためのPublic ED25519キーへのキーポインタ。
    \param [in]  メッセージが署名されたコンテキストを含むバッファへのコンテキストポインタ。
    _Example_
    \code
    ed25519_key key;
    int ret, verified = 0;

    byte sig[] { initialize with received signature };
    byte msg[] = { initialize with message };
    byte context[] = { initialize with context of signature };
    // initialize key with received public key
    ret = wc_ed25519ctx_verify_msg(sig, sizeof(sig), msg, sizeof(msg),
            &verified, &key, );
    if (ret < 0) {
        // error performing verification
    } else if (verified == 0)
        // the signature is invalid
    }
    \endcode
    \sa wc_ed25519_verify_msg
    \sa wc_ed25519ph_verify_hash
    \sa wc_ed25519ph_verify_msg
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519ph_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                            word32 msgLen, int* ret, ed25519_key* key,
                            const byte* context, byte contextLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、メッセージ検証で将来の使用のためにED25519_Keyオブジェクトを初期化します。
    \return 0  ED25519_Keyオブジェクトの初期化に成功したときに返されます。
    \return BAD_FUNC_ARG  キーがNULLの場合は返されます。
    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);
    \endcode
    \sa wc_ed25519_make_key
    \sa wc_ed25519_free
*/

int wc_ed25519_init(ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、使用された後にED25519オブジェクトを解放します。
    _Example_
    \code
    ed25519_key key;
    // initialize key and perform secure exchanges
    ...
    wc_ed25519_free(&key);
    \endcode
    \sa wc_ed25519_init
*/

void wc_ed25519_free(ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、公開鍵を含むバッファからPublic ED25519_Keyペアをインポートします。この関数は圧縮キーと非圧縮キーの両方を処理します。
    \return 0  ED25519_KEYのインポートに成功しました。
    \return BAD_FUNC_ARG  inまたはkeyがnullに評価された場合、またはInlenがED25519キーのサイズよりも小さい場合に返されます。
    \param [in]  公開鍵を含むバッファへのポインタ。
    \param [in]  公開鍵を含むバッファのインレル長。
    _Example_
    \code
    int ret;
    byte pub[] = { initialize Ed25519 public key };

    ed_25519 key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_public(pub, sizeof(pub), &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_export_public
*/

int wc_ed25519_import_public(const byte* in, word32 inLen, ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、ed25519秘密鍵をバッファからのみインポートします。
    \return 0  ED25519キーのインポートに成功しました。
    \return BAD_FUNC_ARG  privまたはkeyがNULLに評価された場合、またはprivSzがED25519_KEY_SIZEと異なる場合に返されます。
    \param [in]  秘密鍵を含むバッファへのPRIVポインタ。
    \param [in]  秘密鍵のPrivsz長さ。
    \param [in]  公開鍵を含むバッファへのPubポインタ。
    \param [in]  公開鍵のPubszの長さ。
    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 32 byte private key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_private_key(priv, sizeof(priv), &key);
    if (ret != 0) {
        // error importing private key
    }
    \endcode
    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_export_private_only
*/

int wc_ed25519_import_private_only(const byte* priv, word32 privSz,
                                   ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、一対のバッファからパブリック/プライベートED25519キーペアをインポートします。この関数は圧縮キーと非圧縮キーの両方を処理します。
    \return 0  ED25519_KEYのインポートに成功しました。
    \return BAD_FUNC_ARG  privまたはkeyがNULLに評価された場合、privSzがED25519_KEY_SIZEと異なるあるいはED25519_PRV_KEY_SIZEとも異なる場合、pubSzがED25519_PUB_KEY_SIZEよりも小さい場合に返されます。
    \param [in]  秘密鍵を含むバッファへのPRIVポインタ。
    \param [in]  秘密鍵のPrivsz長さ。
    \param [in]  公開鍵を含むバッファへのPubポインタ。
    \param [in]  公開鍵のPubszの長さ。
    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 32 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_private_key(priv, sizeof(priv), pub, sizeof(pub),
            &key);
    if (ret != 0) {
        // error importing key
    }
    \endcode
    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_private_only
    \sa wc_ed25519_export_private
*/

int wc_ed25519_import_private_key(const byte* priv, word32 privSz,
                               const byte* pub, word32 pubSz, ed25519_key* key);

/*!
    \ingroup ED25519
    \brief この関数は一対のバッファからEd25519公開鍵/秘密鍵ペアをインポートします。この関数は圧縮キーと非圧縮キーの両方を処理します。公開鍵はtrusted引数により信頼されていないとされた場合には秘密鍵に対して検証されます。
    \return 0 ed25519_keyのインポートに成功しました。
    \return BAD_FUNC_ARG Returned if privあるいはkeyがNULLに評価された場合、privSzがED25519_KEY_SIZEともED25519_PRV_KEY_SIZEとも異なる場合、pubSzがED25519_PUB_KEY_SIZEより小さい場合に返されます。
    \param [in] priv 秘密鍵を保持するバッファへのポインター
    \param [in] privSz 秘密鍵バッファのサイズ
    \param [in] pub 公開鍵を保持するバッファへのポインター
    \param [in] pubSz 公開鍵バッファのサイズ
    \param [in,out] key インポートされた公開鍵/秘密鍵を保持するed25519_keyオブジェクトへのポインター
    \param [in] trusted 公開鍵が信頼できるか否か。
    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 32 byte private key };
    byte pub[]  = { initialize with the corresponding public key };
    ed25519_key key;
    wc_ed25519_init_key(&key);
    ret = wc_ed25519_import_private_key(priv, sizeof(priv), pub, sizeof(pub),
            &key, 1);
    if (ret != 0) {
        // error importing key
    }
    \endcode
    \sa wc_ed25519_import_public
    \sa wc_ed25519_import_public_ex
    \sa wc_ed25519_import_private_only
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_export_private
*/

int wc_ed25519_import_private_key_ex(const byte* priv, word32 privSz,
    const byte* pub, word32 pubSz, ed25519_key* key, int trusted);

/*!
    \ingroup ED25519 
    \brief  この関数は、秘密鍵をED25519_Key構造体からエクスポートします。公開鍵をバッファアウトに格納し、ounterenでこのバッファに書き込まれたバイトを設定します。
    \return 0  公開鍵のエクスポートに成功したら返されます。
    \return BAD_FUNC_ARG  いずれかの入力値がNULLに評価された場合に返されます。
    \return BUFFER_E  提供されたバッファーが秘密鍵を保存するのに十分な大きさでない場合に返されます。このエラーを返すと、outlenに必要なサイズを設定します。
    \param [in]  公開鍵をエクスポートするためのED25519_Key構造体へのキーポインタ。
    \param [out]  公開鍵を保存するバッファへのポインタ。
    _Example_
    \code
    int ret;
    ed25519_key key;
    // initialize key, make key

    char pub[32];
    word32 pubSz = sizeof(pub);

    ret = wc_ed25519_export_public(&key, pub, &pubSz);
    if (ret != 0) {
        // error exporting public key
    }
    \endcode
    \sa wc_ed25519_import_public
    \sa wc_ed25519_export_private_only
*/

int wc_ed25519_export_public(ed25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_Key構造体からの秘密鍵のみをエクスポートします。秘密鍵をバッファアウトに格納し、outlenにこのバッファに書き込まれたバイトを設定します。
    \return 0  秘密鍵のエクスポートに成功したら返されます。
    \return BAD_FUNC_ARG  いずれかの入力値がNULLに評価された場合に返されます。
    \return BUFFER_E  提供されたバッファーが秘密鍵を保存するのに十分な大きさでない場合に返されます。
    \param [in]  秘密鍵をエクスポートするためのED25519_Key構造体へのキーポインタ。
    \param [out]  秘密鍵を保存するバッファへのポインタ。
    _Example_
    \code
    int ret;
    ed25519_key key;
    // initialize key, make key

    char priv[32]; // 32 bytes because only private key
    word32 privSz = sizeof(priv);
    ret = wc_ed25519_export_private_only(&key, priv, &privSz);
    if (ret != 0) {
        // error exporting private key
    }
    \endcode
    \sa wc_ed25519_export_public
    \sa wc_ed25519_import_private_key
*/

int wc_ed25519_export_private_only(ed25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_Key構造体からキーペアをエクスポートします。キーペアをバッファOUTに格納し、ounterenでこのバッファに書き込まれたバイトを設定します。
    \return 0  キーペアのエクスポートに成功したら返されます。
    \return BAD_FUNC_ARG  いずれかの入力値がNULLに評価された場合に返されます。
    \return BUFFER_E  提供されているバッファーがキーペアを保存するのに十分な大きさでない場合に返されます。
    \param [in]  キーペアをエクスポートするためのED25519_Key構造体へのキーポインタ。
    \param [out]  キーペアを保存するバッファへのポインタ。
    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);

    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed25519_make_key(&rng, 32, &key); // initialize 32 byte Ed25519 key

    byte out[64]; // out needs to be a sufficient buffer size
    word32 outLen = sizeof(out);
    int key_size = wc_ed25519_export_private(&key, out, &outLen);
    if (key_size == BUFFER_E) {
        // Check size of out compared to outLen to see if function reset outLen
    }
    \endcode
    \sa wc_ed25519_import_private_key
    \sa wc_ed25519_export_private_only
*/

int wc_ed25519_export_private(ed25519_key* key, byte* out, word32* outLen);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_KEY構造体とは別にプライベートキーと公開鍵をエクスポートします。秘密鍵をバッファーPrivに格納し、PRIVSZでこのバッファに書き込まれたバイトを設定します。公開鍵をバッファPUBに格納し、Pubszでこのバッファに書き込まれたバイトを設定します。
    \return 0  キーペアのエクスポートに成功したら返されます。
    \return BAD_FUNC_ARG  いずれかの入力値がNULLに評価された場合に返されます。
    \return BUFFER_E  提供されているバッファーがキーペアを保存するのに十分な大きさでない場合に返されます。
    \param [in]  キーペアをエクスポートするためのED25519_Key構造体へのキーポインタ。
    \param [out]  秘密鍵を保存するバッファへのPRIVポインタ。
    \param [in,out]  PRIVSZ PIVINSZポインタサイズが表示されているサイズを持つWord32オブジェクトへのポインタ。秘密鍵のエクスポート後に書き込まれたバイト数を設定します。
    \param [out]  パブリックキーを保存するバッファへのPub。
    _Example_
    \code
    int ret;
    ed25519_key key;
    // initialize key, make key

    char pub[32];
    word32 pubSz = sizeof(pub);
    char priv[32];
    word32 privSz = sizeof(priv);

    ret = wc_ed25519_export_key(&key, priv, &pubSz, pub, &pubSz);
    if (ret != 0) {
        // error exporting public key
    }
    \endcode
    \sa wc_ed25519_export_private
    \sa wc_ed25519_export_public
*/

int wc_ed25519_export_key(ed25519_key* key,
                          byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519_KEY構造体の公開鍵をチェックします。
    \return 0  プライベートキーと公開鍵が一致した場合に返されます。
    \return BAD_FUNC_ARG  与えられた鍵がNULLの場合に返されます。
    \return PUBLIC_KEY_E 公開鍵が参照できないか無効の場合に返されます。
    _Example_
    \code
    int ret;
    byte priv[] = { initialize with 57 byte private key };
    byte pub[]  = { initialize with the corresponding public key };

    ed25519_key key;
    wc_ed25519_init_key(&key);
    wc_ed25519_import_private_key(priv, sizeof(priv), pub, sizeof(pub), &key);
    ret = wc_ed25519_check_key(&key);
    if (ret != 0) {
        // error checking key
    }
    \endcode
    \sa wc_ed25519_import_private_key
*/

int wc_ed25519_check_key(ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519  -  32バイトのサイズを返します。
    \return ED25519_KEY_SIZE  有効な秘密鍵のサイズ（32バイト）。
    \return BAD_FUNC_ARG  与えられたキーがNULLの場合に返されます。
    _Example_
    \code
    int keySz;
    ed25519_key key;
    // initialize key, make key
    keySz = wc_ed25519_size(&key);
    if (keySz == 0) {
        // error determining key size
    }
    \endcode
    \sa wc_ed25519_make_key
*/

int wc_ed25519_size(ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、秘密鍵サイズ（secret + public）をバイト単位で返します。
    \return ED25519_PRV_KEY_SIZE  秘密鍵のサイズ（64バイト）。
    \return BAD_FUNC_ARG  key引数がnullの場合に返されます。
    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);

    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed25519_make_key(&rng, 32, &key); // initialize 32 byte Ed25519 key
    int key_size = wc_ed25519_priv_size(&key);
    \endcode
    \sa wc_ed25519_pub_size
*/

int wc_ed25519_priv_size(ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は圧縮鍵サイズをバイト単位で返します（公開鍵）。
    \return ED25519_PUB_KEY_SIZE  圧縮公開鍵のサイズ（32バイト）。
    \return BAD_FUNC_ARG  key引数がnullの場合は返します。
    _Example_
    \code
    ed25519_key key;
    wc_ed25519_init(&key);
    WC_RNG rng;
    wc_InitRng(&rng);

    wc_ed25519_make_key(&rng, 32, &key); // initialize 32 byte Ed25519 key
    int key_size = wc_ed25519_pub_size(&key);
    \endcode
    \sa wc_ed25519_priv_size
*/

int wc_ed25519_pub_size(ed25519_key* key);

/*!
    \ingroup ED25519 
    \brief  この関数は、ED25519シグネチャのサイズ（バイト数64）を返します。
    \return ED25519_SIG_SIZE  ED25519シグネチャ（64バイト）のサイズ。
    \return BAD_FUNC_ARG  key引数がnullの場合は返します。
    _Example_
    \code
    int sigSz;
    ed25519_key key;
    // initialize key, make key

    sigSz = wc_ed25519_sig_size(&key);
    if (sigSz == 0) {
        // error determining sig size
    }
    \endcode
    \sa wc_ed25519_sign_msg
*/

int wc_ed25519_sig_size(ed25519_key* key);
