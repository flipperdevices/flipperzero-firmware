/*!
    \ingroup Poly1305 
    \brief  この関数は、Poly1305コンテキスト構造のキーを設定し、ハッシュに初期化します。注：セキュリティを確保するために、WC_POLY1305FINALでメッセージハッシュを生成した後に新しいキーを設定する必要があります。
    \return 0  キーを正常に設定し、Poly1305構造の初期化
    \return BAD_FUNC_ARG  与えられたキーが32バイトの長さでない場合、またはPoly1305コンテキストがNULLの場合
    \param ctx  初期化するためのPoly1305構造へのポインタ
    \param key  ハッシュに使用する鍵を含むバッファへのポインタ
    _Example_
    \code
    Poly1305 enc;
    byte key[] = { initialize with 32 byte key to use for hashing };
    wc_Poly1305SetKey(&enc, key, sizeof(key));
    \endcode
    \sa wc_Poly1305Update
    \sa wc_Poly1305Final
*/
int wc_Poly1305SetKey(Poly1305* poly1305, const byte* key,
                                  word32 kySz);

/*!
    \ingroup Poly1305 
    \brief  この関数は、Poly1305構造を持つハッシュにメッセージを更新します。
    \return 0  ハッシュへのメッセージの更新に成功しました
    \return BAD_FUNC_ARG  Poly1305構造がNULLの場合に返されます
    \param ctx  HASHにメッセージを更新するためのPoly1305構造へのポインタ
    \param m  ハッシュに追加する必要があるメッセージを含むバッファへのポインタ
    _Example_
    \code
    Poly1305 enc;
    byte key[] = { }; // initialize with 32 byte key to use for encryption

    byte msg[] = { }; // initialize with message to hash
    wc_Poly1305SetKey(&enc, key, sizeof(key));

    if( wc_Poly1305Update(key, msg, sizeof(msg)) != 0 ) {
	    // error updating message to hash
    }
    \endcode
    \sa wc_Poly1305SetKey
    \sa wc_Poly1305Final
*/
int wc_Poly1305Update(Poly1305* poly1305, const byte* m, word32 bytes);

/*!
    \ingroup Poly1305 
    \brief  この関数は入力メッセージのハッシュを計算し、結果をMACに格納します。この後、キーをリセットする必要があります。
    \return 0  最後のMacの計算に成功した
    \return BAD_FUNC_ARG  Poly1305構造がNULLの場合に返されます
    \param ctx  MACを生成するためのPoly1305構造へのポインタ
    _Example_
    \code
    Poly1305 enc;
    byte mac[POLY1305_DIGEST_SIZE]; // space for a 16 byte mac

    byte key[] = { }; // initialize with 32 byte key to use for encryption

    byte msg[] = { }; // initialize with message to hash
    wc_Poly1305SetKey(&enc, key, sizeof(key));
    wc_Poly1305Update(key, msg, sizeof(msg));

    if ( wc_Poly1305Final(&enc, mac) != 0 ) {
    	// error computing final MAC
    }
    \endcode
    \sa wc_Poly1305SetKey
    \sa wc_Poly1305Update
*/
int wc_Poly1305Final(Poly1305* poly1305, byte* tag);

/*!
    \ingroup Poly1305 
    \brief  鍵がロードされ、最近のTLS AEADパディング方式を使用してMAC（タグ）を作成する初期化されたPoly1305構造体を取ります。
    \return 0  成功
    \return BAD_FUNC_ARG  CTX、INPUT、またはTAGがNULLの場合、または追加がNULLで、ADDSZが0より大きい場合、またはTAGSZがWC_POLY1305_MAC_SZより小さい場合に返されます。
    \param ctx  初期化されたPoly1305構造物
    \param additional  使用する追加データ
    \param addSz  追加バッファのサイズ
    \param input  からタグを作成するための入力バッファ
    \param sz  入力バッファのサイズ
    \param tag  作成したタグを保持するためのバッファー
    _Example_
    \code
    Poly1305 ctx;
    byte key[] = { }; // initialize with 32 byte key to use for hashing
    byte additional[] = { }; // initialize with additional data
    byte msg[] = { }; // initialize with message
    byte tag[16];

    wc_Poly1305SetKey(&ctx, key, sizeof(key));
    if(wc_Poly1305_MAC(&ctx, additional, sizeof(additional), (byte*)msg,
    sizeof(msg), tag, sizeof(tag)) != 0)
    {
        // Handle the error
    }
    \endcode
    \sa wc_Poly1305SetKey
    \sa wc_Poly1305Update
    \sa wcPoly1305Final
*/
int wc_Poly1305_MAC(Poly1305* ctx, byte* additional, word32 addSz,
                               byte* input, word32 sz, byte* tag, word32 tagSz);
