/*!
     \ingroup CMAC
     \brief Cmac 構造体をデフォルトで初期化する
     \return 成功したら 0 を返す
     \param cmac Cmac 構造体へのポインタ
     \param キー キー ポインタ
     \param keySz キー ポインタのサイズ (16、24、または 32)
     \param type 常に WC_CMAC_AES = 1
     \param 未使用 使用されていません。互換性に関する将来の潜在的な使用のために存在します

     _例_
     \code
     Cmac cmac[1];
     ret = wc_InitCmac(cmac、キー、keySz、WC_CMAC_AES、NULL);
     もし (ret == 0) {
         ret = wc_CmacUpdate(cmac、in、inSz);
     }
     もし (ret == 0) {
         ret = wc_CmacFinal(cmac, out, outSz);
     }
     \endcode

     \sa wc_InitCmac_ex
     \sa wc_CmacUpdate
     \sa wc_CmacFinal
*/
int wc_InitCmac(Cmac* cmac,
                 const byte* キー、word32 keySz、
                 int型、void*未使用);

/*!
     \ingroup CMAC
     \brief Cmac 構造体をデフォルトで初期化する
     \return 成功したら 0 を返す
     \param cmac Cmac 構造体へのポインタ
     \param キー キー ポインタ
     \param keySz キー ポインタのサイズ (16、24、または 32)
     \param type 常に WC_CMAC_AES = 1
     \param 未使用 使用されていません。互換性に関する将来の潜在的な使用のために存在します
     \param heap 動的割り当てに使用されるヒープ ヒントへのポインター。 通常、スタティック メモリ オプションで使用されます。 NULL にすることができます。
     \param devId 非同期ハードウェアで使用する ID。 非同期ハードウェアを使用していない場合は、INVALID_DEVID に設定します。

     _例_
     \code
     Cmac cmac[1];
     ret = wc_InitCmac_ex(cmac、キー、keySz、WC_CMAC_AES、NULL、NULL、INVALID_DEVID);
     もし (ret == 0) {
         ret = wc_CmacUpdate(cmac、in、inSz);
     }
     もし (ret == 0) {
         ret = wc_CmacFinal(cmac, out, &outSz);
     }
     \endcode

     \sa wc_InitCmac_ex
     \sa wc_CmacUpdate
     \sa wc_CmacFinal
*/
int wc_InitCmac_ex(Cmac* cmac,
                 const byte* キー、word32 keySz、
                 int 型、void* 未使用、void* ヒープ、int devId);

/*!
     \ingroup CMAC
     \brief 暗号ベースのメッセージ認証コード入力データを追加
     \return 成功したら 0 を返す
     \param cmac Cmac 構造体へのポインタ
     \param in 処理する入力データの 
     \param inSz 入力データのサイズ

     _例_
     \code
     ret = wc_CmacUpdate(cmac、in、inSz);
     \endcode

     \sa wc_InitCmac
     \sa wc_CmacFinal
*/
int wc_CmacUpdate(Cmac* cmac,
                   const byte* in, word32 inSz);

/*!
     \ingroup CMAC
     \brief 暗号ベースのメッセージ認証コードを使用して最終結果を生成する
     \return 成功したら 0 を返す
     \param cmac Cmac 構造体へのポインタ
     \param out 結果を返すポインタ
     \param outSz 出力のポインタサイズ (in/out)

     _例_
     \code
     ret = wc_CmacFinal(cmac, out, &outSz);
     \endcode

     \sa wc_InitCmac
     \sa wc_CmacFinal
*/
int wc_CmacFinal(Cmac* cmac,
                  byte* out, word32* outSz);

/*!
     \ingroup CMAC
     \brief CMAC を生成するためのシングル ショット関数
     \return 成功したら 0 を返す
     \param out 結果を返すポインタ
     \param outSz 出力のポインタサイズ (in/out)
     \param in 処理する入力データのポインタ 
     \param inSz 入力データのサイズ
     \param キー キー ポインタ
     \param keySz キー ポインタのサイズ (16、24、または 32)

     _例_
     \code
     ret = wc_AesCmacGenerate(mac, &macSz, msg, msgSz, key, keySz);
     \endcode

     \sa wc_AesCmacVerify
*/
int wc_AesCmacGenerate(byte* out, word32* outSz,
                        const byte* in、word32 inSz、
                        const byte* key, word32 keySz);

/*!
     \ingroup CMAC
     \brief CMAC を検証するためのシングル ショット機能
     \return 成功したら 0 を返す
     \param 結果を返すチェック ポインタ
     \param checkSz チェックアウト バッファのサイズ
     \param in 処理する入力データのポインタ
     \param inSz 入力データのサイズ
     \param キー キー ポインタ
     \param keySz キー ポインタのサイズ (16、24、または 32)

     _例_
     \code
     ret = wc_AesCmacVerify(mac, macSz, msg, msgSz, key, keySz);
     \endcode

     \sa wc_AesCmacGenerate
*/
int wc_AesCmacVerify(const byte* check, word32 checkSz,
                      const byte* in、word32 inSz、
                      const byte* key, word32 keySz);


/*!
     \ingroup CMAC
     \brief ハードウェアがシングル ショットを必要とし、更新をメモリにキャッシュする必要がある場合にのみ、WOLFSSL_HASH_KEEP で使用されます
     \return 成功したら 0 を返す
     処理する入力データの \param
     \param inSz 入力データのサイズ

     _例_
     \code
     ret = wc_CMAC_Grow(cmac、in、inSz)
     \endcode
*/
int wc_CMAC_Grow(Cmac* cmac, const byte* in, int inSz);