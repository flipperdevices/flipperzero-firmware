/*!
    \ingroup SHA 
    \brief  この関数はSHA512を初期化します。これはWC_SHA512HASHによって自動的に呼び出されます。
    \return 0  初期化に成功したときに返されます
    _Example_
    \code
    Sha512 sha512[1];
    if ((ret = wc_InitSha512(sha512)) != 0) {
       WOLFSSL_MSG("wc_InitSha512 failed");
    }
    else {
       wc_Sha512Update(sha512, data, len);
       wc_Sha512Final(sha512, hash);
    }
    \endcode
    \sa wc_Sha512Hash
    \sa wc_Sha512Update
    \sa wc_Sha512Final
*/
int wc_InitSha512(wc_Sha512*);

/*!
    \ingroup SHA 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \param sha512  暗号化に使用するSHA512構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    Sha512 sha512[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha512(sha512)) != 0) {
       WOLFSSL_MSG("wc_InitSha512 failed");
    }
    else {
       wc_Sha512Update(sha512, data, len);
       wc_Sha512Final(sha512, hash);
    }
    \endcode
    \sa wc_Sha512Hash
    \sa wc_Sha512Final
    \sa wc_InitSha512
*/
int wc_Sha512Update(wc_Sha512* sha, const byte* data, word32 len);

/*!
    \ingroup SHA 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。
    \return 0  ハッシュを確定するとうまく返されました。
    \param sha512  暗号化に使用するSHA512構造へのポインタ
    _Example_
    \code
    Sha512 sha512[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha512(sha512)) != 0) {
        WOLFSSL_MSG("wc_InitSha512 failed");
    }
    else {
       wc_Sha512Update(sha512, data, len);
       wc_Sha512Final(sha512, hash);
    }
    \endcode
    \sa wc_Sha512Hash
    \sa wc_Sha512Final
    \sa wc_InitSha512
*/
int wc_Sha512Final(wc_Sha512* sha512, byte* hash);

/*!
    \ingroup SHA 
    \brief  この関数はSHA384を初期化します。これはWC_SHA384HASHによって自動的に呼び出されます。
    \return 0  初期化に成功したときに返されます
    _Example_
    \code
    Sha384 sha384[1];
    if ((ret = wc_InitSha384(sha384)) != 0) {
       WOLFSSL_MSG("wc_InitSha384 failed");
    }
    else {
       wc_Sha384Update(sha384, data, len);
       wc_Sha384Final(sha384, hash);
    }
    \endcode
    \sa wc_Sha384Hash
    \sa wc_Sha384Update
    \sa wc_Sha384Final
*/
int wc_InitSha384(wc_Sha384*);

/*!
    \ingroup SHA 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \param sha384  暗号化に使用するSHA384構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    Sha384 sha384[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha384(sha384)) != 0) {
       WOLFSSL_MSG("wc_InitSha384 failed");
    }
    else {
       wc_Sha384Update(sha384, data, len);
       wc_Sha384Final(sha384, hash);
    }
    \endcode
    \sa wc_Sha384Hash
    \sa wc_Sha384Final
    \sa wc_InitSha384
*/
int wc_Sha384Update(wc_Sha384* sha, const byte* data, word32 len);

/*!
    \ingroup SHA 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。
    \return 0  ファイナライズに成功したときに返されます。
    \param sha384  暗号化に使用するSHA384構造へのポインタ
    _Example_
    \code
    Sha384 sha384[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha384(sha384)) != 0) {
       WOLFSSL_MSG("wc_InitSha384 failed");
    }
    else {
       wc_Sha384Update(sha384, data, len);
       wc_Sha384Final(sha384, hash);
    }
    \endcode
    \sa wc_Sha384Hash
    \sa wc_Sha384Final
    \sa wc_InitSha384
*/
int wc_Sha384Final(wc_Sha384* sha384, byte* hash);
