/*!
    \ingroup BLAKE2 
    \brief  この関数はBlake2 Hash関数で使用するためのBlake2b構造を初期化します。
    \return 0  Blake2B構造の初期化に成功し、ダイジェストサイズを設定したときに返されます。
    \param b2b  初期化するためにBlake2b構造へのポインタ
    _Example_
    \code
    Blake2b b2b;
    // initialize Blake2b structure with 64 byte digest
    wc_InitBlake2b(&b2b, 64);
    \endcode
    \sa wc_Blake2bUpdate
*/
int wc_InitBlake2b(Blake2b* b2b, word32 digestSz);

/*!
    \ingroup BLAKE2 
    \brief  この関数は、与えられた入力データとBlake2Bハッシュを更新します。この関数は、wc_initblake2bの後に呼び出され、最後のハッシュ：wc_blake2bfinalの準備ができているまで繰り返します。
    \return 0  与えられたデータを使用してBlake2B構造を正常に更新すると返されます。
    \return -1  入力データの圧縮中に障害が発生した場合
    \param b2b  更新するBlake2b構造へのポインタ
    \param data  追加するデータを含むバッファへのポインタ
    _Example_
    \code
    int ret;
    Blake2b b2b;
    // initialize Blake2b structure with 64 byte digest
    wc_InitBlake2b(&b2b, 64);

    byte plain[] = { // initialize input };

    ret = wc_Blake2bUpdate(&b2b, plain, sizeof(plain));
    if( ret != 0) {
    	// error updating blake2b
    }
    \endcode
    \sa wc_InitBlake2b
    \sa wc_Blake2bFinal
*/
int wc_Blake2bUpdate(Blake2b* b2b, const byte* data, word32 sz);

/*!
    \ingroup BLAKE2 
    \brief  この関数は、以前に供給された入力データのBlake2bハッシュを計算します。出力ハッシュは長さREQUESTSZ、あるいは要求された場合はB2B構造のDigestSZを使用します。この関数は、wc_initblake2bの後に呼び出され、wc_blake2bupdateは必要な各入力データに対して処理されています。
    \return 0  Blake2B Hashの計算に成功したときに返されました
    \return -1  blake2bハッシュを解析している間に失敗がある場合
    \param b2b  更新するBlake2b構造へのポインタ
    \param final  Blake2Bハッシュを保存するバッファへのポインタ。長さrequestszにする必要があります
    _Example_
    \code
    int ret;
    Blake2b b2b;
    byte hash[64];
    // initialize Blake2b structure with 64 byte digest
    wc_InitBlake2b(&b2b, 64);
    ... // call wc_Blake2bUpdate to add data to hash

    ret = wc_Blake2bFinal(&b2b, hash, 64);
    if( ret != 0) {
    	// error generating blake2b hash
    }
    \endcode
    \sa wc_InitBlake2b
    \sa wc_Blake2bUpdate
*/
int wc_Blake2bFinal(Blake2b* b2b, byte* final, word32 requestSz);
