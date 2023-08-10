/*!
    \ingroup MD2 
    \brief  この関数はMD2を初期化します。これはWC_MD2HASHによって自動的に呼び出されます。
    \return 0  初期化に成功したときに返されます
    _Example_
    \code
    md2 md2[1];
    if ((ret = wc_InitMd2(md2)) != 0) {
       WOLFSSL_MSG("wc_Initmd2 failed");
    }
    else {
       wc_Md2Update(md2, data, len);
       wc_Md2Final(md2, hash);
    }
    \endcode
    \sa wc_Md2Hash
    \sa wc_Md2Update
    \sa wc_Md2Final
*/
void wc_InitMd2(Md2*);

/*!
    \ingroup MD2 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \param md2  暗号化に使用するMD2構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    md2 md2[1];
    byte data[] = { }; // Data to be hashed
    word32 len = sizeof(data);

    if ((ret = wc_InitMd2(md2)) != 0) {
       WOLFSSL_MSG("wc_Initmd2 failed");
    }
    else {
       wc_Md2Update(md2, data, len);
       wc_Md2Final(md2, hash);
    }
    \endcode
    \sa wc_Md2Hash
    \sa wc_Md2Final
    \sa wc_InitMd2
*/
void wc_Md2Update(Md2* md2, const byte* data, word32 len);

/*!
    \ingroup MD2 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。
    \return 0  ファイナライズに成功したときに返されます。
    \param md2  暗号化に使用するMD2構造へのポインタ
    _Example_
    \code
    md2 md2[1];
    byte data[] = { }; // Data to be hashed
    word32 len = sizeof(data);

    if ((ret = wc_InitMd2(md2)) != 0) {
       WOLFSSL_MSG("wc_Initmd2 failed");
    }
    else {
       wc_Md2Update(md2, data, len);
       wc_Md2Final(md2, hash);
    }
    \endcode
    \sa wc_Md2Hash
    \sa wc_Md2Final
    \sa wc_InitMd2
*/
void wc_Md2Final(Md2* md2, byte* hash);

/*!
    \ingroup MD2 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  データを正常にハッシュしたときに返されます。
    \return Memory_E  メモリエラー、メモリを割り当てることができません。これは、小さなスタックオプションが有効になっているだけです。
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    none
    \endcode
    \sa wc_Md2Hash
    \sa wc_Md2Final
    \sa wc_InitMd2
*/
int  wc_Md2Hash(const byte* data, word32 len, byte* hash);
