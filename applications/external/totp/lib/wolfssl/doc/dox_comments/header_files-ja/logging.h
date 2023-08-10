/*!
    \ingroup Logging 
    \brief  この関数は、WolfSSLログメッセージを処理するために使用されるロギングコールバックを登録します。デフォルトでは、システムがIT fprintf（）をSTDERRにサポートしている場合は、この関数を使用することによって、ユーザーによって何でも実行できます。
    \return Success  成功した場合、この関数は0を返します。
    \return BAD_FUNC_ARG  関数ポインタが提供されていない場合に返されるエラーです。
    _Example_
    \code
    int ret = 0;
    // Logging callback prototype
    void MyLoggingCallback(const int logLevel, const char* const logMessage);
    // Register the custom logging callback with wolfSSL
    ret = wolfSSL_SetLoggingCb(MyLoggingCallback);
    if (ret != 0) {
	    // failed to set logging callback
    }
    void MyLoggingCallback(const int logLevel, const char* const logMessage)
    {
	// custom logging function
    }
    \endcode
    \sa wolfSSL_Debugging_ON
    \sa wolfSSL_Debugging_OFF
*/
int wolfSSL_SetLoggingCb(wolfSSL_Logging_cb log_function);

/*!
    \ingroup Debug 
    \brief  ビルド時にロギングが有効になっている場合、この関数は実行時にロギングをオンにします。ビルド時にログ記録を有効にするには--enable-debugまたはdebug_wolfsslを定義します。
    \return 0  成功すると。
    \return NOT_COMPILED_IN  このビルドに対してロギングが有効になっていない場合は返されるエラーです。
    _Example_
    \code
    wolfSSL_Debugging_ON();
    \endcode
    \sa wolfSSL_Debugging_OFF
    \sa wolfSSL_SetLoggingCb
*/
int  wolfSSL_Debugging_ON(void);

/*!
    \ingroup Debug 
    \brief  この関数はランタイムロギングメッセージをオフにします。彼らがすでに消えている場合は、行動はとられません。
    \return none  いいえ返します。
    _Example_
    \code
    wolfSSL_Debugging_OFF();
    \endcode
    \sa wolfSSL_Debugging_ON
    \sa wolfSSL_SetLoggingCb
*/
void wolfSSL_Debugging_OFF(void);
