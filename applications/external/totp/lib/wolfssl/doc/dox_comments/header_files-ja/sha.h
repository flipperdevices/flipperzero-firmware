/*!
    \ingroup SHA 
    \brief  この関数はSHAを初期化します。これは自動的にWC_Shahashによって呼び出されます。
    \return 0  初期化に成功したときに返されます
    _Example_
    \code
    Sha sha[1];
    if ((ret = wc_InitSha(sha)) != 0) {
       WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
       wc_ShaUpdate(sha, data, len);
       wc_ShaFinal(sha, hash);
    }
    \endcode
    \sa wc_ShaHash
    \sa wc_ShaUpdate
    \sa wc_ShaFinal
*/
int wc_InitSha(wc_Sha*);

/*!
    \ingroup SHA 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \param sha  暗号化に使用するSHA構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    Sha sha[1];
    byte data[] = { // Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha(sha)) != 0) {
       WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
       wc_ShaUpdate(sha, data, len);
       wc_ShaFinal(sha, hash);
    }
    \endcode
    \sa wc_ShaHash
    \sa wc_ShaFinal
    \sa wc_InitSha
*/
int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len);

/*!
    \ingroup SHA 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。SHA構造体の状態をリセットします。
    \return 0  ファイナライズに成功したときに返されます。
    \param sha  暗号化に使用するSHA構造へのポインタ
    _Example_
    \code
    Sha sha[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitSha(sha)) != 0) {
       WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
       wc_ShaUpdate(sha, data, len);
       wc_ShaFinal(sha, hash);
    }
    \endcode
    \sa wc_ShaHash
    \sa wc_InitSha
    \sa wc_ShaGetHash
*/
int wc_ShaFinal(wc_Sha* sha, byte* hash);

/*!
    \ingroup SHA 
    \brief  初期化されたSHA構造体によって使用されるメモリをクリーンアップするために使用されます。注：これは、wolfssl_ti_hashが定義されている場合にのみサポートされています。
    \return No  戻り値。
    _Example_
    \code
    Sha sha;
    wc_InitSha(&sha);
    // Use sha
    wc_ShaFree(&sha);
    \endcode
    \sa wc_InitSha
    \sa wc_ShaUpdate
    \sa wc_ShaFinal
*/
void wc_ShaFree(wc_Sha*);

/*!
    \ingroup SHA 
    \brief  ハッシュデータを取得します。結果はハッシュに入れられます。SHA構造体の状態をリセットしません。
    \return 0  ファイナライズに成功したときに返されます。
    \param sha  暗号化に使用するSHA構造へのポインタ
    _Example_
    \code
    Sha sha[1];
    if ((ret = wc_InitSha(sha)) != 0) {
    WOLFSSL_MSG("wc_InitSha failed");
    }
    else {
        wc_ShaUpdate(sha, data, len);
        wc_ShaGetHash(sha, hash);
    }
    \endcode
    \sa wc_ShaHash
    \sa wc_ShaFinal
    \sa wc_InitSha
*/
int wc_ShaGetHash(wc_Sha* sha, byte* hash);
