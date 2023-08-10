/*!
    \ingroup MD5 
    \brief  この関数はMD5を初期化します。これはWC_MD5HASHによって自動的に呼び出されます。
    \return 0  初期化に成功したときに返されます。
    \return BAD_FUNC_ARG  MD5構造がNULL値として渡された場合に返されます。
    _Example_
    \code
    Md5 md5;
    byte* hash;
    if ((ret = wc_InitMd5(&md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       ret = wc_Md5Update(&md5, data, len);
       if (ret != 0) {
    	 // Md5 Update Failure Case.
       }
       ret = wc_Md5Final(&md5, hash);
      if (ret != 0) {
    	// Md5 Final Failure Case.
      }
    }
    \endcode
    \sa wc_Md5Hash
    \sa wc_Md5Update
    \sa wc_Md5Final
*/
int wc_InitMd5(wc_Md5*);

/*!
    \ingroup MD5 
    \brief  長さLENの提供されたバイト配列を絶えずハッシュするように呼び出すことができます。
    \return 0  データをダイジェストに正常に追加すると返されます。
    \return BAD_FUNC_ARG  MD5構造がNULLの場合、またはデータがNULLで、LENがゼロより大きい場合に返されます。DATAパラメーターがNULLでLENがゼロの場合、関数はエラーを返してはいけません。
    \param md5  暗号化に使用するMD5構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    Md5 md5;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitMd5(&md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       ret = wc_Md5Update(&md5, data, len);
       if (ret != 0) {
    	 // Md5 Update Error Case.
       }
       ret = wc_Md5Final(&md5, hash);
       if (ret != 0) {
    	// Md5 Final Error Case.
       }
    }
    \endcode
    \sa wc_Md5Hash
    \sa wc_Md5Final
    \sa wc_InitMd5
*/
int wc_Md5Update(wc_Md5* md5, const byte* data, word32 len);

/*!
    \ingroup MD5 
    \brief  データのハッシュを確定します。結果はハッシュに入れられます。MD5構造体がリセットされます。注：この関数は、habe_intel_qaが定義されている場合にintelqasymmd5（）を呼び出す結果も返します。
    \return 0  ファイナライズに成功したときに返されます。
    \return BAD_FUNC_ARG  MD5構造またはハッシュポインタがNULLで渡された場合に返されます。
    \param md5  暗号化に使用するMD5構造へのポインタ
    _Example_
    \code
    md5 md5[1];
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitMd5(md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       ret = wc_Md5Update(md5, data, len);
       if (ret != 0) {
    	// Md5 Update Failure Case.
       }
      ret = wc_Md5Final(md5, hash);
       if (ret != 0) {
	    // Md5 Final Failure Case.
       }
    }
    \endcode
    \sa wc_Md5Hash
    \sa wc_InitMd5
    \sa wc_Md5GetHash
*/
int wc_Md5Final(wc_Md5* md5, byte* hash);

/*!
    \ingroup MD5 
    \brief  MD5構造をリセットします。注：これは、wolfssl_ti_hashが定義されている場合にのみサポートされています。
    \return none  いいえ返します。
    _Example_
    \code
    Md5 md5;
    byte data[] = { Data to be hashed };
    word32 len = sizeof(data);

    if ((ret = wc_InitMd5(&md5)) != 0) {
        WOLFSSL_MSG("wc_InitMd5 failed");
    }
    else {
        wc_Md5Update(&md5, data, len);
        wc_Md5Final(&md5, hash);
        wc_Md5Free(&md5);
    }
    \endcode
    \sa wc_InitMd5
    \sa wc_Md5Update
    \sa wc_Md5Final
*/
void wc_Md5Free(wc_Md5*);

/*!
    \ingroup MD5 
    \brief  ハッシュデータを取得します。結果はハッシュに入れられます。MD5構造はリセットされません。
    \return none  いいえリターン
    \param md5  暗号化に使用するMD5構造へのポインタ。
    _Example_
    \code
    md5 md5[1];
    if ((ret = wc_InitMd5(md5)) != 0) {
       WOLFSSL_MSG("wc_Initmd5 failed");
    }
    else {
       wc_Md5Update(md5, data, len);
       wc_Md5GetHash(md5, hash);
    }
    \endcode
    \sa wc_Md5Hash
    \sa wc_Md5Final
    \sa wc_InitMd5
*/
int  wc_Md5GetHash(wc_Md5* md5, byte* hash);
