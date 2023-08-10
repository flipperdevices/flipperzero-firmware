/*!
    \brief This function is the receive embedded callback.

    \return Success This function returns the number of bytes read.
    \return WOLFSSL_CBIO_ERR_WANT_READ returned with a “Would block” message
    if the last error was SOCKET_EWOULDBLCOK or SOCKET_EAGAIN.
    \return WOLFSSL_CBIO_ERR_TIMEOUT returned with a “Socket timeout” message.
    \return WOLFSSL_CBIO_ERR_CONN_RST returned with a “Connection reset”
    message if the last error was  SOCKET_ECONNRESET.
    \return WOLFSSL_CBIO_ERR_ISR returned with a “Socket interrupted” message
    if the last error was SOCKET_EINTR.
    \return WOLFSSL_CBIO_ERR_WANT_READ returned with a “Connection refused”
    message if the last error was SOCKET_ECONNREFUSED.
    \return WOLFSSL_CBIO_ERR_CONN_CLOSE returned with a “Connection aborted”
    message if the last error was SOCKET_ECONNABORTED.
    \return WOLFSSL_CBIO_ERR_GENERAL returned with a “General error” message
    if the last error was not specified.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf a char pointer representation of the buffer.
    \param sz the size of the buffer.
    \param ctx a void pointer to user registered context. In the default case
    the ctx is a socket descriptor pointer.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    char* buf;
    int sz;
    void* ctx;
    int bytesRead = EmbedReceive(ssl, buf, sz, ctx);
    if(bytesRead <= 0){
	    // There were no bytes read. Failure case.
    }
    \endcode

    \sa EmbedSend
    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_SSLSetIORecv
*/
int EmbedReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx);

/*!
    \brief This function is the send embedded callback.

    \return Success This function returns the number of bytes sent.
    \return WOLFSSL_CBIO_ERR_WANT_WRITE returned with a “Would block” message
    if the last error was SOCKET_EWOULDBLOCK or SOCKET_EAGAIN.
    \return WOLFSSL_CBIO_ERR_CONN_RST returned with a “Connection reset”
    message if the last error was SOCKET_ECONNRESET.
    \return WOLFSSL_CBIO_ERR_ISR returned with a “Socket interrupted” message
    if the last error was SOCKET_EINTR.
    \return WOLFSSL_CBIO_ERR_CONN_CLOSE returned with a “Socket EPIPE” message
    if the last error was SOCKET_EPIPE.
    \return WOLFSSL_CBIO_ERR_GENERAL returned with a “General error” message
    if the last error was not specified.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf a char pointer representing the buffer.
    \param sz the size of the buffer.
    \param ctx a void pointer to user registered context.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    char* buf;
    int sz;
    void* ctx;
    int dSent = EmbedSend(ssl, buf, sz, ctx);
    if(dSent <= 0){
    	// No byes sent. Failure case.
    }
    \endcode

    \sa EmbedReceive
    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SSLSetIOSend
*/
int EmbedSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);

/*!
    \brief This function is the receive embedded callback.

    \return Success This function returns the nb bytes read if the execution
    was successful.
    \return WOLFSSL_CBIO_ERR_WANT_READ if the connection refused or if a
    ‘would block’ error was thrown in the function.
    \return WOLFSSL_CBIO_ERR_TIMEOUT returned if the socket timed out.
    \return WOLFSSL_CBIO_ERR_CONN_RST returned if the connection reset.
    \return WOLFSSL_CBIO_ERR_ISR returned if the socket was interrupted.
    \return WOLFSSL_CBIO_ERR_GENERAL returned if there was a general error.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf a constant char pointer to the buffer.
    \param sz an int type representing the size of the buffer.
    \param ctx a void pointer to the WOLFSSL_CTX context.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    char* buf;
    int sz = sizeof(buf)/sizeof(char);
    (void*)ctx;
    …
    int nb = EmbedReceiveFrom(ssl, buf, sz, ctx);
    if(nb > 0){
	    // nb is the number of bytes written and is positive
    }
    \endcode

    \sa EmbedSendTo
    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_SSLSetIORecv
    \sa wolfSSL_dtls_get_current_timeout
*/
int EmbedReceiveFrom(WOLFSSL* ssl, char* buf, int sz, void*);

/*!
    \brief This function is the send embedded callback.

    \return Success This function returns the number of bytes sent.
    \return WOLFSSL_CBIO_ERR_WANT_WRITE returned with a “Would Block” message
    if the last error was either SOCKET_EWOULDBLOCK or SOCKET_EAGAIN error.
    \return WOLFSSL_CBIO_ERR_CONN_RST returned with a “Connection reset”
    message if the last error was SOCKET_ECONNRESET.
    \return WOLFSSL_CBIO_ERR_ISR returned with a “Socket interrupted” message
    if the last error was SOCKET_EINTR.
    \return WOLFSSL_CBIO_ERR_CONN_CLOSE returned with a “Socket EPIPE” message
    if the last error was WOLFSSL_CBIO_ERR_CONN_CLOSE.
    \return WOLFSSL_CBIO_ERR_GENERAL returned with a “General error” message
    if the last error was not specified.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf a char pointer representing the buffer.
    \param sz the size of the buffer.
    \param ctx a void pointer to the user registered context. The default case
    is a WOLFSSL_DTLS_CTX structure.

    _Example_
    \code
    WOLFSSL* ssl;
    …
    char* buf;
    int sz;
    void* ctx;

    int sEmbed = EmbedSendto(ssl, buf, sz, ctx);
    if(sEmbed <= 0){
    	// No bytes sent. Failure case.
    }
    \endcode

    \sa EmbedReceiveFrom
    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SSLSetIOSend
*/
int EmbedSendTo(WOLFSSL* ssl, char* buf, int sz, void* ctx);

/*!
    \brief This function is the DTLS Generate Cookie callback.

    \return Success This function returns the number of bytes copied
    into the buffer.
    \return GEN_COOKIE_E returned if the getpeername failed in
    EmbedGenerateCookie.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf byte pointer representing the buffer. It is the destination
    from XMEMCPY().
    \param sz the size of the buffer.
    \param ctx a void pointer to user registered context.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    byte buffer[BUFFER_SIZE];
    int sz = sizeof(buffer)/sizeof(byte);
    void* ctx;
    …
    int ret = EmbedGenerateCookie(ssl, buffer, sz, ctx);

    if(ret > 0){
    	// EmbedGenerateCookie code block for success
    }
    \endcode

    \sa wolfSSL_CTX_SetGenCookie
*/
int EmbedGenerateCookie(WOLFSSL* ssl, unsigned char* buf,
                                           int sz, void*);

/*!
    \brief This function frees the response buffer.

    \return none No returns.

    \param ctx a void pointer to heap hint.
    \param resp a byte pointer representing the response.

    _Example_
    \code
    void* ctx;
    byte* resp; // Response buffer.
    …
    EmbedOcspRespFree(ctx, resp);
    \endcode

    \sa wolfSSL_CertManagerSetOCSP_Cb
    \sa wolfSSL_CertManagerEnableOCSPStapling
    \sa wolfSSL_CertManagerEnableOCSP
*/
void EmbedOcspRespFree(void* ctx, byte* resp);

/*!
    \brief This function registers a receive callback for wolfSSL to get input
    data.  By default, wolfSSL uses EmbedReceive() as the callback which uses
    the system’s TCP recv() function.  The user can register a function to get
    input from memory, some other network module, or from anywhere.  Please see
    the EmbedReceive() function in src/io.c as a guide for how the function
    should work and for error codes.  In particular, IO_ERR_WANT_READ should
    be returned for non blocking receive when no data is ready.

    \return none no Returns.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param callback function to be registered as the receive callback for the
    wolfSSL context, ctx. The signature of this function must follow that as
    shown above in the Synopsis section.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    // Receive callback prototype
    int MyEmbedReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    // Register the custom receive callback with wolfSSL
    wolfSSL_CTX_SetIORecv(ctx, MyEmbedReceive);
    int MyEmbedReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx)
    {
	    // custom EmbedReceive function
    }
    \endcode

    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SetIOReadCtx
    \sa wolfSSL_SetIOWriteCtx
*/
void wolfSSL_CTX_SetIORecv(WOLFSSL_CTX* ctx, CallbackIORecv CBIORecv);

/*!
    \brief This function registers a context for the SSL session’s receive
    callback function.  By default, wolfSSL sets the file descriptor passed to
    wolfSSL_set_fd() as the context when wolfSSL is using the system’s TCP
    library. If you’ve registered your own receive callback you may want to set
    a specific context for the session.  For example, if you’re using memory
    buffers the context may be a pointer to a structure describing where and
    how to access the memory buffers.

    \return none No returns.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param rctx pointer to the context to be registered with the SSL session’s
    (ssl) receive callback function.

    _Example_
    \code
    int sockfd;
    WOLFSSL* ssl = 0;
    ...
    // Manually setting the socket fd as the receive CTX, for example
    wolfSSL_SetIOReadCtx(ssl, &sockfd);
    ...
    \endcode

    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SetIOWriteCtx
*/
void wolfSSL_SetIOReadCtx( WOLFSSL* ssl, void *ctx);

/*!
    \brief This function registers a context for the SSL session’s send
    callback function.  By default, wolfSSL sets the file descriptor passed to
    wolfSSL_set_fd() as the context when wolfSSL is using the system’s TCP
    library. If you’ve registered your own send callback you may want to set a
    specific context for the session.  For example, if you’re using memory
    buffers the context may be a pointer to a structure describing where and
    how to access the memory buffers.

    \return none No returns.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param wctx pointer to the context to be registered with the SSL session’s
    (ssl) send callback function.

    _Example_
    \code
    int sockfd;
    WOLFSSL* ssl = 0;
    ...
    // Manually setting the socket fd as the send CTX, for example
    wolfSSL_SetIOWriteCtx(ssl, &sockfd);
    ...
    \endcode

    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SetIOReadCtx
*/
void wolfSSL_SetIOWriteCtx(WOLFSSL* ssl, void *ctx);

/*!
    \ingroup IO

    \brief This function returns the IOCB_ReadCtx member of the WOLFSSL struct.

    \return pointer This function returns a void pointer to the IOCB_ReadCtx
    member of the WOLFSSL structure.
    \return NULL returned if the WOLFSSL struct is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    void* ioRead;
    ...
    ioRead = wolfSSL_GetIOReadCtx(ssl);
    if(ioRead == NULL){
    	// Failure case. The ssl object was NULL.
    }
    \endcode

    \sa wolfSSL_GetIOWriteCtx
    \sa wolfSSL_SetIOReadFlags
    \sa wolfSSL_SetIOWriteCtx
    \sa wolfSSL_SetIOReadCtx
    \sa wolfSSL_CTX_SetIOSend
*/
void* wolfSSL_GetIOReadCtx( WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function returns the IOCB_WriteCtx member of the WOLFSSL structure.

    \return pointer This function returns a void pointer to the IOCB_WriteCtx
    member of the WOLFSSL structure.
    \return NULL returned if the WOLFSSL struct is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl;
    void* ioWrite;
    ...
    ioWrite = wolfSSL_GetIOWriteCtx(ssl);
    if(ioWrite == NULL){
    	// The function returned NULL.
    }
    \endcode

    \sa wolfSSL_GetIOReadCtx
    \sa wolfSSL_SetIOWriteCtx
    \sa wolfSSL_SetIOReadCtx
    \sa wolfSSL_CTX_SetIOSend
*/
void* wolfSSL_GetIOWriteCtx(WOLFSSL* ssl);

/*!
    \brief This function sets the flags for the receive callback to use for
    the given SSL session.  The receive callback could be either the default
    wolfSSL EmbedReceive callback, or a custom callback specified by the user
    (see  wolfSSL_CTX_SetIORecv). The default flag value is set internally by
    wolfSSL to the value of 0. The default wolfSSL receive callback uses the
    recv() function to receive data from the socket. From the recv() man page:
    “The flags argument to a recv() function is formed by or'ing one or more
    of the values: MSG_OOB process out-of-band data, MSG_PEEK peek at incoming
    message, MSG_WAITALL	wait for full request or error. The MSG_OOB flag
    requests receipt of out-of-band data that would not be received in the
    normal data stream. Some protocols place expedited data at the head of
    the normal data queue, and thus this flag cannot be used with such
    protocols. The MSG_PEEK flag causes the receive operation to return
    data from the beginning of the receive queue without removing that data
    from the queue.  Thus, a subsequent receive call will return the same data.
    The MSG_WAITALL flag requests that the operation block until the full
    request is satisfied.  However, the call may still return less data than
    requested if a signal is caught, an error or disconnect occurs, or the next
    data to be received is of a different type than that returned.”

    \return none No returns.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param flags value of the I/O read flags for the specified SSL
    session (ssl).

    _Example_
    \code
    WOLFSSL* ssl = 0;
    ...
    // Manually setting recv flags to 0
    wolfSSL_SetIOReadFlags(ssl, 0);
    ...
    \endcode

    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SetIOReadCtx
*/
void wolfSSL_SetIOReadFlags( WOLFSSL* ssl, int flags);

/*!
    \brief This function sets the flags for the send callback to use for the
    given SSL session.  The send callback could be either the default wolfSSL
    EmbedSend callback, or a custom callback specified by the user (see
    wolfSSL_CTX_SetIOSend). The default flag value is set internally by wolfSSL
    to the value of 0. The default wolfSSL send callback uses the send()
    function to send data from the socket.  From the send() man page: “The
    flags parameter may include one or more of the following:
    #define MSG_OOB 0x1  // process out-of-band data,
    #define MSG_DONTROUTE  0x4  // bypass routing, use direct interface.
    The flag MSG_OOB is used to send ``out-of-band'' data on sockets that
    support this notion (e.g.  SOCK_STREAM); the underlying protocol must also
    support ``out-of-band'' data.  MSG_DONTROUTE is usually used only by
    diagnostic or routing programs.”

    \return none No returns.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param flags value of the I/O send flags for the specified SSL session (ssl).

    _Example_
    \code
    WOLFSSL* ssl = 0;
    ...
    // Manually setting send flags to 0
    wolfSSL_SetIOWriteFlags(ssl, 0);
    ...
    \endcode

    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_SetIOReadCtx
*/
void wolfSSL_SetIOWriteFlags(WOLFSSL* ssl, int flags);

/*!
    \ingroup IO

    \brief This function sets the nxSocket and nxWait members of the nxCtx
    struct within the WOLFSSL structure.

    \return none No returns.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param nxSocket a pointer to type NX_TCP_SOCKET that is set to the
    nxSocket member of the nxCTX structure.
    \param waitOption a ULONG type that is set to the nxWait member of
    the nxCtx structure.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    NX_TCP_SOCKET* nxSocket;
    ULONG waitOption;
    …
    if(ssl != NULL || nxSocket != NULL || waitOption <= 0){
    wolfSSL_SetIO_NetX(ssl, nxSocket, waitOption);
    } else {
    	// You need to pass in good parameters.
    }
    \endcode

    \sa set_fd
    \sa NetX_Send
    \sa NetX_Receive
*/
void wolfSSL_SetIO_NetX(WOLFSSL* ssl, NX_TCP_SOCKET* nxsocket,
                                      ULONG waitoption);

/*!
    \brief This function sets the callback for the CBIOCookie member of the
    WOLFSSL_CTX structure. The CallbackGenCookie type is a function pointer
    and has the signature:	int (*CallbackGenCookie)(WOLFSSL* ssl, unsigned
    char* buf, int sz, void* ctx);

    \return none No returns.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a CallbackGenCookie type function pointer with the signature
    of CallbackGenCookie.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    int SetGenCookieCB(WOLFSSL* ssl, unsigned char* buf, int sz, void* ctx){
	// Callback function body.
    }
    …
    wolfSSL_CTX_SetGenCookie(ssl->ctx, SetGenCookieCB);
    \endcode

    \sa CallbackGenCookie
*/
void  wolfSSL_CTX_SetGenCookie(WOLFSSL_CTX* ctx, CallbackGenCookie cb);

/*!
    \ingroup Setup

    \brief This function returns the IOCB_CookieCtx member of the
    WOLFSSL structure.

    \return pointer The function returns a void pointer value stored in
    the IOCB_CookieCtx.
    \return NULL if the WOLFSSL struct is NULL

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    void* cookie;
    ...
    cookie = wolfSSL_GetCookieCtx(ssl);
    if(cookie != NULL){
	// You have the cookie
    }
    \endcode

    \sa wolfSSL_SetCookieCtx
    \sa wolfSSL_CTX_SetGenCookie
*/
void* wolfSSL_GetCookieCtx(WOLFSSL* ssl);


/*!
    \ingroup Setup

    \brief This function sets up the ISO-TP context if wolfSSL, for use when
    wolfSSL is compiled with WOLFSSL_ISOTP

    \return 0 on success, WOLFSSL_CBIO_ERR_GENERAL on failure

    \param ssl the wolfSSL context
    \param ctx a user created ISOTP context which this function initializes
    \param recv_fn a user CAN bus receive callback
    \param send_fn a user CAN bus send callback
    \param delay_fn a user microsecond granularity delay function
    \param receive_delay a set amount of microseconds to delay each CAN bus
    packet
    \param receive_buffer a user supplied buffer to receive data, recommended
    that is allocated to ISOTP_DEFAULT_BUFFER_SIZE bytes
    \param receive_buffer_size - The size of receive_buffer
    \param arg an arbitrary pointer sent to recv_fn and send_fn

    _Example_
    \code
    struct can_info can_con_info;
    isotp_wolfssl_ctx isotp_ctx;
    char *receive_buffer = malloc(ISOTP_DEFAULT_BUFFER_SIZE);
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(method);
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    wolfSSL_SetIO_ISOTP(ssl, &isotp_ctx, can_receive, can_send, can_delay, 0,
            receive_buffer, ISOTP_DEFAULT_BUFFER_SIZE, &can_con_info);
    \endcode
 */
int wolfSSL_SetIO_ISOTP(WOLFSSL *ssl, isotp_wolfssl_ctx *ctx,
        can_recv_fn recv_fn, can_send_fn send_fn, can_delay_fn delay_fn,
        word32 receive_delay, char *receive_buffer, int receive_buffer_size,
        void *arg);
