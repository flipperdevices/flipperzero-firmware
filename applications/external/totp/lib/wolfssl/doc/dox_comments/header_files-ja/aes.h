/*!
    \ingroup AES 
    \brief  この関数は、キーを設定して初期化ベクトルを設定することでAES構造を初期化します。
    \return 0  キーと初期化ベクトルを正常に設定します。
    \return BAD_FUNC_ARG  キーの長さが無効な場合は返されます。
    \param aes  変更するAES構造へのポインタ
    \param key  暗号化と復号化のための16,24、または32バイトの秘密鍵
    \param len  渡されたキーの長さ
    \param iv  キーを初期化するために使用される初期化ベクトルへのポインタ
    _Example_
    \code
    Aes enc;
    int ret = 0;
    byte key[] = { some 16, 24 or 32 byte key };
    byte iv[]  = { some 16 byte iv };
    if (ret = wc_AesSetKey(&enc, key, AES_BLOCK_SIZE, iv,
    AES_ENCRYPTION) != 0) {
	// failed to set aes key
    }
    \endcode
    \sa wc_AesSetKeyDirect
    \sa wc_AesSetIV
*/
int  wc_AesSetKey(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir);

/*!
    \ingroup AES 
    \brief  この関数は、特定のAESオブジェクトの初期化ベクトルを設定します。AESオブジェクトは、この関数を呼び出す前に初期化されるべきです。
    \return 0  初期化ベクトルを正常に設定します。
    \return BAD_FUNC_ARG  AESポインタがNULLの場合に返されます。
    \param aes  初期化ベクトルを設定するAES構造へのポインタ
    _Example_
    \code
    Aes enc;
    // set enc key
    byte iv[]  = { some 16 byte iv };
    if (ret = wc_AesSetIV(&enc, iv) != 0) {
	// failed to set aes iv
    }
    \endcode
    \sa wc_AesSetKeyDirect
    \sa wc_AesSetKey
*/
int  wc_AesSetIV(Aes* aes, const byte* iv);

/*!
    \ingroup AES 
    \brief  入力バッファーから平文メッセージを暗号化し、AESでCipher Block Chainingを使用して出力バッファに出力バッファーに入れます。この機能は、メッセージが暗号化される前にAESSetKeyを呼び出すことによってAESオブジェクトが初期化されていることを必要とします。この関数は、入力メッセージがAESブロック長であると仮定し、入力された長さがブロック長の倍数になることを想定しているため、ビルド構成でWolfSSL_AES_CBC_LENGTH_CHECKSが定義されている場合は任意選択でチェックおよび適用されます。ブロック多入力を保証するために、PKCS＃7スタイルのパディングを事前に追加する必要があります。これはあなたのためにパディングを追加するOpenSSL AES-CBCメソッドとは異なります。WOLFSSLと対応するOpenSSL関数を相互運用するには、OpenSSLコマンドライン関数で-nopadオプションを指定して、WolfSSL AESCCENCRYPTメソッドのように動作し、暗号化中に追加のパディングを追加しません。
    \return 0  メッセージの暗号化に成功しました。
    \return BAD_ALIGN_E:  ブロック整列誤差で返される可能性があります
    \return BAD_LENGTH_E  入力長がAESブロック長の倍数でない場合は、ライブラリーがwolfssl_aes_cbc_length_checksで構築されている場合に返されます。
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  暗号化されたメッセージの暗号文を格納する出力バッファへのポインタ
    \param in  暗号化されるメッセージを含む入力バッファへのポインタ
    _Example_
    \code
    Aes enc;
    int ret = 0;
    // initialize enc with AesSetKey, using direction AES_ENCRYPTION
    byte msg[AES_BLOCK_SIZE * n]; // multiple of 16 bytes
    // fill msg with data
    byte cipher[AES_BLOCK_SIZE * n]; // Some multiple of 16 bytes
    if ((ret = wc_AesCbcEncrypt(&enc, cipher, message, sizeof(msg))) != 0 ) {
	// block align error
    }
    \endcode
    \sa wc_AesSetKey
    \sa wc_AesSetIV
    \sa wc_AesCbcDecrypt
*/
int  wc_AesCbcEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);

/*!
    \ingroup AES 
    \brief  入力バッファーから暗号を復号化し、AESでCipher Block Chainingを使用して出力バッファに出力バッファーに入れます。この機能は、メッセージが復号化される前にAESSetKeyを呼び出すことによってAES構造が初期化されていることを必要とします。この関数は、元のメッセージがAESブロック長で整列していたと仮定し、入力された長さがブロック長の倍数になると予想しています。これはOpenSSL AES-CBCメソッドとは異なります。これは、PKCS＃7パディングを自動的に追加するため、ブロックマルチ入力を必要としません。WolfSSL機能と同等のOpenSSL関数を相互運用するには、OpenSSLコマンドライン関数で-nopadオプションを指定し、wolfssl aescbceNcryptメソッドのように動作し、復号化中にエラーを発生させません。
    \return 0  メッセージを正常に復号化します。
    \return BAD_ALIGN_E  ブロック整列エラーで返される可能性があります。
    \return BAD_LENGTH_E  入力長がAESブロック長の倍数でない場合は、ライブラリーがwolfssl_aes_cbc_length_checksで構築されている場合に返されます。
    \param aes  データを復号化するために使用されるAESオブジェクトへのポインタ。
    \param out  復号化されたメッセージのプレーンテキストを保存する出力バッファへのポインタ。
    \param in  復号化する暗号テキストを含む入力バッファへのポインタ。
    _Example_
    \code
    Aes dec;
    int ret = 0;
    // initialize dec with AesSetKey, using direction AES_DECRYPTION
    byte cipher[AES_BLOCK_SIZE * n]; // some multiple of 16 bytes
    // fill cipher with cipher text
    byte plain [AES_BLOCK_SIZE * n];
    if ((ret = wc_AesCbcDecrypt(&dec, plain, cipher, sizeof(cipher))) != 0 ) {
	// block align error
    }
    \endcode
    \sa wc_AesSetKey
    \sa wc_AesCbcEncrypt
*/
int  wc_AesCbcDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);

/*!
    \ingroup AES 
    \brief  入力バッファーからメッセージを暗号化/復号化し、AESを使用してCTRモードを使用して出力バッファーに出力バッファーに入れます。この関数は、wolfssl_aes_counterがコンパイル時に有効になっている場合にのみ有効になります。この機能を呼び出す前に、AES構造体をAessetKeyで初期化する必要があります。この関数は復号化と暗号化の両方に使用されます。_注：暗号化と復号化のための同じAPIを使用することについて。ユーザーは暗号化/復号化のためのAES構造体を区別する必要があります。
    \return int  WolfSSLエラーまたは成功状況に対応する整数値
    \param aes  データを復号化するために使用されるAESオブジェクトへのポインタ
    \param out  暗号化されたメッセージの暗号化テキストを保存する出力バッファへのポインタ
    \param in  暗号化されるプレーンテキストを含む入力バッファへのポインタ
    _Example_
    \code
    Aes enc;
    Aes dec;
    // initialize enc and dec with AesSetKeyDirect, using direction
    AES_ENCRYPTION
    // since the underlying API only calls Encrypt and by default calling
    encrypt on
    // a cipher results in a decryption of the cipher

    byte msg[AES_BLOCK_SIZE * n]; //n being a positive integer making msg
    some multiple of 16 bytes
    // fill plain with message text
    byte cipher[AES_BLOCK_SIZE * n];
    byte decrypted[AES_BLOCK_SIZE * n];
    wc_AesCtrEncrypt(&enc, cipher, msg, sizeof(msg)); // encrypt plain
    wc_AesCtrEncrypt(&dec, decrypted, cipher, sizeof(cipher));
    // decrypt cipher text
    \endcode
    \sa wc_AesSetKey
*/
int wc_AesCtrEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz);

/*!
    \ingroup AES 
    \brief  この関数は、入力ブロックの入力ブロック、IN、OUTPUTブロック、OUTです。提供されたAES構造体のキーを使用します。これはこの機能を呼び出す前にWC_AESSETKEYで初期化される必要があります。WC_AESSETKEYは、IVセットがNULLに呼び出されたはずです。これは、Configure Option WolfSSL_AES_DIRECTが有効になっている場合にのみ有効になります。__ warning：ほぼすべてのユースケースでECBモードは安全性が低いと考えられています。可能な限りECB APIを直接使用しないでください。
    \return int  WolfSSLエラーまたは成功状況に対応する整数値
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  暗号化されたメッセージの暗号化テキストを保存する出力バッファへのポインタ
    _Example_
    \code
    Aes enc;
    // initialize enc with AesSetKey, using direction AES_ENCRYPTION
    byte msg [AES_BLOCK_SIZE]; // 16 bytes
    // initialize msg with plain text to encrypt
    byte cipher[AES_BLOCK_SIZE];
    wc_AesEncryptDirect(&enc, cipher, msg);
    \endcode
    \sa wc_AesDecryptDirect
    \sa wc_AesSetKeyDirect
*/
int wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in);

/*!
    \ingroup AES 
    \brief  この関数は、入力ブロックの1ブロック復号化、IN、IN、OUTPUT OUTです。提供されたAES構造体のキーを使用します。これはこの機能を呼び出す前にWC_AESSETKEYで初期化される必要があります。WC_AESSETKEYは、IVセットがNULLに呼び出されたはずです。これは、Configure Option WolfSSL_AES_DIRECTが有効になっている場合にのみ有効になります。__ warning：ほぼすべてのユースケースでECBモードは安全性が低いと考えられています。可能な限りECB APIを直接使用しないでください。
    \return int  WolfSSLエラーまたは成功状況に対応する整数値
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  復号化された暗号テキストのプレーンテキストを格納する出力バッファへのポインタ
    _Example_
    \code
    Aes dec;
    // initialize enc with AesSetKey, using direction AES_DECRYPTION
    byte cipher [AES_BLOCK_SIZE]; // 16 bytes
    // initialize cipher with cipher text to decrypt
    byte msg[AES_BLOCK_SIZE];
    wc_AesDecryptDirect(&dec, msg, cipher);
    \endcode
    \sa wc_AesEncryptDirect
    \sa wc_AesSetKeyDirect
 */
int wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in);

/*!
    \ingroup AES 
    \brief  この関数は、CTRモードのAESキーをAESで設定するために使用されます。指定されたキー、IV（初期化ベクトル）、および暗号化DIR（方向）でAESオブジェクトを初期化します。構成オプションwolfssl_aes_directが有効になっている場合にのみ有効になります。現在WC_AessetKeyDirectは内部的にWC_AESSETKEYを使用します。__ warning：ほぼすべてのユースケースでECBモードは安全性が低いと考えられています。可能な限りECB APIを直接使用しないでください
    \return 0  キーの設定に成功しました。
    \return BAD_FUNC_ARG  与えられたキーが無効な長さの場合に返されます。
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param key  暗号化と復号化のための16,24、または32バイトの秘密鍵
    \param len  渡されたキーの長さ
    \param iv  キーを初期化するために使用される初期化ベクトル
    _Example_
    \code
    Aes enc;
    int ret = 0;
    byte key[] = { some 16, 24, or 32 byte key };
    byte iv[]  = { some 16 byte iv };
    if (ret = wc_AesSetKeyDirect(&enc, key, sizeof(key), iv,
    AES_ENCRYPTION) != 0) {
	// failed to set aes key
    }
    \endcode
    \sa wc_AesEncryptDirect
    \sa wc_AesDecryptDirect
    \sa wc_AesSetKey
*/
int  wc_AesSetKeyDirect(Aes* aes, const byte* key, word32 len,
                                const byte* iv, int dir);

/*!
    \ingroup AES 
    \brief  この機能は、AES GCM（Galois / Counter Mode）のキーを設定するために使用されます。与えられたキーでAESオブジェクトを初期化します。コンパイル時にConfigureオプションhous_aesgcmが有効になっている場合にのみ有効になります。
    \return 0  キーの設定に成功しました。
    \return BAD_FUNC_ARG  与えられたキーが無効な長さの場合に返されます。
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param key  暗号化と復号化のための16,24、または32バイトの秘密鍵
    _Example_
    \code
    Aes enc;
    int ret = 0;
    byte key[] = { some 16, 24,32 byte key };
    if (ret = wc_AesGcmSetKey(&enc, key, sizeof(key)) != 0) {
	// failed to set aes key
    }
    \endcode
    \sa wc_AesGcmEncrypt
    \sa wc_AesGcmDecrypt
*/
int  wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len);

/*!
    \ingroup AES 
    \brief  この関数は、入力メッセージをバッファINに保持し、結果の暗号テキストを出力バッファOUTに格納します。暗号化する呼び出しごとに新しいIV（初期化ベクトル）が必要です。また、入力認証ベクトル、Authin、AuthTagへの入力認証ベクトルをエンコードします。
    \return 0  入力メッセージの暗号化に成功しました
    \param aes   - データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  暗号テキストを保存する出力バッファへのポインタ
    \param in  暗号化するメッセージを保持している入力バッファへのポインタ
    \param sz  暗号化する入力メッセージの長さ
    \param iv  初期化ベクトルを含むバッファへのポインタ
    \param ivSz  初期化ベクトルの長さ
    \param authTag  認証タグを保存するバッファへのポインタ
    \param authTagSz  希望の認証タグの長さ
    \param authIn  入力認証ベクトルを含むバッファへのポインタ
    _Example_
    \code
    Aes enc;
    // initialize aes structure by calling wc_AesGcmSetKey

    byte plain[AES_BLOCK_LENGTH * n]; //n being a positive integer
    making plain some multiple of 16 bytes
    // initialize plain with msg to encrypt
    byte cipher[sizeof(plain)];
    byte iv[] = // some 16 byte iv
    byte authTag[AUTH_TAG_LENGTH];
    byte authIn[] = // Authentication Vector

    wc_AesGcmEncrypt(&enc, cipher, plain, sizeof(cipher), iv, sizeof(iv),
			authTag, sizeof(authTag), authIn, sizeof(authIn));
    \endcode
    \sa wc_AesGcmSetKey
    \sa wc_AesGcmDecrypt
*/
int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES 
    \brief  この関数は、入力暗号テキストをバッファINに保持し、結果のメッセージテキストを出力バッファOUTに格納します。また、指定された認証タグ、authtagに対して、入力認証ベクトル、Authinをチェックします。
    \return 0  入力メッセージの復号化に成功しました
    \return AES_GCM_AUTH_E  認証タグが提供された認証コードベクトルと一致しない場合、authtag。
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  メッセージテキストを保存する出力バッファへのポインタ
    \param in  暗号テキストを復号化する入力バッファへのポインタ
    \param sz  復号化する暗号テキストの長さ
    \param iv  初期化ベクトルを含むバッファへのポインタ
    \param ivSz  初期化ベクトルの長さ
    \param authTag  認証タグを含むバッファへのポインタ
    \param authTagSz  希望の認証タグの長さ
    \param authIn  入力認証ベクトルを含むバッファへのポインタ
    _Example_
    \code
    Aes enc; //can use the same struct as was passed to wc_AesGcmEncrypt
    // initialize aes structure by calling wc_AesGcmSetKey if not already done

    byte cipher[AES_BLOCK_LENGTH * n]; //n being a positive integer
    making cipher some multiple of 16 bytes
    // initialize cipher with cipher text to decrypt
    byte output[sizeof(cipher)];
    byte iv[] = // some 16 byte iv
    byte authTag[AUTH_TAG_LENGTH];
    byte authIn[] = // Authentication Vector

    wc_AesGcmDecrypt(&enc, output, cipher, sizeof(cipher), iv, sizeof(iv),
			authTag, sizeof(authTag), authIn, sizeof(authIn));
    \endcode
    \sa wc_AesGcmSetKey
    \sa wc_AesGcmEncrypt
*/
int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES 
    \brief  この関数は、GAROISメッセージ認証に使用されるGMACオブジェクトのキーを初期化して設定します。
    \return 0  キーの設定に成功しました
    \return BAD_FUNC_ARG  キーの長さが無効な場合は返されます。
    \param gmac  認証に使用されるGMACオブジェクトへのポインタ
    \param key  認証のための16,24、または32バイトの秘密鍵
    _Example_
    \code
    Gmac gmac;
    key[] = { some 16, 24, or 32 byte length key };
    wc_GmacSetKey(&gmac, key, sizeof(key));
    \endcode
    \sa wc_GmacUpdate
*/
int wc_GmacSetKey(Gmac* gmac, const byte* key, word32 len);

/*!
    \ingroup AES 
    \brief  この関数はAuthin InputのGMACハッシュを生成し、結果をAuthtagバッファに格納します。wc_gmacupdateを実行した後、生成されたauthtagを既知の認証タグに比較してメッセージの信頼性を検証する必要があります。
    \return 0  GMACハッシュの計算に成功しました。
    \param gmac  認証に使用されるGMACオブジェクトへのポインタ
    \param iv  ハッシュに使用される初期化ベクトル
    \param ivSz  使用される初期化ベクトルのサイズ
    \param authIn  確認する認証ベクトルを含むバッファへのポインタ
    \param authInSz  認証ベクトルのサイズ
    \param authTag  GMACハッシュを保存する出力バッファへのポインタ
    _Example_
    \code
    Gmac gmac;
    key[] = { some 16, 24, or 32 byte length key };
    iv[] = { some 16 byte length iv };

    wc_GmacSetKey(&gmac, key, sizeof(key));
    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE]; // will store authentication code

    wc_GmacUpdate(&gmac, iv, sizeof(iv), authIn, sizeof(authIn), tag,
    sizeof(tag));
    \endcode
    \sa wc_GmacSetKey
*/
int wc_GmacUpdate(Gmac* gmac, const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz);

/*!
    \ingroup AES 
    \brief  この関数は、CCMを使用してAESオブジェクトのキーを設定します（CBC-MACのカウンタ）。AES構造体へのポインタを取り、付属のキーで初期化します。
    \return none 
    \param aes  付属のキーを保管するためのAES構造
    \param key  暗号化と復号化のための16,24、または32バイトの秘密鍵
    _Example_
    \code
    Aes enc;
    key[] = { some 16, 24, or 32 byte length key };

    wc_AesCcmSetKey(&aes, key, sizeof(key));
    \endcode
    \sa wc_AesCcmEncrypt
    \sa wc_AesCcmDecrypt
*/
int  wc_AesCcmSetKey(Aes* aes, const byte* key, word32 keySz);

/*!
    \ingroup AES 
    \brief  この関数は、CCMを使用して、入力メッセージ、IN、OUT、OUT、OUTをCCM（CBC-MACのカウンタ）を暗号化します。その後、Authin Inputから認証タグ、AuthtAgを計算して格納します。
    \return none 
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  暗号テキストを保存する出力バッファへのポインタ
    \param in  暗号化するメッセージを保持している入力バッファへのポインタ
    \param sz  暗号化する入力メッセージの長さ
    \param nonce  nonceを含むバッファへのポインタ（1回だけ使用されている数）
    \param nonceSz  ノンスの長さ
    \param authTag  認証タグを保存するバッファへのポインタ
    \param authTagSz  希望の認証タグの長さ
    \param authIn  入力認証ベクトルを含むバッファへのポインタ
    _Example_
    \code
    Aes enc;
    // initialize enc with wc_AesCcmSetKey

    nonce[] = { initialize nonce };
    plain[] = { some plain text message };
    cipher[sizeof(plain)];

    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE]; // will store authentication code

    wc_AesCcmEncrypt(&enc, cipher, plain, sizeof(plain), nonce, sizeof(nonce),
			tag, sizeof(tag), authIn, sizeof(authIn));
    \endcode
    \sa wc_AesCcmSetKey
    \sa wc_AesCcmDecrypt
*/
int  wc_AesCcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES 
    \brief  この関数は、CCMを使用して、入力暗号テキストを、CCM（CBC-MACのカウンタ）を使用して出力バッファーに復号化します。その後、Authin InputからAuthatAg、AuthatAgを計算します。許可タグが無効な場合は、出力バッファをゼロに設定し、AES_CCM_AUTH_Eを返します。
    \return 0  入力メッセージの復号化に成功しました
    \return AES_CCM_AUTH_E  認証タグが提供された認証コードベクトルと一致しない場合、authtag。
    \param aes  データの暗号化に使用されるAESオブジェクトへのポインタ
    \param out  暗号テキストを保存する出力バッファへのポインタ
    \param in  暗号化するメッセージを保持している入力バッファへのポインタ
    \param sz  入力暗号テキストの復号化
    \param nonce  nonceを含むバッファへのポインタ（1回だけ使用されている数）
    \param nonceSz  ノンスの長さ
    \param authTag  認証タグを保存するバッファへのポインタ
    \param authTagSz  希望の認証タグの長さ
    \param authIn  入力認証ベクトルを含むバッファへのポインタ
    _Example_
    \code
    Aes dec;
    // initialize dec with wc_AesCcmSetKey

    nonce[] = { initialize nonce };
    cipher[] = { encrypted message };
    plain[sizeof(cipher)];

    authIn[] = { some 16 byte authentication input };
    tag[AES_BLOCK_SIZE] = { authentication tag received for verification };

    int return = wc_AesCcmDecrypt(&dec, plain, cipher, sizeof(cipher),
    nonce, sizeof(nonce),tag, sizeof(tag), authIn, sizeof(authIn));
    if(return != 0) {
	// decrypt error, invalid authentication code
    }
    \endcode
    \sa wc_AesCcmSetKey
    \sa wc_AesCcmEncrypt
*/
int  wc_AesCcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/*!
    \ingroup AES 
    \brief  これは、暗号化または復号化タイプを修正するためのキーの設定を支援することです。完了したら、AESキーでWC_AESXTSFREEを呼び出すことがユーザーになりました。
    \return 0  成功
    \param aes  ENCRYPT /復号化プロセスのためのAESキー
    \param key  AESキーを保持しているバッファー| ..Tweak Key
    \param len  キーバッファのバイト数の長さ。キーサイズの2倍にする必要があります。すなわち、16バイトのキーについて。
    \param dir  方向、AES_EncryptionまたはAES_Decryptionのいずれか
    \param heap  メモリに使用するヒントヒント。nullになることができます
    _Example_
    \code
    XtsAes aes;

    if(wc_AesXtsSetKey(&aes, key, sizeof(key), AES_ENCRYPTION, NULL, 0) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode
    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsFree
*/
int wc_AesXtsSetKey(XtsAes* aes, const byte* key,
         word32 len, int dir, void* heap, int devId);

/*!
    \ingroup AES 
    \brief  WC_AESXTSENCRYPTと同じプロセスですが、バイト配列の代わりにTweak値としてWord64型を使用します。これは、Word64をバイト配列に変換し、WC_AESXTSENCRYPTを呼び出します。
    \return 0  成功
    \param aes  ブロック暗号化/復号化に使用するAESキー
    \param out  暗号テキストを保持するための出力バッファ
    \param in  暗号化する入力プレーンテキストバッファ
    \param sz  両方ともバッファのサイズ
    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    word64 s = VALUE;

    //set up keys with AES_ENCRYPTION as dir

    if(wc_AesXtsEncryptSector(&aes, cipher, plain, SIZE, s) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode
    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsEncryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

/*!
    \ingroup AES 
    \brief  WC_AESXTSDECRYPTと同じプロセスではなく、BYTE配列の代わりにWord64タイプを使用します。これはWord64をバイト配列に変換するだけです。
    \return 0  成功
    \param aes  ブロック暗号化/復号化に使用するAESキー
    \param out  プレーンテキストを保持するための出力バッファ
    \param in  復号化する暗号テキストバッファーを入力します
    \param sz  両方ともバッファのサイズ
    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    word64 s = VALUE;

    //set up aes key with AES_DECRYPTION as dir and tweak with AES_ENCRYPTION

    if(wc_AesXtsDecryptSector(&aes, plain, cipher, SIZE, s) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode
    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsDecryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

/*!
    \ingroup AES 
    \brief  XTSモードのあるAES。（XTS）XEX暗号化と暗号テキストを盗んだ暗号化。
    \return 0  成功
    \param aes  ブロック暗号化/復号化に使用するAESキー
    \param out  暗号テキストを保持するための出力バッファ
    \param in  暗号化する入力プレーンテキストバッファ
    \param sz  両方ともバッファのサイズ
    \param i  Tweakに使用する値
    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    unsigned char i[AES_BLOCK_SIZE];

    //set up key with AES_ENCRYPTION as dir

    if(wc_AesXtsEncrypt(&aes, cipher, plain, SIZE, i, sizeof(i)) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsEncrypt(XtsAes* aes, byte* out,
         const byte* in, word32 sz, const byte* i, word32 iSz);

/*!
    \ingroup AES 
    \brief  暗号化と同じプロセスですが、AESキーはAES_Decryptionタイプです。
    \return 0  成功
    \param aes  ブロック暗号化/復号化に使用するAESキー
    \param out  プレーンテキストを保持するための出力バッファ
    \param in  復号化する暗号テキストバッファーを入力します
    \param sz  両方ともバッファのサイズ
    \param i  Tweakに使用する値
    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    unsigned char i[AES_BLOCK_SIZE];

    //set up key with AES_DECRYPTION as dir and tweak with AES_ENCRYPTION

    if(wc_AesXtsDecrypt(&aes, plain, cipher, SIZE, i, sizeof(i)) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode
    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
int wc_AesXtsDecrypt(XtsAes* aes, byte* out,
        const byte* in, word32 sz, const byte* i, word32 iSz);

/*!
    \ingroup AES 
    \brief  これはXTSAES構造によって使用されるすべてのリソースを解放することです
    \return 0  成功
    _Example_
    \code
    XtsAes aes;

    if(wc_AesXtsSetKey(&aes, key, sizeof(key), AES_ENCRYPTION, NULL, 0) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode
    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
*/
int wc_AesXtsFree(XtsAes* aes);


/*!
    \ingroup AES 
    \brief  Aes構造体を初期化します。ヒープヒントを設定し、ASYNCハードウェアを使用する場合のIDも設定します。Aes構造体の使用が終了した際にwc_AesFreeを呼び出すのはユーザーに任されています。
    \return 0  成功
    \param aes  初期化にはAES構造
    \param heap  必要に応じてmalloc / freeに使用するヒントヒント
    _Example_
    \code
    Aes enc;
    void* hint = NULL;
    int devId = INVALID_DEVID; //if not using async INVALID_DEVID is default

    //heap hint could be set here if used

    wc_AesInit(&enc, hint, devId);
    \endcode
    \sa wc_AesSetKey
    \sa wc_AesSetIV
*/
int  wc_AesInit(Aes* aes, void* heap, int devId);

/*!
    \ingroup AES
    \brief Aes構造体に関連つけられたリソースを可能なら解放する。
    内部的にはノーオペレーションとなることもありますが、ベストプラクティスとしてどのケースでもこの関数を呼び出すことを推奨します。
    \return 戻り値なし
    \param aes FreeすべきAes構造体へのポインター
    _Example_
    \code
    Aes enc;
    void* hint = NULL;
    int devId = INVALID_DEVID; //if not using async INVALID_DEVID is default
    //heap hint could be set here if used
    wc_AesInit(&enc, hint, devId);
    // ... do some interesting things ...
    wc_AesFree(&enc);
    \endcode
    \sa wc_AesInit
*/
int  wc_AesFree(Aes* aes);

/*!
    \ingroup AES 
    \brief  CFBモードを持つAES。
    \return 0  失敗時の成功と否定的なエラー値
    \param aes  ブロック暗号化/復号化に使用するAESキー
    \param out  暗号テキストを保持するための出力バッファは、少なくともInpectBufferと同じくらい大きい必要があります）
    \param in  暗号化する入力プレーンテキストバッファ
    _Example_
    \code
    Aes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];

    //set up key with AES_ENCRYPTION as dir for both encrypt and decrypt

    if(wc_AesCfbEncrypt(&aes, cipher, plain, SIZE) != 0)
    {
        // Handle error
    }
    \endcode
    \sa wc_AesCfbDecrypt
    \sa wc_AesSetKey
*/
int wc_AesCfbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz);

/*!
    \ingroup AES 
    \brief  CFBモードを持つAES。
    \return 0  失敗時の成功と否定的なエラー値
    \param aes  ブロック暗号化/復号化に使用するAESキー
    \param out  復号化されたテキストを保持するための出力バッファは、少なくともinputBufferと同じ大きさでなければなりません）
    \param in  復号化する入力バッファ
    _Example_
    \code
    Aes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];

    //set up key with AES_ENCRYPTION as dir for both encrypt and decrypt

    if(wc_AesCfbDecrypt(&aes, plain, cipher, SIZE) != 0)
    {
        // Handle error
    }
    \endcode
    \sa wc_AesCfbEncrypt
    \sa wc_AesSetKey
*/
int wc_AesCfbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz);

/*!
    \ingroup AES 
    \brief  この関数は、RFC 5297に記載されているようにSIV（合成初期化ベクトル）暗号化を実行します。
    \return 0  暗号化に成功した場合
    \return BAD_FUNC_ARG  キー、SIV、または出力バッファがNULLの場合。キーサイズが32,48、または64バイトの場合にも返されます。
    \return Other  AESまたはCMAC操作が失敗した場合に返されるその他の負のエラー値。
    \param key  使用する鍵を含むバイトバッファ。
    \param keySz  キーバッファの長さ（バイト単位）。
    \param assoc  追加の認証された関連データ（AD）。
    \param assocSz  ADバッファのバイト数
    \param nonce  一度使用される数。ADと同じ方法でアルゴリズムによって使用されます。
    \param nonceSz  バイト単位のNOCEバッファの長さ。
    \param in  暗号化する平文のバッファ。
    \param inSz  平文バッファの長さ
    \param siv  S2VによるSIV出力（RFC 5297 2.4参照）。
    _Example_
    \code
    byte key[] = { some 32, 48, or 64 byte key };
    byte assoc[] = {0x01, 0x2, 0x3};
    byte nonce[] = {0x04, 0x5, 0x6};
    byte plainText[] = {0xDE, 0xAD, 0xBE, 0xEF};
    byte siv[AES_BLOCK_SIZE];
    byte cipherText[sizeof(plainText)];
    if (wc_AesSivEncrypt(key, sizeof(key), assoc, sizeof(assoc), nonce,
        sizeof(nonce), plainText, sizeof(plainText), siv, cipherText) != 0) {
        // failed to encrypt
    }
    \endcode
    \sa wc_AesSivDecrypt
*/


int wc_AesSivEncrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out);

/*!
    \ingroup AES 
    \brief  この機能は、RFC 5297に記載されているようにSIV（合成初期化ベクトル）復号化を実行する。
    \return 0  復号化に成功した場合
    \return BAD_FUNC_ARG  キー、SIV、または出力バッファがNULLの場合。キーサイズが32,48、または64バイトの場合にも返されます。
    \return AES_SIV_AUTH_E  S2Vによって派生したSIVが入力SIVと一致しない場合（RFC 5297 2.7を参照）。
    \return Other  AESまたはCMAC操作が失敗した場合に返されるその他の負のエラー値。
    \param key  使用する鍵を含むバイトバッファ。
    \param keySz  キーバッファの長さ（バイト単位）。
    \param assoc  追加の認証された関連データ（AD）。
    \param assocSz  ADバッファのバイト数
    \param nonce  一度使用される数。ADと同じ方法で、基礎となるアルゴリズムによって使用されます。
    \param nonceSz  バイト単位のNOCEバッファの長さ。
    \param in  復号化する暗号文バッファー。
    \param inSz  暗号文バッファの長さ
    \param siv  暗号文に付随するSIV（RFC 5297 2.4を参照）。
    _Example_
    \code
    byte key[] = { some 32, 48, or 64 byte key };
    byte assoc[] = {0x01, 0x2, 0x3};
    byte nonce[] = {0x04, 0x5, 0x6};
    byte cipherText[] = {0xDE, 0xAD, 0xBE, 0xEF};
    byte siv[AES_BLOCK_SIZE] = { the SIV that came with the ciphertext };
    byte plainText[sizeof(cipherText)];
    if (wc_AesSivDecrypt(key, sizeof(key), assoc, sizeof(assoc), nonce,
        sizeof(nonce), cipherText, sizeof(cipherText), siv, plainText) != 0) {
        // failed to decrypt
    }
    \endcode
    \sa wc_AesSivEncrypt
*/

int wc_AesSivDecrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out);
