/*!
    \ingroup ChaCha20Poly1305 
    \brief  この関数は、Chacha20 Stream暗号を使用して、Chacha20 Stream暗号を使用して、Output BufferTextに入力メッセージ、InPleaintextを暗号化します。また、Poly-1305認証（暗号テキスト）を実行し、生成した認証タグを出力バッファOutauthTagに格納します。
    \return 0  メッセージの暗号化に成功したら返されました
    \return BAD_FUNC_ARG  暗号化プロセス中にエラーがある場合
    \param inKey  暗号化に使用する32バイトのキーを含むバッファへのポインタ
    \param inIv  暗号化に使用する12バイトのIVを含むバッファへのポインタ
    \param inAAD  任意の長さの追加認証データ（AAD）を含むバッファへのポインタ
    \param inAADLen  入力AADの長さ
    \param inPlaintext  暗号化する平文を含むバッファへのポインタ
    \param inPlaintextLen  暗号化するプレーンテキストの長さ
    \param outCiphertext  暗号文を保存するバッファーへのポインタ
    _Example_
    \code
    byte key[] = { // initialize 32 byte key };
    byte iv[]  = { // initialize 12 byte key };
    byte inAAD[] = { // initialize AAD };

    byte plain[] = { // initialize message to encrypt };
    byte cipher[sizeof(plain)];
    byte authTag[16];

    int ret = wc_ChaCha20Poly1305_Encrypt(key, iv, inAAD, sizeof(inAAD),
    plain, sizeof(plain), cipher, authTag);

    if(ret != 0) {
    	// error running encrypt
    }
    \endcode
    \sa wc_ChaCha20Poly1305_Decrypt
    \sa wc_ChaCha_*
    \sa wc_Poly1305*
*/

int wc_ChaCha20Poly1305_Encrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inPlaintext, const word32 inPlaintextLen,
                byte* outCiphertext,
                byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE]);

/*!
    \ingroup ChaCha20Poly1305 
    \brief  この関数は、Chacha20 Stream暗号を使用して、Chacha20 Stream暗号を使用して、出力バッファ、OutpleAntextに入力された暗号文の入力を復号化します。また、Poly-1305認証を実行し、指定されたINAUTHTAGをINAADで生成された認証（任意の長さの追加認証データ）と比較します。注：生成された認証タグが提供された認証タグと一致しない場合、テキストは復号化されません。
    \return 0  メッセージの復号化に成功したときに返されました
    \return BAD_FUNC_ARG  関数引数のいずれかが予想されるものと一致しない場合に返されます
    \return MAC_CMP_FAILED_E  生成された認証タグが提供されているINAUTHTAGと一致しない場合に返されます。
    \param inKey  復号化に使用する32バイトのキーを含むバッファへのポインタ
    \param inIv  復号化に使用する12バイトのIVを含むバッファへのポインタ
    \param inAAD  任意の長さの追加認証データ（AAD）を含むバッファへのポインタ
    \param inAADLen  入力AADの長さ
    \param inCiphertext  復号化する暗号文を含むバッファへのポインタ
    \param outCiphertextLen  復号化する暗号文の長さ
    \param inAuthTag  認証のための16バイトのダイジェストを含むバッファへのポインタ
    _Example_
    \code
    byte key[]   = { // initialize 32 byte key };
    byte iv[]    = { // initialize 12 byte key };
    byte inAAD[] = { // initialize AAD };

    byte cipher[]    = { // initialize with received ciphertext };
    byte authTag[16] = { // initialize with received authentication tag };

    byte plain[sizeof(cipher)];

    int ret = wc_ChaCha20Poly1305_Decrypt(key, iv, inAAD, sizeof(inAAD),
    cipher, sizeof(cipher), plain, authTag);

    if(ret == MAC_CMP_FAILED_E) {
    	// error during authentication
    } else if( ret != 0) {
    	// error with function arguments
    }
    \endcode
    \sa wc_ChaCha20Poly1305_Encrypt
    \sa wc_ChaCha_*
    \sa wc_Poly1305*
*/

int wc_ChaCha20Poly1305_Decrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inCiphertext, const word32 inCiphertextLen,
                const byte inAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE],
                byte* outPlaintext);
