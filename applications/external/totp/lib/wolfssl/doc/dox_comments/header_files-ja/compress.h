/*!
    \ingroup Compression 
    \brief  この関数は、ハフマン符号化を用いて与えられた入力データを圧縮し、出力をOUTに格納する。出力バッファは、圧縮が可能でないことが存在するため、出力バッファが入力バッファよりも大きいはずです。これはまだルックアップテーブルを必要とします。出力バッファに対してSRCSZ + 0.1％+ 12を割り当てることをお勧めします。
    \return On  入力データの圧縮に成功し、出力バッファに格納されているバイト数を返します。
    \return COMPRESS_INIT_E  圧縮のためにストリームの初期化中にエラーがある場合
    \return COMPRESS_E  圧縮中にエラーが発生した場合は返されます
    \param out  圧縮データを格納する出力バッファへのポインタ
    \param outSz  出力バッファで保存されているサイズ
    \param in  圧縮するメッセージを含むバッファへのポインタ
    \param inSz  圧縮する入力メッセージのサイズ
    _Example_
    \code
    byte message[] = { // initialize text to compress };
    byte compressed[(sizeof(message) + sizeof(message) * .001 + 12 )];
    // Recommends at least srcSz + .1% + 12

    if( wc_Compress(compressed, sizeof(compressed), message, sizeof(message),
    0) != 0){
    	// error compressing data
    }
    \endcode
    \sa wc_DeCompress
*/
int wc_Compress(byte* out, word32 outSz, const byte* in, word32 inSz, word32 flags);

/*!
    \ingroup Compression 
    \brief  この関数は、ハフマン符号化を用いて所定の圧縮データを解凍し、出力をOUTに格納する。
    \return Success  入力データの解凍に成功した場合は、出力バッファに格納されているバイト数を返します。
    \return COMPRESS_INIT_E:  圧縮のためにストリームの初期化中にエラーがある場合
    \return COMPRESS_E:  圧縮中にエラーが発生した場合は返されます
    \param out  解凍されたデータを格納する出力バッファへのポインタ
    \param outSz  出力バッファで保存されているサイズ
    \param in  解凍するメッセージを含むバッファへのポインタ
    _Example_
    \code
    byte compressed[] = { // initialize compressed message };
    byte decompressed[MAX_MESSAGE_SIZE];

    if( wc_DeCompress(decompressed, sizeof(decompressed),
    compressed, sizeof(compressed)) != 0 ) {
    	// error decompressing data
    }
    \endcode
    \sa wc_Compress
*/
int wc_DeCompress(byte* out, word32 outSz, const byte* in, word32 inSz);
