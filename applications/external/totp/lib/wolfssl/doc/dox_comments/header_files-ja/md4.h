/*!
    \ingroup MD4 
    \brief  この関数はMD4を初期化します。これはWC_MD4HASHによって自動的に呼び出されます。
    \return 0  初期化に成功したときに返されます
    _Example_
    \code
    md4 md4[1];
    if ((ret = wc_InitMd4(md4)) != 0) {
       WOLFSSL_MSG("wc_Initmd4 failed");
    }
    else {
       wc_Md4Update(md4, data, len);
       wc_Md4Final(md4, hash);
    }
    \endcode
    \sa wc_Md4Hash
    \sa wc_Md4Update
    \sa wc_Md4Final
*/
void wc_InitMd4(Md4*);

/*!
    \ingroup MD4 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \param md4  暗号化に使用するMD4構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    md4 md4[1];
    byte data[] = { }; // Data to be hashed
    word32 len = sizeof(data);

    if ((ret = wc_InitMd4(md4)) != 0) {
       WOLFSSL_MSG("wc_Initmd4 failed");
    }
    else {
       wc_Md4Update(md4, data, len);
       wc_Md4Final(md4, hash);
    }
    \endcode
    \sa wc_Md4Hash
    \sa wc_Md4Final
    \sa wc_InitMd4
*/
void wc_Md4Update(Md4* md4, const byte* data, word32 len);

/*!
    \ingroup MD4 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。
    \return 0  ファイナライズに成功したときに返されます。
    \param md4  暗号化に使用するMD4構造へのポインタ
    _Example_
    \code
    md4 md4[1];
    if ((ret = wc_InitMd4(md4)) != 0) {
        WOLFSSL_MSG("wc_Initmd4 failed");
    }
    else {
        wc_Md4Update(md4, data, len);
        wc_Md4Final(md4, hash);
    }
    \endcode
    \sa wc_Md4Hash
    \sa wc_Md4Final
    \sa wc_InitMd4
*/
void wc_Md4Final(Md4* md4, byte* hash);
