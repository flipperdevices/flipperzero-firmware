/*!
    \ingroup AES 
    \brief  入力バッファーから暗号を復号化し、AESでCipher Block Chainingを使用して出力バッファに出力バッファーに入れます。この関数は、AES構造を初期化する必要はありません。代わりに、キーとIV（初期化ベクトル）を取り、これらを使用してAESオブジェクトを初期化してから暗号テキストを復号化します。
    \return 0  メッセージの復号化に成功しました
    \return BAD_ALIGN_E  ブロック整列エラーに戻りました
    \return BAD_FUNC_ARG  aesetivの間にキーの長さが無効な場合、またはAESオブジェクトがNULLの場合
    \return MEMORY_E  wolfssl_small_stackが有効になっていて、xmallocがAESオブジェクトのインスタンス化に失敗した場合に返されます。
    \param out  復号化されたメッセージのプレーンテキストを保存する出力バッファへのポインタ
    \param in  復号化される暗号テキストを含む入力バッファへのポインタ
    \param inSz  入力メッセージのサイズ
    \param key  復号化のための16,24、または32バイトの秘密鍵
    _Example_
    \code
    int ret = 0;
    byte key[] = { some 16, 24, or 32 byte key };
    byte iv[]  = { some 16 byte iv };
    byte cipher[AES_BLOCK_SIZE * n]; //n being a positive integer making
    cipher some multiple of 16 bytes
    // fill cipher with cipher text
    byte plain [AES_BLOCK_SIZE * n];
    if ((ret = wc_AesCbcDecryptWithKey(plain, cipher, AES_BLOCK_SIZE, key,
    AES_BLOCK_SIZE, iv)) != 0 ) {
	// Decrypt Error
    }
    \endcode
    \sa wc_AesSetKey
    \sa wc_AesSetIV
    \sa wc_AesCbcEncrypt
    \sa wc_AesCbcDecrypt
*/
int  wc_AesCbcDecryptWithKey(byte* out, const byte* in, word32 inSz,
                                         const byte* key, word32 keySz,
                                         const byte* iv);

/*!
    \ingroup 3DES 
    \brief  この関数は入力暗号文を復号化し、結果の平文を出力バッファーに出力します。暗号ブロックチェーンチェーン（CBC）モードでDES暗号化を使用します。この関数は、wc_des_cbcdecryptの代わりに、ユーザーがDES構造体を直接インスタンス化せずにメッセージを復号化できるようにします。
    \return 0  与えられた暗号文を正常に復号化したときに返されました
    \return MEMORY_E  DES構造体の割り当てスペースが割り当てられている場合に返された
    \param out  復号化された平文を保存するバッファへのポインタ
    \param in  暗号化された暗号文を含む入力バッファへのポインタ
    \param sz  復号化する暗号文の長さ
    \param key  復号化に使用する8バイトのキーを含むバッファへのポインタ
    _Example_
    \code
    int ret;
    byte key[] = { // initialize with 8 byte key };
    byte iv[]  = { // initialize with 8 byte iv };

    byte cipher[]  = { // initialize with ciphertext };
    byte decoded[sizeof(cipher)];

    if ( wc_Des_CbcDecryptWithKey(decoded, cipher, sizeof(cipher), key,
    iv) != 0) {
    	// error decrypting message
    }
    \endcode
    \sa wc_Des_CbcDecrypt
*/
int  wc_Des_CbcDecryptWithKey(byte* out,
                                          const byte* in, word32 sz,
                                          const byte* key, const byte* iv);

/*!
    \ingroup 3DES 
    \brief  この関数は入力平文を暗号化し、結果の暗号文を出力バッファーに出力します。暗号ブロックチェーンチェーン（CBC）モードでDES暗号化を使用します。この関数は、WC_DES_CBCENCRYPTの代わりになり、ユーザーがDES構造を直接インスタンス化せずにメッセージを暗号化できます。
    \return 0  データの暗号化に成功した後に返されます。
    \return MEMORY_E  DES構造体にメモリを割り当てるエラーがある場合は返されます。
    \return <0  暗号化中に任意のエラーに戻ります。
    \param out  最終暗号化データ
    \param in  暗号化されるデータは、DESブロックサイズに埋められなければなりません。
    \param sz  入力バッファのサイズ
    \param key  暗号化に使用するキーへのポインタ。
    _Example_
    \code
    byte key[] = { // initialize with 8 byte key };
    byte iv[]  = { // initialize with 8 byte iv };
    byte in[] = { // Initialize with plaintext };
    byte out[sizeof(in)];
    if ( wc_Des_CbcEncryptWithKey(&out, in, sizeof(in), key, iv) != 0)
    {
    	// error encrypting message
    }
    \endcode
    \sa wc_Des_CbcDecryptWithKey
    \sa wc_Des_CbcEncrypt
*/
int  wc_Des_CbcEncryptWithKey(byte* out,
                                          const byte* in, word32 sz,
                                          const byte* key, const byte* iv);

/*!
    \ingroup 3DES 
    \brief  この関数は入力平文を暗号化し、結果の暗号文を出力バッファーに出力します。暗号ブロックチェーン（CBC）モードでトリプルDES（3DES）暗号化を使用します。この関数は、WC_DES3_CBCENCRYPTの代わりになり、ユーザーがDES3構造を直接インスタンス化せずにメッセージを暗号化できます。
    \return 0  データの暗号化に成功した後に返されます。
    \return MEMORY_E  DES構造体にメモリを割り当てるエラーがある場合は返されます。
    \return <0  暗号化中に任意のエラーに戻ります。
    \param out  最終暗号化データ
    \param in  暗号化されるデータは、DESブロックサイズに埋められなければなりません。
    \param sz  入力バッファのサイズ
    \param key  暗号化に使用するキーへのポインタ。
    _Example_
    \code
    byte key[] = { // initialize with 8 byte key };
    byte iv[]  = { // initialize with 8 byte iv };

    byte in[] = { // Initialize with plaintext };
    byte out[sizeof(in)];

    if ( wc_Des3_CbcEncryptWithKey(&out, in, sizeof(in), key, iv) != 0)
    {
    	// error encrypting message
    }
    \endcode
    \sa wc_Des3_CbcDecryptWithKey
    \sa wc_Des_CbcEncryptWithKey
    \sa wc_Des_CbcDecryptWithKey
*/
int  wc_Des3_CbcEncryptWithKey(byte* out,
                                           const byte* in, word32 sz,
                                           const byte* key, const byte* iv);

/*!
    \ingroup 3DES 
    \brief  この関数は入力暗号文を復号化し、結果の平文を出力バッファーに出力します。暗号ブロックチェーン（CBC）モードでトリプルDES（3DES）暗号化を使用します。この関数は、wc_des3_cbcdecryptの代わりに、ユーザーがDES3構造を直接インスタンス化せずにメッセージを復号化できるようにします。
    \return 0  与えられた暗号文を正常に復号化したときに返されました
    \return MEMORY_E  DES構造体の割り当てスペースが割り当てられている場合に返された
    \param out  復号化された平文を保存するバッファへのポインタ
    \param in  暗号化された暗号文を含む入力バッファへのポインタ
    \param sz  復号化する暗号文の長さ
    \param key  復号化に使用する24バイトのキーを含むバッファへのポインタ
    _Example_
    \code
    int ret;
    byte key[] = { // initialize with 24 byte key };
    byte iv[]  = { // initialize with 8 byte iv };

    byte cipher[]  = { // initialize with ciphertext };
    byte decoded[sizeof(cipher)];

    if ( wc_Des3_CbcDecryptWithKey(decoded, cipher, sizeof(cipher),
    key, iv) != 0) {
    	// error decrypting message
    }
    \endcode
    \sa wc_Des3_CbcDecrypt
*/
int  wc_Des3_CbcDecryptWithKey(byte* out,
                                           const byte* in, word32 sz,
                                           const byte* key, const byte* iv);
