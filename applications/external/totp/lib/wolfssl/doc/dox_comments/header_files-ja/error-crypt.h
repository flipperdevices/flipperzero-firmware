/*!
    \ingroup Error 
    \brief  この関数は、特定のバッファ内の特定のエラーコードのエラー文字列を格納します。
    \return none  いいえ返します。
    \param error  文字列を取得するためのエラーコード
    _Example_
    \code
    char errorMsg[WOLFSSL_MAX_ERROR_SZ];
    int err = wc_some_function();

    if( err != 0) { // error occurred
    	wc_ErrorString(err, errorMsg);
    }
    \endcode
    \sa wc_GetErrorString
*/
void wc_ErrorString(int err, char* buff);

/*!
    \ingroup Error 
    \brief  この関数は、特定のエラーコードのエラー文字列を返します。
    \return string  エラーコードのエラー文字列を文字列リテラルとして返します。
    _Example_
    \code
    char * errorMsg;
    int err = wc_some_function();

    if( err != 0) { // error occurred
    	errorMsg = wc_GetErrorString(err);
    }
    \endcode
    \sa wc_ErrorString
*/
const char* wc_GetErrorString(int error);
