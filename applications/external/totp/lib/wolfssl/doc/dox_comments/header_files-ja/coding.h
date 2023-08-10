/*!
    \ingroup Base_Encoding 
    \brief  この機能は、与えられたBASS64符号化入力、IN、および出力バッファを出力バッファOUTに格納します。また、変数outlen内の出力バッファに書き込まれたサイズも設定します。
    \return 0  Base64エンコード入力の復号化に成功したときに返されます
    \return BAD_FUNC_ARG  復号化された入力を保存するには、出力バッファが小さすぎる場合は返されます。
    \return ASN_INPUT_E  入力バッファ内の文字がBASE64範囲（[A-ZA-Z0-9 + / =]）の外側にある場合、またはBASE64エンコード入力に無効な行が終了した場合
    \param in  デコードする入力バッファへのポインタ
    \param inLen  デコードする入力バッファの長さ
    \param out  デコードされたメッセージを保存する出力バッファへのポインタ
    _Example_
    \code
    byte encoded[] = { // initialize text to decode };
    byte decoded[sizeof(encoded)];
    // requires at least (sizeof(encoded) * 3 + 3) / 4 room

    int outLen = sizeof(decoded);

    if( Base64_Decode(encoded,sizeof(encoded), decoded, &outLen) != 0 ) {
    	// error decoding input buffer
    }
    \endcode
    \sa Base64_Encode
    \sa Base16_Decode
*/
int Base64_Decode(const byte* in, word32 inLen, byte* out,
                               word32* outLen);

/*!
    \ingroup Base_Encoding 
    \brief  この機能は与えられた入力を符号化し、符号化結果を出力バッファOUTに格納します。エスケープ％0A行末の代わりに、従来の '\ N'行の終わりを持つデータを書き込みます。正常に完了すると、この機能はまた、出力バッファに書き込まれたバイト数に統一されます。
    \return 0  Base64エンコード入力の復号化に成功したときに返されます
    \return BAD_FUNC_ARG  出力バッファが小さすぎてエンコードされた入力を保存する場合は返されます。
    \return BUFFER_E  出力バッファがエンコード中に部屋の外に実行された場合に返されます。
    \param in  エンコードする入力バッファへのポインタ
    \param inLen  エンコードする入力バッファの長さ
    \param out  エンコードされたメッセージを保存する出力バッファへのポインタ
    _Example_
    \code
    byte plain[] = { // initialize text to encode };
    byte encoded[MAX_BUFFER_SIZE];

    int outLen = sizeof(encoded);

    if( Base64_Encode(plain, sizeof(plain), encoded, &outLen) != 0 ) {
    	// error encoding input buffer
    }
    \endcode
    \sa Base64_EncodeEsc
    \sa Base64_Decode
*/

int Base64_Encode(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);

/*!
    \ingroup Base_Encoding 
    \brief  この機能は与えられた入力を符号化し、符号化結果を出力バッファOUTに格納します。それは '\ n "行の終わりではなく、％0aエスケープ行の終わりを持つデータを書き込みます。正常に完了すると、この機能はまた、出力バッファに書き込まれたバイト数に統一されます。
    \return 0  Base64エンコード入力の復号化に成功したときに返されます
    \return BAD_FUNC_ARG  出力バッファが小さすぎてエンコードされた入力を保存する場合は返されます。
    \return BUFFER_E  出力バッファがエンコード中に部屋の外に実行された場合に返されます。
    \return ASN_INPUT_E  入力メッセージのデコードの処理中にエラーが発生した場合
    \param in  エンコードする入力バッファへのポインタ
    \param inLen  エンコードする入力バッファの長さ
    \param out  エンコードされたメッセージを保存する出力バッファへのポインタ
    _Example_
    \code
    byte plain[] = { // initialize text to encode };
    byte encoded[MAX_BUFFER_SIZE];

    int outLen = sizeof(encoded);

    if( Base64_EncodeEsc(plain, sizeof(plain), encoded, &outLen) != 0 ) {
    	// error encoding input buffer
    }
    \endcode
    \sa Base64_Encode
    \sa Base64_Decode
*/
int Base64_EncodeEsc(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);

/*!
    \ingroup Base_Encoding 
    \brief  この機能は与えられた入力を符号化し、符号化結果を出力バッファOUTに格納します。それは新しい行なしでデータを書き込みます。正常に完了すると、この関数はまた、出力バッファに書き込まれたバイト数に統一されたものを設定します
    \return 0  Base64エンコード入力の復号化に成功したときに返されます
    \return BAD_FUNC_ARG  出力バッファが小さすぎてエンコードされた入力を保存する場合は返されます。
    \return BUFFER_E  出力バッファがエンコード中に部屋の外に実行された場合に返されます。
    \return ASN_INPUT_E  入力メッセージのデコードの処理中にエラーが発生した場合
    \param in  エンコードする入力バッファへのポインタ
    \param inLen  エンコードする入力バッファの長さ
    \param out  エンコードされたメッセージを保存する出力バッファへのポインタ
    _Example_
    \code
    byte plain[] = { // initialize text to encode };
    byte encoded[MAX_BUFFER_SIZE];
    int outLen = sizeof(encoded);
    if( Base64_Encode_NoNl(plain, sizeof(plain), encoded, &outLen) != 0 ) {
    	// error encoding input buffer
    }
    \endcode
    \sa Base64_Encode
    \sa Base64_Decode
*/

int Base64_Encode_NoNl(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);

/*!
    \ingroup Base_Encoding 
    \brief  この機能は、与えられたBASE16符号化入力、IN、および出力バッファへの結果を記憶する。また、変数outlen内の出力バッファに書き込まれたサイズも設定します。
    \return 0  Base16エンコード入力の復号にうまく復号化したときに返されます
    \return BAD_FUNC_ARG  出力バッファが復号化された入力を保存するにも小さすぎる場合、または入力長が2つの倍数でない場合に返されます。
    \return ASN_INPUT_E  入力バッファ内の文字がBASE16の範囲外にある場合は返されます（[0-9a-f]）
    \param in  デコードする入力バッファへのポインタ
    \param inLen  デコードする入力バッファの長さ
    \param out  デコードされたメッセージを保存する出力バッファへのポインタ
    _Example_
    \code
    byte encoded[] = { // initialize text to decode };
    byte decoded[sizeof(encoded)];
    int outLen = sizeof(decoded);

    if( Base16_Decode(encoded,sizeof(encoded), decoded, &outLen) != 0 ) {
    	// error decoding input buffer
    }
    \endcode
    \sa Base64_Encode
    \sa Base64_Decode
    \sa Base16_Encode
*/

int Base16_Decode(const byte* in, word32 inLen, byte* out, word32* outLen);

/*!
    \ingroup Base_Encoding 
    \brief  BASE16出力へのエンコード入力。
    \return 0  成功
    \return BAD_FUNC_ARG  IN、OUT、またはoutlenがNULLの場合、またはoutlenがInlen Plus 1を超えている場合は返します。
    \param in  エンコードされる入力バッファへのポインタ。
    \param inLen  入力バッファの長さ
    \param out  出力バッファへのポインタ。
    _Example_
    \code
    byte in[] = { // Contents of something to be encoded };
    byte out[NECESSARY_OUTPUT_SIZE];
    word32 outSz = sizeof(out);

    if(Base16_Encode(in, sizeof(in), out, &outSz) != 0)
    {
        // Handle encode error
    }
    \endcode
    \sa Base64_Encode
    \sa Base64_Decode
    \sa Base16_Decode
*/

int Base16_Encode(const byte* in, word32 inLen, byte* out, word32* outLen);
