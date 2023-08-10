/*!
    \ingroup ASN 
    \brief  この関数はデフォルトの証明書を初期化します。デフォルトのオプション：version = 3（0x2）、sigtype = sha_with_rsa、issuer =空白、dayValid = 500、selfsigned = 1（true）発行者としての件名=空白
    \return none  いいえ返します。
    _Example_
    \code
    Cert myCert;
    wc_InitCert(&myCert);
    \endcode
    \sa wc_MakeCert
    \sa wc_MakeCertReq
*/
int wc_InitCert(Cert*);

/*!
    \ingroup ASN 
    \brief  CA署名付き証明書を作成するために使用されます。被写体情報が入力された後に呼び出されました。この関数は、証明書入力からX509証明書V3 RSAまたはECCを作成します。その後、この証明書をDerbufferに書き込みます。証明書を生成するためのRSAKEYまたはECCKEYのいずれかを取ります。このメソッドが呼び出される前に、証明書をWC_INITCERTで初期化する必要があります。
    \return Success  指定された入力証明書からX509証明書を正常に行うと、生成された証明書のサイズを返します。
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return BUFFER_E  提供されたDerbufferが生成された証明書を保存するには小さすぎる場合に返されます
    \return Others  証明書の生成が成功しなかった場合、追加のエラーメッセージが返される可能性があります。
    \param cert  初期化された証明書構造へのポインタ
    \param derBuffer  生成された証明書を保持するバッファへのポインタ
    \param derSz  証明書を保存するバッファのサイズ
    \param rsaKey  証明書の生成に使用されるRSAキーを含むRSAKEY構造体へのポインタ
    \param eccKey  証明書の生成に使用されるECCキーを含むECCKEY構造体へのポインタ
    _Example_
    \code
    Cert myCert;
    wc_InitCert(&myCert);
    WC_RNG rng;
    //initialize rng;
    RsaKey key;
    //initialize key;
    byte * derCert = malloc(FOURK_BUF);
    word32 certSz;
    certSz = wc_MakeCert(&myCert, derCert, FOURK_BUF, &key, NULL, &rng);
    \endcode
    \sa wc_InitCert
    \sa wc_MakeCertReq
*/
int  wc_MakeCert(Cert* cert, byte* derBuffer, word32 derSz, RsaKey* rsaKey,
                             ecc_key* eccKey, WC_RNG* rng);

/*!
    \ingroup ASN 
    \brief  この関数は、入力証明書を使用して証明書署名要求を行い、出力をDerbufferに書き込みます。証明書要求を生成するRSAKEYまたはECCKEYのどちらかを取ります。この関数が証明書要求に署名するためにwc_signcert（）を呼び出す必要があります。この関数の使用例については、WolfCryptテストアプリケーション（./wolfcrypt/test/test.c）を参照してください。
    \return Success  指定された入力証明書からX.509証明書要求を正常に行うと、生成された証明書要求のサイズを返します。
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return BUFFER_E  提供されたDerbufferが生成された証明書を保存するには小さすぎる場合に返されます
    \return Other  証明書要求生成が成功しなかった場合、追加のエラーメッセージが返される可能性があります。
    \param cert  初期化された証明書構造へのポインタ
    \param derBuffer  生成された証明書要求を保持するバッファへのポインタ
    \param derSz  証明書要求を保存するバッファのサイズ
    \param rsaKey  証明書要求を生成するために使用されるRSAキーを含むRSAKEY構造体へのポインタ
    _Example_
    \code
    Cert myCert;
    // initialize myCert
    EccKey key;
    //initialize key;
    byte* derCert = (byte*)malloc(FOURK_BUF);

    word32 certSz;
    certSz = wc_MakeCertReq(&myCert, derCert, FOURK_BUF, NULL, &key);
    \endcode
    \sa wc_InitCert
    \sa wc_MakeCert
*/
int  wc_MakeCertReq(Cert* cert, byte* derBuffer, word32 derSz,
                                    RsaKey* rsaKey, ecc_key* eccKey);

/*!
    \ingroup ASN 
    \brief  この関数はバッファーに署名し、署名をバッファの最後に追加します。署名の種類を取ります。CA署名付き証明書を作成する場合は、wc_makecert（）またはwc_makecertreq（）の後に呼び出す必要があります。
    \return Success  証明書に正常に署名する場合は、CERTの新しいサイズ（署名を含む）を返します。
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return BUFFER_E  提供された証明書を保存するには提供されたバッファが小さすぎる場合に返されます。
    \return Other  証明書の生成が成功しなかった場合、追加のエラーメッセージが返される可能性があります。
    \param requestSz  署名したことを要求している証明書本文のサイズ
    \param sType  作成する署名の種類。有効なオプションは次のとおりです.CTC_MD5WRSA、CTC_SHAWRSA、CTC_SHAWECDSA、CTC_SHA256WECDSA、ANDCTC_SHA256WRSA
    \param buffer  署名する証明書を含むバッファへのポインタ。成功：新たに署名された証明書を保持します
    \param buffSz  新たに署名された証明書を保存するバッファの（合計）サイズ
    \param rsaKey  証明書に署名するために使用されるRSAキーを含むRSAKEY構造体へのポインタ
    \param eccKey  証明書に署名するために使用されるECCキーを含むECCKey構造体へのポインタ
    _Example_
    \code
    Cert myCert;
    byte* derCert = (byte*)malloc(FOURK_BUF);
    // initialize myCert, derCert
    RsaKey key;
    // initialize key;
    WC_RNG rng;
    // initialize rng

    word32 certSz;
    certSz = wc_SignCert(myCert.bodySz, myCert.sigType,derCert,FOURK_BUF,
    &key, NULL,
    &rng);
    \endcode
    \sa wc_InitCert
    \sa wc_MakeCert
*/
int  wc_SignCert(int requestSz, int sigType, byte* derBuffer,
                 word32 derSz, RsaKey* rsaKey, ecc_key* eccKey, WC_RNG* rng);

/*!
    \ingroup ASN 
    \brief  この関数は、以前の2つの関数、wc_makecert、および自己署名のためのwc_signcertの組み合わせです（前の関数はCA要求に使用される場合があります）。証明書を作成してから、それに署名し、自己署名証明書を生成します。
    \return Success  証明書に正常に署名する場合は、CERTの新しいサイズを返します。
    \return MEMORY_E  xmallocでメモリを割り当てるエラーがある場合
    \return BUFFER_E  提供された証明書を保存するには提供されたバッファが小さすぎる場合に返されます。
    \return Other  証明書の生成が成功しなかった場合、追加のエラーメッセージが返される可能性があります。
    \param cert  作成して署名する証明書へのポインタ
    \param buffer  署名付き証明書を保持するためのバッファへのポインタ
    \param buffSz  署名付き証明書を保存するバッファのサイズ
    \param key  証明書に署名するために使用されるRSAキーを含むRSAKEY構造体へのポインタ
    _Example_
    \code
    Cert myCert;
    byte* derCert = (byte*)malloc(FOURK_BUF);
    // initialize myCert, derCert
    RsaKey key;
    // initialize key;
    WC_RNG rng;
    // initialize rng

    word32 certSz;
    certSz = wc_MakeSelfCert(&myCert, derCert, FOURK_BUF, &key, NULL, &rng);
    \endcode
    \sa wc_InitCert
    \sa wc_MakeCert
    \sa wc_SignCert
*/
int  wc_MakeSelfCert(Cert* cert, byte* derBuffer, word32 derSz, RsaKey* key,
                             WC_RNG* rng);
