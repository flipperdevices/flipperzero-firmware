/*!
    \ingroup wolfCrypt 
    \brief  この関数は提供されたwc_hashtypeのOIDを返します。
    \return OID  戻り値0を超えてください
    \return HASH_TYPE_E  ハッシュ型はサポートされていません。
    \return BAD_FUNC_ARG  提供された引数の1つが正しくありません。
    _Example_
    \code
    enum wc_HashType hash_type = WC_HASH_TYPE_SHA256;
    int oid = wc_HashGetOID(hash_type);
    if (oid > 0) {
    	// Success
    }
    \endcode
    \sa wc_HashGetDigestSize
    \sa wc_Hash
*/
int wc_HashGetOID(enum wc_HashType hash_type);

/*!
    \ingroup wolfCrypt 
    \brief  この関数は、hash_typeのダイジェスト（出力）のサイズを返します。返品サイズは、WC_HASHに提供される出力バッファが十分に大きいことを確認するために使用されます。
    \return Success  正の戻り値は、ハッシュのダイジェストサイズを示します。
    \return Error  hash_typeがサポートされていない場合はhash_type_eを返します。
    \return Failure  無効なhash_typeが使用された場合、bad_func_argを返します。
    _Example_
    \code
    int hash_len = wc_HashGetDigestSize(hash_type);
    if (hash_len <= 0) {
    WOLFSSL_MSG("Invalid hash type/len");
    return BAD_FUNC_ARG;
    }
    \endcode
    \sa wc_Hash
*/
int wc_HashGetDigestSize(enum wc_HashType hash_type);

/*!
    \ingroup wolfCrypt 
    \brief  この関数は、提供されたデータバッファ上にハッシュを実行し、提供されたハッシュバッファにそれを返します。
    \return 0  そうでなければ、それ以外の誤り（bad_func_argやbuffer_eなど）。
    \param hash_type  "wc_hash_type_sha256"などの "enum wc_hashtype"からのハッシュ型。
    \param data  ハッシュへのデータを含むバッファへのポインタ。
    \param data_len  データバッファの長さ。
    \param hash  最後のハッシュを出力するために使用されるバッファへのポインタ。
    _Example_
    \code
    enum wc_HashType hash_type = WC_HASH_TYPE_SHA256;
    int hash_len = wc_HashGetDigestSize(hash_type);
    if (hash_len > 0) {
        int ret = wc_Hash(hash_type, data, data_len, hash_data, hash_len);
        if(ret == 0) {
		    // Success
        }
    }
    \endcode
    \sa wc_HashGetDigestSize
*/
int wc_Hash(enum wc_HashType hash_type,
    const byte* data, word32 data_len,
    byte* hash, word32 hash_len);

/*!
    \ingroup MD5 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  データを正常にハッシュしたときに返されます。
    \return Memory_E  メモリエラー、メモリを割り当てることができません。これは、小さなスタックオプションが有効になっているだけです。
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    const byte* data;
    word32 data_len;
    byte* hash;
    int ret;
    ...
    ret = wc_Md5Hash(data, data_len, hash);
    if (ret != 0) {
         // Md5 Hash Failure Case.
    }
    \endcode
    \sa wc_Md5Hash
    \sa wc_Md5Final
    \sa wc_InitMd5
*/
int wc_Md5Hash(const byte* data, word32 len, byte* hash);

/*!
    \ingroup SHA 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  うまく返されました...。
    \return Memory_E  メモリエラー、メモリを割り当てることができません。これは、小さなスタックオプションが有効になっているだけです。
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    none
    \endcode
    \sa wc_ShaHash
    \sa wc_ShaFinal
    \sa wc_InitSha
*/
int wc_ShaHash(const byte* data, word32 len, byte* hash);

/*!
    \ingroup SHA 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  うまく返されました...
    \return Memory_E  メモリエラー、メモリを割り当てることができません。これは、小さなスタックオプションが有効になっているだけです。
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    none
    \endcode
    \sa wc_Sha256Hash
    \sa wc_Sha256Final
    \sa wc_InitSha256
*/
int wc_Sha256Hash(const byte* data, word32 len, byte* hash);

/*!
    \ingroup SHA 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  成功
    \return <0  エラー
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    none
    \endcode
    \sa wc_InitSha224
    \sa wc_Sha224Update
    \sa wc_Sha224Final
*/
int wc_Sha224Hash(const byte* data, word32 len, byte* hash);

/*!
    \ingroup SHA 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  入力されたデータを正常にハッシュしたときに返されます
    \return Memory_E  メモリエラー、メモリを割り当てることができません。これは、小さなスタックオプションが有効になっているだけです。
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    none
    \endcode
    \sa wc_Sha512Hash
    \sa wc_Sha512Final
    \sa wc_InitSha512
*/
int wc_Sha512Hash(const byte* data, word32 len, byte* hash);

/*!
    \ingroup SHA 
    \brief  利便性機能は、すべてのハッシュを処理し、その結果をハッシュに入れます。
    \return 0  データを正常にハッシュしたときに返されます
    \return Memory_E  メモリエラー、メモリを割り当てることができません。これは、小さなスタックオプションが有効になっているだけです。
    \param data  ハッシュへのデータ
    \param len  データの長さ
    _Example_
    \code
    none
    \endcode
    \sa wc_Sha384Hash
    \sa wc_Sha384Final
    \sa wc_InitSha384
*/
int wc_Sha384Hash(const byte* data, word32 len, byte* hash);
