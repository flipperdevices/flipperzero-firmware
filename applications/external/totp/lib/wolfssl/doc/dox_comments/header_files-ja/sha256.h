/*!
    \ingroup SHA 
    \brief  この関数はSHA256を初期化します。これはWC_SHA256HASHによって自動的に呼び出されます。
    \return 0  初期化に成功したときに返されます
    _Example_
    \code
    Sha256 sha256[1];
    if ((ret = wc_InitSha256(sha256)) != 0) {
        WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
        wc_Sha256Update(sha256, data, len);
        wc_Sha256Final(sha256, hash);
    }
    \endcode
    \sa wc_Sha256Hash
    \sa wc_Sha256Update
    \sa wc_Sha256Final
*/
int wc_InitSha256(wc_Sha256*);

/*!
    \ingroup SHA 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \param sha256  暗号化に使用するSHA256構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    Sha256 sha256[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha256(sha256)) != 0) {
       WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
        wc_Sha256Update(sha256, data, len);
        wc_Sha256Final(sha256, hash);
    }
    \endcode
    \sa wc_Sha256Hash
    \sa wc_Sha256Final
    \sa wc_InitSha256
*/
int wc_Sha256Update(wc_Sha256* sha, const byte* data, word32 len);

/*!
    \ingroup SHA 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。SHA256構造体の状態をリセットします。
    \return 0  ファイナライズに成功したときに返されます。
    \param sha256  暗号化に使用するSHA256構造へのポインタ
    _Example_
    \code
    Sha256 sha256[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha256(sha256)) != 0) {
       WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
       wc_Sha256Update(sha256, data, len);
       wc_Sha256Final(sha256, hash);
    }
    \endcode
    \sa wc_Sha256Hash
    \sa wc_Sha256GetHash
    \sa wc_InitSha256
*/
int wc_Sha256Final(wc_Sha256* sha256, byte* hash);

/*!
    \ingroup SHA 
    \brief  SHA256構造をリセットします。注：これは、wolfssl_ti_hashが定義されている場合にのみサポートされています。
    \return none  いいえ返します。
    _Example_
    \code
    Sha256 sha256;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha256(&sha256)) != 0) {
        WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
        wc_Sha256Update(&sha256, data, len);
        wc_Sha256Final(&sha256, hash);
        wc_Sha256Free(&sha256);
    }
    \endcode
    \sa wc_InitSha256
    \sa wc_Sha256Update
    \sa wc_Sha256Final
*/
void wc_Sha256Free(wc_Sha256*);

/*!
    \ingroup SHA 
    \brief  ハッシュデータを取得します。結果はハッシュに入れられます。SHA256構造体の状態をリセットしません。
    \return 0  ファイナライズに成功したときに返されます。
    \param sha256  暗号化に使用するSHA256構造へのポインタ
    _Example_
    \code
    Sha256 sha256[1];
    if ((ret = wc_InitSha256(sha256)) != 0) {
       WOLFSSL_MSG("wc_InitSha256 failed");
    }
    else {
       wc_Sha256Update(sha256, data, len);
       wc_Sha256GetHash(sha256, hash);
    }
    \endcode
    \sa wc_Sha256Hash
    \sa wc_Sha256Final
    \sa wc_InitSha256
*/
int wc_Sha256GetHash(wc_Sha256* sha256, byte* hash);

/*!
    \ingroup SHA 
    \brief  SHA224構造を初期化するために使用されます。
    \return 0  成功
    \return 1  SHA224がNULLなので、エラーが返されました。
    _Example_
    \code
    Sha224 sha224;
    if(wc_InitSha224(&sha224) != 0)
    {
        // Handle error
    }
    \endcode
    \sa wc_Sha224Hash
    \sa wc_Sha224Update
    \sa wc_Sha224Final
*/
int wc_InitSha224(wc_Sha224*);

/*!
    \ingroup SHA 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  成功
    \return 1  関数が失敗した場合はエラーが返されます。
    \return BAD_FUNC_ARG  SHA224またはデータがNULLの場合、エラーが返されます。
    \param sha224  暗号化に使用するSHA224構造へのポインタ。
    \param data  ハッシュするデータ。
    _Example_
    \code
    Sha224 sha224;
    byte data[] = { /* Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha224(&sha224)) != 0) {
       WOLFSSL_MSG("wc_InitSha224 failed");
    }
    else {
      wc_Sha224Update(&sha224, data, len);
      wc_Sha224Final(&sha224, hash);
    }
    \endcode
    \sa wc_InitSha224
    \sa wc_Sha224Final
    \sa wc_Sha224Hash
*/
int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len);

/*!
    \ingroup SHA 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。SHA224構造体の状態をリセットします。
    \return 0  成功
    \return <0  エラー
    \param sha224  暗号化に使用するSHA224構造へのポインタ
    _Example_
    \code
    Sha224 sha224;
    byte data[] = { /* Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha224(&sha224)) != 0) {
        WOLFSSL_MSG("wc_InitSha224 failed");
    }
    else {
        wc_Sha224Update(&sha224, data, len);
        wc_Sha224Final(&sha224, hash);
    }
    \endcode
    \sa wc_InitSha224
    \sa wc_Sha224Hash
    \sa wc_Sha224Update
*/
int wc_Sha224Final(wc_Sha224* sha224, byte* hash);
