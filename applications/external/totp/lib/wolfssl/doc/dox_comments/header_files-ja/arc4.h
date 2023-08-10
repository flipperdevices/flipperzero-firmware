/*!
    \ingroup ARC4 
    \brief  この関数は、バッファ内の入力メッセージを暗号化し、出力バッファーに暗号文を配置するか、またはバッファーから暗号文を復号化したり、ARC4暗号化を使用して、出力バッファーOUTを出力したりします。この関数は暗号化と復号化の両方に使用されます。この方法が呼び出される可能性がある場合は、まずWC_ARC4SETKEYを使用してARC4構造を初期化する必要があります。
    \return none 
    \param arc4  メッセージの処理に使用されるARC4構造へのポインタ
    \param out  処理されたメッセージを保存する出力バッファへのポインタ
    \param in  プロセスするメッセージを含む入力バッファへのポインタ
    _Example_
    \code
    Arc4 enc;
    byte key[] = { key to use for encryption };
    wc_Arc4SetKey(&enc, key, sizeof(key));

    byte plain[] = { plain text to encode };
    byte cipher[sizeof(plain)];
    byte decrypted[sizeof(plain)];
    // encrypt the plain into cipher
    wc_Arc4Process(&enc, cipher, plain, sizeof(plain));
    // decrypt the cipher
    wc_Arc4Process(&enc, decrypted, cipher, sizeof(cipher));
    \endcode
    \sa wc_Arc4SetKey
*/
int wc_Arc4Process(Arc4* arc4, byte* out, const byte* in, word32 length);

/*!
    \ingroup ARC4 
    \brief  この関数はARC4オブジェクトのキーを設定し、それを暗号として使用するために初期化します。WC_ARC4PROCESSを使用した暗号化に使用する前に呼び出される必要があります。
    \return none 
    \param arc4  暗号化に使用されるARC4構造へのポインタ
    \param key  ARC4構造を初期化するためのキー
    _Example_
    \code
    Arc4 enc;
    byte key[] = { initialize with key to use for encryption };
    wc_Arc4SetKey(&enc, key, sizeof(key));
    \endcode
    \sa wc_Arc4Process
*/
int wc_Arc4SetKey(Arc4* arc4, const byte* key, word32 length);
