/*!
    \ingroup Camellia 
    \brief  この関数は、Camelliaオブジェクトのキーと初期化ベクトルを設定し、それを暗号として使用するために初期化します。
    \return 0  キーと初期化ベクトルを正常に設定すると返されます
    \return BAD_FUNC_ARG  入力引数の1つがエラー処理がある場合に返されます
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \param cam  キーとIVを設定する椿構造へのポインタ
    \param key  暗号化と復号化に使用する16,24、または32バイトのキーを含むバッファへのポインタ
    \param len  渡されたキーの長さ
    _Example_
    \code
    Camellia cam;
    byte key[32];
    // initialize key
    byte iv[16];
    // initialize iv
    if( wc_CamelliaSetKey(&cam, key, sizeof(key), iv) != 0) {
    	// error initializing camellia structure
    }
    \endcode
    \sa wc_CamelliaEncryptDirect
    \sa wc_CamelliaDecryptDirect
    \sa wc_CamelliaCbcEncrypt
    \sa wc_CamelliaCbcDecrypt
*/
int  wc_CamelliaSetKey(Camellia* cam,
                                   const byte* key, word32 len, const byte* iv);

/*!
    \ingroup Camellia 
    \brief  この関数は、Camelliaオブジェクトの初期化ベクトルを設定します。
    \return 0  キーと初期化ベクトルを正常に設定すると返されます
    \return BAD_FUNC_ARG  入力引数の1つがエラー処理がある場合に返されます
    \param cam  IVを設定する椿構造へのポインタ
    _Example_
    \code
    Camellia cam;
    byte iv[16];
    // initialize iv
    if( wc_CamelliaSetIV(&cam, iv) != 0) {
	// error initializing camellia structure
    }
    \endcode
    \sa wc_CamelliaSetKey
*/
int  wc_CamelliaSetIV(Camellia* cam, const byte* iv);

/*!
    \ingroup Camellia 
    \brief  この機能は、提供されたCamelliaオブジェクトを使用して1ブロック暗号化します。それはバッファーの最初の16バイトブロックを解析し、暗号化結果をバッファアウトに格納します。この機能を使用する前に、WC_CAMELLIASETKEYを使用してCamelliaオブジェクトを初期化する必要があります。
    \return none  いいえ返します。
    \param cam  暗号化に使用する椿構造へのポインタ
    \param out  暗号化されたブロックを保存するバッファへのポインタ
    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte plain[] = { // initialize with message to encrypt };
    byte cipher[16];

    wc_CamelliaEncryptDirect(&ca, cipher, plain);
    \endcode
    \sa wc_CamelliaDecryptDirect
*/
int  wc_CamelliaEncryptDirect(Camellia* cam, byte* out,
                                                                const byte* in);

/*!
    \ingroup Camellia 
    \brief  この機能は、提供されたCamelliaオブジェクトを使用して1ブロック復号化します。それはバッファ内の最初の16バイトブロックを解析し、それを復号化し、結果をバッファアウトに格納します。この機能を使用する前に、WC_CAMELLIASETKEYを使用してCamelliaオブジェクトを初期化する必要があります。
    \return none  いいえ返します。
    \param cam  暗号化に使用する椿構造へのポインタ
    \param out  復号化された平文ブロックを保存するバッファへのポインタ
    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte cipher[] = { // initialize with encrypted message to decrypt };
    byte decrypted[16];

    wc_CamelliaDecryptDirect(&cam, decrypted, cipher);
    \endcode
    \sa wc_CamelliaEncryptDirect
*/
int  wc_CamelliaDecryptDirect(Camellia* cam, byte* out,
                                                                const byte* in);

/*!
    \ingroup Camellia 
    \brief  この関数は、バッファーの平文を暗号化し、その出力をバッファOUTに格納します。暗号ブロックチェーン（CBC）を使用してCamelliaを使用してこの暗号化を実行します。
    \return none  いいえ返します。
    \param cam  暗号化に使用する椿構造へのポインタ
    \param out  暗号化された暗号文を保存するバッファへのポインタ
    \param in  暗号化する平文を含むバッファへのポインタ
    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte plain[] = { // initialize with encrypted message to decrypt };
    byte cipher[sizeof(plain)];

    wc_CamelliaCbcEncrypt(&cam, cipher, plain, sizeof(plain));
    \endcode
    \sa wc_CamelliaCbcDecrypt
*/
int wc_CamelliaCbcEncrypt(Camellia* cam,
                                          byte* out, const byte* in, word32 sz);

/*!
    \ingroup Camellia 
    \brief  この関数は、バッファ内の暗号文を復号化し、その出力をバッファOUTに格納します。暗号ブロックチェーン（CBC）を搭載したCamelliaを使用してこの復号化を実行します。
    \return none  いいえ返します。
    \param cam  暗号化に使用する椿構造へのポインタ
    \param out  復号化されたメッセージを保存するバッファへのポインタ
    \param in  暗号化された暗号文を含むバッファへのポインタ
    _Example_
    \code
    Camellia cam;
    // initialize cam structure with key and iv
    byte cipher[] = { // initialize with encrypted message to decrypt };
    byte decrypted[sizeof(cipher)];

    wc_CamelliaCbcDecrypt(&cam, decrypted, cipher, sizeof(cipher));
    \endcode
    \sa wc_CamelliaCbcEncrypt
*/
int wc_CamelliaCbcDecrypt(Camellia* cam,
                                          byte* out, const byte* in, word32 sz);
