/*!
    \ingroup RSA
    \brief  この関数は提供されたRsaKey構造体を初期化します。また、ユーザー定義メモリオーバーライドで使用するためのヒープ識別子も取ります（XMALLOC、XFREE、XREALLOCを参照）。wc_rsa_blindingが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return 0  暗号化と復号化で使用するためのRSA構造の初期化に成功したときに返されます。
    \return BAD_FUNC_ARGS  RSAキーポインタがNULLに評価された場合に返されます
    \param key  初期化するRSAKEY構造へのポインタ
    _Example_
    \code
    RsaKey enc;
    int ret;
    ret = wc_InitRsaKey(&enc, NULL); // not using heap hint. No custom memory
    if ( ret != 0 ) {
    	// error initializing RSA key
    }
    \endcode
    \sa wc_RsaInitCavium
    \sa wc_FreeRsaKey
    \sa wc_RsaSetRNG
*/
int  wc_InitRsaKey(RsaKey* key, void* heap);

/*!
    \ingroup RSA
    \brief  この関数は提供されたRsaKey構造体を初期化します。IDとLENは、DEVIDがデバイスを識別している間にデバイス上のキーを識別するために使用されます。また、ユーザー定義メモリオーバーライドで使用するためのヒープ識別子も取ります（XMALLOC、XFREE、XREALLOCを参照）。wc_rsa_blindingが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return 0  暗号化と復号化で使用するためのRSA構造の初期化に成功したときに返されます。
    \return BAD_FUNC_ARGS  RSAキーポインタがNULLに評価された場合に返されます
    \return BUFFER_E  LENがRSA_MAX_ID_LENよりも小さい場合、または大きい場合は返されます。
    \param key  初期化するRsaKey構造体へのポインタ
    \param id  デバイス上のキーの識別子
    \param len  バイト数の識別子の長さ
    \param heap  メモリオーバーライドで使用するためのヒープ識別子へのポインタ。メモリ割り当てのカスタム処理を可能にします。このヒープは、このRSAオブジェクトで使用するためにメモリを割り当てるときに使用されるデフォルトになります。
    _Example_
    \code
    RsaKey enc;
    unsigned char* id = (unsigned char*)"RSA2048";
    int len = 6;
    int devId = 1;
    int ret;
    ret = wc_CryptoDev_RegisterDevice(devId, wc_Pkcs11_CryptoDevCb,
                                      &token);
    if ( ret != 0) {
        // error associating callback and token with device id
    }
    ret = wc_InitRsaKey_Id(&enc, id, len, NULL, devId); // not using heap hint
    if ( ret != 0 ) {
        // error initializing RSA key
    }
    \endcode
    \sa wc_InitRsaKey
    \sa wc_RsaInitCavium
    \sa wc_FreeRsaKey
    \sa wc_RsaSetRNG
*/
int  wc_InitRsaKey_Id(RsaKey* key, unsigned char* id, int len,
        void* heap, int devId);

/*!
    \ingroup RSA
    \brief  この関数はRNGをキーに関連付けます。WC_RSA_BLINDINGが有効になっている場合は必要です。
    \return 0  成功に戻った
    \return BAD_FUNC_ARGS  RSAキーの場合、RNGポインタがNULLに評価された場合
    \param key  関連付けられるRsaKey構造体へのポインタ
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    \endcode
    \sa wc_InitRsaKey
    \sa wc_RsaSetRNG
*/
int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng);

/*!
    \ingroup RSA
    \brief  この関数は、MP_Clearを使用して提供されたRsaKey構造体を解放します。
    \return 0  キーの解放に成功したら返品されます
    _Example_
    \code
    RsaKey enc;
    wc_InitRsaKey(&enc, NULL); // not using heap hint. No custom memory
    ... set key, do encryption

    wc_FreeRsaKey(&enc);
    \endcode
    \sa wc_InitRsaKey
*/
int  wc_FreeRsaKey(RsaKey* key);

/*!
    \ingroup RSA
    \brief  この関数はメッセージをINから暗号化し、その結果を格納します。初期化された公開鍵と乱数発生器が必要です。副作用として、この関数はounlenの中で書き込まれたバイトを返します。
    \return Success  入力メッセージの暗号化に成功したら、成功の場合は0を返し、障害の場合はゼロ未満です。また、outlenの値を格納することによって、OUTに書き込まれた数のバイト数を返します。
    \return BAD_FUNC_ARG  入力パラメータのいずれかが無効な場合に返されます
    \return RSA_BUFFER_E  CipherTextを保存するには、出力バッファが小さすぎる場合は返されます。
    \return RNG_FAILURE_E  提供されたRNG構造体を使用してランダムブロックを生成するエラーがある場合
    \return MP_INIT_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_READ_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_CMP_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_INVMOD_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_EXPTMOD_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_MOD_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_MUL_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_ADD_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_MULMOD_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_TO_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_MEM  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_ZERO_E  メッセージの暗号化中に使用されている数学ライブラリにエラーがある場合に返される可能性があります。
    \param in  暗号化する入力メッセージを含むバッファへのポインタ
    \param inLen  暗号化するメッセージの長さ
    \param out  出力暗号文を保存するバッファへのポインタ
    \param outLen  出力バッファの長さ
    \param key  暗号化に使用する公開鍵を含むRsaKey構造体へのポインタ
    _Example_
    \code
    RsaKey pub;
    int ret = 0;
    byte n[] = { // initialize with received n component of public key };
    byte e[] = { // initialize with received e component of public key };
    byte msg[] = { // initialize with plaintext of message to encrypt };
    byte cipher[256]; // 256 bytes is large enough to store 2048 bit RSA
    ciphertext

    wc_InitRsaKey(&pub, NULL); // not using heap hint. No custom memory
    wc_RsaPublicKeyDecodeRaw(n, sizeof(n), e, sizeof(e), &pub);
    // initialize with received public key parameters
    ret = wc_RsaPublicEncrypt(msg, sizeof(msg), out, sizeof(out), &pub, &rng);
    if ( ret != 0 ) {
    	// error encrypting message
    }
    \endcode
    \sa wc_RsaPrivateDecrypt
*/
int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng);

/*!
    \ingroup RSA
    \brief  この関数は復号化のためにWC_RSAPrivateCrypt関数によって利用されます。
    \return Success  復号化データの長さ
    \return RSA_PAD_E  RSAUNPADエラー、フォーマットの悪いフォーマット
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する復号化データのバイト配列。
    _Example_
    \code
    none
    \endcode
    \sa wc_RsaPrivateDecrypt
*/
int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key);

/*!
    \ingroup RSA
    \brief  この関数は秘密のRSA復号化を提供します。
    \return Success  復号化データの長さ
    \return MEMORY_E  -125、メモリエラーが発生しました
    \return BAD_FUNC_ARG  -173、関数の不良引数が提供されています
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する復号化データのバイト配列。
    \param outLen  の長さ
    _Example_
    \code
    ret = wc_RsaPublicEncrypt(in, inLen, out, sizeof(out), &key, &rng);
    if (ret < 0) {
        return -1;
    }
    ret = wc_RsaPrivateDecrypt(out, ret, plain, sizeof(plain), &key);
    if (ret < 0) {
        return -1;
    }
    \endcode
    \sa RsaUnPad
    \sa wc_RsaFunction
    \sa wc_RsaPrivateDecryptInline
*/
int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key);

/*!
    \ingroup RSA
    \brief  提供された配列に秘密鍵と署名します。
    \return RSA_BUFFER_E:  -131、RSAバッファエラー、出力が小さすぎたり入力が大きすぎたりする
    \param in  暗号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する暗号化データのバイト配列。
    \param outLen  の長さ
    \param key  暗号化に使用する鍵。
    _Example_
    \code
    ret = wc_RsaSSL_Sign(in, inLen, out, sizeof(out), &key, &rng);
    if (ret < 0) {
        return -1;
    }
    memset(plain, 0, sizeof(plain));
    ret = wc_RsaSSL_Verify(out, ret, plain, sizeof(plain), &key);
    if (ret < 0) {
        return -1;
    }
    \endcode
    \sa wc_RsaPad
*/
int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng);

/*!
    \ingroup RSA
    \brief  メッセージがRSAキーによって署名されたことを確認するために使用されます。出力は入力と同じバイト配列を使用します。
    \return >0  テキストの長さ
    \return <0  エラーが発生しました。
    \param in  復号化されるバイト配列。
    \param inLen  バッファ入力の長さ。
    \param out  復号化された情報のポインタへのポインタ。
    _Example_
    \code
    RsaKey key;
    WC_RNG rng;
    int ret = 0;
    long e = 65537; // standard value to use for exponent
    wc_InitRsaKey(&key, NULL); // not using heap hint. No custom memory
    wc_InitRng(&rng);
    wc_MakeRsaKey(&key, 2048, e, &rng);

    byte in[] = { // Initialize with some RSA encrypted information }
    byte* out;
    if(wc_RsaSSL_VerifyInline(in, sizeof(in), &out, &key) < 0)
    {
        // handle error
    }
    \endcode
    \sa wc_RsaSSL_Verify
    \sa wc_RsaSSL_Sign
*/
int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);

/*!
    \ingroup RSA
    \brief  メッセージがキーによって署名されたことを確認するために使用されます。
    \return Success  エラーのないテキストの長さ。
    \return MEMORY_E  メモリ例外
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する復号化データのバイト配列。
    \param outLen  の長さ
    _Example_
    \code
    ret = wc_RsaSSL_Sign(in, inLen, out, sizeof(out), &key, &rng);
    if (ret < 0) {
        return -1;
    }
    memset(plain, 0, sizeof(plain));
    ret = wc_RsaSSL_Verify(out, ret, plain, sizeof(plain), &key);
    if (ret < 0) {
        return -1;
    }
    \endcode
    \sa wc_RsaSSL_Sign
*/
int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);

/*!
    \ingroup RSA
    \brief  提供された配列に秘密鍵と署名します。
    \return RSA_BUFFER_E:  -131、RSAバッファエラー、出力が小さすぎたり入力が大きすぎたりする
    \param in  暗号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する暗号化データのバイト配列。
    \param outLen  の長さ
    \param hash  メッセージに入るハッシュ型
    \param mgf  マスク生成機能識別子
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;

    ret = wc_RsaPSS_Sign((byte*)szMessage, (word32)XSTRLEN(szMessage)+1,
            pSignature, sizeof(pSignature),
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
    if (ret > 0 ){
        sz = ret;
    } else return -1;

    ret = wc_RsaPSS_Verify(pSignature, sz, pt, outLen,
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
    if (ret <= 0)return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Verify
    \sa wc_RsaSetRNG
*/
int  wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out,
                                word32 outLen, enum wc_HashType hash, int mgf,
                                RsaKey* key, WC_RNG* rng);

/*!
    \ingroup RSA
    \brief  入力署名を復号して、メッセージが鍵によって署名されたことを確認します。WC_RSA_BLINDINGが有効な場合、鍵はwc_RsaSetRNGによってRNGに関連付けられなければなりません。
    \return Success  エラーのない場合はテキストの長さを返します
    \return MEMORY_E  メモリ例外
    \return MP_EXPTMOD_E - fastmathを使用する様に構成されている場合にFP_MAX_BITSが鍵サイズの少なくとも2倍に設定されていない(例えば4096-bit長の鍵を使用する場合にはFP_MAX_BITSは8192以上に設定すること)。
    \param in  復号される署名データが格納されているバッファ
    \param inLen  署名データの長さ
    \param out  復号データの出力先バッファ
    \param outLen  出力先バッファサイズ
    \param hash  メッセージに入るハッシュ型
    \param mgf  マスク生成機能識別子
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;
    ret = wc_RsaPSS_Sign((byte*)szMessage, (word32)XSTRLEN(szMessage)+1,
            pSignature, sizeof(pSignature),
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
    if (ret > 0 ){
        sz = ret;
    } else return -1;

    ret = wc_RsaPSS_Verify(pSignature, sz, pt, outLen,
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
    if (ret <= 0)return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_VerifyInline
    \sa wc_RsaPSS_CheckPadding
    \sa wc_RsaSetRNG
*/
int  wc_RsaPSS_Verify(byte* in, word32 inLen, byte* out,
                                  word32 outLen, enum wc_HashType hash, int mgf,
                                  RsaKey* key);

/*!
    \ingroup RSA
    \brief  入力署名を復号化して、メッセージがRSAキーによって署名されたことを確認します。出力は入力と同じバイト配列を使用します。WC_RSA_BLINDINGが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return >0  テキストの長さ
    \return <0  エラーが発生しました。
    \param in  復号化されるバイト配列。
    \param inLen  バッファ入力の長さ。
    \param out  PSSデータを含むアドレスへのポインタ。
    \param hash  メッセージに入るハッシュ型
    \param mgf  マスク生成機能識別子
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;
    ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, pSignatureSz,
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
    if (ret > 0 ){
        sz = ret;
    } else return -1;

    ret = wc_RsaPSS_VerifyInline(pSignature, sz, pt,
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
    if (ret <= 0)return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_VerifyCheck
    \sa wc_RsaPSS_VerifyCheck_ex
    \sa wc_RsaPSS_VerifyCheckInline
    \sa wc_RsaPSS_VerifyCheckInline_ex
    \sa wc_RsaPSS_CheckPadding
    \sa wc_RsaPSS_CheckPadding_ex
    \sa wc_RsaSetRNG
*/


int  wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                                        enum wc_HashType hash, int mgf,
                                        RsaKey* key);
/*!
    \ingroup RSA
    \brief  RSA-PSSで署名されたメッセージを確認してください。塩の長さはハッシュ長に等しい。WC_RSA_BLINDINGが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return the  PSSデータの長さが成功し、負に障害が発生します。
    \return MEMORY_E  メモリ例外
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  PSSデータを含むアドレスへのポインタ。
    \param outLen  の長さ
    \param digest  検証中のデータのハッシュ。
    \param digestLen  ハッシュの長さ
    \param hash  ハッシュアルゴリズム
    \param mgf  マスク生成機能
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;

    if (ret == 0) {
        digestSz = wc_HashGetDigestSize(WC_HASH_TYPE_SHA256);
        ret = wc_Hash(WC_HASH_TYPE_SHA256, message, sz, digest, digestSz);
    } else return -1;

    if (ret == 0) {
        ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, pSignatureSz,
                WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
        if (ret > 0 ){
            sz = ret;
        } else return -1;
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaPSS_VerifyCheck(pSignature, sz, pt, outLen,
                digest, digestSz, WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
            if (ret <= 0) return -1;
    } else return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_VerifyCheck_ex
    \sa wc_RsaPSS_VerifyCheckInline
    \sa wc_RsaPSS_VerifyCheckInline_ex
    \sa wc_RsaPSS_CheckPadding
    \sa wc_RsaPSS_CheckPadding_ex
    \sa wc_RsaSetRNG
*/

int  wc_RsaPSS_VerifyCheck(byte* in, word32 inLen,
                               byte* out, word32 outLen,
                               const byte* digest, word32 digestLen,
                               enum wc_HashType hash, int mgf,
                               RsaKey* key);
/*!
    \ingroup RSA
    \brief  RSA-PSSで署名されたメッセージを確認してください。WC_RSA_BLINDINGが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return the  PSSデータの長さが成功し、負に障害が発生します。
    \return MEMORY_E  メモリ例外
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  PSSデータを含むアドレスへのポインタ。
    \param outLen  の長さ
    \param digest  検証中のデータのハッシュ。
    \param digestLen  ハッシュの長さ
    \param hash  ハッシュアルゴリズム
    \param mgf  マスク生成機能
    \param saltLen  使用される塩の長さ。RSA_PSSS_SALT_LEN_DEFAULT（-1）塩の長さはハッシュ長と同じです。RSA_PSS_SALT_LEN_DISCOVERは、塩の長さがデータから決定されます。
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;

    if (ret == 0) {
        digestSz = wc_HashGetDigestSize(WC_HASH_TYPE_SHA256);
        ret = wc_Hash(WC_HASH_TYPE_SHA256, message, sz, digest, digestSz);
    } else return -1;

    if (ret == 0) {
        ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, pSignatureSz,
                WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
        if (ret > 0 ){
            sz = ret;
        } else return -1;
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaPSS_VerifyCheck_ex(pSignature, sz, pt, outLen,
                digest, digestSz, WC_HASH_TYPE_SHA256, WC_MGF1SHA256, saltLen, &key);
            if (ret <= 0) return -1;
    } else return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_VerifyCheck
    \sa wc_RsaPSS_VerifyCheckInline
    \sa wc_RsaPSS_VerifyCheckInline_ex
    \sa wc_RsaPSS_CheckPadding
    \sa wc_RsaPSS_CheckPadding_ex
    \sa wc_RsaSetRNG
*/
int  wc_RsaPSS_VerifyCheck_ex(byte* in, word32 inLen,
                               byte* out, word32 outLen,
                               const byte* digest, word32 digestLen,
                               enum wc_HashType hash, int mgf, int saltLen,
                               RsaKey* key);

/*!
    \ingroup RSA
    \brief  RSA-PSSで署名されたメッセージを確認してください。入力バッファは出力バッファに再利用されます。塩の長さはハッシュ長に等しい。WC_RSA_BLINDINGが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return the  PSSデータの長さが成功し、負に障害が発生します。
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する復号化データのバイト配列。
    \param digest  検証中のデータのハッシュ。
    \param digestLen  ハッシュの長さ
    \param hash  メッセージに入るハッシュ型
    \param mgf  マスク生成機能識別子
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;

    if (ret == 0) {
        digestSz = wc_HashGetDigestSize(WC_HASH_TYPE_SHA256);
        ret = wc_Hash(WC_HASH_TYPE_SHA256, message, sz, digest, digestSz);
    } else return -1;

    if (ret == 0) {
        ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, pSignatureSz,
                WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
        if (ret > 0 ){
            sz = ret;
        } else return -1;
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaPSS_VerifyCheckInline(pSignature, sz, pt,
                digest, digestSz, WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
            if (ret <= 0) return -1;
    } else return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_VerifyCheck
    \sa wc_RsaPSS_VerifyCheck_ex
    \sa wc_RsaPSS_VerifyCheckInline_ex
    \sa wc_RsaPSS_CheckPadding
    \sa wc_RsaPSS_CheckPadding_ex
    \sa wc_RsaSetRNG
*/
int  wc_RsaPSS_VerifyCheckInline(byte* in, word32 inLen, byte** out,
                               const byte* digest, word32 digentLen,
                               enum wc_HashType hash, int mgf,
                               RsaKey* key);
/*!
    \ingroup RSA
    \brief  RSA-PSSで署名されたメッセージを確認してください。入力バッファは出力バッファに再利用されます。WC_RSA_BLINDINGが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return the  PSSデータの長さが成功し、負に障害が発生します。
    \param in  復号化されるバイト配列。
    \param inLen  の長さ
    \param out  格納する復号化データのバイト配列。
    \param digest  検証中のデータのハッシュ。
    \param digestLen  ハッシュの長さ
    \param hash  メッセージに入るハッシュ型
    \param mgf  マスク生成機能識別子
    \param saltLen  使用される塩の長さ。RSA_PSSS_SALT_LEN_DEFAULT（-1）塩の長さはハッシュ長と同じです。RSA_PSS_SALT_LEN_DISCOVERは、塩の長さがデータから決定されます。
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;

    if (ret == 0) {
        digestSz = wc_HashGetDigestSize(WC_HASH_TYPE_SHA256);
        ret = wc_Hash(WC_HASH_TYPE_SHA256, message, sz, digest, digestSz);
    } else return -1;

    if (ret == 0) {
        ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, pSignatureSz,
                WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
        if (ret > 0 ){
            sz = ret;
        } else return -1;
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaPSS_VerifyCheckInline_ex(pSignature, sz, pt,
                digest, digestSz, WC_HASH_TYPE_SHA256, WC_MGF1SHA256, saltLen, &key);
            if (ret <= 0) return -1;
    } else return -1;

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_VerifyCheck
    \sa wc_RsaPSS_VerifyCheck_ex
    \sa wc_RsaPSS_VerifyCheckInline
    \sa wc_RsaPSS_CheckPadding
    \sa wc_RsaPSS_CheckPadding_ex
    \sa wc_RsaSetRNG
*/
int  wc_RsaPSS_VerifyCheckInline_ex(byte* in, word32 inLen, byte** out,
                               const byte* digest, word32 digentLen,
                               enum wc_HashType hash, int mgf, int saltLen,
                               RsaKey* key);

/*!
    \ingroup RSA
    \brief  PSSデータを確認して、署名が一致するようにします。塩の長さはハッシュ長に等しい。WC_RSA_BLINDINGが有効な場合、キーはWC_RSASETRNGによってRNGに関連付けられなければなりません。
    \return BAD_PADDING_E  PSSデータが無効な場合、NULLがINまたはSIGまたはINSZに渡されると、BAD_FUNC_ARGはハッシュアルゴリズムの長さと同じではありません。
    \return MEMORY_E  メモリ例外
    \param in  検証中のデータのハッシュ。
    \param inSz  ハッシュの長さ
    \param sig  PSSデータを保持するバッファ。
    \param sigSz  PSSデータのサイズ。
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;
    if (ret == 0) {
        digestSz = wc_HashGetDigestSize(WC_HASH_TYPE_SHA256);
        ret = wc_Hash(WC_HASH_TYPE_SHA256, message, sz, digest, digestSz);
    } else return -1;
    ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, sizeof(pSignature),
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
    if (ret > 0 ){
        sz = ret;
    } else return -1;

    verify = wc_RsaPSS_Verify(pSignature, sz, out, outLen,
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
    if (verify <= 0)return -1;

    ret = wc_RsaPSS_CheckPadding(digest, digestSz, out, verify, hash);

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_VerifyInline
    \sa wc_RsaPSS_VerifyCheck
    \sa wc_RsaPSS_VerifyCheck_ex
    \sa wc_RsaPSS_VerifyCheckInline
    \sa wc_RsaPSS_VerifyCheckInline_ex
    \sa wc_RsaPSS_CheckPadding_ex
    \sa wc_RsaSetRNG
*/
int  wc_RsaPSS_CheckPadding(const byte* in, word32 inLen, byte* sig,
                                        word32 sigSz,
                                        enum wc_HashType hashType);
/*!
    \ingroup RSA
    \brief  PSSデータを確認して、署名が一致するようにします。塩の長さはハッシュ長に等しい。
    \return BAD_PADDING_E  PSSデータが無効な場合、NULLがINまたはSIGまたはINSZに渡されると、BAD_FUNC_ARGはハッシュアルゴリズムの長さと同じではありません。
    \return MEMORY_E  メモリ例外
    \param in  検証中のデータのハッシュ。
    \param inSz  ハッシュの長さ
    \param sig  PSSデータを保持するバッファ。
    \param sigSz  PSSデータのサイズ。
    \param hashType  ハッシュアルゴリズム
    \param saltLen  使用される塩の長さ。RSA_PSSS_SALT_LEN_DEFAULT（-1）塩の長さはハッシュ長と同じです。RSA_PSS_SALT_LEN_DISCOVERは、塩の長さがデータから決定されます。
    _Example_
    \code
    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    } else return -1;
    if (ret == 0) {
        ret = wc_RsaSetRNG(&key, &rng);
    } else return -1;
    if (ret == 0) {
            ret = wc_MakeRsaKey(&key, 2048, WC_RSA_EXPONENT, &rng);
    } else return -1;
    if (ret == 0) {
        digestSz = wc_HashGetDigestSize(WC_HASH_TYPE_SHA256);
        ret = wc_Hash(WC_HASH_TYPE_SHA256, message, sz, digest, digestSz);
    } else return -1;
    ret = wc_RsaPSS_Sign(digest, digestSz, pSignature, sizeof(pSignature),
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key, &rng);
    if (ret > 0 ){
        sz = ret;
    } else return -1;

    verify = wc_RsaPSS_Verify(pSignature, sz, out, outLen,
            WC_HASH_TYPE_SHA256, WC_MGF1SHA256, &key);
    if (verify <= 0)return -1;

    ret = wc_RsaPSS_CheckPadding_ex(digest, digestSz, out, verify, hash, saltLen, 0);

    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    \endcode
    \sa wc_RsaPSS_Sign
    \sa wc_RsaPSS_Verify
    \sa wc_RsaPSS_VerifyInline
    \sa wc_RsaPSS_VerifyCheck
    \sa wc_RsaPSS_VerifyCheck_ex
    \sa wc_RsaPSS_VerifyCheckInline
    \sa wc_RsaPSS_VerifyCheckInline_ex
    \sa wc_RsaPSS_CheckPadding
*/
int  wc_RsaPSS_CheckPadding_ex(const byte* in, word32 inLen, byte* sig,
                word32 sigSz, enum wc_HashType hashType, int saltLen, int bits);
/*!
    \ingroup RSA
    \brief  提供されたキー構造の暗号化サイズを返します。
    \return Success  提供されたキー構造の暗号化サイズ。
    _Example_
    \code
    int sz = wc_RsaEncryptSize(&key);
    \endcode
    \sa wc_InitRsaKey
    \sa wc_InitRsaKey_ex
    \sa wc_MakeRsaKey
*/
int  wc_RsaEncryptSize(RsaKey* key);

/*!
    \ingroup RSA
    \brief  この関数はDerフォーマットされたRSA秘密鍵を解析し、秘密鍵を抽出し、それを与えられたResakey構造に格納します。IDXに解析された距離も設定します。
    \return 0  DERエンコード入力から秘密鍵の解析に成功したときに返されます
    \return ASN_PARSE_E  入力バッファから秘密鍵を解析するエラーがある場合に返されます。これは、入力秘密鍵がASN.1規格に従って正しくフォーマットされていない場合に発生する可能性があります。
    \return ASN_RSA_KEY_E  RSAキー入力の秘密鍵要素を読み取るエラーがある場合
    \param input  デコードするDERフォーマット秘密鍵を含むバッファへのポインタ
    \param inOutIdx  キーが始まるバッファ内のインデックスへのポインタ（通常は0）。この関数の副作用として、InoutIDXは入力バッファを介して解析された距離を記憶します
    \param key  デコードされた秘密鍵を保存するRSAKEY構造へのポインタ
    _Example_
    \code
    RsaKey enc;
    word32 idx = 0;
    int ret = 0;
    byte der[] = { // initialize with DER-encoded RSA private key };

    wc_InitRsaKey(&enc, NULL); // not using heap hint. No custom memory
    ret = wc_RsaPrivateKeyDecode(der, &idx, &enc, sizeof(der));
    if( ret != 0 ) {
    	// error parsing private key
    }
    \endcode
    \sa wc_RsaPublicKeyDecode
    \sa wc_MakeRsaKey
*/
int  wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                            RsaKey* key, word32 inSz);

/*!
    \ingroup RSA
    \brief  この関数はDerフォーマットのRSA公開鍵を解析し、公開鍵を抽出し、それを指定されたRsaKey構造体に格納します。IDXに解析された距離も設定します。
    \return 0  DERエンコード入力から公開鍵の解析に成功したときに返された
    \return ASN_PARSE_E  入力バッファから公開鍵を解析したエラーがある場合に返されます。これは、入力公開鍵がASN.1規格に従って正しくフォーマットされていない場合に発生する可能性があります。
    \return ASN_OBJECT_ID_E  ASN.1オブジェクトIDがRSA公開鍵のそれと一致しない場合に返されます。
    \return ASN_EXPECT_0_E  入力キーがASN.1規格に従って正しくフォーマットされていない場合
    \return ASN_BITSTR_E  入力キーがASN.1規格に従って正しくフォーマットされていない場合
    \return ASN_RSA_KEY_E  RSAキー入力の公開鍵要素を読み取るエラーがある場合
    \param input  復号する入力DERエンコードRSA公開鍵を含むバッファへのポインタ
    \param inOutIdx  キーが始まるバッファ内のインデックスへのポインタ（通常は0）。この関数の副作用として、InoutIDXは入力バッファを介して解析された距離を記憶します
    \param key  デコードされた公開鍵を保存するRsaKey構造体へのポインタ
    _Example_
    \code
    RsaKey pub;
    word32 idx = 0;
    int ret = 0;
    byte der[] = { // initialize with DER-encoded RSA public key };

    wc_InitRsaKey(&pub, NULL); // not using heap hint. No custom memory
    ret = wc_RsaPublicKeyDecode(der, &idx, &pub, sizeof(der));
    if( ret != 0 ) {
    	// error parsing public key
    }
    \endcode
    \sa wc_RsaPublicKeyDecodeRaw
*/
int  wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                           RsaKey* key, word32 inSz);

/*!
    \ingroup RSA
    \brief  この関数は、公開弾性率（n）と指数（e）を撮影して、RSA公開鍵の生の要素を復号します。これらの生の要素を提供されたRsaKey構造体に格納し、暗号化/復号化プロセスで使用することができます。
    \return 0  公開鍵の生の要素をRsaKey構造体に復号したときに返された
    \return BAD_FUNC_ARG  いずれかの入力引数がNULLに評価された場合に返されます。
    \return MP_INIT_E  複数の精密整数（MP_INT）ライブラリで使用するための整数の初期化中にエラーがある場合
    \return ASN_GETINT_E  提供されたRSAキー要素、nまたはeのいずれかを読むエラーがある場合に返されます
    \param n  Public RSAキーのRAWモジュラスパラメータを含むバッファへのポインタ
    \param nSz  Nを含むバッファのサイズ
    \param e  Public RSAキーのRAW指数パラメータを含むバッファへのポインタ
    \param eSz  Eを含むバッファのサイズ
    _Example_
    \code
    RsaKey pub;
    int ret = 0;
    byte n[] = { // initialize with received n component of public key };
    byte e[] = { // initialize with received e component of public key };

    wc_InitRsaKey(&pub, NULL); // not using heap hint. No custom memory
    ret = wc_RsaPublicKeyDecodeRaw(n, sizeof(n), e, sizeof(e), &pub);
    if( ret != 0 ) {
    	// error parsing public key elements
    }
    \endcode
    \sa wc_RsaPublicKeyDecode
*/
int  wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz,
                                        const byte* e, word32 eSz, RsaKey* key);

/*!
    \ingroup RSA
    \brief  この機能はRSAKEYキーをDERフォーマットに変換します。結果は出力に書き込まれ、書き込まれたバイト数を返します。
    \return 0  成功
    \return BAD_FUNC_ARG  キーまたは出力がNULLの場合、またはキー - >タイプがRSA_PRIVATEでない場合、またはINLENが出力バッファに十分な大きさでない場合は返されます。
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます。
    \param key  初期化されたRsaKey構造体
    \param output  出力バッファへのポインタ。
    _Example_
    \code
    byte* der;
    // Allocate memory for der
    int derSz = // Amount of memory allocated for der;
    RsaKey key;
    WC_RNG rng;
    long e = 65537; // standard value to use for exponent
    ret = wc_MakeRsaKey(&key, 2048, e, &rng); // generate 2048 bit long
    private key
    wc_InitRsaKey(&key, NULL);
    wc_InitRng(&rng);
    if(wc_RsaKeyToDer(&key, der, derSz) != 0)
    {
        // Handle the error thrown
    }
    \endcode
    \sa wc_RsaKeyToPublicDer
    \sa wc_InitRsaKey
    \sa wc_MakeRsaKey
    \sa wc_InitRng
*/
int wc_RsaKeyToDer(RsaKey* key, byte* output, word32 inLen);

/*!
    \ingroup RSA
    \brief  この機能は、どのパディングを使用するかを選択しながらRSA暗号化を実行します。
    \return size  正常に暗号化されていると、暗号化されたバッファのサイズが返されます
    \return RSA_BUFFER_E  RSAバッファエラー、出力が小さすぎたり入力が大きすぎたりする
    \param in  暗号化のためのバッファへのポインタ
    \param inLen  暗号化するバッファの長さ
    \param out  暗号化されたMSGが作成されました
    \param outLen  暗号化されたMSGを保持するために利用可能なバッファの長さ
    \param key  初期化ずみRsaKey構造体
    \param rng  初期化されたWC_RNG構造体
    \param type  使用するパディングの種類（WC_RSA_OAEP_PADまたはWC_RSA_PKCSV15_PAD）
    \param hash  使用するハッシュの種類（選択はhash.hにあります）
    \param mgf  使用するマスク生成機能の種類
    \param label  暗号化されたメッセージに関連付けるオプションのラベル
    _Example_
    \code
    WC_RNG rng;
    RsaKey key;
    byte in[] = “I use Turing Machines to ask questions”
    byte out[256];
    int ret;
    …

    ret = wc_RsaPublicEncrypt_ex(in, sizeof(in), out, sizeof(out), &key, &rng,
    WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA, WC_MGF1SHA1, NULL, 0);
    if (ret < 0) {
	    //handle error
    }
    \endcode
    \sa wc_RsaPublicEncrypt
    \sa wc_RsaPrivateDecrypt_ex
*/
int  wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                   word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 labelSz);

/*!
    \ingroup RSA
    \brief  この関数はRSAを使用してメッセージを復号化し、どのパディングタイプのオプションを指定します。
    \return size  復号化が成功すると、復号化されたメッセージのサイズが返されます。
    \return MEMORY_E  必要な配列をMallocにMallocにするのに十分なメモリがない場合は返されます。
    \return BAD_FUNC_ARG  関数に渡された引数が渡された場合に返されます。
    \param in  復号化のためのバッファへのポインタ
    \param inLen  復号化するバッファの長さ
    \param out  復号化されたMSGが作成されました
    \param outLen  復号化されたMSGを保持するために利用可能なバッファの長さ
    \param key  初期化ずみRsaKey構造体
    \param type  使用するパディングの種類（WC_RSA_OAEP_PADまたはWC_RSA_PKCSV15_PAD）
    \param hash  使用するハッシュの種類（選択はhash.hにあります）
    \param mgf  使用するマスク生成機能の種類
    \param label  暗号化されたメッセージに関連付けるオプションのラベル
    _Example_
    \code
    WC_RNG rng;
    RsaKey key;
    byte in[] = “I use Turing Machines to ask questions”
    byte out[256];
    byte plain[256];
    int ret;
    …
    ret = wc_RsaPublicEncrypt_ex(in, sizeof(in), out, sizeof(out), &key,
    &rng, WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA, WC_MGF1SHA1, NULL, 0);
    if (ret < 0) {
	    //handle error
    }
    …
    ret = wc_RsaPrivateDecrypt_ex(out, ret, plain, sizeof(plain), &key,
    WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA, WC_MGF1SHA1, NULL, 0);

    if (ret < 0) {
	    //handle error
    }
    \endcode
    \sa none
*/
int  wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen,
                   byte* out, word32 outLen, RsaKey* key, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 labelSz);

/*!
    \ingroup RSA
    \brief  この関数はRSAを使用してメッセージをインラインで復号化し、どのパディングタイプのオプションを示します。INバッファには、呼び出された後に復号化されたメッセージが含まれ、アウトバイトポインタはプレーンテキストがある「IN」バッファ内の場所を指します。
    \return size  復号化が成功すると、復号化されたメッセージのサイズが返されます。
    \return MEMORY_E:  必要な配列をMallocにMallocにするのに十分なメモリがない場合は返されます。
    \return RSA_PAD_E:  パディングのエラーが発生した場合に返されます。
    \return BAD_PADDING_E:  過去のパディングの解析中にエラーが発生した場合に返されます。
    \return BAD_FUNC_ARG:  関数に渡された引数が渡された場合に返されます。
    \param in  復号化のためのバッファへのポインタ
    \param inLen  復号化するバッファの長さ
    \param out  "in"バッファの復号化されたメッセージの位置へのポインタ
    \param key  初期化ずみRsaKey構造体
    \param type  使用するパディングの種類（WC_RSA_OAEP_PADまたはWC_RSA_PKCSV15_PAD）
    \param hash  使用するハッシュの種類（選択はhash.hにあります）
    \param mgf  使用するマスク生成機能の種類
    \param label  暗号化されたメッセージに関連付けるオプションのラベル
    _Example_
    \code
    WC_RNG rng;
    RsaKey key;
    byte in[] = “I use Turing Machines to ask questions”
    byte out[256];
    byte* plain;
    int ret;
    …
    ret = wc_RsaPublicEncrypt_ex(in, sizeof(in), out, sizeof(out), &key,
    &rng, WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA, WC_MGF1SHA1, NULL, 0);

    if (ret < 0) {
	    //handle error
    }
    …
    ret = wc_RsaPrivateDecryptInline_ex(out, ret, &plain, &key,
    WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA, WC_MGF1SHA1, NULL, 0);

    if (ret < 0) {
	    //handle error
    }
    \endcode
    \sa none
*/
int  wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen,
                      byte** out, RsaKey* key, int type, enum wc_HashType hash,
                      int mgf, byte* label, word32 labelSz);

/*!
    \ingroup RSA
    \brief  RSAアルゴリズムに使用されるRsaKey構造体の個々の要素（E、N）をバッファに取り出します。
    \return 0  関数が正常に実行された場合は、エラーなしで返されます。
    \return BAD_FUNC_ARG:  いずれかのパラメータがNULL値で渡された場合に返されます。
    \return RSA_BUFFER_E:  渡されたeまたはnバッファが正しいサイズではない場合に返されます。
    \return MP_MEM:  内部関数にメモリエラーがある場合に返されます。
    \return MP_VAL:  内部関数引数が無効な場合に返されます。
    \param key  検証に使用する鍵。
    \param e  eの値のバッファー。eはRSAモジュラ演算での大きな正の整数です。
    \param eSz  eバッファのサイズ。
    \param n  nの値のバッファー。NはRSAモジュラー演算では大きな正の整数です。
    _Example_
    \code
    Rsa key; // A valid RSA key.
    byte e[ buffer sz E.g. 256 ];
    byte n[256];
    int ret;
    word32 eSz = sizeof(e);
    word32 nSz = sizeof(n);
    ...
    ret = wc_RsaFlattenPublicKey(&key, e, &eSz, n, &nSz);
    if (ret != 0) {
    	// Failure case.
    }
    \endcode
    \sa wc_InitRsaKey
    \sa wc_InitRsaKey_ex
    \sa wc_MakeRsaKey
*/
int  wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz, byte* n,
                            word32* nSz);

/*!
    \ingroup RSA
    \brief  RSA公開鍵をDERフォーマットに変換します。出力に書き込み、書き込まれたバイト数を返します。
    \return >0  成功、書かれたバイト数。
    \return BAD_FUNC_ARG  キーまたは出力がNULLの場合に返されます。
    \return MEMORY_E  エラー割り当てメモリが発生したときに返されます。
    \return <0  エラー
    \param key  変換するRsaKey構造体。
    \param output  保留された出力バッファー。（NULLが長さのみを返す場合）
    _Example_
    \code
    RsaKey key;

    wc_InitRsaKey(&key, NULL);
    // Use key

    const int BUFFER_SIZE = 1024; // Some adequate size for the buffer
    byte output[BUFFER_SIZE];
    if (wc_RsaKeyToPublicDer(&key, output, sizeof(output)) != 0) {
        // Handle Error
    }
    \endcode
    \sa wc_RsaPublicKeyDerSize
    \sa wc_RsaKeyToPublicDer_ex
    \sa wc_InitRsaKey
*/
int wc_RsaKeyToPublicDer(RsaKey* key, byte* output, word32 inLen);

/*!
    \ingroup RSA
    \brief  RSA公開鍵をDERフォーマットに変換します。出力に書き込み、書き込まれたバイト数を返します。with_headerが0の場合（seq + n + e）だけがASN.1 Derフォーマットで返され、ヘッダーを除外します。
    \return >0  成功、書かれたバイト数。
    \return BAD_FUNC_ARG  キーまたは出力がNULLの場合に返されます。
    \return MEMORY_E  エラー割り当てメモリが発生したときに返されます。
    \return <0  エラー
    \param key  変換するRsaKey構造体。
    \param output  保留された出力バッファー。（NULLが長さのみを返す場合）
    _Example_
    \code
    RsaKey key;

    wc_InitRsaKey(&key, NULL);
    // Use key

    const int BUFFER_SIZE = 1024; // Some adequate size for the buffer
    byte output[BUFFER_SIZE];
    if (wc_RsaKeyToPublicDer_ex(&key, output, sizeof(output), 0) != 0) {
        // Handle Error
    }
    \endcode
    \sa wc_RsaPublicKeyDerSize
    \sa wc_RsaKeyToPublicDer
    \sa wc_InitRsaKey
*/
int wc_RsaKeyToPublicDer_ex(RsaKey* key, byte* output, word32 inLen,
    int with_header);

/*!
    \ingroup RSA
    \brief  この関数は、長さサイズ（ビット単位）のRSA秘密鍵を生成し、指数（e）を指定します。次に、このキーを提供されたRsaKey構造体に格納するため、暗号化/復号化に使用できます。Eに使用するセキュア番号は65537です。サイズは、RSA_MIN_SIZEよりも大きく、RSA_MAX_SIZEよりも大きくなる必要があります。この機能が利用可能であるため、コンパイル時にオプションwolfssl_key_genを有効にする必要があります。これは、 - を使用してください./configureを使用する場合は、-enable-keygenで実現できます。
    \return 0  RSA秘密鍵の生成に成功したら返されました
    \return BAD_FUNC_ARG  入力引数のいずれかがNULLの場合、サイズパラメータは必要な範囲外にあるか、eが誤って選択されている場合
    \return RNG_FAILURE_E  提供されたRNG構造体を使用してランダムブロックを生成するエラーがある場合
    \return MP_INIT_E
    \return MP_READ_E  RSAキーの生成中に使用された数学ライブラリにエラーがある場合に返されたRSAキーの生成中に使用された数学ライブラリにエラーがある場合に返される可能性があります。
    \return MP_CMP_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_INVMOD_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_EXPTMOD_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_MOD_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_MUL_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_ADD_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_MULMOD_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_TO_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_MEM  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \return MP_ZERO_E  RSAキーの生成中に使用されている数学ライブラリにエラーがある場合は返される可能性があります。
    \param key  生成された秘密鍵を保存するRSAKEY構造体へのポインタ
    \param size  ビット単位の希望のキー長。rsa_min_sizeより大きく、rsa_max_sizeよりも大きくなる必要があります。
    \param e  キーを生成するために使用する指数パラメータ。安全な選択は65537です
    _Example_
    \code
    RsaKey priv;
    WC_RNG rng;
    int ret = 0;
    long e = 65537; // standard value to use for exponent

    wc_InitRsaKey(&priv, NULL); // not using heap hint. No custom memory
    wc_InitRng(&rng);
    // generate 2048 bit long private key
    ret = wc_MakeRsaKey(&priv, 2048, e, &rng);
    if( ret != 0 ) {
	    // error generating private key
    }
    \endcode
    \sa none
*/
int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);

/*!
    \ingroup RSA
    \brief  この関数は、ブロックされていないRSAコンテキストを設定します。RSANBコンテキストが設定されている場合、RSA関数を多くの小さな操作に分割する高速数学ベースの非ブロッキングEXPTMODが可能になります。wc_rsa_nonblockが定義されているときに有効になっています。
    \return 0  成功
    \return BAD_FUNC_ARG  キーまたはNBがNULLの場合に返されます。
    \param key  RSAキー構造
    _Example_
    \code
    int ret, count = 0;
    RsaKey key;
    RsaNb  nb;

    wc_InitRsaKey(&key, NULL);

    // Enable non-blocking RSA mode - provide context
    ret = wc_RsaSetNonBlock(key, &nb);
    if (ret != 0)
        return ret;

    do {
        ret = wc_RsaSSL_Sign(in, inLen, out, outSz, key, rng);
        count++; // track number of would blocks
        if (ret == FP_WOULDBLOCK) {
            // do "other" work here
        }
    } while (ret == FP_WOULDBLOCK);
    if (ret < 0) {
        return ret;
    }

    printf("RSA non-block sign: size %d, %d times\n", ret, count);
    \endcode
    \sa wc_RsaSetNonBlockTime
*/
int wc_RsaSetNonBlock(RsaKey* key, RsaNb* nb);

/*!
    \ingroup RSA
    \brief  この関数は最大ブロック時間の最大ブロック時間をマイクロ秒単位で設定します。それは、メガヘルツのCPU速度と共に事前計算されたテーブル（TFM.cexptModnbinstを参照）を使用して、提供された最大ブロック時間内に次の動作を完了できるかどうかを判断します。wc_rsa_nonblock_timeが定義されているときに有効になります。
    \return 0  成功
    \return BAD_FUNC_ARG  キーがNULLの場合、またはWC_RSASETNONBLOCKが以前に呼び出され、キー - > NBはNULLの場合に返されます。
    \param key  RsaKey構造体
    \param maxBlockUs  マイクロ秒をブロックする最大時間。
    _Example_
    \code
    RsaKey key;
    RsaNb  nb;

    wc_InitRsaKey(&key, NULL);
    wc_RsaSetNonBlock(key, &nb);
    wc_RsaSetNonBlockTime(&key, 4000, 160); // Block Max = 4 ms, CPU = 160MHz

    \endcode
    \sa wc_RsaSetNonBlock
*/
int wc_RsaSetNonBlockTime(RsaKey* key, word32 maxBlockUs,
    word32 cpuMHz);
