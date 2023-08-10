/*!
    \ingroup PKCS7 
    \brief  この関数は、DERフォーマットの証明書を使用してPKCS7構造を初期化します。空のPKCS7構造を初期化するには、NULL CERTとCERTSZの場合は0を渡すことができます。
    \return 0  PKCS7構造の初期化に成功しました
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return ASN_PARSE_E  証明書ヘッダーの解析中にエラーがある場合
    \return ASN_OBJECT_ID_E  証明書から暗号化タイプの解析中にエラーがある場合に返されます
    \return ASN_EXPECT_0_E  CERTファイルの暗号化仕様にフォーマットエラーがある場合
    \return ASN_BEFORE_DATE_E  日付が証明書開始日以前の場合返却
    \return ASN_AFTER_DATE_E  日付が証明書の有効期限の後にある場合に返されます
    \return ASN_BITSTR_E  証明書からビット文字列を解析したエラーがある場合に返されます。
    \return ECC_CURVE_OID_E  証明書からECCキーの解析中にエラーがある場合
    \return ASN_UNKNOWN_OID_E  証明書が不明なキーオブジェクトIDを使用している場合に返されます
    \return ASN_VERSION_E  allow_v1_extensionsオプションが定義されておらず、証明書がV1またはV2の証明書の場合に返されます。
    \return BAD_FUNC_ARG  証明書拡張機能の処理中にエラーがある場合
    \return ASN_CRIT_EXT_E  証明書の処理中になじみのない重要な拡張機能が発生した場合に返されます。
    \return ASN_SIG_OID_E  署名暗号化タイプが提供されたファイル内の証明書の暗号化タイプと同じでない場合に返されます。
    \return ASN_SIG_CONFIRM_E  認証署名が失敗したことを確認した場合に返されます
    \return ASN_NAME_INVALID_E  証明書の名前がCA名制約によって許可されていない場合に返されます。
    \return ASN_NO_SIGNER_E  証明書の真正性を確認するためのCA署名者がない場合に返されました
    \param pkcs7  デコードされた証明書を保存するPKCS7構造へのポインタ
    \param cert  PKCS7構造を初期化するためのDERフォーマットのASN.1証明書を含むバッファへのポインタ
    _Example_
    \code
    PKCS7 pkcs7;
    byte derBuff[] = { }; // initialize with DER-encoded certificate
    if ( wc_PKCS7_InitWithCert(&pkcs7, derBuff, sizeof(derBuff)) != 0 ) {
    	// error parsing certificate into pkcs7 format
    }
    \endcode
    \sa wc_PKCS7_Free
*/
int  wc_PKCS7_InitWithCert(PKCS7* pkcs7, byte* cert, word32 certSz);

/*!
    \ingroup PKCS7 
    \brief  この関数は、PKCS7の初期化装置によって割り当てられたメモリを解放します。
    \return none  いいえ返します。
    _Example_
    \code
    PKCS7 pkcs7;
    // initialize and use PKCS7 object

    wc_PKCS7_Free(pkcs7);
    \endcode
    \sa wc_PKCS7_InitWithCert
*/
void wc_PKCS7_Free(PKCS7* pkcs7);

/*!
    \ingroup PKCS7 
    \brief  この関数はPKCS7データコンテンツタイプを構築し、PKCS7構造をパーセル可能なPKCS7データパケットを含むバッファにエンコードします。
    \return Success  PKCS7データをバッファに正常にエンコードすると、PKCS7構造内の索引を返します。このインデックスは、出力バッファに書き込まれたバイトにも対応しています。
    \return BUFFER_E  指定されたバッファがエンコードされた証明書を保持するのに十分な大きさでない場合に返されます
    \param pkcs7  符号化するPKCS7構造へのポインタ
    \param output  エンコードされた証明書を保存するバッファへのポインタ
    _Example_
    \code
    PKCS7 pkcs7;
    int ret;

    byte derBuff[] = { }; // initialize with DER-encoded certificate
    byte pkcs7Buff[FOURK_BUF];

    wc_PKCS7_InitWithCert(&pkcs7, derBuff, sizeof(derBuff));
    // update message and data to encode
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;
    pkcs7.content = data;
    pkcs7.contentSz = dataSz;
    ... etc.

    ret = wc_PKCS7_EncodeData(&pkcs7, pkcs7Buff, sizeof(pkcs7Buff));
    if ( ret != 0 ) {
	    // error encoding into output buffer
    }
    \endcode
    \sa wc_PKCS7_InitWithCert
*/
int  wc_PKCS7_EncodeData(PKCS7* pkcs7, byte* output,
                                       word32 outputSz);

/*!
    \ingroup PKCS7 
    \brief  この関数はPKCS7署名付きデータコンテンツタイプを構築し、PKCS7構造をPARSable PKCS7署名付きデータパケットを含むバッファにエンコードします。
    \return Success  PKCS7データをバッファに正常にエンコードすると、PKCS7構造内の索引を返します。このインデックスは、出力バッファに書き込まれたバイトにも対応しています。
    \return BAD_FUNC_ARG  PKCS7構造が署名付きデータパケットを生成するための1つ以上の要求要素が欠落している場合に返されます。
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return PUBLIC_KEY_E  公開鍵の解析中にエラーがある場合
    \return RSA_BUFFER_E  バッファエラーが発生した場合は、小さすぎたり入力が大きすぎたりし過ぎました
    \return BUFFER_E  指定されたバッファがエンコードされた証明書を保持するのに十分な大きさでない場合に返されます
    \return MP_INIT_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_READ_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_CMP_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_INVMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MUL_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_ADD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MULMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_TO_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MEM  署名を生成するエラーがある場合は返却される可能性があります
    \param pkcs7  符号化するPKCS7構造へのポインタ
    \param output  エンコードされた証明書を保存するバッファへのポインタ
    _Example_
    \code
    PKCS7 pkcs7;
    int ret;

    byte data[] = {}; // initialize with data to sign
    byte derBuff[] = { }; // initialize with DER-encoded certificate
    byte pkcs7Buff[FOURK_BUF];

    wc_PKCS7_InitWithCert(&pkcs7, derBuff, sizeof(derBuff));
    // update message and data to encode
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;
    pkcs7.content = data;
    pkcs7.contentSz = dataSz;
    pkcs7.hashOID = SHAh;
    pkcs7.rng = &rng;
    ... etc.

    ret = wc_PKCS7_EncodeSignedData(&pkcs7, pkcs7Buff, sizeof(pkcs7Buff));
    if ( ret != 0 ) {
    	// error encoding into output buffer
    }

    wc_PKCS7_Free(&pkcs7);
    \endcode
    \sa wc_PKCS7_InitWithCert
    \sa wc_PKCS7_VerifySignedData
*/
int  wc_PKCS7_EncodeSignedData(PKCS7* pkcs7,
                                       byte* output, word32 outputSz);

/*!
    \ingroup PKCS7 
    \brief  この関数は、PKCS7の署名付きデータコンテンツタイプを構築し、PKCS7構造をエンコードし、Parsable PKCS7署名付きデータパケットを含むヘッダーおよびフッターバッファにエンコードします。これにはコンテンツは含まれません。ハッシュを計算してデータに提供する必要があります
    \return 0=Success 
    \return BAD_FUNC_ARG  PKCS7構造が署名付きデータパケットを生成するための1つ以上の要求要素が欠落している場合に返されます。
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return PUBLIC_KEY_E  公開鍵の解析中にエラーがある場合
    \return RSA_BUFFER_E  バッファエラーが発生した場合は、小さすぎたり入力が大きすぎたりし過ぎました
    \return BUFFER_E  指定されたバッファがエンコードされた証明書を保持するのに十分な大きさでない場合に返されます
    \return MP_INIT_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_READ_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_CMP_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_INVMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MUL_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_ADD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MULMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_TO_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MEM  署名を生成するエラーがある場合は返却される可能性があります
    \param pkcs7  符号化するPKCS7構造へのポインタ
    \param hashBuf  コンテンツデータの計算ハッシュへのポインタ
    \param hashSz  ダイジェストのサイズ
    \param outputHead  エンコードされた証明書ヘッダーを保存するバッファへのポインタ
    \param outputHeadSz  出力ヘッダーバッファのサイズが入力され、実際のサイズを返します。
    \param outputFoot  エンコードされた証明書フッターを保存するバッファへのポインタ
    _Example_
    \code
    PKCS7 pkcs7;
    int ret;
    byte derBuff[] = { }; // initialize with DER-encoded certificate
    byte data[] = {}; // initialize with data to sign
    byte pkcs7HeadBuff[FOURK_BUF/2];
    byte pkcs7FootBuff[FOURK_BUF/2];
    word32 pkcs7HeadSz = (word32)sizeof(pkcs7HeadBuff);
    word32 pkcs7FootSz = (word32)sizeof(pkcs7HeadBuff);
    enum wc_HashType hashType = WC_HASH_TYPE_SHA;
    byte   hashBuf[WC_MAX_DIGEST_SIZE];
    word32 hashSz = wc_HashGetDigestSize(hashType);

    wc_PKCS7_InitWithCert(&pkcs7, derBuff, sizeof(derBuff));
    // update message and data to encode
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;
    pkcs7.content = NULL;
    pkcs7.contentSz = dataSz;
    pkcs7.hashOID = SHAh;
    pkcs7.rng = &rng;
    ... etc.

    // calculate hash for content
    ret = wc_HashInit(&hash, hashType);
    if (ret == 0) {
        ret = wc_HashUpdate(&hash, hashType, data, sizeof(data));
        if (ret == 0) {
            ret = wc_HashFinal(&hash, hashType, hashBuf);
        }
        wc_HashFree(&hash, hashType);
    }

    ret = wc_PKCS7_EncodeSignedData_ex(&pkcs7, hashBuf, hashSz, pkcs7HeadBuff, 
        &pkcs7HeadSz, pkcs7FootBuff, &pkcs7FootSz);
    if ( ret != 0 ) {
        // error encoding into output buffer
    }

    wc_PKCS7_Free(&pkcs7);
    \endcode
    \sa wc_PKCS7_InitWithCert
    \sa wc_PKCS7_VerifySignedData_ex
*/
int wc_PKCS7_EncodeSignedData_ex(PKCS7* pkcs7, const byte* hashBuf, 
    word32 hashSz, byte* outputHead, word32* outputHeadSz, byte* outputFoot, 
    word32* outputFootSz);

/*!
    \ingroup PKCS7 
    \brief  この関数は、送信されたPKCS7の署名付きデータメッセージを取り、証明書リストと証明書失効リストを抽出してから署名を検証します。与えられたPKCS7構造に抽出されたコンテンツを格納します。
    \return 0  メッセージから情報を抽出することに成功しました
    \return BAD_FUNC_ARG  入力パラメータの1つが無効な場合は返されます
    \return ASN_PARSE_E  与えられたPKIMSGから解析中のエラーがある場合に返されます
    \return PKCS7_OID_E  与えられたPKIMSGが署名付きデータ型ではない場合に返されます
    \return ASN_VERSION_E  PKCS7署名者情報がバージョン1ではない場合に返されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return PUBLIC_KEY_E  公開鍵の解析中にエラーがある場合
    \return RSA_BUFFER_E  バッファエラーが発生した場合は、小さすぎたり入力が大きすぎたりし過ぎません
    \return BUFFER_E  指定されたバッファがエンコードされた証明書を保持するのに十分な大きさでない場合に返されます
    \return MP_INIT_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_READ_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_CMP_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_INVMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MUL_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_ADD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MULMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_TO_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MEM  署名を生成するエラーがある場合は返却される可能性があります
    \param pkcs7  解析された証明書を保存するPKCS7構造へのポインタ
    \param pkiMsg  署名されたメッセージを含むバッファへのポインタを検証および復号化する
    _Example_
    \code
    PKCS7 pkcs7;
    int ret;
    byte pkcs7Buff[] = {}; // the PKCS7 signature

    wc_PKCS7_InitWithCert(&pkcs7, NULL, 0);
    // update message and data to encode
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;
    pkcs7.content = data;
    pkcs7.contentSz = dataSz;
    ... etc.

    ret = wc_PKCS7_VerifySignedData(&pkcs7, pkcs7Buff, sizeof(pkcs7Buff));
    if ( ret != 0 ) {
    	// error encoding into output buffer
    }

    wc_PKCS7_Free(&pkcs7);
    \endcode
    \sa wc_PKCS7_InitWithCert
    \sa wc_PKCS7_EncodeSignedData
*/
int  wc_PKCS7_VerifySignedData(PKCS7* pkcs7,
                                       byte* pkiMsg, word32 pkiMsgSz);


/*!
    \ingroup PKCS7 
    \brief  この機能は、送信されたPKCS7署名付きデータメッセージをHASH /ヘッダー/フッターとして取り出してから、証明書リストと証明書失効リストを抽出してから、署名を検証します。与えられたPKCS7構造に抽出されたコンテンツを格納します。
    \return 0  メッセージから情報を抽出することに成功しました
    \return BAD_FUNC_ARG  入力パラメータの1つが無効な場合は返されます
    \return ASN_PARSE_E  与えられたPKIMSGから解析中のエラーがある場合に返されます
    \return PKCS7_OID_E  与えられたPKIMSGが署名付きデータ型ではない場合に返されます
    \return ASN_VERSION_E  PKCS7署名者情報がバージョン1ではない場合に返されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return PUBLIC_KEY_E  公開鍵の解析中にエラーがある場合
    \return RSA_BUFFER_E  バッファエラーが発生した場合は、小さすぎたり入力が大きすぎたりし過ぎません
    \return BUFFER_E  指定されたバッファがエンコードされた証明書を保持するのに十分な大きさでない場合に返されます
    \return MP_INIT_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_READ_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_CMP_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_INVMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MUL_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_ADD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MULMOD_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_TO_E  署名を生成するエラーがある場合は返却される可能性があります
    \return MP_MEM  署名を生成するエラーがある場合は返却される可能性があります
    \param pkcs7  解析された証明書を保存するPKCS7構造へのポインタ
    \param hashBuf  コンテンツデータの計算ハッシュへのポインタ
    \param hashSz  ダイジェストのサイズ
    \param pkiMsgHead  署名されたメッセージヘッダーを含むバッファへのポインタを検証およびデコードする
    \param pkiMsgHeadSz  署名付きメッセージヘッダーのサイズ
    \param pkiMsgFoot  署名されたメッセージフッターを含むバッファへのポインタを検証してデコードする
    _Example_
    \code
    PKCS7 pkcs7;
    int ret;
    byte data[] = {}; // initialize with data to sign
    byte pkcs7HeadBuff[] = {}; // initialize with PKCS7 header
    byte pkcs7FootBuff[] = {}; // initialize with PKCS7 footer
    enum wc_HashType hashType = WC_HASH_TYPE_SHA;
    byte   hashBuf[WC_MAX_DIGEST_SIZE];
    word32 hashSz = wc_HashGetDigestSize(hashType);

    wc_PKCS7_InitWithCert(&pkcs7, NULL, 0);
    // update message and data to encode
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;
    pkcs7.content = NULL;
    pkcs7.contentSz = dataSz;
    pkcs7.rng = &rng;
    ... etc.

    // calculate hash for content
    ret = wc_HashInit(&hash, hashType);
    if (ret == 0) {
        ret = wc_HashUpdate(&hash, hashType, data, sizeof(data));
        if (ret == 0) {
            ret = wc_HashFinal(&hash, hashType, hashBuf);
        }
        wc_HashFree(&hash, hashType);
    }

    ret = wc_PKCS7_VerifySignedData_ex(&pkcs7, hashBuf, hashSz, pkcs7HeadBuff, 
        sizeof(pkcs7HeadBuff), pkcs7FootBuff, sizeof(pkcs7FootBuff));
    if ( ret != 0 ) {
        // error encoding into output buffer
    }

    wc_PKCS7_Free(&pkcs7);
    \endcode
    \sa wc_PKCS7_InitWithCert
    \sa wc_PKCS7_EncodeSignedData_ex
*/
int wc_PKCS7_VerifySignedData_ex(PKCS7* pkcs7, const byte* hashBuf, 
    word32 hashSz, byte* pkiMsgHead, word32 pkiMsgHeadSz, byte* pkiMsgFoot, 
    word32 pkiMsgFootSz);

/*!
    \ingroup PKCS7 
    \brief  この関数は、PKCS7構造を編集し、PKCS7構造を符号化し、Parsable PKCS7エンベロープデータパケットを含むバッファに編集します。
    \return Success  エンベロープデータ形式でメッセージを正常にエンコードする上で返信され、出力バッファに書き込まれたサイズを返します。
    \return BAD_FUNC_ARG:  入力パラメータの1つが無効な場合、またはPKCS7構造が必要な要素を欠落している場合
    \return ALGO_ID_E  pkcs7構造がサポートされていないアルゴリズムタイプを使用している場合に返されます。現在、DESBとDES3Bのみがサポートされています
    \return BUFFER_E  与えられた出力バッファが小さすぎて出力データを保存する場合に返されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return RNG_FAILURE_E  暗号化の乱数発生器の初期化中にエラーがある場合
    \return DRBG_FAILED  暗号化に使用される乱数発生器を使用して数字を生成するエラーが発生した場合
    \param pkcs7  符号化するPKCS7構造へのポインタ
    \param output  エンコードされた証明書を保存するバッファへのポインタ
    _Example_
    \code
    PKCS7 pkcs7;
    int ret;

    byte derBuff[] = { }; // initialize with DER-encoded certificate
    byte pkcs7Buff[FOURK_BUF];

    wc_PKCS7_InitWithCert(&pkcs7, derBuff, sizeof(derBuff));
    // update message and data to encode
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;
    pkcs7.content = data;
    pkcs7.contentSz = dataSz;
    ... etc.

    ret = wc_PKCS7_EncodeEnvelopedData(&pkcs7, pkcs7Buff, sizeof(pkcs7Buff));
    if ( ret != 0 ) {
    	// error encoding into output buffer
    }
    \endcode
    \sa wc_PKCS7_InitWithCert
    \sa wc_PKCS7_DecodeEnvelopedData
*/
int  wc_PKCS7_EncodeEnvelopedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);

/*!
    \ingroup PKCS7 
    \brief  この関数はPKCS7エンベロープデータコンテンツタイプをアントラップして復号化し、メッセージを出力にデコードします。渡されたPKCS7オブジェクトの秘密鍵を使用してメッセージを復号化します。
    \return On  メッセージから情報を抽出するには、出力に書き込まれたバイト数を返します。
    \return BAD_FUNC_ARG  入力パラメータの1つが無効な場合は返されます
    \return ASN_PARSE_E  与えられたPKIMSGから解析中のエラーがある場合に返されます
    \return PKCS7_OID_E  与えられたPKIMSGがエンベロープデータ型ではない場合に返されます
    \return ASN_VERSION_E  PKCS7署名者情報がバージョン0ではない場合に返されます
    \return MEMORY_E  メモリの割り当て中にエラーが発生した場合に返されます
    \return ALGO_ID_E  pkcs7構造がサポートされていないアルゴリズムタイプを使用している場合に返されます。現在、Signature Generation for Signature GenerationのRSAKを使用して、DESBとDES3Bのみが暗号化でサポートされています。
    \return PKCS7_RECIP_E  提供された受信者と一致するエンベロープデータに受信者が見つからない場合
    \return RSA_BUFFER_E  バッファエラーが原因でRSAシグネチャ検証中にエラーがある場合は、小さすぎたり入力が大きすぎたりすると元に戻されます。
    \return MP_INIT_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_READ_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_CMP_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_INVMOD_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_MOD_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_MUL_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_ADD_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_MULMOD_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_TO_E  署名検証中にエラーがある場合は返却される可能性があります
    \return MP_MEM  署名検証中にエラーがある場合は返却される可能性があります
    \param pkcs7  エンベロープデータパッケージをデコードする秘密鍵を含むPKCS7構造へのポインタ
    \param pkiMsg  エンベロープデータパッケージを含むバッファへのポインタ
    \param pkiMsgSz  包み込まれたデータパッケージのサイズ
    \param output  デコードされたメッセージを保存するバッファへのポインタ
    _Example_
    \code
    PKCS7 pkcs7;
    byte received[] = { }; // initialize with received enveloped message
    byte decoded[FOURK_BUF];
    int decodedSz;

    // initialize pkcs7 with certificate
    // update key
    pkcs7.privateKey = key;
    pkcs7.privateKeySz = keySz;

    decodedSz = wc_PKCS7_DecodeEnvelopedData(&pkcs7, received,
    sizeof(received),decoded, sizeof(decoded));
    if ( decodedSz != 0 ) {
    	// error decoding message
    }
    \endcode
    \sa wc_PKCS7_InitWithCert
    \sa wc_PKCS7_EncodeEnvelopedData
*/
int  wc_PKCS7_DecodeEnvelopedData(PKCS7* pkcs7, byte* pkiMsg,
                                          word32 pkiMsgSz, byte* output,
                                          word32 outputSz);
