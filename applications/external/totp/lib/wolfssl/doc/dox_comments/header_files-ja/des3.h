/*!
    \ingroup 3DES 
    \brief  この関数は、引数として与えられたDES構造体のキーと初期化ベクトル（IV）を設定します。また、これらがまだ初期化されていない場合は、暗号化と復号化に必要なバッファーのスペースを初期化して割り当てます。注：IVが指定されていない場合（i.e.iv == null）初期化ベクトルは、デフォルトのIV 0になります。
    \return 0  DES構造体のキーと初期化ベクトルを正常に設定する
    \param des  初期化するDES構造へのポインタ
    \param key  DES構造を初期化するための8バイトのキーを含むバッファへのポインタ
    \param iv  DES構造を初期化するための8バイトIVを含むバッファへのポインタ。これが提供されていない場合、IVはデフォルトで0になります
    _Example_
    \code
    Des enc; // Des structure used for encryption
    int ret;
    byte key[] = { // initialize with 8 byte key };
    byte iv[]  = { // initialize with 8 byte iv };

    ret = wc_Des_SetKey(&des, key, iv, DES_ENCRYPTION);
    if (ret != 0) {
    	// error initializing des structure
    }
    \endcode
    \sa wc_Des_SetIV
    \sa wc_Des3_SetKey
*/
int  wc_Des_SetKey(Des* des, const byte* key,
                               const byte* iv, int dir);

/*!
    \ingroup 3DES 
    \brief  この関数は、引数として与えられたDES構造体の初期化ベクトル（IV）を設定します。NULL IVを渡したら、初期化ベクトルを0に設定します。
    \return none  いいえ返します。
    \param des  IVを設定するためのDES構造へのポインタ
    _Example_
    \code
    Des enc; // Des structure used for encryption
    // initialize enc with wc_Des_SetKey
    byte iv[]  = { // initialize with 8 byte iv };
    wc_Des_SetIV(&enc, iv);
    }
    \endcode
    \sa wc_Des_SetKey
*/
void wc_Des_SetIV(Des* des, const byte* iv);

/*!
    \ingroup 3DES 
    \brief  この関数は入力メッセージを暗号化し、結果を出力バッファーに格納します。暗号ブロックチェーンチェーン（CBC）モードでDES暗号化を使用します。
    \return 0  与えられた入力メッセージの暗号化に成功したときに返されます
    \param des  暗号化に使用するDES構造へのポインタ
    \param out  暗号化された暗号文を保存するバッファへのポインタ
    \param in  暗号化するメッセージを含む入力バッファへのポインタ
    _Example_
    \code
    Des enc; // Des structure used for encryption
    // initialize enc with wc_Des_SetKey, use mode DES_ENCRYPTION

    byte plain[]  = { // initialize with message };
    byte cipher[sizeof(plain)];

    if ( wc_Des_CbcEncrypt(&enc, cipher, plain, sizeof(plain)) != 0) {
	    // error encrypting message
    }
    \endcode
    \sa wc_Des_SetKey
    \sa wc_Des_CbcDecrypt
*/
int  wc_Des_CbcEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);

/*!
    \ingroup 3DES 
    \brief  この関数は入力暗号文を復号化し、結果の平文を出力バッファーに出力します。暗号ブロックチェーンチェーン（CBC）モードでDES暗号化を使用します。
    \return 0  与えられた暗号文を正常に復号化したときに返されました
    \param des  復号化に使用するDES構造へのポインタ
    \param out  復号化された平文を保存するバッファへのポインタ
    \param in  暗号化された暗号文を含む入力バッファへのポインタ
    _Example_
    \code
    Des dec; // Des structure used for decryption
    // initialize dec with wc_Des_SetKey, use mode DES_DECRYPTION

    byte cipher[]  = { // initialize with ciphertext };
    byte decoded[sizeof(cipher)];

    if ( wc_Des_CbcDecrypt(&dec, decoded, cipher, sizeof(cipher)) != 0) {
    	// error decrypting message
    }
    \endcode
    \sa wc_Des_SetKey
    \sa wc_Des_CbcEncrypt
*/
int  wc_Des_CbcDecrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);

/*!
    \ingroup 3DES 
    \brief  この関数は入力メッセージを暗号化し、結果を出力バッファーに格納します。電子コードブック（ECB）モードでDES暗号化を使用します。
    \return 0:  与えられた平文を正常に暗号化すると返されます。
    \param des  暗号化に使用するDES構造へのポインタ
    \param out  暗号化されたメッセージを保存するバッファへのポインタ
    \param in  暗号化する平文を含む入力バッファへのポインタ
    _Example_
    \code
    Des enc; // Des structure used for encryption
    // initialize enc with wc_Des_SetKey, use mode DES_ENCRYPTION

    byte plain[]  = { // initialize with message to encrypt };
    byte cipher[sizeof(plain)];

    if ( wc_Des_EcbEncrypt(&enc,cipher, plain, sizeof(plain)) != 0) {
    	// error encrypting message
    }
    \endcode
    \sa wc_Des_SetKe
*/
int  wc_Des_EcbEncrypt(Des* des, byte* out,
                                   const byte* in, word32 sz);

/*!
    \ingroup 3DES 
    \brief  この関数は入力メッセージを暗号化し、結果を出力バッファーに格納します。電子コードブック（ECB）モードでDES3暗号化を使用します。警告：ほぼすべてのユースケースでECBモードは安全性が低いと考えられています。可能な限りECB APIを直接使用しないでください。
    \return 0  与えられた平文を正常に暗号化すると返されます
    \param des3  暗号化に使用するDES3構造へのポインタ
    \param out  暗号化されたメッセージを保存するバッファへのポインタ
    \param in  暗号化する平文を含む入力バッファへのポインタ
    _Example_
    \code
    Des3 enc; // Des3 structure used for encryption
    // initialize enc with wc_Des3_SetKey, use mode DES_ENCRYPTION

    byte plain[]  = { // initialize with message to encrypt };
    byte cipher[sizeof(plain)];

    if ( wc_Des3_EcbEncrypt(&enc,cipher, plain, sizeof(plain)) != 0) {
        // error encrypting message
    }
    \endcode
    \sa wc_Des3_SetKey
*/
int wc_Des3_EcbEncrypt(Des3* des, byte* out,
                                   const byte* in, word32 sz);

/*!
    \ingroup 3DES 
    \brief  この関数は、引数として与えられたDES3構造のキーと初期化ベクトル（IV）を設定します。また、これらがまだ初期化されていない場合は、暗号化と復号化に必要なバッファーのスペースを初期化して割り当てます。注：IVが指定されていない場合（i.e.iv == null）初期化ベクトルは、デフォルトのIV 0になります。
    \return 0  DES構造体のキーと初期化ベクトルを正常に設定する
    \param des3  初期化するDES3構造へのポインタ
    \param key  DES3構造を初期化する24バイトのキーを含むバッファへのポインタ
    \param iv  DES3構造を初期化するための8バイトIVを含むバッファへのポインタ。これが提供されていない場合、IVはデフォルトで0になります
    _Example_
    \code
    Des3 enc; // Des3 structure used for encryption
    int ret;
    byte key[] = { // initialize with 24 byte key };
    byte iv[]  = { // initialize with 8 byte iv };

    ret = wc_Des3_SetKey(&des, key, iv, DES_ENCRYPTION);
    if (ret != 0) {
    	// error initializing des structure
    }
    \endcode
    \sa wc_Des3_SetIV
    \sa wc_Des3_CbcEncrypt
    \sa wc_Des3_CbcDecrypt
*/
int  wc_Des3_SetKey(Des3* des, const byte* key,
                                const byte* iv,int dir);

/*!
    \ingroup 3DES 
    \brief  この関数は、引数として与えられたDES3構造の初期化ベクトル（IV）を設定します。NULL IVを渡したら、初期化ベクトルを0に設定します。
    \return none  いいえ返します。
    \param des  IVを設定するためのDES3構造へのポインタ
    _Example_
    \code
    Des3 enc; // Des3 structure used for encryption
    // initialize enc with wc_Des3_SetKey

    byte iv[]  = { // initialize with 8 byte iv };

    wc_Des3_SetIV(&enc, iv);
    }
    \endcode
    \sa wc_Des3_SetKey
*/
int  wc_Des3_SetIV(Des3* des, const byte* iv);

/*!
    \ingroup 3DES 
    \brief  この関数は入力メッセージを暗号化し、結果を出力バッファーに格納します。暗号ブロックチェーン（CBC）モードでトリプルDES（3DES）暗号化を使用します。
    \return 0  与えられた入力メッセージの暗号化に成功したときに返されます
    \param des  暗号化に使用するDES3構造へのポインタ
    \param out  暗号化された暗号文を保存するバッファへのポインタ
    \param in  暗号化するメッセージを含む入力バッファへのポインタ
    _Example_
    \code
    Des3 enc; // Des3 structure used for encryption
    // initialize enc with wc_Des3_SetKey, use mode DES_ENCRYPTION

    byte plain[]  = { // initialize with message };
    byte cipher[sizeof(plain)];

    if ( wc_Des3_CbcEncrypt(&enc, cipher, plain, sizeof(plain)) != 0) {
	    // error encrypting message
    }
    \endcode
    \sa wc_Des3_SetKey
    \sa wc_Des3_CbcDecrypt
*/
int  wc_Des3_CbcEncrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz);

/*!
    \ingroup 3DES 
    \brief  この関数は入力暗号文を復号化し、結果の平文を出力バッファーに出力します。暗号ブロックチェーン（CBC）モードでトリプルDES（3DES）暗号化を使用します。
    \return 0  与えられた暗号文を正常に復号化したときに返されました
    \param des  復号化に使用するDES3構造へのポインタ
    \param out  復号化された平文を保存するバッファへのポインタ
    \param in  暗号化された暗号文を含む入力バッファへのポインタ
    _Example_
    \code
    Des3 dec; // Des structure used for decryption
    // initialize dec with wc_Des3_SetKey, use mode DES_DECRYPTION

    byte cipher[]  = { // initialize with ciphertext };
    byte decoded[sizeof(cipher)];

    if ( wc_Des3_CbcDecrypt(&dec, decoded, cipher, sizeof(cipher)) != 0) {
    	// error decrypting message
    }
    \endcode
    \sa wc_Des3_SetKey
    \sa wc_Des3_CbcEncrypt
*/
int  wc_Des3_CbcDecrypt(Des3* des, byte* out,
                                    const byte* in,word32 sz);
