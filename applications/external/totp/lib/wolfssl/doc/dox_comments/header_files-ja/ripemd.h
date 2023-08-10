/*!
    \ingroup RIPEMD 
    \brief  この関数は、RIPemdのダイジェスト、バッファ、LOLEN ,HILENを初期化することによってRIPemd構造を初期化します。
    \return 0  機能の実行に成功したことに戻ります。RIPEMD構造が初期化されます。
    \return BAD_FUNC_ARG  RIPEMD構造がNULLの場合に返されます。
    _Example_
    \code
    RipeMd md;
    int ret;
    ret = wc_InitRipeMd(&md);
    if (ret != 0) {
    	// Failure case.
    }
    \endcode
    \sa wc_RipeMdUpdate
    \sa wc_RipeMdFinal
*/
int wc_InitRipeMd(RipeMd*);

/*!
    \ingroup RIPEMD 
    \brief  この関数はデータ入力のRIPemdダイジェストを生成し、結果をRIPemd-> Digestバッファに格納します。WC_RIPEMDUPDATEを実行した後、生成されたRIPemd-> Digestを既知の認証タグに比較してメッセージの信頼性を比較する必要があります。
    \return 0  機能の実行に成功したことに戻ります。
    \return BAD_FUNC_ARG  RIPEMD構造がNULLの場合、またはデータがNULLで、LENがゼロでない場合に返されます。データがNULLであり、LENが0の場合、この関数は実行されるはずです。
    \param ripemd:  WC_INTRIPEMDで初期化されるRIPEMD構造へのポインタ
    \param data  ハッシュするデータ
    _Example_
    \code
    const byte* data; // The data to be hashed
    ....
    RipeMd md;
    int ret;
    ret = wc_InitRipeMd(&md);
    if (ret == 0) {
    ret = wc_RipeMdUpdate(&md, plain, sizeof(plain));
    if (ret != 0) {
	// Failure case …
    \endcode
    \sa wc_InitRipeMd
    \sa wc_RipeMdFinal
*/
int wc_RipeMdUpdate(RipeMd* ripemd, const byte* data, word32 len);

/*!
    \ingroup RIPEMD 
    \brief  この関数は計算されたダイジェストをハッシュにコピーします。無傷のブロックがある場合、この方法ではブロックを0Sでパッケージし、ハッシュにコピーする前にそのブロックのラウンドをダイジェストに含めます。RIPEMDの状態がリセットされます。
    \return 0  機能の実行に成功したことに戻ります。RIPEMD構造の状態がリセットされました。
    \return BAD_FUNC_ARG  RIPEMD構造体またはハッシュパラメータがNULLの場合に返されます。
    \param ripemd  WC_INITRIPEMDで初期化するRIPEMD構造へのポインタ、およびWC_RIPEMDUPDATEからハッシュを含む。状態はリセットされます
    _Example_
    \code
    RipeMd md;
    int ret;
    byte   digest[RIPEMD_DIGEST_SIZE];
    const byte* data; // The data to be hashed
    ...
    ret = wc_InitRipeMd(&md);
    if (ret == 0) {
    ret = wc_RipeMdUpdate(&md, plain, sizeof(plain));
    	if (ret != 0) {
    		// RipeMd Update Failure Case.
    }
    ret = wc_RipeMdFinal(&md, digest);
    if (ret != 0) {
    	// RipeMd Final Failure Case.
    }...
    \endcode
    \sa none
*/
int wc_RipeMdFinal(RipeMd* ripemd, byte* hash);
