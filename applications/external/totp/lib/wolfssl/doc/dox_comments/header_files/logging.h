/*!
    \ingroup Logging

    \brief This function registers a logging callback that will be used to
    handle the wolfSSL log message. By default, if the system supports it
    fprintf() to stderr is used but by using this function anything
    can be done by the user.

    \return Success If successful this function will return 0.
    \return BAD_FUNC_ARG is the error that will be returned if a function
    pointer is not provided.

    \param log_function function to register as a logging callback.
    Function signature must follow the above prototype.

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

    \brief If logging has been enabled at build time this function turns on
    logging at runtime.  To enable logging at build time use --enable-debug
    or define DEBUG_WOLFSSL.

    \return 0 upon success.
    \return NOT_COMPILED_IN is the error that will be returned if logging
    isn’t enabled for this build.

    \param none No parameters.

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

    \brief This function turns off runtime logging messages.  If they’re
    already off, no action is taken.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    wolfSSL_Debugging_OFF();
    \endcode

    \sa wolfSSL_Debugging_ON
    \sa wolfSSL_SetLoggingCb
*/
void wolfSSL_Debugging_OFF(void);
