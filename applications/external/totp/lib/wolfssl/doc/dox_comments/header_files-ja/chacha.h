/*!
    \ingroup ChaCha 
    \brief  この関数はChachaオブジェクトの初期化ベクトル（nonce）を設定し、暗号として使用するために初期化します。WC_CHACHA_SETKEYを使用して、キーが設定された後に呼び出されるべきです。暗号化の各ラウンドに差し違いを使用する必要があります。
    \return 0  初期化ベクトルを正常に設定すると返されます
    \return BAD_FUNC_ARG  CTX入力引数の処理中にエラーが発生した場合
    \param ctx  IVを設定するChacha構造へのポインタ
    \param inIv  Chacha構造を初期化するための12バイトの初期化ベクトルを含むバッファへのポインタ
    _Example_
    \code
    ChaCha enc;
    // initialize enc with wc_Chacha_SetKey
    byte iv[12];
    // initialize iv
    if( wc_Chacha_SetIV(&enc, iv, 0) != 0) {
	    // error initializing ChaCha structure
    }
    \endcode
    \sa wc_Chacha_SetKey
    \sa wc_Chacha_Process
*/
int wc_Chacha_SetIV(ChaCha* ctx, const byte* inIv, word32 counter);

/*!
    \ingroup ChaCha 
    \brief  この関数は、バッファ入力からテキストを処理し、暗号化または復号化し、結果をバッファ出力に格納します。
    \return 0  入力の暗号化または復号化に成功したときに返されます
    \return BAD_FUNC_ARG  CTX入力引数の処理中にエラーが発生した場合
    \param ctx  IVを設定するChacha構造へのポインタ
    \param output  出力暗号文または復号化された平文を保存するバッファへのポインタ
    \param input  暗号化する入力平文を含むバッファへのポインタまたは復号化する入力暗号文
    _Example_
    \code
    ChaCha enc;
    // initialize enc with wc_Chacha_SetKey and wc_Chacha_SetIV

    byte plain[] = { // initialize plaintext };
    byte cipher[sizeof(plain)];
    if( wc_Chacha_Process(&enc, cipher, plain, sizeof(plain)) != 0) {
	    // error processing ChaCha cipher
    }
    \endcode
    \sa wc_Chacha_SetKey
    \sa wc_Chacha_Process
*/
int wc_Chacha_Process(ChaCha* ctx, byte* cipher, const byte* plain,
                              word32 msglen);

/*!
    \ingroup ChaCha 
    \brief  この関数はChachaオブジェクトのキーを設定し、それを暗号として使用するために初期化します。NONCEをWC_CHACHA_SETIVで設定する前に、WC_CHACHA_PROCESSを使用した暗号化に使用する前に呼び出す必要があります。
    \return 0  キーの設定に成功したときに返されます
    \return BAD_FUNC_ARG  CTX入力引数の処理中にエラーが発生した場合、またはキーが16または32バイトの長さがある場合
    \param ctx  キーを設定するChacha構造へのポインタ
    \param key  Chacha構造を初期化するための16または32バイトのキーを含むバッファへのポインタ
    _Example_
    \code
    ChaCha enc;
    byte key[] = { // initialize key };

    if( wc_Chacha_SetKey(&enc, key, sizeof(key)) != 0) {
	    // error initializing ChaCha structure
    }
    \endcode
    \sa wc_Chacha_SetIV
    \sa wc_Chacha_Process
*/
int wc_Chacha_SetKey(ChaCha* ctx, const byte* key, word32 keySz);
