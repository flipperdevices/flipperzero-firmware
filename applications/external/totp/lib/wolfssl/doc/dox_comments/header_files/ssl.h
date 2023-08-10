/*!
    \brief This function initializes the DTLS v1.2 client method.

    \return pointer This function returns a pointer to a new
    WOLFSSL_METHOD structure.

    \param none No parameters.

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method());
    …
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    \endcode

    \sa wolfSSL_Init
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfDTLSv1_2_client_method_ex(void* heap);

/*!
    \ingroup Setup

    \brief This function returns a WOLFSSL_METHOD similar to
    wolfSSLv23_client_method except that it is not determined
    which side yet (server/client).

    \return WOLFSSL_METHOD* On successful creations returns a WOLFSSL_METHOD
    pointer
    \return NULL Null if memory allocation error or failure to create method

    \param none No parameters.

    _Example_
    \code
    WOLFSSL* ctx;
    ctx  = wolfSSL_CTX_new(wolfSSLv23_method());
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
WOLFSSL_METHOD *wolfSSLv23_method(void);

/*!
    \ingroup Setup

    \brief The wolfSSLv3_server_method() function is used to indicate
    that the application is a server and will only support the SSL 3.0
    protocol.  This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the
    failure value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfSSLv3_server_method();
    if (method == NULL) {
	    unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfDTLSv1_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new

*/
WOLFSSL_METHOD *wolfSSLv3_server_method(void);

/*!
    \ingroup Setup

    \brief The wolfSSLv3_client_method() function is used to indicate
    that the application is a client and will only support the SSL 3.0
    protocol.  This function allocates memory for and initializes a
    new wolfSSL_METHOD structure to be used when creating the SSL/TLS
    context with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the
    failure value of the underlying malloc() implementation will be
    returned (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfSSLv3_client_method();
    if (method == NULL) {
	    unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfDTLSv1_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfSSLv3_client_method(void);

/*!
    \ingroup Setup

    \brief The wolfTLSv1_server_method() function is used to indicate that the
    application is a server and will only support the TLS 1.0 protocol. This
    function allocates memory for and initializes a new wolfSSL_METHOD
    structure to be used when creating the SSL/TLS context with
    wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_server_method();
    if (method == NULL) {
	    unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfDTLSv1_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_server_method(void);

/*!
    \ingroup Setup

    \brief The wolfTLSv1_client_method() function is used to indicate
    that the application is a client and will only support the TLS 1.0
    protocol.  This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC,
    the failure value of the underlying malloc() implementation
    will be returned (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_client_method();
    if (method == NULL) {
	    unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfDTLSv1_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_client_method(void);

/*!
    \ingroup Setup

    \brief The wolfTLSv1_1_server_method() function is used to indicate
    that the application is a server and will only support the TLS 1.1
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS
    context with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_1_server_method();
    if (method == NULL) {
        // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfDTLSv1_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_1_server_method(void);

/*!
    \ingroup Setup

    \brief The wolfTLSv1_1_client_method() function is used to indicate
    that the application is a client and will only support the TLS 1.0
    protocol. This function allocates memory for and initializes a
    new wolfSSL_METHOD structure to be used when creating the SSL/TLS
    context with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the
    newly created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_1_client_method();
    if (method == NULL) {
        // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfDTLSv1_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_1_client_method(void);

/*!
    \ingroup Setup

    \brief The wolfTLSv1_2_server_method() function is used to indicate
    that the application is a server and will only support the TLS 1.2
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_2_server_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfDTLSv1_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_2_server_method(void);

/*!
    \ingroup Setup

    \brief The wolfTLSv1_2_client_method() function is used to indicate
    that the application is a client and will only support the TLS 1.2
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_2_client_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfDTLSv1_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_2_client_method(void);

/*!
    \ingroup Setup

    \brief The wolfDTLSv1_client_method() function is used to indicate that
    the application is a client and will only support the DTLS 1.0 protocol.
    This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new(). This function is only available when wolfSSL has
    been compiled with DTLS support (--enable-dtls,
    or by defining wolfSSL_DTLS).

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfDTLSv1_client_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfDTLSv1_client_method(void);

/*!
    \ingroup Setup

    \brief The wolfDTLSv1_server_method() function is used to indicate
    that the application is a server and will only support the DTLS 1.0
    protocol.  This function allocates memory for and initializes a
    new wolfSSL_METHOD structure to be used when creating the SSL/TLS
    context with wolfSSL_CTX_new(). This function is only available
    when wolfSSL has been compiled with DTLS support (--enable-dtls,
    or by defining wolfSSL_DTLS).

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfDTLSv1_server_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfDTLSv1_server_method(void);
/*!
    \ingroup Setup

    \brief The wolfDTLSv1_3_server_method() function is used to indicate that
    the application is a server and will only support the DTLS 1.3
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context with
    wolfSSL_CTX_new(). This function is only available when wolfSSL has been
    compiled with DTLSv1.3 support (--enable-dtls13, or by defining
    wolfSSL_DTLS13).

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfDTLSv1_3_server_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode


    \sa wolfDTLSv1_3_client_method
*/

WOLFSSL_METHOD *wolfDTLSv1_3_server_method(void);
/*!
    \ingroup Setup

    \brief The wolfDTLSv1_3_client_method() function is used to indicate that
    the application is a client and will only support the DTLS 1.3
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context with
    wolfSSL_CTX_new(). This function is only available when wolfSSL has been
    compiled with DTLSv1.3 support (--enable-dtls13, or by defining
    wolfSSL_DTLS13).

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfDTLSv1_3_client_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode


    \sa wolfDTLSv1_3_server_method
*/
WOLFSSL_METHOD* wolfDTLSv1_3_client_method(void);
/*!
    \ingroup Setup

    \brief The wolfDTLS_server_method() function is used to indicate that the
    application is a server and will support the highest version of DTLS
    available and all the version up to the minimum version allowed.  The
    default minimum version allowed is based on the define
    WOLFSSL_MIN_DTLS_DOWNGRADE and can be changed at runtime using
    wolfSSL_SetMinVersion(). This function allocates memory for and initializes
    a new wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new(). This function is only available when wolfSSL has
    been compiled with DTLS support (--enable-dtls, or by defining
    wolfSSL_DTLS).

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfDTLS_server_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode


    \sa wolfDTLS_client_method
    \sa wolfSSL_SetMinVersion
*/
WOLFSSL_METHOD *wolfDTLS_server_method(void);
/*!
    \ingroup Setup

    \brief The wolfDTLS_client_method() function is used to indicate that the
    application is a client and will support the highest version of DTLS
    available and all the version up to the minimum version allowed.  The
    default minimum version allowed is based on the define
    WOLFSSL_MIN_DTLS_DOWNGRADE and can be changed at runtime using
    wolfSSL_SetMinVersion(). This function allocates memory for and initializes
    a new wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new(). This function is only available when wolfSSL has
    been compiled with DTLS support (--enable-dtls, or by defining
    wolfSSL_DTLS).

    \return * If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfDTLS_client_method();
    if (method == NULL) {
	    // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode


    \sa wolfDTLS_server_method
    \sa wolfSSL_SetMinVersion
*/
WOLFSSL_METHOD *wolfDTLS_client_method(void);
/*!
    \brief This function creates and initializes a WOLFSSL_METHOD for the
    server side.

    \return This function returns a WOLFSSL_METHOD pointer.

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfDTLSv1_2_server_method());
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    …
    \endcode

    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfDTLSv1_2_server_method(void);

/*!
    \ingroup Setup

    \brief Since there is some differences between the first release and
    newer versions of chacha-poly AEAD construction we have added an option
    to communicate with servers/clients using the older version. By default
    wolfSSL uses the new version.

    \return 0 upon success

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param value whether or not to use the older version of setting up the
    information for poly1305. Passing a flag value of 1 indicates yes use the
    old poly AEAD, to switch back to using the new version pass a flag value
    of 0.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...

    ret = wolfSSL_use_old_poly(ssl, 1);
    if (ret != 0) {
        // failed to set poly1305 AEAD version
    }
    \endcode

    \sa none
*/
int wolfSSL_use_old_poly(WOLFSSL* ssl, int value);

/*!
    \brief The wolfSSL_dtls_import() function is used to parse in a serialized
    session state. This allows for picking up the connection after the
    handshake has been completed.

    \return Success If successful, the amount of the buffer read will be
    returned.
    \return Failure All unsuccessful return values will be less than 0.
    \return VERSION_ERROR If a version mismatch is found ie DTLS v1 and ctx
    was set up for DTLS v1.2 then VERSION_ERROR is returned.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf serialized session to import.
    \param sz size of serialized session buffer.

    _Example_
    \code
    WOLFSSL* ssl;
    int ret;
    unsigned char buf[MAX];
    bufSz = MAX;
    ...
    //get information sent from wc_dtls_export function and place it in buf
    fread(buf, 1, bufSz, input);
    ret = wolfSSL_dtls_import(ssl, buf, bufSz);
    if (ret < 0) {
    // handle error case
    }
    // no wolfSSL_accept needed since handshake was already done
    ...
    ret = wolfSSL_write(ssl) and wolfSSL_read(ssl);
    ...
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_dtls_set_export
*/
int wolfSSL_dtls_import(WOLFSSL* ssl, unsigned char* buf,
                                                               unsigned int sz);


/*!
    \brief Used to import a serialized TLS session. This function is for
    importing the state of the connection.
    WARNING: buf contains sensitive information about the state and is best to
    be encrypted before storing if stored.
    Additional debug info can be displayed with the macro
    WOLFSSL_SESSION_EXPORT_DEBUG defined.

    \return the number of bytes read from buffer 'buf'

    \param ssl WOLFSSL structure to import the session into
    \param buf serialized session
    \param sz  size of buffer 'buf'

    \sa wolfSSL_dtls_import
    \sa wolfSSL_tls_export
 */
int wolfSSL_tls_import(WOLFSSL* ssl, const unsigned char* buf,
        unsigned int sz);

/*!
    \brief The wolfSSL_CTX_dtls_set_export() function is used to set
    the callback function for exporting a session. It is allowed to
    pass in NULL as the parameter func to clear the export function
    previously stored. Used on the server side and is called immediately
    after handshake is completed.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG If null or not expected arguments are passed in

    \param ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param func wc_dtls_export function to use when exporting a session.

    _Example_
    \code
    int send_session(WOLFSSL* ssl, byte* buf, word32 sz, void* userCtx);
    // body of send session (wc_dtls_export) that passses
    // buf (serialized session) to destination
    WOLFSSL_CTX* ctx;
    int ret;
    ...
    ret = wolfSSL_CTX_dtls_set_export(ctx, send_session);
    if (ret != SSL_SUCCESS) {
        // handle error case
    }
    ...
    ret = wolfSSL_accept(ssl);
    ...
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_new
    \sa wolfSSL_dtls_set_export
    \sa Static buffer use
*/
int wolfSSL_CTX_dtls_set_export(WOLFSSL_CTX* ctx,
                                                           wc_dtls_export func);

/*!
    \brief The wolfSSL_dtls_set_export() function is used to set the callback
    function for exporting a session. It is allowed to pass in NULL as the
    parameter func to clear the export function previously stored. Used on
    the server side and is called immediately after handshake is completed.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG If null or not expected arguments are passed in

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param func wc_dtls_export function to use when exporting a session.

    _Example_
    \code
    int send_session(WOLFSSL* ssl, byte* buf, word32 sz, void* userCtx);
    // body of send session (wc_dtls_export) that passses
    // buf (serialized session) to destination
    WOLFSSL* ssl;
    int ret;
    ...
    ret = wolfSSL_dtls_set_export(ssl, send_session);
    if (ret != SSL_SUCCESS) {
        // handle error case
    }
    ...
    ret = wolfSSL_accept(ssl);
    ...
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_dtls_set_export
*/
int wolfSSL_dtls_set_export(WOLFSSL* ssl, wc_dtls_export func);

/*!
    \brief The wolfSSL_dtls_export() function is used to serialize a
    WOLFSSL session into the provided buffer. Allows for less memory
    overhead than using a function callback for sending a session and
    choice over when the session is serialized. If buffer is NULL when
    passed to function then sz will be set to the size of buffer needed
    for serializing the WOLFSSL session.

    \return Success If successful, the amount of the buffer used will
    be returned.
    \return Failure All unsuccessful return values will be less than 0.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf buffer to hold serialized session.
    \param sz size of buffer.

    _Example_
    \code
    WOLFSSL* ssl;
    int ret;
    unsigned char buf[MAX];
    bufSz = MAX;
    ...
    ret = wolfSSL_dtls_export(ssl, buf, bufSz);
    if (ret < 0) {
        // handle error case
    }
    ...
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_dtls_set_export
    \sa wolfSSL_dtls_import
*/
int wolfSSL_dtls_export(WOLFSSL* ssl, unsigned char* buf,
                                                              unsigned int* sz);

/*!
    \brief Used to export a serialized TLS session. This function is for
    importing a serialized state of the connection.
    In most cases wolfSSL_get1_session should be used instead of
    wolfSSL_tls_export.
    Additional debug info can be displayed with the macro
    WOLFSSL_SESSION_EXPORT_DEBUG defined.
    WARNING: buf contains sensitive information about the state and is best to
             be encrypted before storing if stored.

    \return the number of bytes written into buffer 'buf'

    \param ssl WOLFSSL structure to export the session from
    \param buf output of serialized session
    \param sz  size in bytes set in 'buf'

    \sa wolfSSL_dtls_import
    \sa wolfSSL_tls_import
 */
int wolfSSL_tls_export(WOLFSSL* ssl, unsigned char* buf,
        unsigned int* sz);

/*!
    \brief This function is used to set aside static memory for a CTX. Memory
    set aside is then used for the CTX’s lifetime and for any SSL objects
    created from the CTX. By passing in a NULL ctx pointer and a
    wolfSSL_method_func function the creation of the CTX itself will also
    use static memory. wolfSSL_method_func has the function signature of
    WOLFSSL_METHOD* (*wolfSSL_method_func)(void* heap);. Passing in 0 for max
    makes it behave as if not set and no max concurrent use restrictions is
    in place. The flag value passed in determines how the memory is used and
    behavior while operating. Available flags are the following: 0 - default
    general memory, WOLFMEM_IO_POOL - used for input/output buffer when
    sending receiving messages and overrides general memory, so all memory
    in buffer passed in is used for IO, WOLFMEM_IO_FIXED - same as
    WOLFMEM_IO_POOL but each SSL now keeps two buffers to themselves for
    their lifetime, WOLFMEM_TRACK_STATS - each SSL keeps track of memory
    stats while running.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE upon failure.

    \param ctx address of pointer to a WOLFSSL_CTX structure.
    \param method function to create protocol. (should be NULL if ctx is not
    also NULL)
    \param buf memory to use for all operations.
    \param sz size of memory buffer being passed in.
    \param flag type of memory.
    \param max max concurrent operations.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    int ret;
    unsigned char memory[MAX];
    int memorySz = MAX;
    unsigned char IO[MAX];
    int IOSz = MAX;
    int flag = WOLFMEM_IO_FIXED | WOLFMEM_TRACK_STATS;
    ...
    // create ctx also using static memory, start with general memory to use
    ctx = NULL:
    ret = wolfSSL_CTX_load_static_memory(&ctx, wolfSSLv23_server_method_ex,
    memory, memorySz, 0,    MAX_CONCURRENT_HANDSHAKES);
    if (ret != SSL_SUCCESS) {
    // handle error case
    }
    // load in memory for use with IO
    ret = wolfSSL_CTX_load_static_memory(&ctx, NULL, IO, IOSz, flag,
    MAX_CONCURRENT_IO);
    if (ret != SSL_SUCCESS) {
    // handle error case
    }
    ...
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_is_static_memory
    \sa wolfSSL_is_static_memory
*/
int wolfSSL_CTX_load_static_memory(WOLFSSL_CTX** ctx,
                                            wolfSSL_method_func method,
                                            unsigned char* buf, unsigned int sz,
                                            int flag, int max);

/*!
    \brief This function does not change any of the connections behavior
    and is used only for gathering information about the static memory usage.

    \return 1 is returned if using static memory for the CTX is true.
    \return 0 is returned if not using static memory.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param mem_stats structure to hold information about static memory usage.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    int ret;
    WOLFSSL_MEM_STATS mem_stats;
    ...
    //get information about static memory with CTX
    ret = wolfSSL_CTX_is_static_memory(ctx, &mem_stats);
    if (ret == 1) {
        // handle case of is using static memory
        // print out or inspect elements of mem_stats
    }
    if (ret == 0) {
        //handle case of ctx not using static memory
    }
    …
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_load_static_memory
    \sa wolfSSL_is_static_memory
*/
int wolfSSL_CTX_is_static_memory(WOLFSSL_CTX* ctx,
                                                 WOLFSSL_MEM_STATS* mem_stats);

/*!
    \brief wolfSSL_is_static_memory is used to gather information about
    a SSL’s static memory usage. The return value indicates if static
    memory is being used and WOLFSSL_MEM_CONN_STATS will be filled out
    if and only if the flag WOLFMEM_TRACK_STATS was passed to the parent
    CTX when loading in static memory.

    \return 1 is returned if using static memory for the CTX is true.
    \return 0 is returned if not using static memory.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param mem_stats structure to contain static memory usage.

    _Example_
    \code
    WOLFSSL* ssl;
    int ret;
    WOLFSSL_MEM_CONN_STATS mem_stats;
    ...
    ret = wolfSSL_is_static_memory(ssl, mem_stats);
    if (ret == 1) {
        // handle case when is static memory
        // investigate elements in mem_stats if WOLFMEM_TRACK_STATS flag
    }
    ...
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_is_static_memory
*/
int wolfSSL_is_static_memory(WOLFSSL* ssl,
                                            WOLFSSL_MEM_CONN_STATS* mem_stats);

/*!
    \ingroup CertsKeys

    \brief This function loads a certificate file into the SSL context
    (WOLFSSL_CTX).  The file is provided by the file argument. The
    format argument specifies the format type of the file, either
    SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.  Please see the examples
    for proper usage.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE If the function call fails, possible causes might
    include the file is in the wrong format, or the wrong format has been
    given using the “format” argument, file doesn’t exist, can’t be read,
    or is corrupted, an out of memory condition occurs, Base16 decoding
    fails on the file.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new()
    \param file a pointer to the name of the file containing the certificate
    to be loaded into the wolfSSL SSL context.
    \param format - format of the certificates pointed to by file. Possible
    options are SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_use_certificate_file(ctx, “./client-cert.pem”,
                                     SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    // error loading cert file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_use_certificate_file
    \sa wolfSSL_use_certificate_buffer
*/
int wolfSSL_CTX_use_certificate_file(WOLFSSL_CTX* ctx, const char* file,
                                     int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a private key file into the SSL context
    (WOLFSSL_CTX). The file is provided by the file argument. The format
    argument specifies the format type of the file - SSL_FILETYPE_ASN1or
    SSL_FILETYPE_PEM.  Please see the examples for proper usage.

    If using an external key store and do not have the private key you can 
    instead provide the public key and register the crypro callback to handle 
    the signing. For this you can build with either build with crypto callbacks 
    or PK callbacks. To enable crypto callbacks use --enable-cryptocb
    or WOLF_CRYPTO_CB and register a crypto callback using 
    wc_CryptoCb_RegisterDevice and set the associated devId using 
    wolfSSL_CTX_SetDevId.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE The file is in the wrong format, or the wrong format
    has been given using the “format” argument. The file doesn’t exist, can’t
    be read, or is corrupted. An out of memory condition occurs. Base16
    decoding fails on the file. The key file is encrypted but no password
    is provided.

    \param none No parameters.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_use_PrivateKey_file(ctx, “./server-key.pem”,
                                    SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    // error loading key file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_use_PrivateKey_file
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wc_CryptoCb_RegisterDevice
    \sa wolfSSL_CTX_SetDevId
*/
int wolfSSL_CTX_use_PrivateKey_file(WOLFSSL_CTX* ctx, const char* file, int format);

/*!
    \ingroup CertsKeys

    \brief This function loads PEM-formatted CA certificate files into the SSL
    context (WOLFSSL_CTX).  These certificates will be treated as trusted root
    certificates and used to verify certs received from peers during the SSL
    handshake. The root certificate file, provided by the file argument, may
    be a single certificate or a file containing multiple certificates.
    If multiple CA certs are included in the same file, wolfSSL will load them
    in the same order they are presented in the file.  The path argument is
    a pointer to the name of a directory that contains certificates of
    trusted root CAs. If the value of file is not NULL, path may be specified
    as NULL if not needed.  If path is specified and NO_WOLFSSL_DIR was not
    defined when building the library, wolfSSL will load all CA certificates
    located in the given directory. This function will attempt to load all
    files in the directory. This function expects PEM formatted CERT_TYPE
    file with header “-----BEGIN CERTIFICATE-----”.

    \return SSL_SUCCESS up success.
    \return SSL_FAILURE will be returned if ctx is NULL, or if both file and
    path are NULL.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return ASN_BEFORE_DATE_E will be returned if the current date is before the
    before date.
    \return ASN_AFTER_DATE_E will be returned if the current date is after the
    after date.
    \return BUFFER_E will be returned if a chain buffer is bigger than the
    receiving buffer.
    \return BAD_PATH_ERROR will be returned if opendir() fails when trying
    to open path.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param file pointer to name of the file containing PEM-formatted CA
    certificates.
    \param path pointer to the name of a directory to load PEM-formatted
    certificates from.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_load_verify_locations(ctx, “./ca-cert.pem”, NULL);
    if (ret != WOLFSSL_SUCCESS) {
    	// error loading CA certs
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_locations_ex
    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_file
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_CTX_use_certificate_chain_file
    \sa wolfSSL_use_certificate_file
    \sa wolfSSL_use_PrivateKey_file
    \sa wolfSSL_use_certificate_chain_file
*/
int wolfSSL_CTX_load_verify_locations(WOLFSSL_CTX* ctx, const char* file,
                                                const char* format);

/*!
    \ingroup CertsKeys

    \brief This function loads PEM-formatted CA certificate files into the SSL
    context (WOLFSSL_CTX).  These certificates will be treated as trusted root
    certificates and used to verify certs received from peers during the SSL
    handshake. The root certificate file, provided by the file argument, may
    be a single certificate or a file containing multiple certificates.
    If multiple CA certs are included in the same file, wolfSSL will load them
    in the same order they are presented in the file.  The path argument is
    a pointer to the name of a directory that contains certificates of
    trusted root CAs. If the value of file is not NULL, path may be specified
    as NULL if not needed.  If path is specified and NO_WOLFSSL_DIR was not
    defined when building the library, wolfSSL will load all CA certificates
    located in the given directory. This function will attempt to load all
    files in the directory based on flags specified. This function expects PEM
    formatted CERT_TYPE files with header “-----BEGIN CERTIFICATE-----”.

    \return SSL_SUCCESS up success.
    \return SSL_FAILURE will be returned if ctx is NULL, or if both file and
    path are NULL. This will also be returned if at least one cert is loaded
    successfully but there is one or more that failed. Check error stack for reason.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BUFFER_E will be returned if a chain buffer is bigger than the
    receiving buffer.
    \return BAD_PATH_ERROR will be returned if opendir() fails when trying
    to open path.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param file pointer to name of the file containing PEM-formatted CA
    certificates.
    \param path pointer to the name of a directory to load PEM-formatted
    certificates from.
    \param flags possible mask values are: WOLFSSL_LOAD_FLAG_IGNORE_ERR,
    WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY and WOLFSSL_LOAD_FLAG_PEM_CA_ONLY

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, “./certs/external",
        WOLFSSL_LOAD_FLAG_PEM_CA_ONLY);
    if (ret != WOLFSSL_SUCCESS) {
        // error loading CA certs
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_file
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_CTX_use_certificate_chain_file
    \sa wolfSSL_use_certificate_file
    \sa wolfSSL_use_PrivateKey_file
    \sa wolfSSL_use_certificate_chain_file
*/
int wolfSSL_CTX_load_verify_locations_ex(WOLFSSL_CTX* ctx, const char* file,
                                         const char* path, unsigned int flags);

/*!
    \ingroup CertsKeys

    \brief This function returns a pointer to an array of strings representing
    directories wolfSSL will search for system CA certs when
    wolfSSL_CTX_load_system_CA_certs is called.

    \return Valid pointer on success.
    \return NULL pointer on failure.

    \param num pointer to a word32 that will be populated with the length of the
    array of strings.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    const char** dirs;
    word32 numDirs;

    dirs = wolfSSL_get_system_CA_dirs(&numDirs);
    for (int i = 0; i < numDirs; ++i) {
        printf("Potential system CA dir: %s\n", dirs[i]);
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_system_CA_certs
    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_load_verify_locations_ex
*/
const char** wolfSSL_get_system_CA_dirs(word32* num);

/*!
    \ingroup CertsKeys

    \brief This function attempts to load CA certificates into a WOLFSSL_CTX
    from an OS-dependent CA certificate store. Loaded certificates will be
    trusted. The platforms supported and tested are: Linux (Debian, Ubuntu, 
    Gentoo, Fedora, RHEL), Windows 10/11, Android, Apple OS X and iOS.

    \return WOLFSSL_SUCCESS on success.
    \return WOLFSSL_BAD_PATH if no system CA certs were loaded.
    \return WOLFSSL_FAILURE for other failure types (e.g. Windows cert store
    wasn't properly closed).

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_load_system_CA_certs(ctx,);
    if (ret != WOLFSSL_SUCCESS) {
        // error loading system CA certs
    }
    ...
    \endcode

    \sa wolfSSL_get_system_CA_dirs
    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_load_verify_locations_ex
*/
int wolfSSL_CTX_load_system_CA_certs(WOLFSSL_CTX* ctx);

/*!
    \ingroup Setup

    \brief This function loads a certificate to use for verifying a peer
    when performing a TLS/SSL handshake. The peer certificate sent during the
    handshake is compared by using the SKID when available and the signature.
    If these two things do not match then any loaded CAs are used. Feature is
    enabled by defining the macro WOLFSSL_TRUST_PEER_CERT. Please see the
    examples for proper usage.

    \return SSL_SUCCES upon success.
    \return SSL_FAILURE will be returned if ctx is NULL, or if both file and
    type are invalid.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param file pointer to name of the file containing certificates
    \param type type of certificate being loaded ie SSL_FILETYPE_ASN1
    or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    ...

    ret = wolfSSL_CTX_trust_peer_cert(ctx, “./peer-cert.pem”,
    SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
        // error loading trusted peer cert
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_file
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_CTX_use_certificate_chain_file
    \sa wolfSSL_CTX_trust_peer_buffer
    \sa wolfSSL_CTX_Unload_trust_peers
    \sa wolfSSL_use_certificate_file
    \sa wolfSSL_use_PrivateKey_file
    \sa wolfSSL_use_certificate_chain_file
*/
int wolfSSL_CTX_trust_peer_cert(WOLFSSL_CTX* ctx, const char* file, int type);

/*!
    \ingroup CertsKeys

    \brief This function loads a chain of certificates into the SSL
    context (WOLFSSL_CTX).  The file containing the certificate chain
    is provided by the file argument, and must contain PEM-formatted
    certificates. This function will process up to MAX_CHAIN_DEPTH
    (default = 9, defined in internal.h) certificates, plus the subject cert.

    \return SSL_SUCCESS upon success
    \return SSL_FAILURE If the function call fails, possible causes might
    include the file is in the wrong format, or the wrong format has been
    given using the “format” argument, file doesn’t exist, can’t be read,
    or is corrupted, an out of memory condition occurs.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new()
    \param file a pointer to the name of the file containing the chain of
    certificates to be loaded into the wolfSSL SSL context.  Certificates
    must be in PEM format.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_use_certificate_chain_file(ctx, “./cert-chain.pem”);
    if (ret != SSL_SUCCESS) {
	    // error loading cert file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_certificate_file
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_use_certificate_file
    \sa wolfSSL_use_certificate_buffer
*/
int wolfSSL_CTX_use_certificate_chain_file(WOLFSSL_CTX *ctx,
                                                     const char *file);

/*!
    \ingroup openSSL

    \brief This function loads the private RSA key used in the SSL connection
    into the SSL context (WOLFSSL_CTX).  This function is only available when
    wolfSSL has been compiled with the OpenSSL compatibility layer enabled
    (--enable-opensslExtra, #define OPENSSL_EXTRA), and is identical to the
    more-typically used wolfSSL_CTX_use_PrivateKey_file() function. The file
    argument contains a pointer to the RSA private key file, in the format
    specified by format.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE  If the function call fails, possible causes might
    include: The input key file is in the wrong format, or the wrong format
    has been given using the “format” argument, file doesn’t exist, can’t
    be read, or is corrupted, an out of memory condition occurs.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new()
    \param file a pointer to the name of the file containing the RSA private
    key to be loaded into the wolfSSL SSL context, with format as specified
    by format.
    \param format the encoding type of the RSA private key specified by file.
    Possible values include SSL_FILETYPE_PEM and SSL_FILETYPE_ASN1.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_use_RSAPrivateKey_file(ctx, “./server-key.pem”,
                                       SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    // error loading private key file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_use_RSAPrivateKey_file
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_PrivateKey_file
*/
int wolfSSL_CTX_use_RSAPrivateKey_file(WOLFSSL_CTX* ctx, const char* file, int format);

/*!
    \ingroup IO

    \brief This function returns the maximum chain depth allowed, which is 9 by
    default, for a valid session i.e. there is a non-null session object (ssl).

    \return MAX_CHAIN_DEPTH returned if the WOLFSSL_CTX structure is not
    NULL. By default the value is 9.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    long sslDep = wolfSSL_get_verify_depth(ssl);

    if(sslDep > EXPECTED){
    	// The verified depth is greater than what was expected
    } else {
    	// The verified depth is smaller or equal to the expected value
    }
    \endcode

    \sa wolfSSL_CTX_get_verify_depth
*/
long wolfSSL_get_verify_depth(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function gets the certificate chaining depth using the
    CTX structure.

    \return MAX_CHAIN_DEPTH returned if the CTX struct is not NULL. The
    constant representation of the max certificate chain peer depth.
    \return BAD_FUNC_ARG returned if the CTX structure is NULL.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_METHOD method; // protocol method
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new(method);
    …
    long ret = wolfSSL_CTX_get_verify_depth(ctx);

    if(ret == EXPECTED){
    	//  You have the expected value
    } else {
    	//  Handle an unexpected depth
    }
    \endcode

    \sa wolfSSL_CTX_use_certificate_chain_file
    \sa wolfSSL_get_verify_depth
*/
long wolfSSL_CTX_get_verify_depth(WOLFSSL_CTX* ctx);

/*!
    \ingroup openSSL

    \brief This function loads a certificate file into the SSL session
    (WOLFSSL structure).  The certificate file is provided by the file
    argument.  The format argument specifies the format type of the file -
    either SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    \return SSL_SUCCESS upon success
    \return SSL_FAILURE If the function call fails, possible causes might
    include: The file is in the wrong format, or the wrong format has been
    given using the “format” argument, file doesn’t exist, can’t be read,
    or is corrupted, an out of memory condition occurs, Base16 decoding
    fails on the file

    \param ssl a pointer to a WOLFSSL structure, created with wolfSSL_new().
    \param file a pointer to the name of the file containing the certificate to
    be loaded into the wolfSSL SSL session, with format as specified by format.
    \param format the encoding type of the certificate specified by file.
    Possible values include SSL_FILETYPE_PEM and SSL_FILETYPE_ASN1.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_use_certificate_file(ssl, “./client-cert.pem”,
                                 SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    	// error loading cert file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_certificate_file
    \sa wolfSSL_use_certificate_buffer
*/
int wolfSSL_use_certificate_file(WOLFSSL* ssl, const char* file, int format);

/*!
    \ingroup openSSL

    \brief This function loads a private key file into the SSL session
    (WOLFSSL structure).  The key file is provided by the file argument.
    The format argument specifies the format type of the file -
    SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    If using an external key store and do not have the private key you can 
    instead provide the public key and register the crypro callback to handle 
    the signing. For this you can build with either build with crypto callbacks 
    or PK callbacks. To enable crypto callbacks use --enable-cryptocb or
    WOLF_CRYPTO_CB and register a crypto callback using 
    wc_CryptoCb_RegisterDevice and set the associated devId using 
    wolfSSL_SetDevId.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE If the function call fails, possible causes might
    include: The file is in the wrong format, or the wrong format has been
    given using the “format” argument, The file doesn’t exist, can’t be read,
    or is corrupted, An out of memory condition occurs, Base16 decoding
    fails on the file, The key file is encrypted but no password is provided

    \param ssl a pointer to a WOLFSSL structure, created with wolfSSL_new().
    \param file a pointer to the name of the file containing the key file to
    be loaded into the wolfSSL SSL session, with format as specified by format.
    \param format the encoding type of the key specified by file.  Possible
    values include SSL_FILETYPE_PEM and SSL_FILETYPE_ASN1.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_use_PrivateKey_file(ssl, “./server-key.pem”,
                                SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    // error loading key file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wc_CryptoCb_RegisterDevice
    \sa wolfSSL_SetDevId
*/
int wolfSSL_use_PrivateKey_file(WOLFSSL* ssl, const char* file, int format);

/*!
    \ingroup openSSL

    \brief This function loads a chain of certificates into the SSL
    session (WOLFSSL structure).  The file containing the certificate
    chain is provided by the file argument, and must contain PEM-formatted
    certificates.  This function will process up to MAX_CHAIN_DEPTH
    (default = 9, defined in internal.h) certificates, plus the
    subject certificate.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE If the function call fails, possible causes
    might include: The file is in the wrong format, or the wrong format
    has been given using the “format” argument, file doesn’t exist,
    can’t be read, or is corrupted, an out of memory condition occurs

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new()
    \param file a pointer to the name of the file containing the chain
    of certificates to be loaded into the wolfSSL SSL session.
    Certificates must be in PEM format.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ctx;
    ...
    ret = wolfSSL_use_certificate_chain_file(ssl, “./cert-chain.pem”);
    if (ret != SSL_SUCCESS) {
    	// error loading cert file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_certificate_chain_file
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_use_certificate_chain_file(WOLFSSL* ssl, const char *file);

/*!
    \ingroup openSSL

    \brief This function loads the private RSA key used in the SSL
    connection into the SSL session (WOLFSSL structure). This
    function is only available when wolfSSL has been compiled with
    the OpenSSL compatibility layer enabled (--enable-opensslExtra,
    #define OPENSSL_EXTRA), and is identical to the more-typically
    used wolfSSL_use_PrivateKey_file() function. The file argument
    contains a pointer to the RSA private key file, in the format
    specified by format.

    \return SSL_SUCCESS upon success
    \return SSL_FAILURE If the function call fails, possible causes might
    include: The input key file is in the wrong format, or the wrong format
    has been given using the “format” argument, file doesn’t exist, can’t
    be read, or is corrupted, an out of memory condition occurs

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new()
    \param file a pointer to the name of the file containing the RSA private
    key to be loaded into the wolfSSL SSL session, with format as specified
    by format.
    \param format the encoding type of the RSA private key specified by file.
    Possible values include SSL_FILETYPE_PEM and SSL_FILETYPE_ASN1.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_use_RSAPrivateKey_file(ssl, “./server-key.pem”,
                                   SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    // error loading private key file
    }
    ...
    \endcode

    \sa wolfSSL_CTX_use_RSAPrivateKey_file
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_PrivateKey_file
*/
int wolfSSL_use_RSAPrivateKey_file(WOLFSSL* ssl, const char* file, int format);

/*!
    \ingroup CertsKeys

    \brief This function is similar to wolfSSL_CTX_load_verify_locations,
    but allows the loading of DER-formatted CA files into the SSL context
    (WOLFSSL_CTX).  It may still be used to load PEM-formatted CA files as
    well. These certificates will be treated as trusted root certificates
    and used to verify certs received from peers during the SSL handshake.
    The root certificate file, provided by the file argument, may be a single
    certificate or a file containing multiple certificates.  If multiple CA
    certs are included in the same file, wolfSSL will load them in the same
    order they are presented in the file.  The format argument specifies the
    format which the certificates are in either, SSL_FILETYPE_PEM or
    SSL_FILETYPE_ASN1 (DER). Unlike wolfSSL_CTX_load_verify_locations,
    this function does not allow the loading of CA certificates from a given
    directory path. Note that this function is only available when the wolfSSL
    library was compiled with WOLFSSL_DER_LOAD defined.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE upon failure.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new()
    \param file a pointer to the name of the file containing the CA
    certificates to be loaded into the wolfSSL SSL context, with format
    as specified by format.
    \param format the encoding type of the certificates specified by file.
    Possible values include SSL_FILETYPE_PEM and SSL_FILETYPE_ASN1.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_der_load_verify_locations(ctx, “./ca-cert.der”,
                                          SSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
	    // error loading CA certs
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_load_verify_buffer
*/
int wolfSSL_CTX_der_load_verify_locations(WOLFSSL_CTX* ctx,
                                          const char* file, int format);

/*!
    \ingroup Setup

    \brief This function creates a new SSL context, taking a desired
    SSL/TLS protocol method for input.

    \return pointer If successful the call will return a pointer to the
    newly-created WOLFSSL_CTX.
    \return NULL upon failure.

    \param method pointer to the desired WOLFSSL_METHOD to use for the SSL
    context. This is created using one of the wolfSSLvXX_XXXX_method()
    functions to specify SSL/TLS/DTLS protocol level.

    _Example_
    \code
    WOLFSSL_CTX*    ctx    = 0;
    WOLFSSL_METHOD* method = 0;

    method = wolfSSLv3_client_method();
    if (method == NULL) {
    	// unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
    	// context creation failed
    }
    \endcode

    \sa wolfSSL_new
*/
WOLFSSL_CTX* wolfSSL_CTX_new(WOLFSSL_METHOD*);

/*!
    \ingroup Setup

    \brief This function creates a new SSL session, taking an already
    created SSL context as input.

    \return * If successful the call will return a pointer to the
    newly-created wolfSSL structure.
    \return NULL Upon failure.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL*     ssl = NULL;
    WOLFSSL_CTX* ctx = 0;

    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
	    // context creation failed
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
	    // SSL object creation failed
    }
    \endcode

    \sa wolfSSL_CTX_new
*/
WOLFSSL* wolfSSL_new(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function assigns a file descriptor (fd) as the
    input/output facility for the SSL connection. Typically this will be
    a socket file descriptor.

    \return SSL_SUCCESS upon success.
    \return Bad_FUNC_ARG upon failure.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param fd file descriptor to use with SSL/TLS connection.

    _Example_
    \code
    int sockfd;
    WOLFSSL* ssl = 0;
    ...

    ret = wolfSSL_set_fd(ssl, sockfd);
    if (ret != SSL_SUCCESS) {
    	// failed to set SSL file descriptor
    }
    \endcode

    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_SetIOReadCtx
    \sa wolfSSL_SetIOWriteCtx
*/
int  wolfSSL_set_fd(WOLFSSL* ssl, int fd);

/*!
    \ingroup Setup

    \brief This function assigns a file descriptor (fd) as the
    input/output facility for the SSL connection. Typically this will be
    a socket file descriptor. This is a DTLS specific API because it marks that
    the socket is connected. recvfrom and sendto calls on this fd will have the
    addr and addr_len parameters set to NULL.

    \return SSL_SUCCESS upon success.
    \return Bad_FUNC_ARG upon failure.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param fd file descriptor to use with SSL/TLS connection.

    _Example_
    \code
    int sockfd;
    WOLFSSL* ssl = 0;
    ...
    if (connect(sockfd, peer_addr, peer_addr_len) != 0) {
        // handle connect error
    }
    ...
    ret = wolfSSL_set_dtls_fd_connected(ssl, sockfd);
    if (ret != SSL_SUCCESS) {
        // failed to set SSL file descriptor
    }
    \endcode

    \sa wolfSSL_CTX_SetIOSend
    \sa wolfSSL_CTX_SetIORecv
    \sa wolfSSL_SetIOReadCtx
    \sa wolfSSL_SetIOWriteCtx
    \sa wolfDTLS_SetChGoodCb
*/
int wolfSSL_set_dtls_fd_connected(WOLFSSL* ssl, int fd);

/*!
    \ingroup Setup

    \brief Allows setting a callback for a correctly processed and verified DTLS
           client hello. When using a cookie exchange mechanism (either the
           HelloVerifyRequest in DTLS 1.2 or the HelloRetryRequest with a cookie
           extension in DTLS 1.3) this callback is called after the cookie
           exchange has succeeded. This is useful to use one WOLFSSL object as
           the listener for new connections and being able to isolate the
           WOLFSSL object once the ClientHello is verified (either through a
           cookie exchange or just checking if the ClientHello had the correct
           format).
           DTLS 1.2:
           https://datatracker.ietf.org/doc/html/rfc6347#section-4.2.1
           DTLS 1.3:
           https://www.rfc-editor.org/rfc/rfc8446#section-4.2.2

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG upon failure.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param fd file descriptor to use with SSL/TLS connection.

    _Example_
    \code

    // Called when we have verified a connection
    static int chGoodCb(WOLFSSL* ssl, void* arg)
    {
        // setup peer and file descriptors

    }

    if (wolfDTLS_SetChGoodCb(ssl, chGoodCb, NULL) != WOLFSSL_SUCCESS) {
         // error setting callback
    }
    \endcode

    \sa wolfSSL_set_dtls_fd_connected
*/
int wolfDTLS_SetChGoodCb(WOLFSSL* ssl, ClientHelloGoodCb cb, void* user_ctx);

/*!
    \ingroup IO

    \brief Get the name of cipher at priority level passed in.

    \return string Success
    \return 0 Priority is either out of bounds or not valid.

    \param priority Integer representing the priority level of a cipher.

    _Example_
    \code
    printf("The cipher at 1 is %s", wolfSSL_get_cipher_list(1));
    \endcode

    \sa wolfSSL_CIPHER_get_name
    \sa wolfSSL_get_current_cipher
*/
char* wolfSSL_get_cipher_list(int priority);

/*!
    \ingroup IO

    \brief This function gets the ciphers enabled in wolfSSL.

    \return SSL_SUCCESS returned if the function executed without error.
    \return BAD_FUNC_ARG returned if the buf parameter was NULL or if the
    len argument was less than or equal to zero.
    \return BUFFER_E returned if the buffer is not large enough and
    will overflow.

    \param buf a char pointer representing the buffer.
    \param len the length of the buffer.

    _Example_
    \code
    static void ShowCiphers(void){
	char* ciphers;
	int ret = wolfSSL_get_ciphers(ciphers, (int)sizeof(ciphers));

	if(ret == SSL_SUCCES){
	    	printf(“%s\n”, ciphers);
	    }
    }
    \endcode

    \sa GetCipherNames
    \sa wolfSSL_get_cipher_list
    \sa ShowCiphers
*/
int  wolfSSL_get_ciphers(char* buf, int len);

/*!
    \ingroup IO

    \brief This function gets the cipher name in the format DHE-RSA by
    passing through argument to wolfSSL_get_cipher_name_internal.

    \return string This function returns the string representation of the
    cipher suite that was matched.
    \return NULL error or cipher not found.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    char* cipherS = wolfSSL_get_cipher_name(ssl);

    if(cipher == NULL){
	    // There was not a cipher suite matched
    } else {
	    // There was a cipher suite matched
	    printf(“%s\n”, cipherS);
    }
    \endcode

    \sa wolfSSL_CIPHER_get_name
    \sa wolfSSL_get_current_cipher
    \sa wolfSSL_get_cipher_name_internal
*/
const char* wolfSSL_get_cipher_name(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function returns the file descriptor (fd) used as the
    input/output facility for the SSL connection.  Typically this
    will be a socket file descriptor.

    \return fd If successful the call will return the SSL session file
    descriptor.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int sockfd;
    WOLFSSL* ssl = 0;
    ...
    sockfd = wolfSSL_get_fd(ssl);
    ...
    \endcode

    \sa wolfSSL_set_fd
*/
int  wolfSSL_get_fd(const WOLFSSL*);

/*!
    \ingroup Setup

    \brief This function informs the WOLFSSL object that the underlying
     I/O is non-blocking. After an application creates a WOLFSSL object,
     if it will be used with a non-blocking socket, call
    wolfSSL_set_using_nonblock() on it. This lets the WOLFSSL object know
     that receiving EWOULDBLOCK means that the recvfrom call would
    block rather than that it timed out.

    \return none No return.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param nonblock value used to set non-blocking flag on WOLFSSL object.
    Use 1 to specify non-blocking, otherwise 0.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    ...
    wolfSSL_set_using_nonblock(ssl, 1);
    \endcode

    \sa wolfSSL_get_using_nonblock
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_get_current_timeout
*/
void wolfSSL_set_using_nonblock(WOLFSSL* ssl, int nonblock);

/*!
    \ingroup IO

    \brief This function allows the application to determine if wolfSSL is
    using non-blocking I/O.  If wolfSSL is using non-blocking I/O, this
    function will return 1, otherwise 0. After an application creates a
    WOLFSSL object, if it will be used with a non-blocking socket, call
    wolfSSL_set_using_nonblock() on it. This lets the WOLFSSL object know
    that receiving EWOULDBLOCK means that the recvfrom call would block
    rather than that it timed out.

    \return 0 underlying I/O is blocking.
    \return 1 underlying I/O is non-blocking.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_get_using_nonblock(ssl);
    if (ret == 1) {
    	// underlying I/O is non-blocking
    }
    ...
    \endcode

    \sa wolfSSL_set_session
*/
int  wolfSSL_get_using_nonblock(WOLFSSL*);

/*!
    \ingroup IO

    \brief This function writes sz bytes from the buffer, data, to the SSL
    connection, ssl. If necessary, wolfSSL_write() will negotiate an SSL/TLS
    session if the handshake has not already been performed yet by
    wolfSSL_connect() or wolfSSL_accept(). wolfSSL_write() works with both
    blocking and non-blocking I/O.  When the underlying I/O is non-blocking,
    wolfSSL_write() will return when the underlying I/O could not satisfy the
    needs of wolfSSL_write() to continue.  In this case, a call to
    wolfSSL_get_error() will yield either SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE.  The calling process must then repeat the call to
    wolfSSL_write() when the underlying I/O is ready. If the underlying I/O
    is blocking, wolfSSL_write() will only return once the buffer data of
    size sz has been completely written or an error occurred.

    \return >0 the number of bytes written upon success.
    \return 0 will be returned upon failure.  Call wolfSSL_get_error() for
    the specific error code.
    \return SSL_FATAL_ERROR will be returned upon failure when either an error
    occurred or, when using non-blocking sockets, the SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE error was received and and the application needs to
    call wolfSSL_write() again.  Use wolfSSL_get_error() to get a specific
    error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param data data buffer which will be sent to peer.
    \param sz size, in bytes, of data to send to the peer (data).

    _Example_
    \code
    WOLFSSL* ssl = 0;
    char msg[64] = “hello wolfssl!”;
    int msgSz = (int)strlen(msg);
    int flags;
    int ret;
    ...

    ret = wolfSSL_write(ssl, msg, msgSz);
    if (ret <= 0) {
    	// wolfSSL_write() failed, call wolfSSL_get_error()
    }
    \endcode

    \sa wolfSSL_send
    \sa wolfSSL_read
    \sa wolfSSL_recv
*/
int  wolfSSL_write(WOLFSSL* ssl, const void* data, int sz);

/*!
    \ingroup IO

    \brief This function reads sz bytes from the SSL session (ssl)
    internal read buffer into the buffer data. The bytes read are removed
    from the internal receive buffer. If necessary wolfSSL_read() will
    negotiate an SSL/TLS session if the handshake has not already been
    performed yet by wolfSSL_connect() or wolfSSL_accept(). The SSL/TLS
    protocol uses SSL records which have a maximum size of 16kB (the max
    record size can be controlled by the MAX_RECORD_SIZE define in
    <wolfssl_root>/wolfssl/internal.h).  As such, wolfSSL needs to read an
    entire SSL record internally before it is able to process and decrypt the
    record.  Because of this, a call to wolfSSL_read() will only be able to
    return the maximum buffer size which has been decrypted at the time of
    calling.  There may be additional not-yet-decrypted data waiting in the
    internal wolfSSL receive buffer which will be retrieved and decrypted with
    the next call to wolfSSL_read(). If sz is larger than the number of bytes
    in the internal read buffer, SSL_read() will return the bytes available in
    the internal read buffer.  If no bytes are buffered in the internal read
    buffer yet, a call to wolfSSL_read() will trigger processing of the next
    record.

    \return >0 the number of bytes read upon success.
    \return 0 will be returned upon failure.  This may be caused by a either a
    clean (close notify alert) shutdown or just that the peer closed the
    connection.  Call wolfSSL_get_error() for the specific error code.
    \return SSL_FATAL_ERROR will be returned upon failure when either an error
    occurred or, when using non-blocking sockets, the SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE error was received and and the application needs to
    call wolfSSL_read() again.  Use wolfSSL_get_error() to get a specific
    error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param data buffer where wolfSSL_read() will place data read.
    \param sz number of bytes to read into data.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    char reply[1024];
    ...

    input = wolfSSL_read(ssl, reply, sizeof(reply));
    if (input > 0) {
    	// “input” number of bytes returned into buffer “reply”
    }

    See wolfSSL examples (client, server, echoclient, echoserver) for more
    complete examples of wolfSSL_read().
    \endcode

    \sa wolfSSL_recv
    \sa wolfSSL_write
    \sa wolfSSL_peek
    \sa wolfSSL_pending
*/
int  wolfSSL_read(WOLFSSL* ssl, void* data, int sz);

/*!
    \ingroup IO

    \brief This function copies sz bytes from the SSL session (ssl) internal
    read buffer into the buffer data. This function is identical to
    wolfSSL_read() except that the data in the internal SSL session
    receive buffer is not removed or modified. If necessary, like
    wolfSSL_read(), wolfSSL_peek() will negotiate an SSL/TLS session if
    the handshake has not already been performed yet by wolfSSL_connect()
    or wolfSSL_accept(). The SSL/TLS protocol uses SSL records which have a
    maximum size of 16kB (the max record size can be controlled by the
    MAX_RECORD_SIZE define in <wolfssl_root>/wolfssl/internal.h).  As such,
    wolfSSL needs to read an entire SSL record internally before it is able
    to process and decrypt the record.  Because of this, a call to
    wolfSSL_peek() will only be able to return the maximum buffer size which
    has been decrypted at the time of calling.  There may be additional
    not-yet-decrypted data waiting in the internal wolfSSL receive buffer
    which will be retrieved and decrypted with the next call to
    wolfSSL_peek() / wolfSSL_read(). If sz is larger than the number of bytes
    in the internal read buffer, SSL_peek() will return the bytes available
    in the internal read buffer.  If no bytes are buffered in the internal
    read buffer yet, a call to wolfSSL_peek() will trigger processing of the
    next record.

    \return >0 the number of bytes read upon success.
    \return 0 will be returned upon failure.  This may be caused by a either
    a clean (close notify alert) shutdown or just that the peer closed the
    connection.  Call wolfSSL_get_error() for the specific error code.
    \return SSL_FATAL_ERROR will be returned upon failure when either an
    error occurred or, when using non-blocking sockets, the
    SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE error was received and and
    the application needs to call wolfSSL_peek() again. Use
    wolfSSL_get_error() to get a specific error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param data buffer where wolfSSL_peek() will place data read.
    \param sz number of bytes to read into data.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    char reply[1024];
    ...

    input = wolfSSL_peek(ssl, reply, sizeof(reply));
    if (input > 0) {
	    // “input” number of bytes returned into buffer “reply”
    }
    \endcode

    \sa wolfSSL_read
*/
int  wolfSSL_peek(WOLFSSL* ssl, void* data, int sz);

/*!
    \ingroup IO

    \brief This function is called on the server side and waits for an SSL
    client to initiate the SSL/TLS handshake.  When this function is called,
    the underlying communication channel has already been set up.
    wolfSSL_accept() works with both blocking and non-blocking I/O.
    When the underlying I/O is non-blocking, wolfSSL_accept() will return
    when the underlying I/O could not satisfy the needs of wolfSSL_accept
    to continue the handshake.  In this case, a call to wolfSSL_get_error()
    will yield either SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE.
    The calling process must then repeat the call to wolfSSL_accept when
    data is available to read and wolfSSL will pick up where it left off.
    When using a non-blocking socket, nothing needs to be done, but select()
    can be used to check for the required condition. If the underlying I/O
    is blocking, wolfSSL_accept() will only return once the handshake has
    been finished or an error occurred.

    \return SSL_SUCCESS upon success.
    \return SSL_FATAL_ERROR will be returned if an error occurred. To get a
    more detailed error code, call wolfSSL_get_error().

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...

    ret = wolfSSL_accept(ssl);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_connect
*/
int  wolfSSL_accept(WOLFSSL*);

/*!
    \ingroup Setup

    \brief This function frees an allocated WOLFSSL_CTX object.  This
    function decrements the CTX reference count and only frees the context
    when the reference count has reached 0.

    \return none No return.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    wolfSSL_CTX_free(ctx);
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_new
    \sa wolfSSL_free
*/
void wolfSSL_CTX_free(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function frees an allocated wolfSSL object.

    \return none No return.

    \param ssl pointer to the SSL object, created with wolfSSL_new().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL* ssl = 0;
    ...
    wolfSSL_free(ssl);
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_new
    \sa wolfSSL_CTX_free
*/
void wolfSSL_free(WOLFSSL*);

/*!
    \ingroup TLS

    \brief This function shuts down an active SSL/TLS connection using
    the SSL session, ssl.  This function will try to send a “close notify”
    alert to the peer. The calling application can choose to wait for the
    peer to send its “close notify” alert in response or just go ahead
    and shut down the underlying connection after directly calling
    wolfSSL_shutdown (to save resources).  Either option is allowed by
    the TLS specification.  If the underlying connection will be used
    again in the future, the complete two-directional shutdown procedure
    must be performed to keep synchronization intact between the peers.
    wolfSSL_shutdown() works with both blocking and non-blocking I/O.
    When the underlying I/O is non-blocking, wolfSSL_shutdown() will
    return an error if the underlying I/O could not satisfy the needs of
    wolfSSL_shutdown() to continue. In this case, a call to
    wolfSSL_get_error() will yield either SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE.  The calling process must then repeat the call
    to wolfSSL_shutdown() when the underlying I/O is ready.

    \return SSL_SUCCESS will be returned upon success.
    \return SSL_SHUTDOWN_NOT_DONE will be returned when shutdown has not
    finished, and the function should be called again.
    \return SSL_FATAL_ERROR will be returned upon failure. Call
    wolfSSL_get_error() for a more specific error code.

    \param ssl pointer to the SSL session created with wolfSSL_new().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int ret = 0;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_shutdown(ssl);
    if (ret != 0) {
	    // failed to shut down SSL connection
    }
    \endcode

    \sa wolfSSL_free
    \sa wolfSSL_CTX_free
*/
int  wolfSSL_shutdown(WOLFSSL*);

/*!
    \ingroup IO

    \brief This function writes sz bytes from the buffer, data, to the SSL
    connection, ssl, using the specified flags for the underlying write
    operation. If necessary wolfSSL_send() will negotiate an SSL/TLS session
    if the handshake has not already been performed yet by wolfSSL_connect()
    or wolfSSL_accept(). wolfSSL_send() works with both blocking and
    non-blocking I/O.  When the underlying I/O is non-blocking, wolfSSL_send()
    will return when the underlying I/O could not satisfy the needs of
    wolfSSL_send to continue.  In this case, a call to wolfSSL_get_error()
    will yield either SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE.
    The calling process must then repeat the call to wolfSSL_send() when
    the underlying I/O is ready. If the underlying I/O is blocking,
    wolfSSL_send() will only return once the buffer data of size sz has
    been completely written or an error occurred.

    \return >0 the number of bytes written upon success.
    \return 0 will be returned upon failure.  Call wolfSSL_get_error() for
    the specific error code.
    \return SSL_FATAL_ERROR will be returned upon failure when either an error
    occurred or, when using non-blocking sockets, the SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE error was received and and the application needs to
    call wolfSSL_send() again.  Use wolfSSL_get_error() to get a specific
    error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param data data buffer to send to peer.
    \param sz size, in bytes, of data to be sent to peer.
    \param flags the send flags to use for the underlying send operation.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    char msg[64] = “hello wolfssl!”;
    int msgSz = (int)strlen(msg);
    int flags = ... ;
    ...

    input = wolfSSL_send(ssl, msg, msgSz, flags);
    if (input != msgSz) {
    	// wolfSSL_send() failed
    }
    \endcode

    \sa wolfSSL_write
    \sa wolfSSL_read
    \sa wolfSSL_recv
*/
int  wolfSSL_send(WOLFSSL* ssl, const void* data, int sz, int flags);

/*!
    \ingroup IO

    \brief This function reads sz bytes from the SSL session (ssl) internal
    read buffer into the buffer data using the specified flags for the
    underlying recv operation.  The bytes read are removed from the internal
    receive buffer.  This function is identical to wolfSSL_read() except
    that it allows the application to set the recv flags for the underlying
    read operation. If necessary wolfSSL_recv() will negotiate an SSL/TLS
    session if the handshake has not already been performed yet by
    wolfSSL_connect() or wolfSSL_accept(). The SSL/TLS protocol uses
    SSL records which have a maximum size of 16kB (the max record size
    can be controlled by the MAX_RECORD_SIZE define in
    <wolfssl_root>/wolfssl/internal.h). As such, wolfSSL needs to read an
    entire SSL record internally before it is able to process and decrypt
    the record. Because of this, a call to wolfSSL_recv() will only be
    able to return the maximum buffer size which has been decrypted at
    the time of calling.  There may be additional not-yet-decrypted data
    waiting in the internal wolfSSL receive buffer which will be
    retrieved and decrypted with the next call to wolfSSL_recv(). If sz
    is larger than the number of bytes in the internal read buffer,
    SSL_recv() will return the bytes available in the internal read buffer.
    If no bytes are buffered in the internal read buffer yet, a call to
    wolfSSL_recv() will trigger processing of the next record.

    \return >0 the number of bytes read upon success.
    \return 0 will be returned upon failure. This may be caused by a either
    a clean (close notify alert) shutdown or just that the peer closed the
    connection. Call wolfSSL_get_error() for the specific error code.
    \return SSL_FATAL_ERROR will be returned upon failure when either an error
    occurred or, when using non-blocking sockets, the SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE error was received and and the application needs to
    call wolfSSL_recv() again.  Use wolfSSL_get_error() to get a specific
    error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param data buffer where wolfSSL_recv() will place data read.
    \param sz number of bytes to read into data.
    \param flags the recv flags to use for the underlying recv operation.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    char reply[1024];
    int flags = ... ;
    ...

    input = wolfSSL_recv(ssl, reply, sizeof(reply), flags);
    if (input > 0) {
    	// “input” number of bytes returned into buffer “reply”
    }
    \endcode

    \sa wolfSSL_read
    \sa wolfSSL_write
    \sa wolfSSL_peek
    \sa wolfSSL_pending
*/
int  wolfSSL_recv(WOLFSSL* ssl, void* data, int sz, int flags);

/*!
    \ingroup Debug

    \brief This function returns a unique error code describing why the
    previous API function call (wolfSSL_connect, wolfSSL_accept, wolfSSL_read,
    wolfSSL_write, etc.) resulted in an error return code (SSL_FAILURE).
    The return value of the previous function is passed to wolfSSL_get_error
    through ret. After wolfSSL_get_error is called and returns the unique
    error code, wolfSSL_ERR_error_string() may be called to get a
    human-readable error string.  See wolfSSL_ERR_error_string() for more
    information.

    \return On successful completion, this function will return the
    unique error code describing why the previous API function failed.
    \return SSL_ERROR_NONE will be returned if ret > 0. For ret <= 0, there are
    some cases when this value can also be returned when a previous API appeared
    to return an error code but no error actually occurred. An example is
    calling wolfSSL_read() with a zero sz parameter. A 0 return from
    wolfSSL_read() usually indicates an error but in this case no error
    occurred. If wolfSSL_get_error() is called afterwards, SSL_ERROR_NONE will
    be returned.

    \param ssl pointer to the SSL object, created with wolfSSL_new().
    \param ret return value of the previous function that resulted in an error
    return code.

    _Example_
    \code
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...
    err = wolfSSL_get_error(ssl, 0);
    wolfSSL_ERR_error_string(err, buffer);
    printf(“err = %d, %s\n”, err, buffer);
    \endcode

    \sa wolfSSL_ERR_error_string
    \sa wolfSSL_ERR_error_string_n
    \sa wolfSSL_ERR_print_errors_fp
    \sa wolfSSL_load_error_strings
*/
int  wolfSSL_get_error(WOLFSSL* ssl, int ret);

/*!
    \ingroup IO

    \brief This function gets the alert history.

    \return SSL_SUCCESS returned when the function completed successfully.
    Either there was alert history or there wasn’t, either way, the
    return value is SSL_SUCCESS.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param h a pointer to a WOLFSSL_ALERT_HISTORY structure that will hold the
    WOLFSSL struct’s alert_history member’s value.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(protocol method);
    WOLFSSL* ssl = wolfSSL_new(ctx);
    WOLFSSL_ALERT_HISTORY* h;
    ...
    wolfSSL_get_alert_history(ssl, h);
    // h now has a copy of the ssl->alert_history  contents
    \endcode

    \sa wolfSSL_get_error
*/
int  wolfSSL_get_alert_history(WOLFSSL* ssl, WOLFSSL_ALERT_HISTORY *h);

/*!
    \ingroup Setup

    \brief This function sets the session to be used when the SSL object,
    ssl, is used to establish a SSL/TLS connection. For session resumption,
    before calling wolfSSL_shutdown() with your session object, an application
    should save the session ID from the object with a call to
    wolfSSL_get1_session(), which returns a pointer to the session.
    Later, the application should create a new WOLFSSL object and assign
    the saved session with wolfSSL_set_session().  At this point, the
    application may call wolfSSL_connect() and wolfSSL will try to resume
    the session.  The wolfSSL server code allows session resumption by default.
    The object returned by wolfSSL_get1_session() needs to be freed after the
    application is done with it by calling wolfSSL_SESSION_free() on it.

    \return SSL_SUCCESS will be returned upon successfully setting the session.
    \return SSL_FAILURE will be returned on failure.  This could be caused
    by the session cache being disabled, or if the session has timed out.

    \return When OPENSSL_EXTRA and WOLFSSL_ERROR_CODE_OPENSSL are defined,
    SSL_SUCCESS will be returned even if the session has timed out.

    \param ssl pointer to the SSL object, created with wolfSSL_new().
    \param session pointer to the WOLFSSL_SESSION used to set the session
    for ssl.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    WOLFSSL_SESSION* session;
    ...
    session = wolfSSL_get1_session(ssl);
    if (session == NULL) {
        // failed to get session object from ssl object
    }
    ...
    ret = wolfSSL_set_session(ssl, session);
    if (ret != SSL_SUCCESS) {
    	// failed to set the SSL session
    }
    wolfSSL_SESSION_free(session);
    ...
    \endcode

    \sa wolfSSL_get1_session
*/
int        wolfSSL_set_session(WOLFSSL* ssl, WOLFSSL_SESSION* session);

/*!
    \ingroup IO

    \brief When NO_SESSION_CACHE_REF is defined this function returns a pointer
    to the current session (WOLFSSL_SESSION) used in ssl. This function returns
    a non-persistent pointer to the WOLFSSL_SESSION object. The pointer returned
    will be freed when wolfSSL_free is called. This call should only be used to
    inspect or modify the current session. For session resumption it is
    recommended to use wolfSSL_get1_session(). For backwards compatibility when
    NO_SESSION_CACHE_REF is not defined this function returns a persistent
    session object pointer that is stored in the local cache. The cache size is
    finite and there is a risk that the session object will be overwritten by
    another ssl connection by the time the application calls
    wolfSSL_set_session() on it. It is recommended to define
    NO_SESSION_CACHE_REF in your application and to use wolfSSL_get1_session()
    for session resumption.

    \return pointer If successful the call will return a pointer to the the
    current SSL session object.
    \return NULL will be returned if ssl is NULL, the SSL session cache is
    disabled, wolfSSL doesn’t have the Session ID available, or mutex
    functions fail.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl;
    WOLFSSL_SESSION* session;
    ...
    session = wolfSSL_get_session(ssl);
    if (session == NULL) {
	    // failed to get session pointer
    }
    ...
    \endcode

    \sa wolfSSL_get1_session
    \sa wolfSSL_set_session
*/
WOLFSSL_SESSION* wolfSSL_get_session(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function flushes session from the session cache which
    have expired. The time, tm, is used for the time comparison. Note
    that wolfSSL currently uses a static table for sessions, so no flushing
    is needed. As such, this function is currently just a stub. This
    function provides OpenSSL compatibility (SSL_flush_sessions) when
    wolfSSL is compiled with the OpenSSL compatibility layer.

    \return none No returns.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param tm time used in session expiration comparison.

    _Example_
    \code
    WOLFSSL_CTX* ssl;
    ...
    wolfSSL_flush_sessions(ctx, time(0));
    \endcode

    \sa wolfSSL_get1_session
    \sa wolfSSL_set_session
*/
void       wolfSSL_flush_sessions(WOLFSSL_CTX* ctx, long tm);

/*!
    \ingroup TLS

    \brief This function associates the client session with the server id.
    If the newSession flag is on, an existing session won’t be reused.

    \return SSL_SUCCESS returned if the function executed without error.
    \return BAD_FUNC_ARG returned if the WOLFSSL struct or id parameter
    is NULL or if len is not greater than zero.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param id a constant byte pointer that will be copied to the
    serverID member of the WOLFSSL_SESSION structure.
    \param len an int type representing the length of the session id parameter.
    \param newSession an int type representing the flag to denote whether
    to reuse a session or not.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    const byte id[MAX_SIZE];  // or dynamically create space
    int len = 0; // initialize length
    int newSession = 0; // flag to allow
    …
    int ret = wolfSSL_SetServerID(ssl, id, len, newSession);

    if (ret == WOLFSSL_SUCCESS) {
	    // The Id was successfully set
    }
    \endcode

    \sa wolfSSL_set_session
*/
int        wolfSSL_SetServerID(WOLFSSL* ssl, const unsigned char* id,
                                         int len, int newSession);

/*!
    \ingroup IO

    \brief This function gets the session index of the WOLFSSL structure.

    \return int The function returns an int type representing the
    sessionIndex within the WOLFSSL struct.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    ...
    int sesIdx = wolfSSL_GetSessionIndex(ssl);

    if(sesIdx < 0 || sesIdx > sizeof(ssl->sessionIndex)/sizeof(int)){
    	// You have an out of bounds index number and something is not right.
    }
    \endcode

    \sa wolfSSL_GetSessionAtIndex
*/
int wolfSSL_GetSessionIndex(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function gets the session at specified index of the session
    cache and copies it into memory. The WOLFSSL_SESSION structure holds
    the session information.

    \return SSL_SUCCESS returned if the function executed successfully and
    no errors were thrown.
    \return BAD_MUTEX_E returned if there was an unlock or lock mutex error.
    \return SSL_FAILURE returned if the function did not execute successfully.

    \param idx an int type representing the session index.
    \param session a pointer to the WOLFSSL_SESSION structure.

    _Example_
    \code
    int idx; // The index to locate the session.
    WOLFSSL_SESSION* session;  // Buffer to copy to.
    ...
    if(wolfSSL_GetSessionAtIndex(idx, session) != SSL_SUCCESS){
    	// Failure case.
    }
    \endcode

    \sa UnLockMutex
    \sa LockMutex
    \sa wolfSSL_GetSessionIndex
*/
int wolfSSL_GetSessionAtIndex(int index, WOLFSSL_SESSION* session);

/*!
    \ingroup IO

    \brief Returns the peer certificate chain from the WOLFSSL_SESSION struct.

    \return pointer A pointer to a WOLFSSL_X509_CHAIN structure that
    contains the peer certification chain.

    \param session a pointer to a WOLFSSL_SESSION structure.

    _Example_
    \code
    WOLFSSL_SESSION* session;
    WOLFSSL_X509_CHAIN* chain;
    ...
    chain = wolfSSL_SESSION_get_peer_chain(session);
    if(!chain){
    	// There was no chain. Failure case.
    }
    \endcode

    \sa wolfSSL_GetSessionAtIndex
    \sa wolfSSL_GetSessionIndex
    \sa AddSession
*/

    WOLFSSL_X509_CHAIN* wolfSSL_SESSION_get_peer_chain(WOLFSSL_SESSION* session);

/*!
    \ingroup Setup

    \brief This function sets the verification method for remote peers and
    also allows a verify callback to be registered with the SSL context.
    The verify callback will be called only when a verification failure has
    occurred.  If no verify callback is desired, the NULL pointer can be used
    for verify_callback. The verification mode of peer certificates is a
    logically OR’d list of flags.  The possible flag values include:
    SSL_VERIFY_NONE Client mode: the client will not verify the certificate
    received from the server and the handshake will continue as normal.
    Server mode: the server will not send a certificate request to the client.
    As such, client verification will not be enabled. SSL_VERIFY_PEER Client
    mode: the client will verify the certificate received from the server
    during the handshake.  This is turned on by default in wolfSSL, therefore,
    using this option has no effect. Server mode: the server will send a
    certificate request to the client and verify the client certificate
    received. SSL_VERIFY_FAIL_IF_NO_PEER_CERT Client mode: no effect when
    used on the client side. Server mode: the verification will fail on the
    server side if the client fails to send a certificate when requested to
    do so (when using SSL_VERIFY_PEER on the SSL server).
    SSL_VERIFY_FAIL_EXCEPT_PSK Client mode: no effect when used on the client
    side. Server mode: the verification is the same as
    SSL_VERIFY_FAIL_IF_NO_PEER_CERT except in the case of a PSK connection.
    If a PSK connection is being made then the connection will go through
    without a peer cert.

    \return none No return.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param mode session timeout value in seconds
    \param verify_callback callback to be called when verification fails.
    If no callback is desired, the NULL pointer can be used for
    verify_callback.

    _Example_
    \code
    WOLFSSL_CTX*    ctx    = 0;
    ...
    wolfSSL_CTX_set_verify(ctx, (WOLFSSL_VERIFY_PEER |
                           WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT), NULL);
    \endcode

    \sa wolfSSL_set_verify
*/
void wolfSSL_CTX_set_verify(WOLFSSL_CTX* ctx, int mode,
                                      VerifyCallback verify_callback);

/*!
    \ingroup Setup

    \brief This function sets the verification method for remote peers and
    also allows a verify callback to be registered with the SSL session.
    The verify callback will be called only when a verification failure has
    occurred. If no verify callback is desired, the NULL pointer can be used
    for verify_callback. The verification mode of peer certificates is a
    logically OR’d list of flags.  The possible flag values include:
    SSL_VERIFY_NONE Client mode: the client will not verify the certificate
    received from the server and the handshake will continue as normal. Server
    mode: the server will not send a certificate request to the client.
    As such, client verification will not be enabled. SSL_VERIFY_PEER Client
    mode: the client will verify the certificate received from the server
    during the handshake. This is turned on by default in wolfSSL, therefore,
    using this option has no effect. Server mode: the server will send a
    certificate request to the client and verify the client certificate
    received. SSL_VERIFY_FAIL_IF_NO_PEER_CERT Client mode: no effect when
    used on the client side. Server mode: the verification will fail on the
    server side if the client fails to send a certificate when requested to do
    so (when using SSL_VERIFY_PEER on the SSL server).
    SSL_VERIFY_FAIL_EXCEPT_PSK Client mode: no effect when used on the client
    side. Server mode: the verification is the same as
    SSL_VERIFY_FAIL_IF_NO_PEER_CERT except in the case of a PSK connection.
    If a PSK connection is being made then the connection will go through
    without a peer cert.

    \return none No return.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param mode session timeout value in seconds.
    \param verify_callback callback to be called when verification fails.
    If no callback is desired, the NULL pointer can
    be used for verify_callback.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    ...
    wolfSSL_set_verify(ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
    \endcode

    \sa wolfSSL_CTX_set_verify
*/
void wolfSSL_set_verify(WOLFSSL* ssl, int mode, VerifyCallback verify_callback);

/*!
    \ingroup CertsKeys

    \brief This function stores user CTX object information for verify callback.

    \return none No return.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param ctx a void pointer that is set to WOLFSSL structure’s verifyCbCtx
    member’s value.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    (void*)ctx;
    ...
    if(ssl != NULL){
    wolfSSL_SetCertCbCtx(ssl, ctx);
    } else {
	    // Error case, the SSL is not initialized properly.
    }
    \endcode

    \sa wolfSSL_CTX_save_cert_cache
    \sa wolfSSL_CTX_restore_cert_cache
    \sa wolfSSL_CTX_set_verify
*/
void wolfSSL_SetCertCbCtx(WOLFSSL* ssl, void* ctx);

/*!
    \ingroup CertsKeys

    \brief This function stores user CTX object information for verify callback.

    \return none No return.

    \param ctx a pointer to a WOLFSSL_CTX structure.
    \param userCtx a void pointer that is used to set WOLFSSL_CTX structure’s
    verifyCbCtx member’s value.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    void* userCtx = NULL; // Assign some user defined context
    ...
    if(ctx != NULL){
        wolfSSL_SetCertCbCtx(ctx, userCtx);
    } else {
        // Error case, the SSL is not initialized properly.
    }
    \endcode

    \sa wolfSSL_CTX_save_cert_cache
    \sa wolfSSL_CTX_restore_cert_cache
    \sa wolfSSL_CTX_set_verify
*/
void wolfSSL_CTX_SetCertCbCtx(WOLFSSL_CTX* ctx, void* userCtx);

/*!
    \ingroup IO

    \brief This function returns the number of bytes which are buffered and
    available in the SSL object to be read by wolfSSL_read().

    \return int This function returns the number of bytes pending.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int pending = 0;
    WOLFSSL* ssl = 0;
    ...

    pending = wolfSSL_pending(ssl);
    printf(“There are %d bytes buffered and available for reading”, pending);
    \endcode

    \sa wolfSSL_recv
    \sa wolfSSL_read
    \sa wolfSSL_peek
*/
int  wolfSSL_pending(WOLFSSL*);

/*!
    \ingroup Debug

    \brief This function is for OpenSSL compatibility (SSL_load_error_string)
    only and takes no action.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    wolfSSL_load_error_strings();
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_ERR_error_string
    \sa wolfSSL_ERR_error_string_n
    \sa wolfSSL_ERR_print_errors_fp
    \sa wolfSSL_load_error_strings
*/
void wolfSSL_load_error_strings(void);

/*!
    \ingroup TLS

    \brief This function is called internally in wolfSSL_CTX_new(). This
    function is a wrapper around wolfSSL_Init() and exists for OpenSSL
    compatibility (SSL_library_init) when wolfSSL has been compiled with
    OpenSSL compatibility layer.  wolfSSL_Init() is the more typically-used
    wolfSSL initialization function.

    \return SSL_SUCCESS If successful the call will return.
    \return SSL_FATAL_ERROR is returned upon failure.

    \param none No parameters.

    _Example_
    \code
    int ret = 0;
    ret = wolfSSL_library_init();
    if (ret != SSL_SUCCESS) {
	    failed to initialize wolfSSL
    }
    ...
    \endcode

    \sa wolfSSL_Init
    \sa wolfSSL_Cleanup
*/
int  wolfSSL_library_init(void);

/*!
    \brief This function sets the Device Id at the WOLFSSL session level.

    \return WOLFSSL_SUCCESS upon success.
    \return BAD_FUNC_ARG if ssl is NULL.

    \param ssl pointer to a SSL object, created with wolfSSL_new().
    \param devId ID to use with async hardware

    _Example_
    \code
    WOLFSSL* ssl;
    int DevId = -2;

    wolfSSL_SetDevId(ssl, devId);

    \endcode

    \sa wolfSSL_CTX_SetDevId
    \sa wolfSSL_CTX_GetDevId
*/
int wolfSSL_SetDevId(WOLFSSL* ssl, int devId);

/*!
    \brief This function sets the Device Id at the WOLFSSL_CTX context level.

    \return WOLFSSL_SUCCESS upon success.
    \return BAD_FUNC_ARG if ssl is NULL.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param devId ID to use with async hardware

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    int DevId = -2;

    wolfSSL_CTX_SetDevId(ctx, devId);

    \endcode

    \sa wolfSSL_SetDevId
    \sa wolfSSL_CTX_GetDevId
*/
int wolfSSL_CTX_SetDevId(WOLFSSL_CTX* ctx, int devId);

/*!
    \brief This function retrieves the Device Id.

    \return devId upon success.
    \return INVALID_DEVID if both ssl and ctx are NULL.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param ssl pointer to a SSL object, created with wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx;

    wolfSSL_CTX_GetDevId(ctx, ssl);

    \endcode

    \sa wolfSSL_SetDevId
    \sa wolfSSL_CTX_SetDevId

*/
int wolfSSL_CTX_GetDevId(WOLFSSL_CTX* ctx, WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function enables or disables SSL session caching.
    Behavior depends on the value used for mode. The following values
    for mode are available: SSL_SESS_CACHE_OFF- disable session caching.
    Session caching is turned on by default. SSL_SESS_CACHE_NO_AUTO_CLEAR -
    Disable auto-flushing of the session cache. Auto-flushing is turned on
    by default.

    \return SSL_SUCCESS will be returned upon success.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param mode modifier used to change behavior of the session cache.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    ret = wolfSSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
    if (ret != SSL_SUCCESS) {
    	// failed to turn SSL session caching off
    }
    \endcode

    \sa wolfSSL_flush_sessions
    \sa wolfSSL_get1_session
    \sa wolfSSL_set_session
    \sa wolfSSL_get_sessionID
    \sa wolfSSL_CTX_set_timeout
*/
long wolfSSL_CTX_set_session_cache_mode(WOLFSSL_CTX* ctx, long mode);

/*!
    \brief This function sets the session secret callback function. The
    SessionSecretCb type has the signature: int (*SessionSecretCb)(WOLFSSL* ssl,
    void* secret, int* secretSz, void* ctx). The sessionSecretCb member of
    the WOLFSSL struct is set to the parameter cb.

    \return SSL_SUCCESS returned if the execution of the function did not
    return an error.
    \return SSL_FATAL_ERROR returned if the WOLFSSL structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a SessionSecretCb type that is a function pointer with the above
    signature.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    // Signature of SessionSecretCb
    int SessionSecretCB (WOLFSSL* ssl, void* secret, int* secretSz,
    void* ctx) = SessionSecretCb;
    …
    int wolfSSL_set_session_secret_cb(ssl, SessionSecretCB, (void*)ssl->ctx){
	    // Function body.
    }
    \endcode

    \sa SessionSecretCb
*/
int  wolfSSL_set_session_secret_cb(WOLFSSL* ssl, SessionSecretCb cb, void* ctx);

/*!
    \ingroup IO

    \brief This function persists the session cache to file. It doesn’t use
    memsave because of additional memory use.

    \return SSL_SUCCESS returned if the function executed without error.
    The session cache has been written to a file.
    \return SSL_BAD_FILE returned if fname cannot be opened or is otherwise
    corrupt.
    \return FWRITE_ERROR returned if XFWRITE failed to write to the file.
    \return BAD_MUTEX_E returned if there was a mutex lock failure.

    \param name is a constant char pointer that points to a file for writing.

    _Example_
    \code
    const char* fname;
    ...
    if(wolfSSL_save_session_cache(fname) != SSL_SUCCESS){
    	// Fail to write to file.
    }
    \endcode

    \sa XFWRITE
    \sa wolfSSL_restore_session_cache
    \sa wolfSSL_memrestore_session_cache
*/
int  wolfSSL_save_session_cache(const char*);

/*!
    \ingroup IO

    \brief This function restores the persistent session cache from file. It
    does not use memstore because of additional memory use.

    \return SSL_SUCCESS returned if the function executed without error.
    \return SSL_BAD_FILE returned if the file passed into the function was
    corrupted and could not be opened by XFOPEN.
    \return FREAD_ERROR returned if the file had a read error from XFREAD.
    \return CACHE_MATCH_ERROR returned if the session cache header match
    failed.
    \return BAD_MUTEX_E returned if there was a mutex lock failure.

    \param fname a constant char pointer file input that will be read.

    _Example_
    \code
    const char *fname;
    ...
    if(wolfSSL_restore_session_cache(fname) != SSL_SUCCESS){
        // Failure case. The function did not return SSL_SUCCESS.
    }
    \endcode

    \sa XFREAD
    \sa XFOPEN
*/
int  wolfSSL_restore_session_cache(const char*);

/*!
    \ingroup IO

    \brief This function persists session cache to memory.

    \return SSL_SUCCESS returned if the function executed without error.
    The session cache has been successfully persisted to memory.
    \return BAD_MUTEX_E returned if there was a mutex lock error.
    \return BUFFER_E returned if the buffer size was too small.

    \param mem a void pointer representing the destination for the memory
    copy, XMEMCPY().
    \param sz an int type representing the size of mem.

    _Example_
    \code
    void* mem;
    int sz; // Max size of the memory buffer.
    …
    if(wolfSSL_memsave_session_cache(mem, sz) != SSL_SUCCESS){
    	// Failure case, you did not persist the session cache to memory
    }
    \endcode

    \sa XMEMCPY
    \sa wolfSSL_get_session_cache_memsize
*/
int  wolfSSL_memsave_session_cache(void* mem, int sz);

/*!
    \ingroup IO

    \brief This function restores the persistent session cache from memory.

    \return SSL_SUCCESS returned if the function executed without an error.
    \return BUFFER_E returned if the memory buffer is too small.
    \return BAD_MUTEX_E returned if the session cache mutex lock failed.
    \return CACHE_MATCH_ERROR returned if the session cache header match
    failed.

    \param mem a constant void pointer containing the source of the
    restoration.
    \param sz an integer representing the size of the memory buffer.

    _Example_
    \code
    const void* memoryFile;
    int szMf;
    ...
    if(wolfSSL_memrestore_session_cache(memoryFile, szMf) != SSL_SUCCESS){
    	// Failure case. SSL_SUCCESS was not returned.
    }
    \endcode

    \sa wolfSSL_save_session_cache
*/
int  wolfSSL_memrestore_session_cache(const void* mem, int sz);

/*!
    \ingroup IO

    \brief This function returns how large the session cache save buffer
    should be.

    \return int This function returns an integer that represents the size of
    the session cache save buffer.

    \param none No parameters.

    _Example_
    \code
    int sz = // Minimum size for error checking;
    ...
    if(sz < wolfSSL_get_session_cache_memsize()){
        // Memory buffer is too small
    }
    \endcode

    \sa wolfSSL_memrestore_session_cache
*/
int  wolfSSL_get_session_cache_memsize(void);

/*!
    \ingroup CertsKeys

    \brief This function writes the cert cache from memory to file.

    \return SSL_SUCCESS if CM_SaveCertCache exits normally.
    \return BAD_FUNC_ARG is returned if either of the arguments are NULL.
    \return SSL_BAD_FILE if the cert cache save file could not be opened.
    \return BAD_MUTEX_E if the lock mutex failed.
    \return MEMORY_E the allocation of memory failed.
    \return FWRITE_ERROR Certificate cache file write failed.

    \param ctx a pointer to a WOLFSSL_CTX structure, holding the
    certificate information.
    \param fname  the cert cache buffer.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol def );
    const char* fname;
    ...
    if(wolfSSL_CTX_save_cert_cache(ctx, fname)){
	    // file was written.
    }
    \endcode

    \sa CM_SaveCertCache
    \sa DoMemSaveCertCache
*/
int  wolfSSL_CTX_save_cert_cache(WOLFSSL_CTX* ctx, const char* fname);

/*!
    \ingroup CertsKeys

    \brief This function persistes certificate cache from a file.

    \return SSL_SUCCESS returned if the function, CM_RestoreCertCache,
    executes normally.
    \return SSL_BAD_FILE returned if XFOPEN returns XBADFILE. The file is
    corrupted.
    \return MEMORY_E returned if the allocated memory for the temp buffer
    fails.
    \return BAD_FUNC_ARG returned if fname or ctx have a NULL value.

    \param ctx a pointer to a WOLFSSL_CTX structure, holding the certificate
    information.
    \param fname the cert cache buffer.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    const char* fname = "path to file";
    ...
    if(wolfSSL_CTX_restore_cert_cache(ctx, fname)){
    	// check to see if the execution was successful
    }
    \endcode

    \sa CM_RestoreCertCache
    \sa XFOPEN
*/
int  wolfSSL_CTX_restore_cert_cache(WOLFSSL_CTX* ctx, const char* fname);

/*!
    \ingroup CertsKeys

    \brief This function persists the certificate cache to memory.

    \return SSL_SUCCESS returned on successful execution of the function.
    No errors were thrown.
    \return BAD_MUTEX_E mutex error where the WOLFSSL_CERT_MANAGER member
    caLock was not 0 (zero).
    \return BAD_FUNC_ARG returned if ctx, mem, or used is NULL or if sz
    is less than or equal to 0 (zero).
    \return BUFFER_E output buffer mem was too small.

    \param ctx a pointer to a WOLFSSL_CTX structure, created
    using wolfSSL_CTX_new().
    \param mem a void pointer to the destination (output buffer).
    \param sz the size of the output buffer.
    \param used a pointer to size of the cert cache header.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol );
    void* mem;
    int sz;
    int* used;
    ...
    if(wolfSSL_CTX_memsave_cert_cache(ctx, mem, sz, used) != SSL_SUCCESS){
	    // The function returned with an error
    }
    \endcode

    \sa DoMemSaveCertCache
    \sa GetCertCacheMemSize
    \sa CM_MemRestoreCertCache
    \sa CM_GetCertCacheMemSize
*/
int  wolfSSL_CTX_memsave_cert_cache(WOLFSSL_CTX* ctx, void* mem, int sz, int* used);

/*!
    \ingroup Setup

    \brief This function restores the certificate cache from memory.

    \return SSL_SUCCESS returned if the function and subroutines
    executed without an error.
    \return BAD_FUNC_ARG returned if the ctx or mem parameters are
    NULL or if the sz parameter is less than or equal to zero.
    \return BUFFER_E returned if the cert cache memory buffer is too small.
    \return CACHE_MATCH_ERROR returned if there was a cert cache
    header mismatch.
    \return BAD_MUTEX_E returned if the lock mutex on failed.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param mem a void pointer with a value that will be restored to
    the certificate cache.
    \param sz an int type that represents the size of the mem parameter.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    void* mem;
    int sz = (*int) sizeof(mem);
    …
    if(wolfSSL_CTX_memrestore_cert_cache(ssl->ctx, mem, sz)){
    	// The success case
    }
    \endcode

    \sa CM_MemRestoreCertCache
*/
int  wolfSSL_CTX_memrestore_cert_cache(WOLFSSL_CTX* ctx, const void* mem, int sz);

/*!
    \ingroup CertsKeys

    \brief Returns the size the certificate cache save buffer needs to be.

    \return int integer value returned representing the memory size
    upon success.
    \return BAD_FUNC_ARG is returned if the WOLFSSL_CTX struct is NULL.
    \return BAD_MUTEX_E - returned if there was a mutex lock error.

    \param ctx a pointer to a wolfSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new(protocol);
    ...
    int certCacheSize = wolfSSL_CTX_get_cert_cache_memsize(ctx);

    if(certCacheSize != BAD_FUNC_ARG || certCacheSize != BAD_MUTEX_E){
	// Successfully retrieved the memory size.
    }
    \endcode

    \sa CM_GetCertCacheMemSize
*/
int  wolfSSL_CTX_get_cert_cache_memsize(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function sets cipher suite list for a given WOLFSSL_CTX.
    This cipher suite list becomes the default list for any new SSL sessions
    (WOLFSSL) created using this context.  The ciphers in the list should be
    sorted in order of preference from highest to lowest.  Each call to
    wolfSSL_CTX_set_cipher_list() resets the cipher suite list for the
    specific SSL context to the provided list each time the function is
    called. The cipher suite list, list, is a null-terminated text string,
    and a colon-delimited list.  For example, one value for list may be
    "DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:AES256-SHA256" Valid cipher
    values are the full name values from the cipher_names[] array in
    src/internal.c (for a definite list of valid cipher values check
    src/internal.c)

    \return SSL_SUCCESS will be returned upon successful function completion.
    \return SSL_FAILURE will be returned on failure.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param list null-terminated text string and a colon-delimited list of
    cipher suites to use with the specified SSL context.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    ret = wolfSSL_CTX_set_cipher_list(ctx,
    “DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:AES256-SHA256”);
    if (ret != SSL_SUCCESS) {
    	// failed to set cipher suite list
    }
    \endcode

    \sa wolfSSL_set_cipher_list
    \sa wolfSSL_CTX_new
*/
int  wolfSSL_CTX_set_cipher_list(WOLFSSL_CTX* ctx, const char* list);

/*!
    \ingroup Setup

    \brief This function sets cipher suite list for a given WOLFSSL object
    (SSL session).  The ciphers in the list should be sorted in order of
    preference from highest to lowest.  Each call to wolfSSL_set_cipher_list()
    resets the cipher suite list for the specific SSL session to the provided
    list each time the function is called. The cipher suite list, list, is a
    null-terminated text string, and a colon-delimited list. For example, one
    value for list may be
    "DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:AES256-SHA256".
    Valid cipher values are the full name values from the cipher_names[]
    array in src/internal.c (for a definite list of valid cipher values
    check src/internal.c)

    \return SSL_SUCCESS will be returned upon successful function completion.
    \return SSL_FAILURE will be returned on failure.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param list null-terminated text string and a colon-delimited list of
    cipher suites to use with the specified SSL session.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_set_cipher_list(ssl,
    “DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:AES256-SHA256”);
    if (ret != SSL_SUCCESS) {
    	// failed to set cipher suite list
    }
    \endcode

    \sa wolfSSL_CTX_set_cipher_list
    \sa wolfSSL_new
*/
int  wolfSSL_set_cipher_list(WOLFSSL* ssl, const char* list);

/*!
    \brief This function informs the WOLFSSL DTLS object that the underlying
     UDP I/O is non-blocking. After an application creates a WOLFSSL object,
     if it will be used with a non-blocking UDP socket, call
    wolfSSL_dtls_set_using_nonblock() on it. This lets the WOLFSSL object know
     that receiving EWOULDBLOCK means that the recvfrom call would
    block rather than that it timed out.

    \return none No return.

    \param ssl pointer to the DTLS session, created with wolfSSL_new().
    \param nonblock value used to set non-blocking flag on WOLFSSL object.
    Use 1 to specify non-blocking, otherwise 0.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    ...
    wolfSSL_dtls_set_using_nonblock(ssl, 1);
    \endcode

    \sa wolfSSL_dtls_get_using_nonblock
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_get_current_timeout
*/
void wolfSSL_dtls_set_using_nonblock(WOLFSSL* ssl, int nonblock);
/*!
    \brief This function allows the application to determine if wolfSSL is
    using non-blocking I/O with UDP. If wolfSSL is using non-blocking I/O, this
    function will return 1, otherwise 0. After an application creates a
    WOLFSSL object, if it will be used with a non-blocking UDP socket, call
    wolfSSL_dtls_set_using_nonblock() on it. This lets the WOLFSSL object know
    that receiving EWOULDBLOCK means that the recvfrom call would block
    rather than that it timed out. This function is only meaningful to DTLS
    sessions.

    \return 0 underlying I/O is blocking.
    \return 1 underlying I/O is non-blocking.

    \param ssl pointer to the DTLS session, created with wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_dtls_get_using_nonblock(ssl);
    if (ret == 1) {
    	// underlying I/O is non-blocking
    }
    ...
    \endcode

    \sa wolfSSL_dtls_set_using_nonblock
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_set_using_nonblock
*/
int  wolfSSL_dtls_get_using_nonblock(WOLFSSL*);
/*!
    \brief This function returns the current timeout value in seconds for
    the WOLFSSL object. When using non-blocking sockets, something in the user
    code needs to decide when to check for available recv data and how long
    it has been waiting. The value returned by this function indicates how
    long the application should wait.

    \return seconds The current DTLS timeout value in seconds
    \return NOT_COMPILED_IN if wolfSSL was not built with DTLS support.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int timeout = 0;
    WOLFSSL* ssl;
    ...
    timeout = wolfSSL_get_dtls_current_timeout(ssl);
    printf(“DTLS timeout (sec) = %d\n”, timeout);
    \endcode

    \sa wolfSSL_dtls
    \sa wolfSSL_dtls_get_peer
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_set_peer
*/
int  wolfSSL_dtls_get_current_timeout(WOLFSSL* ssl);
/*!
    \brief This function returns true if the application should setup a quicker
    timeout. When using non-blocking sockets, something in the user code needs
    to decide when to check for available data and how long it needs to wait. If
    this function returns true, it means that the library already detected some
    disruption in the communication, but it wants to wait for a little longer in
    case some messages from the other peers are still in flight. Is up to the
    application to fine tune the value of this timer, a good one may be
    dtls_get_current_timeout() / 4.

    \return true if the application code should setup a quicker timeout

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_dtls
    \sa wolfSSL_dtls_get_peer
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_set_peer
    \sa wolfSSL_dtls13_set_send_more_acks
*/
int  wolfSSL_dtls13_use_quick_timeout(WOLFSSL *ssl);
/*!
  \ingroup Setup

    \brief This function sets whether the library should send ACKs to the other
    peer immediately when detecting disruption or not. Sending ACKs immediately
    assures minimum latency but it may consume more bandwidth than necessary. If
    the application manages the timer by itself and this option is set to 0 then
    application code can use wolfSSL_dtls13_use_quick_timeout() to determine if
    it should setup a quicker timeout to send those delayed ACKs.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param value 1 to set the option, 0 to disable the option

    \sa wolfSSL_dtls
    \sa wolfSSL_dtls_get_peer
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_set_peer
    \sa wolfSSL_dtls13_use_quick_timeout
*/
void  wolfSSL_dtls13_set_send_more_acks(WOLFSSL *ssl, int value);

/*!
    \ingroup Setup

    \brief This function sets the dtls timeout.

    \return SSL_SUCCESS returned if the function executes without an error.
    The dtls_timeout_init and the dtls_timeout members of SSL have been set.
    \return BAD_FUNC_ARG returned if the WOLFSSL struct is NULL or if
    the timeout is not greater than 0. It will also return if the timeout
    argument exceeds the maximum value allowed.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param timeout an int type that will be set to the dtls_timeout_init
    member of the WOLFSSL structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    int timeout = TIMEOUT;
    ...
    if(wolfSSL_dtls_set_timeout_init(ssl, timeout)){
    	// the dtls timeout was set
    } else {
    	// Failed to set DTLS timeout.
    }
    \endcode

    \sa wolfSSL_dtls_set_timeout_max
    \sa wolfSSL_dtls_got_timeout
*/
int  wolfSSL_dtls_set_timeout_init(WOLFSSL* ssl, int);

/*!
    \brief This function sets the maximum dtls timeout.

    \return SSL_SUCCESS returned if the function executed without an error.
    \return BAD_FUNC_ARG returned if the WOLFSSL struct is NULL or if
    the timeout argument is not greater than zero or is less than the
    dtls_timeout_init member of the WOLFSSL structure.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param timeout an int type representing the dtls maximum timeout.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    int timeout = TIMEOUTVAL;
    ...
    int ret = wolfSSL_dtls_set_timeout_max(ssl);
    if(!ret){
    	// Failed to set the max timeout
    }
    \endcode

    \sa wolfSSL_dtls_set_timeout_init
    \sa wolfSSL_dtls_got_timeout
*/
int  wolfSSL_dtls_set_timeout_max(WOLFSSL* ssl, int);

/*!
    \brief When using non-blocking sockets with DTLS, this function should
    be called on the WOLFSSL object when the controlling code thinks the
    transmission has timed out. It performs the actions needed to retry
    the last transmit, including adjusting the timeout value. If it
    has been too long, this will return a failure.

    \return SSL_SUCCESS will be returned upon success
    \return SSL_FATAL_ERROR will be returned if there have been too many
    retransmissions/timeouts without getting a response from the peer.
    \return NOT_COMPILED_IN will be returned if wolfSSL was not compiled with
    DTLS support.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    See the following files for usage examples:
    <wolfssl_root>/examples/client/client.c
    <wolfssl_root>/examples/server/server.c
    \endcode

    \sa wolfSSL_dtls_get_current_timeout
    \sa wolfSSL_dtls_get_peer
    \sa wolfSSL_dtls_set_peer
    \sa wolfSSL_dtls
*/
int  wolfSSL_dtls_got_timeout(WOLFSSL* ssl);

/*!
    \brief When using non-blocking sockets with DTLS, this function retransmits
    the last handshake flight ignoring the expected timeout value and
    retransmit count. It is useful for applications that are using DTLS and
    need to manage even the timeout and retry count.

    \return SSL_SUCCESS will be returned upon success
    \return SSL_FATAL_ERROR will be returned if there have been too many
    retransmissions/timeouts without getting a response from the peer.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_dtls_retransmit(ssl);
    \endcode

    \sa wolfSSL_dtls_get_current_timeout
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls
*/
int wolfSSL_dtls_retransmit(WOLFSSL* ssl);

/*!
    \brief This function is used to determine if the SSL session has been
    configured to use DTLS.

    \return 1 If the SSL session (ssl) has been configured to use DTLS, this
    function will return 1.
    \return 0 otherwise.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_dtls(ssl);
    if (ret) {
    	// SSL session has been configured to use DTLS
    }
    \endcode

    \sa wolfSSL_dtls_get_current_timeout
    \sa wolfSSL_dtls_get_peer
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_set_peer
*/
int  wolfSSL_dtls(WOLFSSL* ssl);

/*!
    \brief This function sets the DTLS peer, peer (sockaddr_in) with size of
    peerSz.

    \return SSL_SUCCESS will be returned upon success.
    \return SSL_FAILURE will be returned upon failure.
    \return SSL_NOT_IMPLEMENTED will be returned if wolfSSL was not compiled
    with DTLS support.

    \param ssl    a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param peer   pointer to peer’s sockaddr_in structure. If NULL then the peer
                  information in ssl is cleared.
    \param peerSz size of the sockaddr_in structure pointed to by peer. If 0
                  then the peer information in ssl is cleared.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    sockaddr_in addr;
    ...
    ret = wolfSSL_dtls_set_peer(ssl, &addr, sizeof(addr));
    if (ret != SSL_SUCCESS) {
	    // failed to set DTLS peer
    }
    \endcode

    \sa wolfSSL_dtls_get_current_timeout
    \sa wolfSSL_dtls_get_peer
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls
*/
int  wolfSSL_dtls_set_peer(WOLFSSL* ssl, void* peer, unsigned int peerSz);

/*!
    \brief This function gets the sockaddr_in (of size peerSz) of the current
    DTLS peer.  The function will compare peerSz to the actual DTLS peer size
    stored in the SSL session.  If the peer will fit into peer, the peer’s
    sockaddr_in will be copied into peer, with peerSz set to the size of peer.

    \return SSL_SUCCESS will be returned upon success.
    \return SSL_FAILURE will be returned upon failure.
    \return SSL_NOT_IMPLEMENTED will be returned if wolfSSL was not compiled
    with DTLS support.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param peer pointer to memory location to store peer’s sockaddr_in
    structure.
    \param peerSz input/output size. As input, the size of the allocated memory
    pointed to by peer.  As output, the size of the actual sockaddr_in structure
    pointed to by peer.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    sockaddr_in addr;
    ...
    ret = wolfSSL_dtls_get_peer(ssl, &addr, sizeof(addr));
    if (ret != SSL_SUCCESS) {
	    // failed to get DTLS peer
    }
    \endcode

    \sa wolfSSL_dtls_get_current_timeout
    \sa wolfSSL_dtls_got_timeout
    \sa wolfSSL_dtls_set_peer
    \sa wolfSSL_dtls
*/
int  wolfSSL_dtls_get_peer(WOLFSSL* ssl, void* peer, unsigned int* peerSz);

/*!
    \ingroup Debug

    \brief This function converts an error code returned by
    wolfSSL_get_error() into a more human-readable error string.
    errNumber is the error code returned by wolfSSL_get_error() and data
    is the storage buffer which the error string will be placed in.
    The maximum length of data is 80 characters by default, as defined by
    MAX_ERROR_SZ is wolfssl/wolfcrypt/error.h.

    \return success On successful completion, this function returns the same
    string as is returned in data.
    \return failure Upon failure, this function returns a string with the
    appropriate failure reason, msg.

    \param errNumber error code returned by wolfSSL_get_error().
    \param data output buffer containing human-readable error string matching
    errNumber.

    _Example_
    \code
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...
    err = wolfSSL_get_error(ssl, 0);
    wolfSSL_ERR_error_string(err, buffer);
    printf(“err = %d, %s\n”, err, buffer);
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_ERR_error_string_n
    \sa wolfSSL_ERR_print_errors_fp
    \sa wolfSSL_load_error_strings
*/
char* wolfSSL_ERR_error_string(unsigned long,char*);

/*!
    \ingroup Debug

    \brief This function is a version of wolfSSL_ERR_error_string() where
    len specifies the maximum number of characters that may be written to buf.
    Like wolfSSL_ERR_error_string(), this function converts an error code
    returned from wolfSSL_get_error() into a more human-readable error string.
    The human-readable string is placed in buf.

    \return none No returns.

    \param e error code returned by wolfSSL_get_error().
    \param buff output buffer containing human-readable error string matching e.
    \param len maximum length in characters which may be written to buf.

    _Example_
    \code
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...
    err = wolfSSL_get_error(ssl, 0);
    wolfSSL_ERR_error_string_n(err, buffer, 80);
    printf(“err = %d, %s\n”, err, buffer);
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_ERR_error_string
    \sa wolfSSL_ERR_print_errors_fp
    \sa wolfSSL_load_error_strings
*/
void  wolfSSL_ERR_error_string_n(unsigned long e, char* buf,
                                           unsigned long sz);

/*!
    \ingroup TLS

    \brief This function checks the shutdown conditions in closeNotify or
    connReset or sentNotify members of the Options structure. The Options
    structure is within the WOLFSSL structure.

    \return 1 SSL_SENT_SHUTDOWN is returned.
    \return 2 SS_RECEIVED_SHUTDOWN is returned.

    \param ssl a constant pointer to a WOLFSSL structure, created using
    wolfSSL_new().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    …
    int ret;
    ret = wolfSSL_get_shutdown(ssl);

    if(ret == 1){
	    SSL_SENT_SHUTDOWN
    } else if(ret == 2){
	    SSL_RECEIVED_SHUTDOWN
    } else {
	    Fatal error.
    }
    \endcode

    \sa wolfSSL_SESSION_free
*/
int  wolfSSL_get_shutdown(const WOLFSSL*);

/*!
    \ingroup IO

    \brief This function returns the resuming member of the options struct. The
    flag indicates whether or not to reuse a session. If not, a new session must
    be established.

    \return This function returns an int type held in the Options structure
    representing the flag for session reuse.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    if(!wolfSSL_session_reused(sslResume)){
	    // No session reuse allowed.
    }
    \endcode

    \sa wolfSSL_SESSION_free
    \sa wolfSSL_GetSessionIndex
    \sa wolfSSL_memsave_session_cache
*/
int  wolfSSL_session_reused(WOLFSSL*);

/*!
    \ingroup TLS

    \brief This function checks to see if the connection is established.

    \return 0 returned if the connection is not established, i.e. the WOLFSSL
    struct is NULL or the handshake is not done.
    \return 1 returned if the connection is not established i.e. the WOLFSSL
    struct is null or the handshake is not done.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _EXAMPLE_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_is_init_finished(ssl)){
	    Handshake is done and connection is established
    }
    \endcode

    \sa wolfSSL_set_accept_state
    \sa wolfSSL_get_keys
    \sa wolfSSL_set_shutdown
*/
int  wolfSSL_is_init_finished(WOLFSSL*);

/*!
    \ingroup IO

    \brief Returns the SSL version being used as a string.

    \return "SSLv3" Using SSLv3
    \return "TLSv1" Using TLSv1
    \return "TLSv1.1" Using TLSv1.1
    \return "TLSv1.2" Using TLSv1.2
    \return "TLSv1.3" Using TLSv1.3
    \return "DTLS": Using DTLS
    \return "DTLSv1.2" Using DTLSv1.2
    \return "unknown" There was a problem determining which version of TLS
    being used.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    WOLFSSL_METHOD method = // Some wolfSSL method
    ctx = wolfSSL_CTX_new(method);
    ssl = wolfSSL_new(ctx);
    printf(wolfSSL_get_version("Using version: %s", ssl));
    \endcode

    \sa wolfSSL_lib_version
*/
const char*  wolfSSL_get_version(WOLFSSL*);

/*!
    \ingroup IO

    \brief Returns the current cipher suit an ssl session is using.

    \return ssl->options.cipherSuite An integer representing the current
    cipher suite.
    \return 0 The ssl session provided is null.

    \param ssl The SSL session to check.

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    WOLFSSL_METHOD method = // Some wolfSSL method
    ctx = wolfSSL_CTX_new(method);
    ssl = wolfSSL_new(ctx);

    if(wolfSSL_get_current_cipher_suite(ssl) == 0)
    {
        // Error getting cipher suite
    }
    \endcode

    \sa wolfSSL_CIPHER_get_name
    \sa wolfSSL_get_current_cipher
    \sa wolfSSL_get_cipher_list
*/
int  wolfSSL_get_current_cipher_suite(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function returns a pointer to the current cipher in the
    ssl session.

    \return The function returns the address of the cipher member of the
    WOLFSSL struct. This is a pointer to the WOLFSSL_CIPHER structure.
    \return NULL returned if the WOLFSSL structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    WOLFSSL_CIPHER* cipherCurr = wolfSSL_get_current_cipher;

    if(!cipherCurr){
    	// Failure case.
    } else {
    	// The cipher was returned to cipherCurr
    }
    \endcode

    \sa wolfSSL_get_cipher
    \sa wolfSSL_get_cipher_name_internal
    \sa wolfSSL_get_cipher_name
*/
WOLFSSL_CIPHER*  wolfSSL_get_current_cipher(WOLFSSL*);

/*!
    \ingroup IO

    \brief This function matches the cipher suite in the SSL object with
    the available suites and returns the string representation.

    \return string This function returns the string representation of the
    matched cipher suite.
    \return none It will return “None” if there are no suites matched.

    \param cipher a constant pointer to a WOLFSSL_CIPHER structure.

    _Example_
    \code
    // gets cipher name in the format DHE_RSA ...
    const char* wolfSSL_get_cipher_name_internal(WOLFSSL* ssl){
	WOLFSSL_CIPHER* cipher;
	const char* fullName;
    …
	cipher = wolfSSL_get_curent_cipher(ssl);
	fullName = wolfSSL_CIPHER_get_name(cipher);

	if(fullName){
		// sanity check on returned cipher
	}
    \endcode

    \sa wolfSSL_get_cipher
    \sa wolfSSL_get_current_cipher
    \sa wolfSSL_get_cipher_name_internal
    \sa wolfSSL_get_cipher_name
*/
const char*  wolfSSL_CIPHER_get_name(const WOLFSSL_CIPHER* cipher);

/*!
    \ingroup IO

    \brief This function matches the cipher suite in the SSL object with
    the available suites.

    \return This function returns the string value of the suite matched. It
    will return “None” if there are no suites matched.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    #ifdef WOLFSSL_DTLS
    …
    // make sure a valid suite is used
    if(wolfSSL_get_cipher(ssl) == NULL){
	    WOLFSSL_MSG(“Can not match cipher suite imported”);
	    return MATCH_SUITE_ERROR;
    }
    …
    #endif // WOLFSSL_DTLS
    \endcode

    \sa wolfSSL_CIPHER_get_name
    \sa wolfSSL_get_current_cipher
*/
const char*  wolfSSL_get_cipher(WOLFSSL*);

/*!
    \ingroup Setup

    \brief This function returns the WOLFSSL_SESSION from the WOLFSSL structure
    as a reference type. This requires calling wolfSSL_SESSION_free to release
    the session reference. The WOLFSSL_SESSION pointed to contains all the
    necessary information required to perform a session resumption and
    reestablish the connection without a new handshake. For
    session resumption, before calling wolfSSL_shutdown() with your session
    object, an application should save the session ID from the object with a
    call to wolfSSL_get1_session(), which returns a pointer to the session.
    Later, the application should create a new WOLFSSL object and assign the
    saved session with wolfSSL_set_session().  At this point, the application
    may call wolfSSL_connect() and wolfSSL will try to resume the session.
    The wolfSSL server code allows session resumption by default. The object
    returned by wolfSSL_get1_session() needs to be freed after the application
    is done with it by calling wolfSSL_SESSION_free() on it.

    \return WOLFSSL_SESSION On success return session pointer.
    \return NULL will be returned if ssl is NULL, the SSL session cache is
    disabled, wolfSSL doesn’t have the Session ID available, or mutex
    functions fail.

    \param ssl WOLFSSL structure to get session from.

    _Example_
    \code
    WOLFSSL* ssl;
    WOLFSSL_SESSION* ses;
    // attempt/complete handshake
    wolfSSL_connect(ssl);
    ses  = wolfSSL_get1_session(ssl);
    // check ses information
    // disconnect / setup new SSL instance
    wolfSSL_set_session(ssl, ses);
    // attempt/resume handshake
    wolfSSL_SESSION_free(ses);
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
    \sa wolfSSL_SESSION_free
*/
WOLFSSL_SESSION* wolfSSL_get1_session(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief The wolfSSLv23_client_method() function is used to indicate that
    the application is a client and will support the highest protocol
    version supported by the server between SSL 3.0 - TLS 1.3.  This function
    allocates memory for and initializes a new WOLFSSL_METHOD structure
    to be used when creating the SSL/TLS context with wolfSSL_CTX_new().
    Both wolfSSL clients and servers have robust version downgrade capability.
    If a specific protocol version method is used on either side, then only
    that version will be negotiated or an error will be returned.  For
    example, a client that uses TLSv1 and tries to connect to a SSLv3 only
    server will fail, likewise connecting to a TLSv1.1 will fail as well.
    To resolve this issue, a client that uses the wolfSSLv23_client_method()
    function will use the highest protocol version supported by the server and
    downgrade to SSLv3 if needed. In this case, the client will be able to
    connect to a server running SSLv3 - TLSv1.3.

    \return pointer upon success a pointer to a WOLFSSL_METHOD.
    \return Failure If memory allocation fails when calling XMALLOC,
    the failure value of the underlying malloc() implementation will be
    returned (typically NULL with errno will be set to ENOMEM).

    \param none No parameters

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;
    method = wolfSSLv23_client_method();
    if (method == NULL) {
	// unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfDTLSv1_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD* wolfSSLv23_client_method(void);

/*!
    \ingroup IO

    \brief This is used to set a byte pointer to the start of the
    internal memory buffer.

    \return size On success the size of the buffer is returned
    \return SSL_FATAL_ERROR If an error case was encountered.

    \param bio WOLFSSL_BIO structure to get memory buffer of.
    \param p byte pointer to set to memory buffer.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    const byte* p;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_mem());
    ret  = wolfSSL_BIO_get_mem_data(bio, &p);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_set_fp
    \sa wolfSSL_BIO_free
*/
int wolfSSL_BIO_get_mem_data(WOLFSSL_BIO* bio,void* p);

/*!
    \ingroup IO

    \brief Sets the file descriptor for bio to use.

    \return SSL_SUCCESS(1) upon success.

    \param bio WOLFSSL_BIO structure to set fd.
    \param fd file descriptor to use.
    \param closeF flag for behavior when closing fd.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    int fd;
    // setup bio
    wolfSSL_BIO_set_fd(bio, fd, BIO_NOCLOSE);
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_free
*/
long wolfSSL_BIO_set_fd(WOLFSSL_BIO* b, int fd, int flag);

/*!
    \ingroup IO

    \brief Sets the close flag, used to indicate that the i/o stream should be
     closed when the BIO is freed

    \return SSL_SUCCESS(1) upon success.

    \param bio WOLFSSL_BIO structure.
    \param flag flag for behavior when closing i/o stream.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    // setup bio
    wolfSSL_BIO_set_close(bio, BIO_NOCLOSE);
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_free
*/
int wolfSSL_BIO_set_close(WOLFSSL_BIO *b, long flag);

/*!
    \ingroup IO

    \brief This is used to get a BIO_SOCKET type WOLFSSL_BIO_METHOD.

    \return WOLFSSL_BIO_METHOD pointer to a WOLFSSL_BIO_METHOD structure
    that is a socket type

    \param none No parameters.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    bio = wolfSSL_BIO_new(wolfSSL_BIO_s_socket);
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
*/
WOLFSSL_BIO_METHOD *wolfSSL_BIO_s_socket(void);

/*!
    \ingroup IO

    \brief This is used to set the size of write buffer for a
    WOLFSSL_BIO. If write buffer has been previously set this
    function will free it when resetting the size. It is similar to
    wolfSSL_BIO_reset in that it resets read and write indexes to 0.

    \return SSL_SUCCESS On successfully setting the write buffer.
    \return SSL_FAILURE If an error case was encountered.

    \param bio WOLFSSL_BIO structure to set fd.
    \param size size of buffer to allocate.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    int ret;
    bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem());
    ret = wolfSSL_BIO_set_write_buf_size(bio, 15000);
    // check return value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_free
*/
int  wolfSSL_BIO_set_write_buf_size(WOLFSSL_BIO *b, long size);

/*!
    \ingroup IO

    \brief This is used to pair two bios together. A pair of bios acts
    similar to a two way pipe writing to one can be read by the other
    and vice versa. It is expected that both bios be in the same thread,
    this function is not thread safe. Freeing one of the two bios removes
    both from being paired. If a write buffer size was not previously
    set for either of the bios it is set to a default size of 17000
    (WOLFSSL_BIO_SIZE) before being paired.

    \return SSL_SUCCESS On successfully pairing the two bios.
    \return SSL_FAILURE If an error case was encountered.

    \param b1 WOLFSSL_BIO structure to set pair.
    \param b2 second WOLFSSL_BIO structure to complete pair.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    WOLFSSL_BIO* bio2;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_bio());
    bio2 = wolfSSL_BIO_new(wolfSSL_BIO_s_bio());
    ret = wolfSSL_BIO_make_bio_pair(bio, bio2);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_free
*/
int  wolfSSL_BIO_make_bio_pair(WOLFSSL_BIO *b1, WOLFSSL_BIO *b2);

/*!
    \ingroup IO

    \brief This is used to set the read request flag back to 0.

    \return SSL_SUCCESS On successfully setting value.
    \return SSL_FAILURE If an error case was encountered.

    \param bio WOLFSSL_BIO structure to set read request flag.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    int ret;
    ...
    ret = wolfSSL_BIO_ctrl_reset_read_request(bio);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new, wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_new, wolfSSL_BIO_free
*/
int  wolfSSL_BIO_ctrl_reset_read_request(WOLFSSL_BIO *b);

/*!
    \ingroup IO

    \brief This is used to get a buffer pointer for reading from. Unlike
    wolfSSL_BIO_nread the internal read index is not advanced by the number
    returned from the function call. Reading past the value returned can
    result in reading out of array bounds.

    \return >=0 on success return the number of bytes to read

    \param bio WOLFSSL_BIO structure to read from.
    \param buf pointer to set at beginning of read array.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    char* bufPt;
    int ret;
    // set up bio
    ret = wolfSSL_BIO_nread0(bio, &bufPt); // read as many bytes as possible
    // handle negative ret check
    // read ret bytes from bufPt
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_nwrite0
*/
int  wolfSSL_BIO_nread0(WOLFSSL_BIO *bio, char **buf);

/*!
    \ingroup IO

    \brief This is used to get a buffer pointer for reading from. The internal
    read index is advanced by the number returned from the function call with
    buf being pointed to the beginning of the buffer to read from. In the
    case that less bytes are in the read buffer than the value requested with
    num the lesser value is returned. Reading past the value returned can
    result in reading out of array bounds.

    \return >=0 on success return the number of bytes to read
    \return WOLFSSL_BIO_ERROR(-1) on error case with nothing to read return -1

    \param bio WOLFSSL_BIO structure to read from.
    \param buf pointer to set at beginning of read array.
    \param num number of bytes to try and read.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    char* bufPt;
    int ret;

    // set up bio
    ret = wolfSSL_BIO_nread(bio, &bufPt, 10); // try to read 10 bytes
    // handle negative ret check
    // read ret bytes from bufPt
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_nwrite
*/
int  wolfSSL_BIO_nread(WOLFSSL_BIO *bio, char **buf, int num);

/*!
    \ingroup IO

    \brief Gets a pointer to the buffer for writing as many bytes as returned by
    the function. Writing more bytes to the pointer returned then the value
    returned can result in writing out of bounds.

    \return int Returns the number of bytes that can be written to the buffer
    pointer returned.
    \return WOLFSSL_BIO_UNSET(-2) in the case that is not part of a bio pair
    \return WOLFSSL_BIO_ERROR(-1) in the case that there is no more room to
    write to

    \param bio WOLFSSL_BIO structure to write to.
    \param buf pointer to buffer to write to.
    \param num number of bytes desired to be written.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    char* bufPt;
    int ret;
    // set up bio
    ret = wolfSSL_BIO_nwrite(bio, &bufPt, 10); // try to write 10 bytes
    // handle negative ret check
    // write ret bytes to bufPt
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_free
    \sa wolfSSL_BIO_nread
*/
int  wolfSSL_BIO_nwrite(WOLFSSL_BIO *bio, char **buf, int num);

/*!
    \ingroup IO

    \brief Resets bio to an initial state. As an example for type BIO_BIO
    this resets the read and write index.

    \return 0 On successfully resetting the bio.
    \return WOLFSSL_BIO_ERROR(-1) Returned on bad input or unsuccessful reset.

    \param bio WOLFSSL_BIO structure to reset.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    // setup bio
    wolfSSL_BIO_reset(bio);
    //use pt
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_free
*/
int  wolfSSL_BIO_reset(WOLFSSL_BIO *bio);

/*!
    \ingroup IO

    \brief This function adjusts the file pointer to the offset given. This
    is the offset from the head of the file.

    \return 0 On successfully seeking.
    \return -1 If an error case was encountered.

    \param bio WOLFSSL_BIO structure to set.
    \param ofs offset into file.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    XFILE fp;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
    ret  = wolfSSL_BIO_set_fp(bio, &fp);
    // check ret value
    ret  = wolfSSL_BIO_seek(bio, 3);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_set_fp
    \sa wolfSSL_BIO_free
*/
int  wolfSSL_BIO_seek(WOLFSSL_BIO *bio, int ofs);

/*!
    \ingroup IO

    \brief This is used to set and write to a file. WIll overwrite any data
    currently in the file and is set to close the file when the bio is freed.

    \return SSL_SUCCESS On successfully opening and setting file.
    \return SSL_FAILURE If an error case was encountered.

    \param bio WOLFSSL_BIO structure to set file.
    \param name name of file to write to.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
    ret  = wolfSSL_BIO_write_filename(bio, “test.txt”);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_file
    \sa wolfSSL_BIO_set_fp
    \sa wolfSSL_BIO_free
*/
int  wolfSSL_BIO_write_filename(WOLFSSL_BIO *bio, char *name);

/*!
    \ingroup IO

    \brief This is used to set the end of file value. Common value is -1 so
    as not to get confused with expected positive values.

    \return 0 returned on completion

    \param bio WOLFSSL_BIO structure to set end of file value.
    \param v value to set in bio.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_mem());
    ret  = wolfSSL_BIO_set_mem_eof_return(bio, -1);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_set_fp
    \sa wolfSSL_BIO_free
*/
long wolfSSL_BIO_set_mem_eof_return(WOLFSSL_BIO *bio, int v);

/*!
    \ingroup IO

    \brief This is a getter function for WOLFSSL_BIO memory pointer.

    \return SSL_SUCCESS On successfully getting the pointer SSL_SUCCESS is
    returned (currently value of 1).
    \return SSL_FAILURE Returned if NULL arguments are passed in (currently
    value of 0).

    \param bio pointer to the WOLFSSL_BIO structure for getting memory pointer.
    \param ptr structure that is currently a char*. Is set to point to
    bio’s memory.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    WOLFSSL_BUF_MEM* pt;
    // setup bio
    wolfSSL_BIO_get_mem_ptr(bio, &pt);
    //use pt
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
*/
long wolfSSL_BIO_get_mem_ptr(WOLFSSL_BIO *bio, WOLFSSL_BUF_MEM **m);

/*!
    \ingroup CertsKeys

    \brief This function copies the name of the x509 into a buffer.

    \return A char pointer to the buffer with the WOLFSSL_X509_NAME structures
    name member’s data is returned if the function executed normally.

    \param name a pointer to a WOLFSSL_X509 structure.
    \param in a buffer to hold the name copied from the
    WOLFSSL_X509_NAME structure.
    \param sz the maximum size of the buffer.

    _Example_
    \code
    WOLFSSL_X509 x509;
    char* name;
    ...
    name = wolfSSL_X509_NAME_oneline(wolfSSL_X509_get_issuer_name(x509), 0, 0);

    if(name <= 0){
    	// There’s nothing in the buffer.
    }
    \endcode

    \sa wolfSSL_X509_get_subject_name
    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_isCA
    \sa wolfSSL_get_peer_certificate
    \sa wolfSSL_X509_version
*/
char*       wolfSSL_X509_NAME_oneline(WOLFSSL_X509_NAME* name, char* in, int sz);

/*!
    \ingroup CertsKeys

    \brief This function returns the name of the certificate issuer.

    \return point a pointer to the WOLFSSL_X509 struct’s issuer member is
    returned.
    \return NULL if the cert passed in is NULL.

    \param cert a pointer to a WOLFSSL_X509 structure.

    _Example_
    \code
    WOLFSSL_X509* x509;
    WOLFSSL_X509_NAME issuer;
    ...
    issuer = wolfSSL_X509_NAME_oneline(wolfSSL_X509_get_issuer_name(x509), 0, 0);

    if(!issuer){
    	// NULL was returned
    } else {
    	// issuer hods the name of the certificate issuer.
    }
    \endcode

    \sa wolfSSL_X509_get_subject_name
    \sa wolfSSL_X509_get_isCA
    \sa wolfSSL_get_peer_certificate
    \sa wolfSSL_X509_NAME_oneline
*/
WOLFSSL_X509_NAME*  wolfSSL_X509_get_issuer_name(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief This function returns the subject member of the WOLFSSL_X509
    structure.

    \return pointer a pointer to the WOLFSSL_X509_NAME structure. The pointer
    may be NULL if the WOLFSSL_X509 struct is NULL or if the subject member of
    the structure is NULL.

    \param cert a pointer to a WOLFSSL_X509 structure.

    _Example_
    \code
    WOLFSSL_X509* cert;
    WOLFSSL_X509_NAME name;
    …
    name = wolfSSL_X509_get_subject_name(cert);
    if(name == NULL){
	    // Deal with the NULL cacse
    }
    \endcode

    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_isCA
    \sa wolfSSL_get_peer_certificate
*/
WOLFSSL_X509_NAME*  wolfSSL_X509_get_subject_name(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief Checks the isCa member of the WOLFSSL_X509 structure and returns
    the value.

    \return isCA returns the value in the isCA member of the WOLFSSL_X509
    structure is returned.
    \return 0 returned if there is not a valid x509 structure passed in.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl;
    ...
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_X509_get_isCA(ssl)){
    	// This is the CA
    }else {
    	// Failure case
    }
    \endcode

    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_isCA
*/
int  wolfSSL_X509_get_isCA(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief This function gets the text related to the passed in NID value.

    \return int returns the size of the text buffer.

    \param name WOLFSSL_X509_NAME to search for text.
    \param nid NID to search for.
    \param buf buffer to hold text when found.
    \param len length of buffer.

    _Example_
    \code
    WOLFSSL_X509_NAME* name;
    char buffer[100];
    int bufferSz;
    int ret;
    // get WOLFSSL_X509_NAME
    ret = wolfSSL_X509_NAME_get_text_by_NID(name, NID_commonName,
    buffer, bufferSz);

    //check ret value
    \endcode

    \sa none
*/
int wolfSSL_X509_NAME_get_text_by_NID(WOLFSSL_X509_NAME* name, int nid,
                                      char* buf, int len);

/*!
    \ingroup CertsKeys

    \brief This function returns the value stored in the sigOID
    member of the WOLFSSL_X509 structure.

    \return 0 returned if the WOLFSSL_X509 structure is NULL.
    \return int an integer value is returned which was retrieved from
    the x509 object.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_X509 x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
							DYNAMIC_TYPE_X509);
    ...
    int x509SigType = wolfSSL_X509_get_signature_type(x509);

    if(x509SigType != EXPECTED){
	// Deal with an unexpected value
    }
    \endcode

    \sa wolfSSL_X509_get_signature
    \sa wolfSSL_X509_version
    \sa wolfSSL_X509_get_der
    \sa wolfSSL_X509_get_serial_number
    \sa wolfSSL_X509_notBefore
    \sa wolfSSL_X509_notAfter
    \sa wolfSSL_X509_free
*/
int wolfSSL_X509_get_signature_type(WOLFSSL_X509*);

/*!
    \brief This function frees a WOLFSSL_X509 structure.


    \param x509 a pointer to the WOLFSSL_X509 struct.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509*)XMALOC(sizeof(WOLFSSL_X509), NULL,
    DYNAMIC_TYPE_X509) ;

    wolfSSL_X509_free(x509);

    \endcode

    \sa wolfSSL_X509_get_signature
    \sa wolfSSL_X509_version
    \sa wolfSSL_X509_get_der
    \sa wolfSSL_X509_get_serial_number
    \sa wolfSSL_X509_notBefore
    \sa wolfSSL_X509_notAfter

*/
void wolfSSL_X509_free(WOLFSSL_X509* x509);

/*!
    \ingroup CertsKeys

    \brief Gets the X509 signature and stores it in the buffer.

    \return SSL_SUCCESS returned if the function successfully executes.
    The signature is loaded into the buffer.
    \return SSL_FATAL_ERRROR returns if the x509 struct or the bufSz member
    is NULL. There is also a check for the length member of the sig structure
    (sig is a member of x509).

    \param x509 pointer to a WOLFSSL_X509 structure.
    \param buf a char pointer to the buffer.
    \param bufSz an integer pointer to the size of the buffer.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509)XMALOC(sizeof(WOLFSSL_X509), NULL,
    DYNAMIC_TYPE_X509);
    unsigned char* buf; // Initialize
    int* bufSz = sizeof(buf)/sizeof(unsigned char);
    ...
    if(wolfSSL_X509_get_signature(x509, buf, bufSz) != SSL_SUCCESS){
	    // The function did not execute successfully.
    } else{
	    // The buffer was written to correctly.
    }
    \endcode

    \sa wolfSSL_X509_get_serial_number
    \sa wolfSSL_X509_get_signature_type
    \sa wolfSSL_X509_get_device_type
*/
int wolfSSL_X509_get_signature(WOLFSSL_X509* x509, unsigned char* buf, int* bufSz);

/*!
    \ingroup CertsKeys

    \brief This function adds a certificate to the WOLFSSL_X509_STRE structure.

    \return SSL_SUCCESS If certificate is added successfully.
    \return SSL_FATAL_ERROR: If certificate is not added successfully.

    \param str certificate store to add the certificate to.
    \param x509 certificate to add.

    _Example_
    \code
    WOLFSSL_X509_STORE* str;
    WOLFSSL_X509* x509;
    int ret;
    ret = wolfSSL_X509_STORE_add_cert(str, x509);
    //check ret value
    \endcode

    \sa wolfSSL_X509_free
*/
int wolfSSL_X509_STORE_add_cert(WOLFSSL_X509_STORE* store, WOLFSSL_X509* x509);

/*!
    \ingroup CertsKeys

    \brief This function is a getter function for chain variable
    in WOLFSSL_X509_STORE_CTX structure. Currently chain is not populated.

    \return pointer if successful returns WOLFSSL_STACK
    (same as STACK_OF(WOLFSSL_X509)) pointer
    \return Null upon failure

    \param ctx certificate store ctx to get parse chain from.

    _Example_
    \code
    WOLFSSL_STACK* sk;
    WOLFSSL_X509_STORE_CTX* ctx;
    sk = wolfSSL_X509_STORE_CTX_get_chain(ctx);
    //check sk for NULL and then use it. sk needs freed after done.
    \endcode

    \sa wolfSSL_sk_X509_free
*/
WOLFSSL_STACK* wolfSSL_X509_STORE_CTX_get_chain(
                                                   WOLFSSL_X509_STORE_CTX* ctx);

/*!
    \ingroup CertsKeys

    \brief This function takes in a flag to change the behavior of the
    WOLFSSL_X509_STORE structure passed in. An example of a flag used
    is WOLFSSL_CRL_CHECK.

    \return SSL_SUCCESS If no errors were encountered when setting the flag.
    \return <0 a negative value will be returned upon failure.

    \param str certificate store to set flag in.
    \param flag flag for behavior.

    _Example_
    \code
    WOLFSSL_X509_STORE* str;
    int ret;
    // create and set up str
    ret = wolfSSL_X509_STORE_set_flags(str, WOLFSSL_CRL_CHECKALL);
    If (ret != SSL_SUCCESS) {
    	//check ret value and handle error case
    }
    \endcode

    \sa wolfSSL_X509_STORE_new
    \sa wolfSSL_X509_STORE_free
*/
int wolfSSL_X509_STORE_set_flags(WOLFSSL_X509_STORE* store,
                                                            unsigned long flag);

/*!
    \ingroup CertsKeys

    \brief This function the certificate "not before" validity encoded as
    a byte array.


    \return NULL returned if the WOLFSSL_X509 structure is NULL.
    \return byte is returned that contains the notBeforeData.

    \param x509 pointer to a WOLFSSL_X509 structure.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
							DYNAMIC_TYPE_X509);
    ...
    byte* notBeforeData = wolfSSL_X509_notBefore(x509);


    \endcode

    \sa wolfSSL_X509_get_signature
    \sa wolfSSL_X509_version
    \sa wolfSSL_X509_get_der
    \sa wolfSSL_X509_get_serial_number
    \sa wolfSSL_X509_notAfter
    \sa wolfSSL_X509_free
*/
const byte* wolfSSL_X509_notBefore(WOLFSSL_X509* x509);

/*!
    \ingroup CertsKeys

    \brief This function the certificate "not after" validity encoded as
    a byte array.

    \return NULL returned if the WOLFSSL_X509 structure is NULL.
    \return byte is returned that contains the notAfterData.

    \param x509 pointer to a WOLFSSL_X509 structure.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
							DYNAMIC_TYPE_X509);
    ...
    byte* notAfterData = wolfSSL_X509_notAfter(x509);


    \endcode

    \sa wolfSSL_X509_get_signature
    \sa wolfSSL_X509_version
    \sa wolfSSL_X509_get_der
    \sa wolfSSL_X509_get_serial_number
    \sa wolfSSL_X509_notBefore
    \sa wolfSSL_X509_free
*/
const byte* wolfSSL_X509_notAfter(WOLFSSL_X509* x509);

/*!
    \ingroup Setup

    \brief This function is used to copy a WOLFSSL_ASN1_INTEGER
    value to a WOLFSSL_BIGNUM structure.

    \return pointer On successfully copying the WOLFSSL_ASN1_INTEGER
    value a WOLFSSL_BIGNUM pointer is returned.
    \return Null upon failure.

    \param ai WOLFSSL_ASN1_INTEGER structure to copy from.
    \param bn if wanting to copy into an already existing
    WOLFSSL_BIGNUM struct then pass in a pointer to it.
    Optionally this can be NULL and a new WOLFSSL_BIGNUM
    structure will be created.

    _Example_
    \code
    WOLFSSL_ASN1_INTEGER* ai;
    WOLFSSL_BIGNUM* bn;
    // create ai
    bn = wolfSSL_ASN1_INTEGER_to_BN(ai, NULL);

    // or if having already created bn and wanting to reuse structure
    // wolfSSL_ASN1_INTEGER_to_BN(ai, bn);
    // check bn is or return value is not NULL
    \endcode

    \sa none
*/
WOLFSSL_BIGNUM *wolfSSL_ASN1_INTEGER_to_BN(const WOLFSSL_ASN1_INTEGER *ai,
                                       WOLFSSL_BIGNUM *bn);

/*!
    \ingroup Setup

    \brief This function adds the certificate to the internal chain
    being built in the WOLFSSL_CTX structure.

    \return SSL_SUCCESS after successfully adding the certificate.
    \return SSL_FAILURE if failing to add the certificate to the chain.

    \param ctx WOLFSSL_CTX structure to add certificate to.
    \param x509 certificate to add to the chain.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    WOLFSSL_X509* x509;
    int ret;
    // create ctx
    ret = wolfSSL_CTX_add_extra_chain_cert(ctx, x509);
    // check ret value
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
*/
long wolfSSL_CTX_add_extra_chain_cert(WOLFSSL_CTX* ctx, WOLFSSL_X509* x509);

/*!
    \ingroup Setup

    \brief This function returns the get read ahead flag from a
    WOLFSSL_CTX structure.

    \return flag On success returns the read ahead flag.
    \return SSL_FAILURE If ctx is NULL then SSL_FAILURE is returned.

    \param ctx WOLFSSL_CTX structure to get read ahead flag from.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    int flag;
    // setup ctx
    flag = wolfSSL_CTX_get_read_ahead(ctx);
    //check flag
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
    \sa wolfSSL_CTX_set_read_ahead
*/
int  wolfSSL_CTX_get_read_ahead(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function sets the read ahead flag in the WOLFSSL_CTX structure.

    \return SSL_SUCCESS If ctx read ahead flag set.
    \return SSL_FAILURE If ctx is NULL then SSL_FAILURE is returned.

    \param ctx WOLFSSL_CTX structure to set read ahead flag.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    int flag;
    int ret;
    // setup ctx
    ret = wolfSSL_CTX_set_read_ahead(ctx, flag);
    // check return value
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
    \sa wolfSSL_CTX_get_read_ahead
*/
int  wolfSSL_CTX_set_read_ahead(WOLFSSL_CTX* ctx, int v);

/*!
    \ingroup Setup

    \brief This function sets the options argument to use with OCSP.

    \return SSL_FAILURE If ctx or it’s cert manager is NULL.
    \return SSL_SUCCESS If successfully set.

    \param ctx WOLFSSL_CTX structure to set user argument.
    \param arg user argument.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    void* data;
    int ret;
    // setup ctx
    ret = wolfSSL_CTX_set_tlsext_status_arg(ctx, data);

    //check ret value
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
*/
long wolfSSL_CTX_set_tlsext_status_arg(WOLFSSL_CTX* ctx, void* arg);

/*!
    \ingroup Setup

    \brief This function sets the optional argument to be passed to
    the PRF callback.

    \return SSL_FAILURE If ctx is NULL.
    \return SSL_SUCCESS If successfully set.

    \param ctx WOLFSSL_CTX structure to set user argument.
    \param arg user argument.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    void* data;
    int ret;
    // setup ctx
    ret = wolfSSL_CTX_set_tlsext_opaques_prf_input_callback_arg(ctx, data);
    //check ret value
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
*/
long wolfSSL_CTX_set_tlsext_opaque_prf_input_callback_arg(
        WOLFSSL_CTX* ctx, void* arg);

/*!
    \ingroup Setup

    \brief This function sets the options mask in the ssl.
    Some valid options are, SSL_OP_ALL, SSL_OP_COOKIE_EXCHANGE,
    SSL_OP_NO_SSLv2, SSL_OP_NO_SSLv3, SSL_OP_NO_TLSv1,
    SSL_OP_NO_TLSv1_1, SSL_OP_NO_TLSv1_2, SSL_OP_NO_COMPRESSION.

    \return val Returns the updated options mask value stored in ssl.

    \param s WOLFSSL structure to set options mask.
    \param op This function sets the options mask in the ssl.
    Some valid options are:
    SSL_OP_ALL
    SSL_OP_COOKIE_EXCHANGE
    SSL_OP_NO_SSLv2
    SSL_OP_NO_SSLv3
    SSL_OP_NO_TLSv1
    SSL_OP_NO_TLSv1_1
    SSL_OP_NO_TLSv1_2
    SSL_OP_NO_COMPRESSION

    _Example_
    \code
    WOLFSSL* ssl;
    unsigned long mask;
    mask = SSL_OP_NO_TLSv1
    mask  = wolfSSL_set_options(ssl, mask);
    // check mask
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
    \sa wolfSSL_get_options
*/
long wolfSSL_set_options(WOLFSSL *s, long op);

/*!
    \ingroup Setup

    \brief This function returns the current options mask.

    \return val Returns the mask value stored in ssl.

    \param ssl WOLFSSL structure to get options mask from.

    _Example_
    \code
    WOLFSSL* ssl;
    unsigned long mask;
    mask  = wolfSSL_get_options(ssl);
    // check mask
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
    \sa wolfSSL_set_options
*/
long wolfSSL_get_options(const WOLFSSL *s);

/*!
    \ingroup Setup

    \brief This is used to set the debug argument passed around.

    \return SSL_SUCCESS On successful setting argument.
    \return SSL_FAILURE If an NULL ssl passed in.

    \param ssl WOLFSSL structure to set argument in.
    \param arg argument to use.

    _Example_
    \code
    WOLFSSL* ssl;
    void* args;
    int ret;
    // create ssl object
    ret  = wolfSSL_set_tlsext_debug_arg(ssl, args);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
long wolfSSL_set_tlsext_debug_arg(WOLFSSL *s, void *arg);

/*!
    \ingroup openSSL

    \brief This function is called when the client application request
    that a server send back an OCSP status response (also known as
    OCSP stapling).Currently, the only supported type is
    TLSEXT_STATUSTYPE_ocsp.

    \return 1 upon success.
    \return 0 upon error.

    \param s pointer to WolfSSL struct which is created by SSL_new() function
    \param type ssl extension type which TLSEXT_STATUSTYPE_ocsp is
    only supported.

    _Example_
    \code
    WOLFSSL *ssl;
    WOLFSSL_CTX *ctx;
    int ret;
    ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
    ssl = wolfSSL_new(ctx);
    ret = WolfSSL_set_tlsext_status_type(ssl,TLSEXT_STATUSTYPE_ocsp);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_new
    \sa wolfSSL_free
    \sa wolfSSL_CTX_free
*/
long wolfSSL_set_tlsext_status_type(WOLFSSL *s, int type);

/*!
    \ingroup Setup

    \brief This is used to get the results after trying to verify the peer's
    certificate.

    \return X509_V_OK On successful verification.
    \return SSL_FAILURE If an NULL ssl passed in.

    \param ssl WOLFSSL structure to get verification results from.

    _Example_
    \code
    WOLFSSL* ssl;
    long ret;
    // attempt/complete handshake
    ret  = wolfSSL_get_verify_result(ssl);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
long wolfSSL_get_verify_result(const WOLFSSL *ssl);

/*!
    \ingroup Debug

    \brief This function converts an error code returned by
    wolfSSL_get_error() into a more human-readable error string
    and prints that string to the output file - fp.  err is the
    error code returned by wolfSSL_get_error() and fp is the
    file which the error string will be placed in.

    \return none No returns.

    \param fp output file for human-readable error string to be written to.
    \param err error code returned by wolfSSL_get_error().

    _Example_
    \code
    int err = 0;
    WOLFSSL* ssl;
    FILE* fp = ...
    ...
    err = wolfSSL_get_error(ssl, 0);
    wolfSSL_ERR_print_errors_fp(fp, err);
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_ERR_error_string
    \sa wolfSSL_ERR_error_string_n
    \sa wolfSSL_load_error_strings
*/
void  wolfSSL_ERR_print_errors_fp(XFILE fp, int err);

/*!
    \ingroup Debug

    \brief This function uses the provided callback to handle error reporting.
    The callback function is executed for each error line. The string, length,
    and userdata are passed into the callback parameters.

    \return none No returns.

    \param cb the callback function.
    \param u userdata to pass into the callback function.

    _Example_
    \code
    int error_cb(const char *str, size_t len, void *u)
    { fprintf((FILE*)u, "%-*.*s\n", (int)len, (int)len, str); return 0; }
    ...
    FILE* fp = ...
    wolfSSL_ERR_print_errors_cb(error_cb, fp);
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_ERR_error_string
    \sa wolfSSL_ERR_error_string_n
    \sa wolfSSL_load_error_strings
*/
void  wolfSSL_ERR_print_errors_cb (
        int (*cb)(const char *str, size_t len, void *u), void *u);

/*!
    \brief The function sets the client_psk_cb member of the
    WOLFSSL_CTX structure.

    \return none No returns.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param cb wc_psk_client_callback is a function pointer that will be
    stored in the WOLFSSL_CTX structure. Return value is the key length on
    success or zero on error.
    unsigned int (*wc_psk_client_callback)
    PSK client callback parameters:
    WOLFSSL* ssl - Pointer to the wolfSSL structure
    const char* hint - A stored string that could be displayed to provide a
                        hint to the user.
    char* identity - The ID will be stored here.
    unsigned int id_max_len - Size of the ID buffer.
    unsigned char* key - The key will be stored here.
    unsigned int key_max_len - The max size of the key.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol def );
    …
    static WC_INLINE unsigned int my_psk_client_cb(WOLFSSL* ssl, const char* hint,
    char* identity, unsigned int id_max_len, unsigned char* key,
    Unsigned int key_max_len){
    …
    wolfSSL_CTX_set_psk_client_callback(ctx, my_psk_client_cb);
    \endcode

    \sa wolfSSL_set_psk_client_callback
    \sa wolfSSL_set_psk_server_callback
    \sa wolfSSL_CTX_set_psk_server_callback
    \sa wolfSSL_CTX_set_psk_client_callback
*/
void wolfSSL_CTX_set_psk_client_callback(WOLFSSL_CTX* ctx,
                                                    wc_psk_client_callback);

/*!
    \brief Sets the PSK client side callback.

    \return none No returns.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a function pointer to type wc_psk_client_callback. Return value
    is the key length on success or zero on error.
    unsigned int (*wc_psk_client_callback)
    PSK client callback parameters:
    WOLFSSL* ssl - Pointer to the wolfSSL structure
    const char* hint - A stored string that could be displayed to provide a
                        hint to the user.
    char* identity - The ID will be stored here.
    unsigned int id_max_len - Size of the ID buffer.
    unsigned char* key - The key will be stored here.
    unsigned int key_max_len - The max size of the key.

    _Example_
    \code
    WOLFSSL* ssl;
    static WC_INLINE unsigned int my_psk_client_cb(WOLFSSL* ssl, const char* hint,
    char* identity, unsigned int id_max_len, unsigned char* key,
    Unsigned int key_max_len){
    …
    if(ssl){
    wolfSSL_set_psk_client_callback(ssl, my_psk_client_cb);
    } else {
    	// could not set callback
    }
    \endcode

    \sa wolfSSL_CTX_set_psk_client_callback
    \sa wolfSSL_CTX_set_psk_server_callback
    \sa wolfSSL_set_psk_server_callback
*/
void wolfSSL_set_psk_client_callback(WOLFSSL* ssl,
                                                    wc_psk_client_callback);

/*!
    \ingroup CertsKeys

    \brief This function returns the psk identity hint.

    \return pointer a const char pointer to the value that was stored in
    the arrays member of the WOLFSSL structure is returned.
    \return NULL returned if the WOLFSSL or Arrays structures are NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    char* idHint;
    ...
    idHint = wolfSSL_get_psk_identity_hint(ssl);
    if(idHint){
    	// The hint was retrieved
    	return idHint;
    } else {
    	// Hint wasn’t successfully retrieved
    }
    \endcode

    \sa wolfSSL_get_psk_identity
*/
const char* wolfSSL_get_psk_identity_hint(const WOLFSSL*);

/*!
    \ingroup CertsKeys

    \brief The function returns a constant pointer to the client_identity
    member of the Arrays structure.

    \return string the string value of the client_identity member of the
    Arrays structure.
    \return NULL if the WOLFSSL structure is NULL or if the Arrays member of
    the WOLFSSL structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    const char* pskID;
    ...
    pskID = wolfSSL_get_psk_identity(ssl);

    if(pskID == NULL){
	    // There is not a value in pskID
    }
    \endcode

    \sa wolfSSL_get_psk_identity_hint
    \sa wolfSSL_use_psk_identity_hint
*/
const char* wolfSSL_get_psk_identity(const WOLFSSL*);

/*!
    \ingroup CertsKeys

    \brief This function stores the hint argument in the server_hint
    member of the WOLFSSL_CTX structure.

    \return SSL_SUCCESS returned for successful execution of the function.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param hint a constant char pointer that will be copied to the
    WOLFSSL_CTX structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    const char* hint;
    int ret;
    …
    ret = wolfSSL_CTX_use_psk_identity_hint(ctx, hint);
    if(ret == SSL_SUCCESS){
    	// Function was successful.
	return ret;
    } else {
    	// Failure case.
    }
    \endcode

    \sa wolfSSL_use_psk_identity_hint
*/
int wolfSSL_CTX_use_psk_identity_hint(WOLFSSL_CTX* ctx, const char* hint);

/*!
    \ingroup CertsKeys

    \brief This function stores the hint argument in the server_hint member
    of the Arrays structure within the WOLFSSL structure.

    \return SSL_SUCCESS returned if the hint was successfully stored in the
    WOLFSSL structure.
    \return SSL_FAILURE returned if the WOLFSSL or Arrays structures are NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param hint a constant character pointer that holds the hint to be saved
    in memory.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    const char* hint;
    ...
    if(wolfSSL_use_psk_identity_hint(ssl, hint) != SSL_SUCCESS){
    	// Handle failure case.
    }
    \endcode

    \sa wolfSSL_CTX_use_psk_identity_hint
*/
int wolfSSL_use_psk_identity_hint(WOLFSSL* ssl, const char* hint);

/*!
    \brief This function sets the psk callback for the server side in
    the WOLFSSL_CTX structure.

    \return none No returns.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a function pointer for the callback and will be stored in
    the WOLFSSL_CTX structure. Return value is the key length on success or
    zero on error.
    unsigned int (*wc_psk_server_callback)
    PSK server callback parameters
    WOLFSSL* ssl - Pointer to the wolfSSL structure
    char* identity - The ID will be stored here.
    unsigned char* key - The key will be stored here.
    unsigned int key_max_len - The max size of the key.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    static unsigned int my_psk_server_cb(WOLFSSL* ssl, const char* identity,
                           unsigned char* key, unsigned int key_max_len)
    {
        // Function body.
    }
    …
    if(ctx != NULL){
        wolfSSL_CTX_set_psk_server_callback(ctx, my_psk_server_cb);
    } else {
    	// The CTX object was not properly initialized.
    }
    \endcode

    \sa wc_psk_server_callback
    \sa wolfSSL_set_psk_client_callback
    \sa wolfSSL_set_psk_server_callback
    \sa wolfSSL_CTX_set_psk_client_callback
*/
void wolfSSL_CTX_set_psk_server_callback(WOLFSSL_CTX* ctx,
                                                    wc_psk_server_callback cb);

/*!
    \brief Sets the psk callback for the server side by setting the
    WOLFSSL structure options members.

    \return none No returns.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a function pointer for the callback and will be stored in
    the WOLFSSL structure. Return value is the key length on success or  zero
    on error.
    unsigned int (*wc_psk_server_callback)
    PSK server callback parameters
    WOLFSSL* ssl - Pointer to the wolfSSL structure
    char* identity - The ID will be stored here.
    unsigned char* key - The key will be stored here.
    unsigned int key_max_len - The max size of the key.


    _Example_
    \code
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    …
    static unsigned int my_psk_server_cb(WOLFSSL* ssl, const char* identity,
                           unsigned char* key, unsigned int key_max_len)
    {
        // Function body.
    }
    …
    if(ssl != NULL && cb != NULL){
        wolfSSL_set_psk_server_callback(ssl, my_psk_server_cb);
    }
    \endcode

    \sa wolfSSL_set_psk_client_callback
    \sa wolfSSL_CTX_set_psk_server_callback
    \sa wolfSSL_CTX_set_psk_client_callback
    \sa wolfSSL_get_psk_identity_hint
    \sa wc_psk_server_callback
    \sa InitSuites
*/
void wolfSSL_set_psk_server_callback(WOLFSSL* ssl,
                                                    wc_psk_server_callback cb);


/*!
    \brief Sets a PSK user context in the WOLFSSL structure options member.

    \return WOLFSSL_SUCCESS or WOLFSSL_FAILURE

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param psk_ctx void pointer to user PSK context

    \sa wolfSSL_get_psk_callback_ctx
    \sa wolfSSL_CTX_set_psk_callback_ctx
    \sa wolfSSL_CTX_get_psk_callback_ctx
*/
int wolfSSL_set_psk_callback_ctx(WOLFSSL* ssl, void* psk_ctx);

/*!
    \brief Sets a PSK user context in the WOLFSSL_CTX structure.

    \return WOLFSSL_SUCCESS or WOLFSSL_FAILURE

    \param ctx a pointer to a WOLFSSL_CTX structure, created using wolfSSL_CTX_new().
    \param psk_ctx void pointer to user PSK context

    \sa wolfSSL_set_psk_callback_ctx
    \sa wolfSSL_get_psk_callback_ctx
    \sa wolfSSL_CTX_get_psk_callback_ctx
*/
int wolfSSL_CTX_set_psk_callback_ctx(WOLFSSL_CTX* ctx, void* psk_ctx);

/*!
    \brief Get a PSK user context in the WOLFSSL structure options member.

    \return void pointer to user PSK context

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_set_psk_callback_ctx
    \sa wolfSSL_CTX_set_psk_callback_ctx
    \sa wolfSSL_CTX_get_psk_callback_ctx
*/
void* wolfSSL_get_psk_callback_ctx(WOLFSSL* ssl);

/*!
    \brief Get a PSK user context in the WOLFSSL_CTX structure.

    \return void pointer to user PSK context

    \param ctx a pointer to a WOLFSSL_CTX structure, created using wolfSSL_CTX_new().

    \sa wolfSSL_CTX_set_psk_callback_ctx
    \sa wolfSSL_set_psk_callback_ctx
    \sa wolfSSL_get_psk_callback_ctx
*/
void* wolfSSL_CTX_get_psk_callback_ctx(WOLFSSL_CTX* ctx);

/*!
    \ingroup Setup

    \brief This function enables the havAnon member of the CTX structure if
    HAVE_ANON is defined during compilation.

    \return SSL_SUCCESS returned if the function executed successfully and the
    haveAnnon member of the CTX is set to 1.
    \return SSL_FAILURE returned if the CTX structure was NULL.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    #ifdef HAVE_ANON
	if(cipherList == NULL){
	    wolfSSL_CTX_allow_anon_cipher(ctx);
	    if(wolfSSL_CTX_set_cipher_list(ctx, “ADH_AES128_SHA”) != SSL_SUCCESS){
		    // failure case
	    }
    }
    #endif
    \endcode

    \sa none
*/
int wolfSSL_CTX_allow_anon_cipher(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief The wolfSSLv23_server_method() function is used to indicate
    that the application is a server and will support clients connecting
    with protocol version from SSL 3.0 - TLS 1.3.  This function allocates
    memory for and initializes a new WOLFSSL_METHOD structure to be used when
    creating the SSL/TLS context with wolfSSL_CTX_new().

    \return pointer If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return Failure If memory allocation fails when calling XMALLOC, the
    failure value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    \param none No parameters

    _Example_
    \code
    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfSSLv23_server_method();
    if (method == NULL) {
    	// unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfDTLSv1_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfSSLv23_server_method(void);

/*!
    \ingroup Setup

    \brief This is used to get the internal error state of the WOLFSSL structure.

    \return wolfssl_error returns ssl error state, usually a negative
    \return BAD_FUNC_ARG if ssl is NULL.

    \return ssl WOLFSSL structure to get state from.

    _Example_
    \code
    WOLFSSL* ssl;
    int ret;
    // create ssl object
    ret  = wolfSSL_state(ssl);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int  wolfSSL_state(WOLFSSL* ssl);

/*!
    \ingroup CertsKeys

    \brief This function gets the peer’s certificate.

    \return pointer a pointer to the peerCert member of the WOLFSSL_X509
    structure if it exists.
    \return 0 returned if the peer certificate issuer size is not defined.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    WOLFSSL_X509* peerCert = wolfSSL_get_peer_certificate(ssl);

    if(peerCert){
    	// You have a pointer peerCert to the peer certification
    }
    \endcode

    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_subject_name
    \sa wolfSSL_X509_get_isCA
*/
WOLFSSL_X509* wolfSSL_get_peer_certificate(WOLFSSL* ssl);

/*!
    \ingroup Debug

    \brief This function is similar to calling wolfSSL_get_error() and
    getting SSL_ERROR_WANT_READ in return.  If the underlying error state
    is SSL_ERROR_WANT_READ, this function will return 1, otherwise, 0.

    \return 1 wolfSSL_get_error() would return SSL_ERROR_WANT_READ, the
    underlying I/O has data available for reading.
    \return 0 There is no SSL_ERROR_WANT_READ error state.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int ret;
    WOLFSSL* ssl = 0;
    ...

    ret = wolfSSL_want_read(ssl);
    if (ret == 1) {
    	// underlying I/O has data available for reading (SSL_ERROR_WANT_READ)
    }
    \endcode

    \sa wolfSSL_want_write
    \sa wolfSSL_get_error
*/
int wolfSSL_want_read(WOLFSSL*);

/*!
    \ingroup Debug

    \brief This function is similar to calling wolfSSL_get_error() and getting
    SSL_ERROR_WANT_WRITE in return. If the underlying error state is
    SSL_ERROR_WANT_WRITE, this function will return 1, otherwise, 0.

    \return 1 wolfSSL_get_error() would return SSL_ERROR_WANT_WRITE, the
    underlying I/O needs data to be written in order for progress to be
    made in the underlying SSL connection.
    \return 0 There is no SSL_ERROR_WANT_WRITE error state.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int ret;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_want_write(ssl);
    if (ret == 1) {
    	// underlying I/O needs data to be written (SSL_ERROR_WANT_WRITE)
    }
    \endcode

    \sa wolfSSL_want_read
    \sa wolfSSL_get_error
*/
int wolfSSL_want_write(WOLFSSL*);

/*!
    \ingroup Setup

    \brief wolfSSL by default checks the peer certificate for a valid date
    range and a verified signature.  Calling this function before
    wolfSSL_connect() or wolfSSL_accept() will add a domain name check to
    the list of checks to perform.  dn holds the domain name to check
    against the peer certificate when it’s received.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE will be returned if a memory error was encountered.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param dn domain name to check against the peer certificate when received.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    char* domain = (char*) “www.yassl.com”;
    ...

    ret = wolfSSL_check_domain_name(ssl, domain);
    if (ret != SSL_SUCCESS) {
       // failed to enable domain name check
    }
    \endcode

    \sa none
*/
int wolfSSL_check_domain_name(WOLFSSL* ssl, const char* dn);

/*!
    \ingroup TLS

    \brief Initializes the wolfSSL library for use.  Must be called once per
    application and before any other call to the library.

    \return SSL_SUCCESS If successful the call will return.
    \return BAD_MUTEX_E is an error that may be returned.
    \return WC_INIT_E wolfCrypt initialization error returned.

    _Example_
    \code
    int ret = 0;
    ret = wolfSSL_Init();
    if (ret != SSL_SUCCESS) {
	    failed to initialize wolfSSL library
    }

    \endcode

    \sa wolfSSL_Cleanup
*/
int wolfSSL_Init(void);

/*!
    \ingroup TLS

    \brief Un-initializes the wolfSSL library from further use. Doesn’t have
    to be called, though it will free any resources used by the library.

    \return SSL_SUCCESS return no errors.
    \return BAD_MUTEX_E a mutex error return.]

    _Example_
    \code
    wolfSSL_Cleanup();
    \endcode

    \sa wolfSSL_Init
*/
int wolfSSL_Cleanup(void);

/*!
    \ingroup IO

    \brief This function returns the current library version.

    \return LIBWOLFSSL_VERSION_STRING a const char pointer defining the
    version.

    \param none No parameters.

    _Example_
    \code
    char version[MAXSIZE];
    version = wolfSSL_KeepArrays();
    …
    if(version != ExpectedVersion){
	    // Handle the mismatch case
    }
    \endcode

    \sa word32_wolfSSL_lib_version_hex
*/
const char* wolfSSL_lib_version(void);

/*!
    \ingroup IO

    \brief This function returns the current library version in hexadecimal
    notation.

    \return LILBWOLFSSL_VERSION_HEX returns the hexadecimal version defined in
     wolfssl/version.h.

    \param none No parameters.

    _Example_
    \code
    word32 libV;
    libV = wolfSSL_lib_version_hex();

    if(libV != EXPECTED_HEX){
	    // How to handle an unexpected value
    } else {
	    // The expected result for libV
    }
    \endcode

    \sa wolfSSL_lib_version
*/
word32 wolfSSL_lib_version_hex(void);

/*!
    \ingroup IO

    \brief Performs the actual connect or accept based on the side of the SSL
    method.  If called from the client side then an wolfSSL_connect() is done
    while a wolfSSL_accept() is performed if called from the server side.

    \return SSL_SUCCESS will be returned if successful. (Note, older versions
    will return 0.)
    \return SSL_FATAL_ERROR will be returned if the underlying call resulted
    in an error. Use wolfSSL_get_error() to get a specific error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int ret = SSL_FATAL_ERROR;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_negotiate(ssl);
    if (ret == SSL_FATAL_ERROR) {
    	// SSL establishment failed
	int error_code = wolfSSL_get_error(ssl);
	...
    }
    ...
    \endcode

    \sa SSL_connect
    \sa SSL_accept
*/
int wolfSSL_negotiate(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief Turns on the ability to use compression for the SSL connection.
    Both sides must have compression turned on otherwise compression will not be
    used. The zlib library performs the actual data compression. To compile
    into the library use --with-libz for the configure system and define
    HAVE_LIBZ otherwise. Keep in mind that while compressing data before
    sending decreases the actual size of the messages being sent and received,
    the amount of data saved by compression usually takes longer in time to
    analyze than it does to send it raw on all but the slowest of networks.

    \return SSL_SUCCESS upon success.
    \return NOT_COMPILED_IN will be returned if compression support wasn’t
    built into the library.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_set_compression(ssl);
    if (ret == SSL_SUCCESS) {
    	// successfully enabled compression for SSL session
    }
    \endcode

    \sa none
*/
int wolfSSL_set_compression(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets the SSL session timeout value in seconds.

    \return SSL_SUCCESS will be returned upon successfully setting the session.
    \return BAD_FUNC_ARG will be returned if ssl is NULL.

    \param ssl pointer to the SSL object, created with wolfSSL_new().
    \param to value, in seconds, used to set the SSL session timeout.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl = 0;
    ...

    ret = wolfSSL_set_timeout(ssl, 500);
    if (ret != SSL_SUCCESS) {
    	// failed to set session timeout value
    }
    ...
    \endcode

    \sa wolfSSL_get1_session
    \sa wolfSSL_set_session
*/
int wolfSSL_set_timeout(WOLFSSL* ssl, unsigned int to);

/*!
    \ingroup Setup

    \brief This function sets the timeout value for SSL sessions, in seconds,
    for the specified SSL context.

    \return the previous timeout value, if WOLFSSL_ERROR_CODE_OPENSSL is 
    \return defined on success. If not defined, SSL_SUCCESS will be returned.
    \return BAD_FUNC_ARG will be returned when the input context (ctx) is null.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param to session timeout value in seconds.

    _Example_
    \code
    WOLFSSL_CTX*    ctx    = 0;
    ...
    ret = wolfSSL_CTX_set_timeout(ctx, 500);
    if (ret != SSL_SUCCESS) {
	    // failed to set session timeout value
    }
    \endcode

    \sa wolfSSL_flush_sessions
    \sa wolfSSL_get1_session
    \sa wolfSSL_set_session
    \sa wolfSSL_get_sessionID
    \sa wolfSSL_CTX_set_session_cache_mode
*/
int wolfSSL_CTX_set_timeout(WOLFSSL_CTX* ctx, unsigned int to);

/*!
    \ingroup openSSL

    \brief Retrieves the peer’s certificate chain.

    \return chain If successful the call will return the peer’s
    certificate chain.
    \return 0 will be returned if an invalid WOLFSSL pointer is passed to the
    function.

    \param ssl pointer to a valid WOLFSSL structure.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_get_chain_count
    \sa wolfSSL_get_chain_length
    \sa wolfSSL_get_chain_cert
    \sa wolfSSL_get_chain_cert_pem
*/
WOLFSSL_X509_CHAIN* wolfSSL_get_peer_chain(WOLFSSL* ssl);

/*!
    \ingroup openSSL

    \brief Retrieve's the peers certificate chain count.

    \return Success If successful the call will return the peer’s certificate
    chain count.
    \return 0 will be returned if an invalid chain pointer is passed to
    the function.

    \param chain pointer to a valid WOLFSSL_X509_CHAIN structure.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_get_peer_chain
    \sa wolfSSL_get_chain_length
    \sa wolfSSL_get_chain_cert
    \sa wolfSSL_get_chain_cert_pem
*/
int  wolfSSL_get_chain_count(WOLFSSL_X509_CHAIN* chain);

/*!
    \ingroup openSSL

    \brief Retrieves the peer’s ASN1.DER certificate length in bytes
    at index (idx).

    \return Success If successful the call will return the peer’s
    certificate length in bytes by index.
    \return 0 will be returned if an invalid chain pointer is passed
    to the function.

    \param chain pointer to a valid WOLFSSL_X509_CHAIN structure.
    \param idx index to start of chain.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_get_peer_chain
    \sa wolfSSL_get_chain_count
    \sa wolfSSL_get_chain_cert
    \sa wolfSSL_get_chain_cert_pem
*/
int  wolfSSL_get_chain_length(WOLFSSL_X509_CHAIN* chain, int idx);

/*!
    \ingroup openSSL

    \brief Retrieves the peer’s ASN1.DER certificate at index (idx).

    \return Success If successful the call will return the peer’s
    certificate by index.
    \return 0 will be returned if an invalid chain pointer is passed
    to the function.

    \param chain pointer to a valid WOLFSSL_X509_CHAIN structure.
    \param idx index to start of chain.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_get_peer_chain
    \sa wolfSSL_get_chain_count
    \sa wolfSSL_get_chain_length
    \sa wolfSSL_get_chain_cert_pem
*/
unsigned char* wolfSSL_get_chain_cert(WOLFSSL_X509_CHAIN* chain, int idx);

/*!
    \ingroup CertsKeys

    \brief This function gets the peer’s wolfSSL_X509_certificate at
    index (idx) from the chain of certificates.

    \return pointer returns a pointer to a WOLFSSL_X509 structure.

    \param chain a pointer to the WOLFSSL_X509_CHAIN used for no dynamic
    memory SESSION_CACHE.
    \param idx the index of the WOLFSSL_X509 certificate.

    Note that it is the user's responsibility to free the returned memory
    by calling wolfSSL_FreeX509().

    _Example_
    \code
    WOLFSSL_X509_CHAIN* chain = &session->chain;
    int idx = 999; // set idx
    ...
    WOLFSSL_X509_CHAIN ptr;
    prt = wolfSSL_get_chain_X509(chain, idx);

    if(ptr != NULL){
        // ptr contains the cert at the index specified
        wolfSSL_FreeX509(ptr);
    } else {
        // ptr is NULL
    }
    \endcode

    \sa InitDecodedCert
    \sa ParseCertRelative
    \sa CopyDecodedToX509
*/
WOLFSSL_X509* wolfSSL_get_chain_X509(WOLFSSL_X509_CHAIN* chain, int idx);

/*!
    \ingroup openSSL

    \brief Retrieves the peer’s PEM certificate at index (idx).

    \return Success If successful the call will return the peer’s
    certificate by index.
    \return 0 will be returned if an invalid chain pointer is passed to
    the function.

    \param chain pointer to a valid WOLFSSL_X509_CHAIN structure.
    \param idx indexto start of chain.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_get_peer_chain
    \sa wolfSSL_get_chain_count
    \sa wolfSSL_get_chain_length
    \sa wolfSSL_get_chain_cert
*/
int  wolfSSL_get_chain_cert_pem(WOLFSSL_X509_CHAIN* chain, int idx,
                                unsigned char* buf, int inLen, int* outLen);

/*!
    \ingroup openSSL

    \brief Retrieves the session’s ID.  The session ID is always 32 bytes long.

    \return id The session ID.

    \param session pointer to a valid wolfssl session.

    _Example_
    \code
    none
    \endcode

    \sa SSL_get_session
*/
const unsigned char* wolfSSL_get_sessionID(const WOLFSSL_SESSION* s);

/*!
    \ingroup openSSL

    \brief Retrieves the peer’s certificate serial number. The serial
    number buffer (in) should be at least 32 bytes long and be provided
    as the *inOutSz argument as input. After calling the function *inOutSz
    will hold the actual length in bytes written to the in buffer.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG will be returned if a bad function argument
    was encountered.

    \param in The serial number buffer and should be at least 32 bytes long
    \param inOutSz will hold the actual length in bytes written to the
    in buffer.

    _Example_
    \code
    none
    \endcode

    \sa SSL_get_peer_certificate
*/
int  wolfSSL_X509_get_serial_number(WOLFSSL_X509* x509, unsigned char* in,
                                    int* inOutSz);

/*!
    \ingroup CertsKeys

    \brief Returns the common name of the subject from the certificate.

    \return NULL returned if the x509 structure is null
    \return string a string representation of the subject's common
    name is returned upon success

    \param x509 a pointer to a WOLFSSL_X509 structure containing
    certificate information.

    _Example_
    \code
    WOLFSSL_X509 x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
							DYNAMIC_TYPE_X509);
    ...
    int x509Cn = wolfSSL_X509_get_subjectCN(x509);
    if(x509Cn == NULL){
	    // Deal with NULL case
    } else {
	    // x509Cn contains the common name
    }
    \endcode

    \sa wolfSSL_X509_Name_get_entry
    \sa wolfSSL_X509_get_next_altname
    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_subject_name

*/
char*  wolfSSL_X509_get_subjectCN(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief This function gets the DER encoded certificate in the
    WOLFSSL_X509 struct.

    \return buffer This function returns the DerBuffer structure’s
    buffer member, which is of type byte.
    \return NULL returned if the x509 or outSz parameter is NULL.

    \param x509 a pointer to a WOLFSSL_X509 structure containing
    certificate information.
    \param outSz length of the derBuffer member of the WOLFSSL_X509 struct.

    _Example_
    \code
    WOLFSSL_X509 x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
							DYNAMIC_TYPE_X509);
    int* outSz; // initialize
    ...
    byte* x509Der = wolfSSL_X509_get_der(x509, outSz);
    if(x509Der == NULL){
	    // Failure case one of the parameters was NULL
    }
    \endcode

    \sa wolfSSL_X509_version
    \sa wolfSSL_X509_Name_get_entry
    \sa wolfSSL_X509_get_next_altname
    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_subject_name
*/
const unsigned char* wolfSSL_X509_get_der(WOLFSSL_X509* x509, int* outSz);

/*!
    \ingroup CertsKeys

    \brief This function checks to see if x509 is NULL and if it’s not,
    it returns the notAfter member of the x509 struct.

    \return pointer to struct with ASN1_TIME to the notAfter
    member of the x509 struct.
    \return NULL returned if the x509 object is NULL.

    \param x509 a pointer to the WOLFSSL_X509 struct.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509)XMALOC(sizeof(WOLFSSL_X509), NULL,
    DYNAMIC_TYPE_X509) ;
    ...
    const WOLFSSL_ASN1_TIME* notAfter = wolfSSL_X509_get_notAfter(x509);
    if(notAfter == NULL){
        // Failure case, the x509 object is null.
    }
    \endcode

    \sa wolfSSL_X509_get_notBefore
*/
WOLFSSL_ASN1_TIME* wolfSSL_X509_get_notAfter(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief This function retrieves the version of the X509 certificate.

    \return 0 returned if the x509 structure is NULL.
    \return version the version stored in the x509 structure will be returned.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_X509* x509;
    int version;
    ...
    version = wolfSSL_X509_version(x509);
    if(!version){
	    // The function returned 0, failure case.
    }
    \endcode

    \sa wolfSSL_X509_get_subject_name
    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_isCA
    \sa wolfSSL_get_peer_certificate
*/
int wolfSSL_X509_version(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief If NO_STDIO_FILESYSTEM is defined this function will allocate
    heap memory, initialize a WOLFSSL_X509 structure and return a pointer to it.

    \return *WOLFSSL_X509 WOLFSSL_X509 structure pointer is returned if
    the function executes successfully.
    \return NULL if the call to XFTELL macro returns a negative value.

    \param x509 a pointer to a WOLFSSL_X509 pointer.
    \param file a defined type that is a pointer to a FILE.

    _Example_
    \code
    WOLFSSL_X509* x509a = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
    DYNAMIC_TYPE_X509);
    WOLFSSL_X509** x509 = x509a;
    XFILE file;  (mapped to struct fs_file*)
    ...
    WOLFSSL_X509* newX509 = wolfSSL_X509_d2i_fp(x509, file);
    if(newX509 == NULL){
	    // The function returned NULL
    }
    \endcode

    \sa wolfSSL_X509_d2i
    \sa XFTELL
    \sa XREWIND
    \sa XFSEEK
*/
WOLFSSL_X509*
        wolfSSL_X509_d2i_fp(WOLFSSL_X509** x509, FILE* file);

/*!
    \ingroup CertsKeys

    \brief The function loads the x509 certificate into memory.

    \return pointer a successful execution returns pointer to a
    WOLFSSL_X509 structure.
    \return NULL returned if the certificate was not able to be written.

    \param fname the certificate file to be loaded.
    \param format the format of the certificate.

    _Example_
    \code
    #define cliCert    “certs/client-cert.pem”
    …
    X509* x509;
    …
    x509 = wolfSSL_X509_load_certificate_file(cliCert, SSL_FILETYPE_PEM);
    AssertNotNull(x509);
    \endcode

    \sa InitDecodedCert
    \sa PemToDer
    \sa wolfSSL_get_certificate
    \sa AssertNotNull
*/
WOLFSSL_X509*
    wolfSSL_X509_load_certificate_file(const char* fname, int format);

/*!
    \ingroup CertsKeys

    \brief This function copies the device type from the x509 structure
    to the buffer.

    \return pointer returns a byte pointer holding the device type from
    the x509 structure.
    \return NULL returned if the buffer size is NULL.

    \param x509 pointer to a WOLFSSL_X509 structure, created with
    WOLFSSL_X509_new().
    \param in a pointer to a byte type that will hold the device type
    (the buffer).
    \param inOutSz the minimum of either the parameter inOutSz or the
    deviceTypeSz member of the x509 structure.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509)XMALOC(sizeof(WOLFSSL_X509), NULL,
    DYNAMIC_TYPE_X509);
    byte* in;
    int* inOutSz;
    ...
    byte* deviceType = wolfSSL_X509_get_device_type(x509, in, inOutSz);

    if(!deviceType){
	    // Failure case, NULL was returned.
    }
    \endcode

    \sa wolfSSL_X509_get_hw_type
    \sa wolfSSL_X509_get_hw_serial_number
    \sa wolfSSL_X509_d2i
*/
unsigned char*
           wolfSSL_X509_get_device_type(WOLFSSL_X509* x509, unsigned char* in,
                                        int* inOutSz);

/*!
    \ingroup CertsKeys

    \brief The function copies the hwType member of the WOLFSSL_X509
    structure to the buffer.

    \return byte The function returns a byte type of the data previously held
    in the hwType member of the WOLFSSL_X509 structure.
    \return NULL returned if  inOutSz is NULL.

    \param x509 a pointer to a WOLFSSL_X509 structure containing certificate
    information.
    \param in pointer to type byte that represents the buffer.
    \param inOutSz pointer to type int that represents the size of the buffer.

    _Example_
    \code
    WOLFSSL_X509* x509;  // X509 certificate
    byte* in;  // initialize the buffer
    int* inOutSz;  // holds the size of the buffer
    ...
    byte* hwType = wolfSSL_X509_get_hw_type(x509, in, inOutSz);

    if(hwType == NULL){
	    // Failure case function returned NULL.
    }
    \endcode

    \sa wolfSSL_X509_get_hw_serial_number
    \sa wolfSSL_X509_get_device_type
*/
unsigned char*
           wolfSSL_X509_get_hw_type(WOLFSSL_X509* x509, unsigned char* in,
                                    int* inOutSz);

/*!
    \ingroup CertsKeys

    \brief This function returns the hwSerialNum member of the x509 object.

    \return pointer the function returns a byte pointer to the in buffer that
    will contain the serial number loaded from the x509 object.

    \param x509 pointer to a WOLFSSL_X509 structure containing certificate
    information.
    \param in a pointer to the buffer that will be copied to.
    \param inOutSz a pointer to the size of the buffer.

    _Example_
    \code
    char* serial;
    byte* in;
    int* inOutSz;
    WOLFSSL_X509 x509;
    ...
    serial = wolfSSL_X509_get_hw_serial_number(x509, in, inOutSz);

    if(serial == NULL || serial <= 0){
    	// Failure case
    }
    \endcode

    \sa wolfSSL_X509_get_subject_name
    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_isCA
    \sa wolfSSL_get_peer_certificate
    \sa wolfSSL_X509_version
*/
unsigned char*
           wolfSSL_X509_get_hw_serial_number(WOLFSSL_X509* x509,
                                             unsigned char* in, int* inOutSz);

/*!
    \ingroup IO

    \brief This function is called on the client side and initiates an
    SSL/TLS handshake with a server only long enough to get the peer’s
    certificate chain.  When this function is called, the underlying
    communication channel has already been set up. wolfSSL_connect_cert()
    works with both blocking and non-blocking I/O.  When the underlying I/O
    is non-blocking, wolfSSL_connect_cert() will return when the underlying
    I/O could not satisfy the needs of wolfSSL_connect_cert() to continue the
    handshake.  In this case, a call to wolfSSL_get_error() will yield either
    SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE.  The calling process must then
    repeat the call to wolfSSL_connect_cert() when the underlying I/O is ready
    and wolfSSL will pick up where it left off. When using a non-blocking
    socket, nothing needs to be done, but select() can be used to check for
    the required condition. If the underlying I/O is blocking,
    wolfSSL_connect_cert() will only return once the peer’s certificate chain
    has been received.

    \return SSL_SUCCESS upon success.
    \return SSL_FAILURE will be returned if the SSL session parameter is NULL.
    \return SSL_FATAL_ERROR will be returned if an error occurred. To get a more
    detailed error code, call wolfSSL_get_error().

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...
    ret = wolfSSL_connect_cert(ssl);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_connect
    \sa wolfSSL_accept
*/
int  wolfSSL_connect_cert(WOLFSSL* ssl);

/*!
    \ingroup openSSL

    \brief wolfSSL_d2i_PKCS12_bio (d2i_PKCS12_bio) copies in the PKCS12
    information from WOLFSSL_BIO to the structure WC_PKCS12. The information
    is divided up in the structure as a list of Content Infos along with a
    structure to hold optional MAC information. After the information has been
    divided into chunks (but not decrypted) in the structure WC_PKCS12, it can
    then be parsed and decrypted by calling.

    \return WC_PKCS12 pointer to a WC_PKCS12 structure.
    \return Failure If function failed it will return NULL.

    \param bio WOLFSSL_BIO structure to read PKCS12 buffer from.
    \param pkcs12 WC_PKCS12 structure pointer for new PKCS12 structure created.
    Can be NULL

    _Example_
    \code
    WC_PKCS12* pkcs;
    WOLFSSL_BIO* bio;
    WOLFSSL_X509* cert;
    WOLFSSL_EVP_PKEY* pkey;
    STACK_OF(X509) certs;
    //bio loads in PKCS12 file
    wolfSSL_d2i_PKCS12_bio(bio, &pkcs);
    wolfSSL_PKCS12_parse(pkcs, “a password”, &pkey, &cert, &certs)
    wc_PKCS12_free(pkcs)
    //use cert, pkey, and optionally certs stack
    \endcode

    \sa wolfSSL_PKCS12_parse
    \sa wc_PKCS12_free
*/
WC_PKCS12* wolfSSL_d2i_PKCS12_bio(WOLFSSL_BIO* bio,
                                       WC_PKCS12** pkcs12);

/*!
    \ingroup openSSL

    \brief wolfSSL_i2d_PKCS12_bio (i2d_PKCS12_bio) copies in the cert
    information from the structure WC_PKCS12 to WOLFSSL_BIO.

    \return 1 for success.
    \return Failure 0.

    \param bio WOLFSSL_BIO structure to write PKCS12 buffer to.
    \param pkcs12 WC_PKCS12 structure for PKCS12 structure as input.

    _Example_
    \code
    WC_PKCS12 pkcs12;
    FILE *f;
    byte buffer[5300];
    char file[] = "./test.p12";
    int bytes;
    WOLFSSL_BIO* bio;
    pkcs12 = wc_PKCS12_new();
    f = fopen(file, "rb");
    bytes = (int)fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    //convert the DER file into an internal structure
    wc_d2i_PKCS12(buffer, bytes, pkcs12);
    bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem());
    //convert PKCS12 structure into bio
    wolfSSL_i2d_PKCS12_bio(bio, pkcs12);
    wc_PKCS12_free(pkcs)
    //use bio
    \endcode

    \sa wolfSSL_PKCS12_parse
    \sa wc_PKCS12_free
*/
WC_PKCS12* wolfSSL_i2d_PKCS12_bio(WOLFSSL_BIO* bio,
                                       WC_PKCS12* pkcs12);

/*!
    \ingroup openSSL

    \brief PKCS12 can be enabled with adding –enable-opensslextra to the
    configure command. It can use triple DES and RC4 for decryption so would
    recommend also enabling these features when enabling opensslextra
    (--enable-des3 –enable-arc4). wolfSSL does not currently support RC2 so
    decryption with RC2 is currently not available. This may be noticeable
    with default encryption schemes used by OpenSSL command line to create
    .p12 files. wolfSSL_PKCS12_parse (PKCS12_parse). The first thing this
    function does is check the MAC is correct if present. If the MAC fails
    then the function returns and does not try to decrypt any of the stored
    Content Infos. This function then parses through each Content Info
    looking for a bag type, if the bag type is known it is decrypted as
    needed and either stored in the list of certificates being built or as
    a key found. After parsing through all bags the key found is then
    compared with the certificate list until a matching pair is found.
    This matching pair is then returned as the key and certificate,
    optionally the certificate list found is returned as a STACK_OF
    certificates. At the moment a CRL, Secret or SafeContents bag will be
    skipped over and not parsed. It can be seen if these or other “Unknown”
    bags are skipped over by viewing the debug print out. Additional attributes
    such as friendly name are skipped over when parsing a PKCS12 file.

    \return SSL_SUCCESS On successfully parsing PKCS12.
    \return SSL_FAILURE If an error case was encountered.

    \param pkcs12 WC_PKCS12 structure to parse.
    \param paswd password for decrypting PKCS12.
    \param pkey structure to hold private key decoded from PKCS12.
    \param cert structure to hold certificate decoded from PKCS12.
    \param stack optional stack of extra certificates.

    _Example_
    \code
    WC_PKCS12* pkcs;
    WOLFSSL_BIO* bio;
    WOLFSSL_X509* cert;
    WOLFSSL_EVP_PKEY* pkey;
    STACK_OF(X509) certs;
    //bio loads in PKCS12 file
    wolfSSL_d2i_PKCS12_bio(bio, &pkcs);
    wolfSSL_PKCS12_parse(pkcs, “a password”, &pkey, &cert, &certs)
    wc_PKCS12_free(pkcs)
    //use cert, pkey, and optionally certs stack
    \endcode

    \sa wolfSSL_d2i_PKCS12_bio
    \sa wc_PKCS12_free
*/
int wolfSSL_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
     WOLFSSL_EVP_PKEY** pkey, WOLFSSL_X509** cert, WOLF_STACK_OF(WOLFSSL_X509)** ca);

/*!
    \ingroup CertsKeys

    \brief Server Diffie-Hellman Ephemeral parameters setting. This function
    sets up the group parameters to be used if the server negotiates a cipher
    suite that uses DHE.

    \return SSL_SUCCESS upon success.
    \return MEMORY_ERROR will be returned if a memory error was encountered.
    \return SIDE_ERROR will be returned if this function is called on an SSL
    client instead of an SSL server.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param p Diffie-Hellman prime number parameter.
    \param pSz size of p.
    \param g Diffie-Hellman “generator” parameter.
    \param gSz size of g.

    _Example_
    \code
    WOLFSSL* ssl;
    static unsigned char p[] = {...};
    static unsigned char g[] = {...};
    ...
    wolfSSL_SetTmpDH(ssl, p, sizeof(p), g, sizeof(g));
    \endcode

    \sa SSL_accept
*/
int  wolfSSL_SetTmpDH(WOLFSSL* ssl, const unsigned char* p, int pSz,
                                const unsigned char* g, int gSz);

/*!
    \ingroup CertsKeys

    \brief The function calls the wolfSSL_SetTMpDH_buffer_wrapper,
    which is a wrapper for Diffie-Hellman parameters.

    \return SSL_SUCCESS on successful execution.
    \return SSL_BAD_FILETYPE if the file type is not PEM and is not
    ASN.1. It will also be returned if the wc_DhParamsLoad does not
    return normally.
    \return SSL_NO_PEM_HEADER returns from PemToDer if there is not
    a PEM header.
    \return SSL_BAD_FILE returned if there is a file error in PemToDer.
    \return SSL_FATAL_ERROR returned from PemToDer if there was a copy error.
    \return MEMORY_E - if there was a memory allocation error.
    \return BAD_FUNC_ARG returned if the WOLFSSL struct is NULL or if
    there was otherwise a NULL argument passed to a subroutine.
    \return DH_KEY_SIZE_E is returned if their is a key size error in
    wolfSSL_SetTmpDH() or in wolfSSL_CTX_SetTmpDH().
    \return SIDE_ERROR returned if it is not the server side
    in wolfSSL_SetTmpDH.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf allocated buffer passed in from wolfSSL_SetTMpDH_file_wrapper.
    \param sz a long int that holds the size of the file
    (fname within wolfSSL_SetTmpDH_file_wrapper).
    \param format an integer type passed through from
    wolfSSL_SetTmpDH_file_wrapper() that is a representation of the certificate
    format.

    _Example_
    \code
    Static int wolfSSL_SetTmpDH_file_wrapper(WOLFSSL_CTX* ctx, WOLFSSL* ssl,
    Const char* fname, int format);
    long sz = 0;
    byte* myBuffer = staticBuffer[FILE_BUFFER_SIZE];
    …
    if(ssl)
    ret = wolfSSL_SetTmpDH_buffer(ssl, myBuffer, sz, format);
    \endcode

    \sa wolfSSL_SetTmpDH_buffer_wrapper
    \sa wc_DhParamsLoad
    \sa wolfSSL_SetTmpDH
    \sa PemToDer
    \sa wolfSSL_CTX_SetTmpDH
    \sa wolfSSL_CTX_SetTmpDH_file
*/
int  wolfSSL_SetTmpDH_buffer(WOLFSSL* ssl, const unsigned char* b, long sz,
                                       int format);

/*!
    \ingroup CertsKeys

    \brief This function calls wolfSSL_SetTmpDH_file_wrapper to set server
    Diffie-Hellman parameters.

    \return SSL_SUCCESS returned on successful completion of this function
    and its subroutines.
    \return MEMORY_E returned if a memory allocation failed in this function
    or a subroutine.
    \return SIDE_ERROR if the side member of the Options structure found
    in the WOLFSSL struct is not the server side.
    \return SSL_BAD_FILETYPE returns if the certificate fails a set of checks.
    \return DH_KEY_SIZE_E returned if the DH parameter's key size is less than
    the value of the minDhKeySz member in the WOLFSSL struct.
    \return DH_KEY_SIZE_E returned if the DH parameter's key size is greater
    than the value of the maxDhKeySz member in the WOLFSSL struct.
    \return BAD_FUNC_ARG returns if an argument value is NULL that is not
    permitted such as, the WOLFSSL structure.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param fname a constant char pointer holding the certificate.
    \param format an integer type that holds the format of the certification.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    const char* dhParam;
    …
    AssertIntNE(SSL_SUCCESS,
    wolfSSL_SetTmpDH_file(ssl, dhParam, SSL_FILETYPE_PEM));
    \endcode

    \sa wolfSSL_CTX_SetTmpDH_file
    \sa wolfSSL_SetTmpDH_file_wrapper
    \sa wolfSSL_SetTmpDH_buffer
    \sa wolfSSL_CTX_SetTmpDH_buffer
    \sa wolfSSL_SetTmpDH_buffer_wrapper
    \sa wolfSSL_SetTmpDH
    \sa wolfSSL_CTX_SetTmpDH
*/
int  wolfSSL_SetTmpDH_file(WOLFSSL* ssl, const char* f, int format);

/*!
    \ingroup CertsKeys

    \brief Sets the parameters for the server CTX Diffie-Hellman.

    \return SSL_SUCCESS returned if the function and all subroutines
    return without error.
    \return BAD_FUNC_ARG returned if the CTX, p or g parameters are NULL.
    \return DH_KEY_SIZE_E returned if the DH parameter's key size is less than
    the value of the minDhKeySz member of the WOLFSSL_CTX struct.
    \return DH_KEY_SIZE_E returned if the DH parameter's key size is greater
    than the value of the maxDhKeySz member of the WOLFSSL_CTX struct.
    \return MEMORY_E returned if the allocation of memory failed in this
    function or a subroutine.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param p a constant unsigned char pointer loaded into the buffer
    member of the serverDH_P struct.
    \param pSz an int type representing the size of p, initialized
    to MAX_DH_SIZE.
    \param g a constant unsigned char pointer loaded into the buffer
    member of the serverDH_G struct.
    \param gSz an int type representing the size of g, initialized to
    MAX_DH_SIZE.

    _Exmaple_
    \code
    WOLFSSL_CTX* ctx =  WOLFSSL_CTX_new( protocol );
    byte* p;
    byte* g;
    word32 pSz = (word32)sizeof(p)/sizeof(byte);
    word32 gSz = (word32)sizeof(g)/sizeof(byte);
    …
    int ret =  wolfSSL_CTX_SetTmpDH(ctx, p, pSz, g, gSz);

    if(ret != SSL_SUCCESS){
    	// Failure case
    }
    \endcode

    \sa wolfSSL_SetTmpDH
    \sa wc_DhParamsLoad
*/
int  wolfSSL_CTX_SetTmpDH(WOLFSSL_CTX* ctx, const unsigned char* p,
                                    int pSz, const unsigned char* g, int gSz);

/*!
    \ingroup CertsKeys

    \brief A wrapper function that calls wolfSSL_SetTmpDH_buffer_wrapper

    \return 0 returned for a successful execution.
    \return BAD_FUNC_ARG returned if the ctx or buf parameters are NULL.
    \return MEMORY_E if there is a memory allocation error.
    \return SSL_BAD_FILETYPE returned if format is not correct.

    \param ctx a pointer to a WOLFSSL structure, created using
    wolfSSL_CTX_new().
    \param buf a pointer to a constant unsigned char type that is
    allocated as the buffer and passed through to
    wolfSSL_SetTmpDH_buffer_wrapper.
    \param sz a long integer type that is derived from the fname parameter
    in wolfSSL_SetTmpDH_file_wrapper().
    \param format an integer type passed through from
    wolfSSL_SetTmpDH_file_wrapper().

    _Example_
    \code
    static int wolfSSL_SetTmpDH_file_wrapper(WOLFSSL_CTX* ctx, WOLFSSL* ssl,
    Const char* fname, int format);
    #ifdef WOLFSSL_SMALL_STACK
    byte staticBuffer[1]; // force heap usage
    #else
    byte* staticBuffer;
    long sz = 0;
    …
    if(ssl){
    	ret = wolfSSL_SetTmpDH_buffer(ssl, myBuffer, sz, format);
    } else {
    ret = wolfSSL_CTX_SetTmpDH_buffer(ctx, myBuffer, sz, format);
    }
    \endcode

    \sa wolfSSL_SetTmpDH_buffer_wrapper
    \sa wolfSSL_SetTMpDH_buffer
    \sa wolfSSL_SetTmpDH_file_wrapper
    \sa wolfSSL_CTX_SetTmpDH_file
*/
int  wolfSSL_CTX_SetTmpDH_buffer(WOLFSSL_CTX* ctx, const unsigned char* b,
                                           long sz, int format);

/*!
    \ingroup CertsKeys

    \brief The function calls wolfSSL_SetTmpDH_file_wrapper to set the server
    Diffie-Hellman parameters.

    \return SSL_SUCCESS returned if the wolfSSL_SetTmpDH_file_wrapper or any
    of its subroutines return successfully.
    \return MEMORY_E returned if an allocation of dynamic memory fails in a
    subroutine.
    \return BAD_FUNC_ARG returned if the ctx or fname parameters are NULL or
    if
    a subroutine is passed a NULL argument.
    \return SSL_BAD_FILE returned if the certificate file is unable to open or
    if the a set of checks on the file fail from wolfSSL_SetTmpDH_file_wrapper.
    \return SSL_BAD_FILETYPE returned if the format is not PEM or ASN.1 from
    wolfSSL_SetTmpDH_buffer_wrapper().
    \return DH_KEY_SIZE_E returned if the DH parameter's key size is less than
    the value of the minDhKeySz member of the WOLFSSL_CTX struct.
    \return DH_KEY_SIZE_E returned if the DH parameter's key size is greater
    than the value of the maxDhKeySz member of the WOLFSSL_CTX struct.
    \return SIDE_ERROR returned in wolfSSL_SetTmpDH() if the side is not the
    server end.
    \return SSL_NO_PEM_HEADER returned from PemToDer if there is no PEM header.
    \return SSL_FATAL_ERROR returned from PemToDer if there is a memory copy
    failure.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param fname a constant character pointer to a certificate file.
    \param format an integer type passed through from
    wolfSSL_SetTmpDH_file_wrapper() that is a representation of
    the certificate format.

    _Example_
    \code
    #define dhParam     “certs/dh2048.pem”
    #DEFINE aSSERTiNTne(x, y)     AssertInt(x, y, !=, ==)
    WOLFSSL_CTX* ctx;
    …
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()))
    …
    AssertIntNE(SSL_SUCCESS, wolfSSL_CTX_SetTmpDH_file(NULL, dhParam,
    SSL_FILETYPE_PEM));
    \endcode

    \sa wolfSSL_SetTmpDH_buffer_wrapper
    \sa wolfSSL_SetTmpDH
    \sa wolfSSL_CTX_SetTmpDH
    \sa wolfSSL_SetTmpDH_buffer
    \sa wolfSSL_CTX_SetTmpDH_buffer
    \sa wolfSSL_SetTmpDH_file_wrapper
    \sa AllocDer
    \sa PemToDer
*/
int  wolfSSL_CTX_SetTmpDH_file(WOLFSSL_CTX* ctx, const char* f,
                                             int format);

/*!
    \ingroup CertsKeys

    \brief This function sets the minimum size (in bits) of the Diffie Hellman
    key size by accessing the minDhKeySz member in the WOLFSSL_CTX structure.

    \return SSL_SUCCESS returned if the function completes successfully.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX struct is NULL or if
    the keySz_bits is greater than 16,000 or not divisible by 8.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param keySz_bits a word16 type used to set the minimum DH key size in bits.
    The WOLFSSL_CTX struct holds this information in the minDhKeySz member.

    _Example_
    \code
    public static int CTX_SetMinDhKey_Sz(IntPtr ctx, short minDhKey){
    …
    return wolfSSL_CTX_SetMinDhKey_Sz(local_ctx, minDhKeyBits);
    \endcode

    \sa wolfSSL_SetMinDhKey_Sz
    \sa wolfSSL_CTX_SetMaxDhKey_Sz
    \sa wolfSSL_SetMaxDhKey_Sz
    \sa wolfSSL_GetDhKey_Sz
    \sa wolfSSL_CTX_SetTMpDH_file
*/
int wolfSSL_CTX_SetMinDhKey_Sz(WOLFSSL_CTX* ctx, word16);

/*!
    \ingroup CertsKeys

    \brief Sets the minimum size (in bits) for a Diffie-Hellman key in the
    WOLFSSL structure.

    \return SSL_SUCCESS the minimum size was successfully set.
    \return BAD_FUNC_ARG the WOLFSSL structure was NULL or if the keySz_bits is
    greater than 16,000 or not divisible by 8.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param keySz_bits a word16 type used to set the minimum DH key size in bits.
    The WOLFSSL_CTX struct holds this information in the minDhKeySz member.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    word16 keySz_bits;
    ...
    if(wolfSSL_SetMinDhKey_Sz(ssl, keySz_bits) != SSL_SUCCESS){
	    // Failed to set.
    }
    \endcode

    \sa wolfSSL_CTX_SetMinDhKey_Sz
    \sa wolfSSL_GetDhKey_Sz
*/
int wolfSSL_SetMinDhKey_Sz(WOLFSSL* ssl, word16 keySz_bits);

/*!
    \ingroup CertsKeys

    \brief This function sets the maximum size (in bits) of the Diffie Hellman
    key size by accessing the maxDhKeySz member in the WOLFSSL_CTX structure.

    \return SSL_SUCCESS returned if the function completes successfully.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX struct is NULL or if
    the keySz_bits is greater than 16,000 or not divisible by 8.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param keySz_bits a word16 type used to set the maximum DH key size in bits.
    The WOLFSSL_CTX struct holds this information in the maxDhKeySz member.

    _Example_
    \code
    public static int CTX_SetMaxDhKey_Sz(IntPtr ctx, short maxDhKey){
    …
    return wolfSSL_CTX_SetMaxDhKey_Sz(local_ctx, keySz_bits);
    \endcode

    \sa wolfSSL_SetMinDhKey_Sz
    \sa wolfSSL_CTX_SetMinDhKey_Sz
    \sa wolfSSL_SetMaxDhKey_Sz
    \sa wolfSSL_GetDhKey_Sz
    \sa wolfSSL_CTX_SetTMpDH_file
*/
int wolfSSL_CTX_SetMaxDhKey_Sz(WOLFSSL_CTX* ctx, word16 keySz_bits);

/*!
    \ingroup CertsKeys

    \brief Sets the maximum size (in bits) for a Diffie-Hellman key in the
    WOLFSSL structure.

    \return SSL_SUCCESS the maximum size was successfully set.
    \return BAD_FUNC_ARG the WOLFSSL structure was NULL or the keySz parameter
    was greater than the allowable size or not divisible by 8.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param keySz a word16 type representing the bit size of the maximum DH key.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    word16 keySz;
    ...
    if(wolfSSL_SetMaxDhKey(ssl, keySz) != SSL_SUCCESS){
	    // Failed to set.
    }
    \endcode

    \sa wolfSSL_CTX_SetMaxDhKey_Sz
    \sa wolfSSL_GetDhKey_Sz
*/
int wolfSSL_SetMaxDhKey_Sz(WOLFSSL* ssl, word16 keySz_bits);

/*!
    \ingroup CertsKeys

    \brief Returns the value of dhKeySz (in bits) that is a member of the
    options structure. This value represents the Diffie-Hellman key size in
    bytes.

    \return dhKeySz returns the value held in ssl->options.dhKeySz which is an
    integer value representing a size in bits.
    \return BAD_FUNC_ARG returns if the WOLFSSL struct is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    int dhKeySz;
    ...
    dhKeySz = wolfSSL_GetDhKey_Sz(ssl);

    if(dhKeySz == BAD_FUNC_ARG || dhKeySz <= 0){
    	// Failure case
    } else {
    	// dhKeySz holds the size of the key.
    }
    \endcode

    \sa wolfSSL_SetMinDhKey_sz
    \sa wolfSSL_CTX_SetMinDhKey_Sz
    \sa wolfSSL_CTX_SetTmpDH
    \sa wolfSSL_SetTmpDH
    \sa wolfSSL_CTX_SetTmpDH_file
*/
int wolfSSL_GetDhKey_Sz(WOLFSSL*);

/*!
    \ingroup CertsKeys

    \brief Sets the minimum RSA key size in both the WOLFSSL_CTX structure
    and the WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS returned on successful execution of the function.
    \return BAD_FUNC_ARG returned if the ctx structure is NULL or the keySz
    is less than zero or not divisible by 8.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param keySz a short integer type stored in minRsaKeySz in the ctx
    structure and the cm structure converted to bytes.

    _Example_
    \code
    WOLFSSL_CTX* ctx = SSL_CTX_new(method);
    (void)minDhKeyBits;
    ourCert = myoptarg;
    …
    minDhKeyBits = atoi(myoptarg);
    …
    if(wolfSSL_CTX_SetMinRsaKey_Sz(ctx, minRsaKeyBits) != SSL_SUCCESS){
    …
    \endcode

    \sa wolfSSL_SetMinRsaKey_Sz
*/
int wolfSSL_CTX_SetMinRsaKey_Sz(WOLFSSL_CTX* ctx, short keySz);

/*!
    \ingroup CertsKeys

    \brief Sets the minimum allowable key size in bits for RSA located in the
    WOLFSSL structure.

    \return SSL_SUCCESS the minimum was set successfully.
    \return BAD_FUNC_ARG returned if the ssl structure is NULL or if the ksySz
    is less than zero or not divisible by 8.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param keySz a short integer value representing the the minimum key in bits.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    short keySz;
    …

    int isSet =  wolfSSL_SetMinRsaKey_Sz(ssl, keySz);
    if(isSet != SSL_SUCCESS){
	    Failed to set.
    }
    \endcode

    \sa wolfSSL_CTX_SetMinRsaKey_Sz
*/
int wolfSSL_SetMinRsaKey_Sz(WOLFSSL* ssl, short keySz);

/*!
    \ingroup CertsKeys

    \brief Sets the minimum size in bits for the ECC key in the WOLF_CTX
    structure and the WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS returned for a successful execution and the minEccKeySz
    member is set.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX struct is NULL or if
    the keySz is negative or not divisible by 8.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param keySz a short integer type that represents the minimum ECC key
    size in bits.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    short keySz; // minimum key size
    …
    if(wolfSSL_CTX_SetMinEccKey(ctx, keySz) != SSL_SUCCESS){
	    // Failed to set min key size
    }
    \endcode

    \sa wolfSSL_SetMinEccKey_Sz
*/
int wolfSSL_CTX_SetMinEccKey_Sz(WOLFSSL_CTX* ssl, short keySz);

/*!
    \ingroup CertsKeys

    \brief Sets the value of the minEccKeySz member of the options structure.
    The options struct is a member of the WOLFSSL structure and is
    accessed through the ssl parameter.

    \return SSL_SUCCESS if the function successfully set the minEccKeySz
    member of the options structure.
    \return BAD_FUNC_ARG if the WOLFSSL_CTX structure is NULL or if the
    key size (keySz) is less than 0 (zero) or not divisible by 8.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param keySz value used to set the minimum ECC key size. Sets
    value in the options structure.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx); // New session
    short keySz = 999; // should be set to min key size allowable
    ...
    if(wolfSSL_SetMinEccKey_Sz(ssl, keySz) != SSL_SUCCESS){
	    // Failure case.
    }
    \endcode

    \sa wolfSSL_CTX_SetMinEccKey_Sz
    \sa wolfSSL_CTX_SetMinRsaKey_Sz
    \sa wolfSSL_SetMinRsaKey_Sz
*/
int wolfSSL_SetMinEccKey_Sz(WOLFSSL* ssl, short keySz);

/*!
    \ingroup CertsKeys

    \brief This function is used by EAP_TLS and EAP-TTLS to derive
    keying material from the master secret.

    \return BUFFER_E returned if the actual size of the buffer exceeds
    the maximum size allowable.
    \return MEMORY_E returned if there is an error with memory allocation.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param msk a void pointer variable that will hold the result
    of the p_hash function.
    \param len an unsigned integer that represents the length of
    the msk variable.
    \param label a constant char pointer that is copied from in wc_PRF().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);;
    void* msk;
    unsigned int len;
    const char* label;
    …
    return wolfSSL_make_eap_keys(ssl, msk, len, label);
    \endcode

    \sa wc_PRF
    \sa wc_HmacFinal
    \sa wc_HmacUpdate
*/
int wolfSSL_make_eap_keys(WOLFSSL* ssl, void* key, unsigned int len,
                                                             const char* label);

/*!
    \ingroup IO

    \brief Simulates writev semantics but doesn’t actually do block at a time
    because of SSL_write() behavior and because front adds may be small.
    Makes porting into software that uses writev easier.

    \return >0 the number of bytes written upon success.
    \return 0 will be returned upon failure.  Call wolfSSL_get_error() for
    the specific error code.
    \return MEMORY_ERROR will be returned if a memory error was encountered.
    \return SSL_FATAL_ERROR will be returned upon failure when either an error
    occurred or, when using non-blocking sockets, the SSL_ERROR_WANT_READ or
    SSL_ERROR_WANT_WRITE error was received and and the application needs to
    call wolfSSL_write() again.  Use wolfSSL_get_error() to get a specific
    error code.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param iov array of I/O vectors to write
    \param iovcnt number of vectors in iov array.

    _Example_
    \code
    WOLFSSL* ssl = 0;
    char *bufA = “hello\n”;
    char *bufB = “hello world\n”;
    int iovcnt;
    struct iovec iov[2];

    iov[0].iov_base = buffA;
    iov[0].iov_len = strlen(buffA);
    iov[1].iov_base = buffB;
    iov[1].iov_len = strlen(buffB);
    iovcnt = 2;
    ...
    ret = wolfSSL_writev(ssl, iov, iovcnt);
    // wrote “ret” bytes, or error if <= 0.
    \endcode

    \sa wolfSSL_write
*/
int wolfSSL_writev(WOLFSSL* ssl, const struct iovec* iov,
                                     int iovcnt);

/*!
    \ingroup Setup

    \brief This function unloads the CA signer list and frees
    the whole signer table.

    \return SSL_SUCCESS returned on successful execution of the function.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX struct is NULL or there
    are otherwise unpermitted argument values passed in a subroutine.
    \return BAD_MUTEX_E returned if there was a mutex error. The LockMutex()
    did not return 0.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_METHOD method = wolfTLSv1_2_client_method();
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new(method);
    …
    if(!wolfSSL_CTX_UnloadCAs(ctx)){
    	// The function did not unload CAs
    }
    \endcode

    \sa wolfSSL_CertManagerUnloadCAs
    \sa LockMutex
    \sa FreeSignerTable
    \sa UnlockMutex
*/
int wolfSSL_CTX_UnloadCAs(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function is used to unload all previously loaded trusted peer
    certificates. Feature is enabled by defining the macro
    WOLFSSL_TRUST_PEER_CERT.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG will be returned if ctx is NULL.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_Unload_trust_peers(ctx);
    if (ret != SSL_SUCCESS) {
        // error unloading trusted peer certs
    }
    ...
    \endcode

    \sa wolfSSL_CTX_trust_peer_buffer
    \sa wolfSSL_CTX_trust_peer_cert
*/
int wolfSSL_CTX_Unload_trust_peers(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function loads a certificate to use for verifying a peer
    when performing a TLS/SSL handshake. The peer certificate sent during
    the handshake is compared by using the SKID when available and the
    signature. If these two things do not match then any loaded CAs are used.
    Is the same functionality as wolfSSL_CTX_trust_peer_cert except is from
    a buffer instead of a file. Feature is enabled by defining the macro
    WOLFSSL_TRUST_PEER_CERT Please see the examples for proper usage.

    \return SSL_SUCCESS upon success
    \return SSL_FAILURE will be returned if ctx is NULL, or if both file and
    type are invalid.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param buffer pointer to the buffer containing certificates.
    \param sz length of the buffer input.
    \param type type of certificate being loaded i.e. SSL_FILETYPE_ASN1 or
    SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx;
    ...

    ret = wolfSSL_CTX_trust_peer_buffer(ctx, bufferPtr, bufferSz,
    SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    // error loading trusted peer cert
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_file
    \sa wolfSSL_CTX_use_PrivateKey_file
    \sa wolfSSL_CTX_use_certificate_chain_file
    \sa wolfSSL_CTX_trust_peer_cert
    \sa wolfSSL_CTX_Unload_trust_peers
    \sa wolfSSL_use_certificate_file
    \sa wolfSSL_use_PrivateKey_file
    \sa wolfSSL_use_certificate_chain_file
*/
int wolfSSL_CTX_trust_peer_buffer(WOLFSSL_CTX* ctx, const unsigned char* in,
                                  long sz, int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a CA certificate buffer into the WOLFSSL
    Context. It behaves like the non-buffered version, only differing in
    its ability to be called with a buffer as input instead of a file.
    The buffer is provided by the in argument of size sz. format specifies
    the format type of the buffer; SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.
    More than one CA certificate may be loaded per buffer as long as the
    format is in PEM.  Please see the examples for proper usage.

    \return SSL_SUCCESS upon success
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BUFFER_E will be returned if a chain buffer is bigger than
    the receiving buffer.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param in pointer to the CA certificate buffer.
    \param sz size of the input CA certificate buffer, in.
    \param format format of the buffer certificate, either SSL_FILETYPE_ASN1
    or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    int sz = 0;
    WOLFSSL_CTX* ctx;
    byte certBuff[...];
    ...

    ret = wolfSSL_CTX_load_verify_buffer(ctx, certBuff, sz, SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    	// error loading CA certs from buffer
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_CTX_load_verify_buffer(WOLFSSL_CTX* ctx, const unsigned char* in,
                                   long sz, int format);


/*!
    \ingroup CertsKeys

    \brief This function loads a CA certificate buffer into the WOLFSSL
    Context. It behaves like the non-buffered version, only differing in
    its ability to be called with a buffer as input instead of a file.
    The buffer is provided by the in argument of size sz. format specifies
    the format type of the buffer; SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.
    More than one CA certificate may be loaded per buffer as long as the
    format is in PEM.  The _ex version was added in PR 2413 and supports 
    additional arguments for userChain and flags.

    \return SSL_SUCCESS upon success
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BUFFER_E will be returned if a chain buffer is bigger than
    the receiving buffer.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param in pointer to the CA certificate buffer.
    \param sz size of the input CA certificate buffer, in.
    \param format format of the buffer certificate, either SSL_FILETYPE_ASN1
    or SSL_FILETYPE_PEM.
    \param userChain If using format WOLFSSL_FILETYPE_ASN1 this set to non-zero
    indicates a chain of DER's is being presented.
    \param flags: See ssl.h around WOLFSSL_LOAD_VERIFY_DEFAULT_FLAGS.

    _Example_
    \code
    int ret = 0;
    int sz = 0;
    WOLFSSL_CTX* ctx;
    byte certBuff[...];
    ...

    // Example for force loading an expired certificate
    ret = wolfSSL_CTX_load_verify_buffer_ex(ctx, certBuff, sz, SSL_FILETYPE_PEM,
        0, (WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY));
    if (ret != SSL_SUCCESS) {
    	// error loading CA certs from buffer
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_CTX_load_verify_buffer_ex(WOLFSSL_CTX* ctx,
                                      const unsigned char* in, long sz,
                                      int format, int userChain, word32 flags);

/*!
    \ingroup CertsKeys

    \brief This function loads a CA certificate chain buffer into the WOLFSSL
    Context. It behaves like the non-buffered version, only differing in
    its ability to be called with a buffer as input instead of a file.
    The buffer is provided by the in argument of size sz. format specifies
    the format type of the buffer; SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.
    More than one CA certificate may be loaded per buffer as long as the
    format is in PEM.  Please see the examples for proper usage.

    \return SSL_SUCCESS upon success
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BUFFER_E will be returned if a chain buffer is bigger than
    the receiving buffer.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param in pointer to the CA certificate buffer.
    \param sz size of the input CA certificate buffer, in.
    \param format format of the buffer certificate, either SSL_FILETYPE_ASN1
    or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    int sz = 0;
    WOLFSSL_CTX* ctx;
    byte certBuff[...];
    ...

    ret = wolfSSL_CTX_load_verify_chain_buffer_format(ctx,
                         certBuff, sz, WOLFSSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        // error loading CA certs from buffer
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_locations
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_CTX_load_verify_chain_buffer_format(WOLFSSL_CTX* ctx,
                                               const unsigned char* in,
                                               long sz, int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a certificate buffer into the WOLFSSL Context.
    It behaves like the non-buffered version, only differing in its ability
    to be called with a buffer as input instead of a file.  The buffer is
    provided by the in argument of size sz.  format specifies the format
    type of the buffer; SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.  Please
    see the examples for proper usage.

    \return SSL_SUCCESS upon success
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param in the input buffer containing the certificate to be loaded.
    \param sz the size of the input buffer.
    \param format the format of the certificate located in the input
    buffer (in).  Possible values are SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    int sz = 0;
    WOLFSSL_CTX* ctx;
    byte certBuff[...];
    ...
    ret = wolfSSL_CTX_use_certificate_buffer(ctx, certBuff, sz, SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    // error loading certificate from buffer
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_CTX_use_certificate_buffer(WOLFSSL_CTX* ctx,
                                       const unsigned char* in, long sz,
                                       int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a private key buffer into the SSL Context.
    It behaves like the non-buffered version, only differing in its ability
    to be called with a buffer as input instead of a file.  The buffer is
    provided by the in argument of size sz.  format specifies the format type
    of the buffer; SSL_FILETYPE_ASN1or SSL_FILETYPE_PEM.  Please see the
    examples for proper usage.

    \return SSL_SUCCESS upon success
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return NO_PASSWORD will be returned if the key file is encrypted but no
    password is provided.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param in the input buffer containing the private key to be loaded.
    \param sz the size of the input buffer.
    \param format the format of the private key located in the input
    buffer (in).  Possible values are SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    int sz = 0;
    WOLFSSL_CTX* ctx;
    byte keyBuff[...];
    ...
    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, keyBuff, sz, SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    	// error loading private key from buffer
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_CTX_use_PrivateKey_buffer(WOLFSSL_CTX* ctx,
                                      const unsigned char* in, long sz,
                                      int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a certificate chain buffer into the WOLFSSL
    Context. It behaves like the non-buffered version, only differing in
    its ability to be called with a buffer as input instead of a file.
    The buffer is provided by the in argument of size sz.  The buffer must
    be in PEM format and start with the subject’s certificate, ending with
    the root certificate. Please see the examples for proper usage.

    \return SSL_SUCCESS upon success
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BUFFER_E will be returned if a chain buffer is bigger than
    the receiving buffer.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param in the input buffer containing the PEM-formatted certificate
    chain to be loaded.
    \param sz the size of the input buffer.

    _Example_
    \code
    int ret = 0;
    int sz = 0;
    WOLFSSL_CTX* ctx;
    byte certChainBuff[...];
    ...
    ret = wolfSSL_CTX_use_certificate_chain_buffer(ctx, certChainBuff, sz);
    if (ret != SSL_SUCCESS) {
    	// error loading certificate chain from buffer
    }
    ...
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_CTX_use_certificate_chain_buffer(WOLFSSL_CTX* ctx,
                                             const unsigned char* in, long sz);

/*!
    \ingroup CertsKeys

    \brief This function loads a certificate buffer into the WOLFSSL object.
    It behaves like the non-buffered version, only differing in its ability
    to be called with a buffer as input instead of a file. The buffer
    is provided by the in argument of size sz.  format specifies the format
    type of the buffer; SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.
    Please see the examples for proper usage.

    \return SSL_SUCCESS upon success.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t
    be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param in buffer containing certificate to load.
    \param sz size of the certificate located in buffer.
    \param format format of the certificate to be loaded.
    Possible values are SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    int buffSz;
    int ret;
    byte certBuff[...];
    WOLFSSL* ssl = 0;
    ...

    ret = wolfSSL_use_certificate_buffer(ssl, certBuff, buffSz, SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    	// failed to load certificate from buffer
    }
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_PrivateKey_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_use_certificate_buffer(WOLFSSL* ssl, const unsigned char* in,
                                               long sz, int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a private key buffer into the WOLFSSL object.
    It behaves like the non-buffered version, only differing in its ability
    to be called with a buffer as input instead of a file.  The buffer is
    provided by the in argument of size sz. format specifies the format
    type of the buffer; SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.  Please
    see the examples for proper usage.

    \return SSL_SUCCESS upon success.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return NO_PASSWORD will be returned if the key file is encrypted but no
    password is provided.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param in buffer containing private key to load.
    \param sz size of the private key located in buffer.
    \param format format of the private key to be loaded.  Possible values are
    SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    int buffSz;
    int ret;
    byte keyBuff[...];
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_use_PrivateKey_buffer(ssl, keyBuff, buffSz, SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    	// failed to load private key from buffer
    }
    \endcode

    \sa wolfSSL_use_PrivateKey
    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_certificate_chain_buffer
*/
int wolfSSL_use_PrivateKey_buffer(WOLFSSL* ssl, const unsigned char* in,
                                               long sz, int format);

/*!
    \ingroup CertsKeys

    \brief This function loads a certificate chain buffer into the WOLFSSL
    object.  It behaves like the non-buffered version, only differing in its
    ability to be called with a buffer as input instead of a file. The buffer
    is provided by the in argument of size sz.  The buffer must be in PEM format
    and start with the subject’s certificate, ending with the root certificate.
    Please see the examples for proper usage.

    \return SSL_SUCCES upon success.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BUFFER_E will be returned if a chain buffer is bigger than
    the receiving buffer.

    \param ssl pointer to the SSL session, created with wolfSSL_new().
    \param in buffer containing certificate to load.
    \param sz size of the certificate located in buffer.

    _Example_
    \code
    int buffSz;
    int ret;
    byte certChainBuff[...];
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_use_certificate_chain_buffer(ssl, certChainBuff, buffSz);
    if (ret != SSL_SUCCESS) {
    	// failed to load certificate chain from buffer
    }
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa wolfSSL_CTX_use_certificate_buffer
    \sa wolfSSL_CTX_use_PrivateKey_buffer
    \sa wolfSSL_CTX_use_certificate_chain_buffer
    \sa wolfSSL_use_certificate_buffer
    \sa wolfSSL_use_PrivateKey_buffer
*/
int wolfSSL_use_certificate_chain_buffer(WOLFSSL* ssl,
                                         const unsigned char* in, long sz);

/*!
    \ingroup CertsKeys

    \brief This function unloads any certificates or keys that SSL owns.

    \return SSL_SUCCESS - returned if the function executed successfully.
    \return BAD_FUNC_ARG - returned if the WOLFSSL object is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    int unloadKeys = wolfSSL_UnloadCertsKeys(ssl);
    if(unloadKeys != SSL_SUCCESS){
	    // Failure case.
    }
    \endcode

    \sa wolfSSL_CTX_UnloadCAs
*/
int wolfSSL_UnloadCertsKeys(WOLFSSL*);

/*!
    \ingroup Setup

    \brief This function turns on grouping of handshake messages where possible.

    \return SSL_SUCCESS will be returned upon success.
    \return BAD_FUNC_ARG will be returned if the input context is null.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    ret = wolfSSL_CTX_set_group_messages(ctx);
    if (ret != SSL_SUCCESS) {
	    // failed to set handshake message grouping
    }
    \endcode

    \sa wolfSSL_set_group_messages
    \sa wolfSSL_CTX_new
*/
int wolfSSL_CTX_set_group_messages(WOLFSSL_CTX*);

/*!
    \ingroup Setup

    \brief This function turns on grouping of handshake messages where possible.

    \return SSL_SUCCESS will be returned upon success.
    \return BAD_FUNC_ARG will be returned if the input context is null.

    \param ssl pointer to the SSL session, created with wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = 0;
    ...
    ret = wolfSSL_set_group_messages(ssl);
    if (ret != SSL_SUCCESS) {
	// failed to set handshake message grouping
    }
    \endcode

    \sa wolfSSL_CTX_set_group_messages
    \sa wolfSSL_new
*/
int wolfSSL_set_group_messages(WOLFSSL*);

/*!
    \brief This function sets the fuzzer callback.

    \return none No returns.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cbf a CallbackFuzzer type that is a function pointer of the form:
    int (*CallbackFuzzer)(WOLFSSL* ssl, const unsigned char* buf, int sz, int
    type, void* fuzzCtx);
    \param fCtx a void pointer type that will be set to the fuzzerCtx member of
    the WOLFSSL structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    void* fCtx;

    int callbackFuzzerCB(WOLFSSL* ssl, const unsigned char* buf, int sz,
				int type, void* fuzzCtx){
    // function definition
    }
    …
    wolfSSL_SetFuzzerCb(ssl, callbackFuzzerCB, fCtx);
    \endcode

    \sa CallbackFuzzer
*/
void wolfSSL_SetFuzzerCb(WOLFSSL* ssl, CallbackFuzzer cbf, void* fCtx);

/*!
    \brief This function sets a new dtls cookie secret.

    \return 0 returned if the function executed without an error.
    \return BAD_FUNC_ARG returned if there was an argument passed
    to the function with an unacceptable value.
    \return COOKIE_SECRET_SZ returned if the secret size is 0.
    \return MEMORY_ERROR returned if there was a problem allocating
    memory for a new cookie secret.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param secret a constant byte pointer representing the secret buffer.
    \param secretSz the size of the buffer.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    const* byte secret;
    word32 secretSz; // size of secret
    …
    if(!wolfSSL_DTLS_SetCookieSecret(ssl, secret, secretSz)){
    	// Code block for failure to set DTLS cookie secret
    } else {
    	// Success! Cookie secret is set.
    }
    \endcode

    \sa ForceZero
    \sa wc_RNG_GenerateBlock
*/
int   wolfSSL_DTLS_SetCookieSecret(WOLFSSL* ssl,
                                               const unsigned char* secret,
                                               unsigned int secretSz);

/*!
    \brief This function retrieves the random number.

    \return rng upon success.
    \return NULL if ssl is NULL.

    \param ssl pointer to a SSL object, created with wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl;

    wolfSSL_GetRNG(ssl);

    \endcode

    \sa  wolfSSL_CTX_new_rng

*/
WC_RNG* wolfSSL_GetRNG(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets the minimum downgrade version allowed.
    Applicable only when the connection allows downgrade using
    (wolfSSLv23_client_method or wolfSSLv23_server_method).

    \return SSL_SUCCESS returned if the function returned without
    error and the minimum version is set.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX structure was
    NULL or if the minimum version is not supported.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param version an integer representation of the version to be set as the
    minimum: WOLFSSL_SSLV3 = 0, WOLFSSL_TLSV1 = 1, WOLFSSL_TLSV1_1 = 2 or
    WOLFSSL_TLSV1_2 = 3.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    int version; // macrop representation
    …
    if(wolfSSL_CTX_SetMinVersion(ssl->ctx, version) != SSL_SUCCESS){
    	// Failed to set min version
    }
    \endcode

    \sa SetMinVersionHelper
*/
int wolfSSL_CTX_SetMinVersion(WOLFSSL_CTX* ctx, int version);

/*!
    \ingroup TLS

    \brief This function sets the minimum downgrade version allowed.
    Applicable only when the connection allows downgrade using
    (wolfSSLv23_client_method or wolfSSLv23_server_method).

    \return SSL_SUCCESS returned if this function and its subroutine executes
    without error.
    \return BAD_FUNC_ARG returned if the SSL object is NULL.  In
    the subroutine this error is thrown if there is not a good version match.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param version an integer representation of the version to be set as the
    minimum: WOLFSSL_SSLV3 = 0, WOLFSSL_TLSV1 = 1, WOLFSSL_TLSV1_1 = 2 or
    WOLFSSL_TLSV1_2 = 3.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new(protocol method);
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    int version;  macro representation
    …
    if(wolfSSL_CTX_SetMinVersion(ssl->ctx, version) != SSL_SUCCESS){
	    Failed to set min version
    }
    \endcode

    \sa SetMinVersionHelper
*/
int wolfSSL_SetMinVersion(WOLFSSL* ssl, int version);

/*!
    \brief This function returns the size of the WOLFSSL object and will be
    dependent on build options and settings.  If SHOW_SIZES has been defined
    when building wolfSSL, this function will also print the sizes of individual
    objects within the WOLFSSL object (Suites, Ciphers, etc.) to stdout.

    \return size This function returns the size of the WOLFSSL object.

    \param none No parameters.

    _Example_
    \code
    int size = 0;
    size = wolfSSL_GetObjectSize();
    printf(“sizeof(WOLFSSL) = %d\n”, size);
    \endcode

    \sa wolfSSL_new
*/
int wolfSSL_GetObjectSize(void);  /* object size based on build */
/*!
    \brief Returns the record layer size of the plaintext input. This is helpful
    when an application wants to know how many bytes will be sent across the
    Transport layer, given a specified plaintext input size. This function
    must be called after the SSL/TLS handshake has been completed.

    \return size Upon success, the requested size will be returned
    \return INPUT_SIZE_E will be returned if the input size is greater than the
    maximum TLS fragment size (see wolfSSL_GetMaxOutputSize())
    \return BAD_FUNC_ARG will be returned upon invalid function argument, or if
    the SSL/TLS handshake has not been completed yet

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().
    \param inSz size of plaintext data.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetMaxOutputSize
*/
int wolfSSL_GetOutputSize(WOLFSSL* ssl, int inSz);

/*!
    \brief Returns the maximum record layer size for plaintext data.  This
    will correspond to either the maximum SSL/TLS record size as specified
    by the protocol standard, the maximum TLS fragment size as set by the
    TLS Max Fragment Length extension. This function is helpful when the
    application has called wolfSSL_GetOutputSize() and received a INPUT_SIZE_E
    error. This function must be called after the SSL/TLS handshake has been
    completed.

    \return size Upon success, the maximum output size will be returned
    \return BAD_FUNC_ARG will be returned upon invalid function argument,
    or if the SSL/TLS handshake has not been completed yet.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetOutputSize
*/
int wolfSSL_GetMaxOutputSize(WOLFSSL*);

/*!
    \ingroup Setup

    \brief This function sets the SSL/TLS protocol version for the specified
    SSL session (WOLFSSL object) using the version as specified by version.
    This will override the protocol setting for the SSL session (ssl) -
    originally defined and set by the SSL context (wolfSSL_CTX_new())
    method type.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG will be returned if the input SSL object is
    NULL or an incorrect protocol version is given for version.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param version SSL/TLS protocol version.  Possible values include
    WOLFSSL_SSLV3, WOLFSSL_TLSV1, WOLFSSL_TLSV1_1, WOLFSSL_TLSV1_2.

    _Example_
    \code
    int ret = 0;
    WOLFSSL* ssl;
    ...

    ret = wolfSSL_SetVersion(ssl, WOLFSSL_TLSV1);
    if (ret != SSL_SUCCESS) {
        // failed to set SSL session protocol version
    }
    \endcode

    \sa wolfSSL_CTX_new
*/
int wolfSSL_SetVersion(WOLFSSL* ssl, int version);

/*!
    \brief Allows caller to set the Atomic User Record Processing
    Mac/Encrypt Callback.  The callback should return 0 for success
    or < 0 for an error.  The ssl and ctx pointers are available
    for the user’s convenience.  macOut is the output buffer where
    the result of the mac should be stored.  macIn is the mac input
    buffer and macInSz notes the size of the buffer.  macContent
    and macVerify are needed for wolfSSL_SetTlsHmacInner() and be
    passed along as is.  encOut is the output buffer where the result
    on the encryption should be stored.  encIn is the input buffer to
    encrypt while encSz is the size of the input.  An example callback
    can be found wolfssl/test.h myMacEncryptCb().

    \return none No return.

    \param No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_SetMacEncryptCtx
    \sa wolfSSL_GetMacEncryptCtx
*/
void  wolfSSL_CTX_SetMacEncryptCb(WOLFSSL_CTX* ctx, CallbackMacEncrypti cb);

/*!
    \brief Allows caller to set the Atomic User Record Processing Mac/Encrypt
    Callback Context to ctx.

    \return none No return.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetMacEncryptCb
    \sa wolfSSL_GetMacEncryptCtx
*/
void  wolfSSL_SetMacEncryptCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Atomic User Record Processing
    Mac/Encrypt Callback Context previously stored with
    wolfSSL_SetMacEncryptCtx().

    \return pointer If successful the call will return a valid pointer
    to the context.
    \return NULL will be returned for a blank context.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetMacEncryptCb
    \sa wolfSSL_SetMacEncryptCtx
*/
void* wolfSSL_GetMacEncryptCtx(WOLFSSL* ssl);

/*!
    \brief Allows caller to set the Atomic User Record Processing
    Decrypt/Verify Callback.  The callback should return 0 for success
    or < 0 for an error.  The ssl and ctx pointers are available for
    the user’s convenience.  decOut is the output buffer where the result
    of the decryption should be stored.  decIn is the encrypted input
    buffer and decInSz notes the size of the buffer.  content and verify
    are needed for wolfSSL_SetTlsHmacInner() and be passed along as is.
    padSz is an output variable that should be set with the total value
    of the padding.  That is, the mac size plus any padding and pad bytes.
    An example callback can be found wolfssl/test.h myDecryptVerifyCb().

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_SetMacEncryptCtx
    \sa wolfSSL_GetMacEncryptCtx
*/
void  wolfSSL_CTX_SetDecryptVerifyCb(WOLFSSL_CTX* ctx,
                                               CallbackDecryptVerify cb);

/*!
    \brief Allows caller to set the Atomic User Record Processing
    Decrypt/Verify Callback Context to ctx.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetDecryptVerifyCb
    \sa wolfSSL_GetDecryptVerifyCtx
*/
void  wolfSSL_SetDecryptVerifyCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Atomic User Record Processing
    Decrypt/Verify Callback Context previously stored with
    wolfSSL_SetDecryptVerifyCtx().

    \return pointer If successful the call will return a valid pointer to the
    context.
    \return NULL will be returned for a blank context.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetDecryptVerifyCb
    \sa wolfSSL_SetDecryptVerifyCtx
*/
void* wolfSSL_GetDecryptVerifyCtx(WOLFSSL* ssl);

/*!
    \brief Allows retrieval of the Hmac/Mac secret from the handshake process.
    The verify parameter specifies whether this is for verification of a
    peer message.

    \return pointer If successful the call will return a valid pointer to the
    secret.  The size of the secret can be obtained from wolfSSL_GetHmacSize().
    \return NULL will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().
    \param verify specifies whether this is for verification of a peer message.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetHmacSize
*/
const unsigned char* wolfSSL_GetMacSecret(WOLFSSL* ssl, int verify);

/*!
    \brief Allows retrieval of the client write key from the handshake process.

    \return pointer If successful the call will return a valid pointer to the
    key. The size of the key can be obtained from wolfSSL_GetKeySize().
    \return NULL will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetKeySize
    \sa wolfSSL_GetClientWriteIV
*/
const unsigned char* wolfSSL_GetClientWriteKey(WOLFSSL*);

/*!
    \brief Allows retrieval of the client write IV (initialization vector)
    from the handshake process.

    \return pointer If successful the call will return a valid pointer to the
    IV.  The size of the IV can be obtained from wolfSSL_GetCipherBlockSize().
    \return NULL will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetCipherBlockSize()
    \sa wolfSSL_GetClientWriteKey()
*/
const unsigned char* wolfSSL_GetClientWriteIV(WOLFSSL*);

/*!
    \brief Allows retrieval of the server write key from the handshake process.

    \return pointer If successful the call will return a valid pointer to the
    key.  The size of the key can be obtained from wolfSSL_GetKeySize().
    \return NULL will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetKeySize
    \sa wolfSSL_GetServerWriteIV
*/
const unsigned char* wolfSSL_GetServerWriteKey(WOLFSSL*);

/*!
    \brief Allows retrieval of the server write IV (initialization vector)
    from the handshake process.

    \return pointer If successful the call will return a valid pointer to the
    IV.  The size of the IV can be obtained from wolfSSL_GetCipherBlockSize().
    \return NULL will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    \sa wolfSSL_GetCipherBlockSize
    \sa wolfSSL_GetClientWriteKey
*/
const unsigned char* wolfSSL_GetServerWriteIV(WOLFSSL*);

/*!
    \brief Allows retrieval of the key size from the handshake process.

    \return size If successful the call will return the key size in bytes.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetClientWriteKey
    \sa wolfSSL_GetServerWriteKey
*/
int                  wolfSSL_GetKeySize(WOLFSSL*);

/*!
    \ingroup CertsKeys

    \brief Returns the iv_size member of the specs structure
    held in the WOLFSSL struct.

    \return iv_size returns the value held in ssl->specs.iv_size.
    \return BAD_FUNC_ARG returned if the WOLFSSL structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    int ivSize;
    ...
    ivSize = wolfSSL_GetIVSize(ssl);

    if(ivSize > 0){
    	// ivSize holds the specs.iv_size value.
    }
    \endcode

    \sa wolfSSL_GetKeySize
    \sa wolfSSL_GetClientWriteIV
    \sa wolfSSL_GetServerWriteIV
*/
int                  wolfSSL_GetIVSize(WOLFSSL*);

/*!
    \brief Allows retrieval of the side of this WOLFSSL connection.

    \return success If successful the call will return either
    WOLFSSL_SERVER_END or WOLFSSL_CLIENT_END depending on the
    side of WOLFSSL object.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetClientWriteKey
    \sa wolfSSL_GetServerWriteKey
*/
int                  wolfSSL_GetSide(WOLFSSL*);

/*!
    \brief Allows caller to determine if the negotiated protocol version
    is at least TLS version 1.1 or greater.

    \return true/false If successful the call will return 1 for true or
    0 for false.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetSide
*/
int                  wolfSSL_IsTLSv1_1(WOLFSSL*);

/*!
    \brief Allows caller to determine the negotiated bulk cipher algorithm
    from the handshake.

    \return If successful the call will return one of the following:
    wolfssl_cipher_null, wolfssl_des, wolfssl_triple_des, wolfssl_aes,
    wolfssl_aes_gcm, wolfssl_aes_ccm, wolfssl_camellia.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetCipherBlockSize
    \sa wolfSSL_GetKeySize
*/
int                  wolfSSL_GetBulkCipher(WOLFSSL*);

/*!
    \brief Allows caller to determine the negotiated cipher block size from
    the handshake.

    \return size If successful the call will return the size in bytes of the
    cipher block size.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetBulkCipher
    \sa wolfSSL_GetKeySize
*/
int                  wolfSSL_GetCipherBlockSize(WOLFSSL*);

/*!
    \brief Allows caller to determine the negotiated aead mac size from the
    handshake.  For cipher type WOLFSSL_AEAD_TYPE.

    \return size If successful the call will return the size in bytes of the
    aead mac size.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetBulkCipher
    \sa wolfSSL_GetKeySize
*/
int                  wolfSSL_GetAeadMacSize(WOLFSSL*);

/*!
    \brief Allows caller to determine the negotiated (h)mac size from the
    handshake. For cipher types except WOLFSSL_AEAD_TYPE.

    \return size If successful the call will return the size in bytes of
    the (h)mac size.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetBulkCipher
    \sa wolfSSL_GetHmacType
*/
int                  wolfSSL_GetHmacSize(WOLFSSL*);

/*!
    \brief Allows caller to determine the negotiated (h)mac type from the
    handshake.  For cipher types except WOLFSSL_AEAD_TYPE.

    \return If successful the call will return one of the following:
    MD5, SHA, SHA256, SHA384.
    \return BAD_FUNC_ARG may be returned for an error state.
    \return SSL_FATAL_ERROR may also be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetBulkCipher
    \sa wolfSSL_GetHmacSize
*/
int                  wolfSSL_GetHmacType(WOLFSSL*);

/*!
    \brief Allows caller to determine the negotiated cipher type
    from the handshake.

    \return If successful the call will return one of the following:
    WOLFSSL_BLOCK_TYPE, WOLFSSL_STREAM_TYPE, WOLFSSL_AEAD_TYPE.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetBulkCipher
    \sa wolfSSL_GetHmacType
*/
int                  wolfSSL_GetCipherType(WOLFSSL*);

/*!
    \brief Allows caller to set the Hmac Inner vector for message
    sending/receiving.  The result is written to inner which should
    be at least wolfSSL_GetHmacSize() bytes.  The size of the message
    is specified by sz, content is the type of message, and verify
    specifies whether this is a verification of a peer message. Valid
    for cipher types excluding WOLFSSL_AEAD_TYPE.

    \return 1 upon success.
    \return BAD_FUNC_ARG will be returned for an error state.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_GetBulkCipher
    \sa wolfSSL_GetHmacType
*/
int wolfSSL_SetTlsHmacInner(WOLFSSL* ssl, byte* inner,
                            word32 sz, int content, int verify);

/*!
    \brief Allows caller to set the Public Key Callback for ECC Signing.
    The callback should return 0 for success or < 0 for an error.
    The ssl and ctx pointers are available for the user’s convenience.
    in is the input buffer to sign while inSz denotes the length of the input.
    out is the output buffer where the result of the signature should be stored.
    outSz is an input/output variable that specifies the size of the output
    buffer upon invocation and the actual size of the signature should be stored
    there before returning.  keyDer is the ECC Private key in ASN1 format and
    keySz is the length of the key in bytes.  An example callback can be found
    wolfssl/test.h myEccSign().

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_SetEccSignCtx
    \sa wolfSSL_GetEccSignCtx
*/
void  wolfSSL_CTX_SetEccSignCb(WOLFSSL_CTX* ctx, CallbackEccSign cb);

/*!
    \brief Allows caller to set the Public Key Ecc Signing Callback
    Context to ctx.

    \return none No returns.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().
    \param ctx a pointer to the user context to be stored

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetEccSignCb
    \sa wolfSSL_GetEccSignCtx
*/
void  wolfSSL_SetEccSignCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Public Key Ecc Signing Callback
    Context previously stored with wolfSSL_SetEccSignCtx().

    \return pointer If successful the call will return a valid pointer
    to the context.
    \return NULL will be returned for a blank context.

    \param ssl a pointer to a WOLFSSL object, created using wolfSSL_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetEccSignCb
    \sa wolfSSL_SetEccSignCtx
*/
void* wolfSSL_GetEccSignCtx(WOLFSSL* ssl);

/*!
    \brief Allows caller to set the Public Key Ecc Signing Callback
    Context to ctx.

    \return none No returns.

    \param ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param ctx a pointer to the user context to be stored

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetEccSignCb
    \sa wolfSSL_CTX_GetEccSignCtx
*/
void  wolfSSL_CTX_SetEccSignCtx(WOLFSSL_CTX* ctx, void *userCtx);

/*!
    \brief Allows caller to retrieve the Public Key Ecc Signing Callback
    Context previously stored with wolfSSL_SetEccSignCtx().

    \return pointer If successful the call will return a valid pointer
    to the context.
    \return NULL will be returned for a blank context.

    \param ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetEccSignCb
    \sa wolfSSL_CTX_SetEccSignCtx
*/
void* wolfSSL_CTX_GetEccSignCtx(WOLFSSL_CTX* ctx);

/*!
    \brief Allows caller to set the Public Key Callback for ECC Verification.
    The callback should return 0 for success or < 0 for an error.
    The ssl and ctx pointers are available for the user’s convenience.
    sig is the signature to verify and sigSz denotes the length of the
    signature. hash is an input buffer containing the digest of the message
    and hashSz denotes the length in bytes of the hash.  result is an output
    variable where the result of the verification should be stored, 1 for
    success and 0 for failure.  keyDer is the ECC Private key in ASN1
    format and keySz is the length of the key in bytes.  An example
    callback can be found wolfssl/test.h myEccVerify().

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_SetEccVerifyCtx
    \sa wolfSSL_GetEccVerifyCtx
*/
void  wolfSSL_CTX_SetEccVerifyCb(WOLFSSL_CTX* ctx, CallbackEccVerify cb);

/*!
    \brief Allows caller to set the Public Key Ecc Verification Callback
    Context to ctx.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetEccVerifyCb
    \sa wolfSSL_GetEccVerifyCtx
*/
void  wolfSSL_SetEccVerifyCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Public Key Ecc Verification Callback
    Context previously stored with wolfSSL_SetEccVerifyCtx().

    \return pointer If successful the call will return a valid pointer to the
    context.
    \return NULL will be returned for a blank context.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetEccVerifyCb
    \sa wolfSSL_SetEccVerifyCtx
*/
void* wolfSSL_GetEccVerifyCtx(WOLFSSL* ssl);

/*!
    \brief Allows caller to set the Public Key Callback for RSA Signing.
    The callback should return 0 for success or < 0 for an error.
    The ssl and ctx pointers are available for the user’s convenience.
    in is the input buffer to sign while inSz denotes the length of the input.
    out is the output buffer where the result of the signature should be stored.
    outSz is an input/output variable that specifies the size of the output
    buffer upon invocation and the actual size of the signature should be
    stored there before returning.  keyDer is the RSA Private key in ASN1 format
    and keySz is the length of the key in bytes.  An example callback can be
    found wolfssl/test.h myRsaSign().

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_SetRsaSignCtx
    \sa wolfSSL_GetRsaSignCtx
*/
void  wolfSSL_CTX_SetRsaSignCb(WOLFSSL_CTX* ctx, CallbackRsaSign cb);

/*!
    \brief Allows caller to set the Public Key RSA Signing Callback Context
    to ctx.

    \return none No Returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaSignCb
    \sa wolfSSL_GetRsaSignCtx
*/
void  wolfSSL_SetRsaSignCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Public Key RSA Signing Callback
    Context previously stored with wolfSSL_SetRsaSignCtx().

    \return pointer If successful the call will return a valid pointer to the
    context.
    \return NULL will be returned for a blank context.

    \param none No parameters.
    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaSignCb
    \sa wolfSSL_SetRsaSignCtx
*/
void* wolfSSL_GetRsaSignCtx(WOLFSSL* ssl);

/*!
    \brief Allows caller to set the Public Key Callback for RSA Verification.
    The callback should return the number of plaintext bytes for success or
    < 0 for an error.  The ssl and ctx pointers are available for the user’s
    convenience.  sig is the signature to verify and sigSz denotes the length
    of the signature.  out should be set to the beginning of the verification
    buffer after the decryption process and any padding.  keyDer is the RSA
    Public key in ASN1 format and keySz is the length of the key in bytes.
    An example callback can be found wolfssl/test.h myRsaVerify().

    \return none No returns.

    \param none No parameters.

    \sa wolfSSL_SetRsaVerifyCtx
    \sa wolfSSL_GetRsaVerifyCtx
*/
void  wolfSSL_CTX_SetRsaVerifyCb(WOLFSSL_CTX* ctx, CallbackRsaVerify cb);

/*!
    \brief Allows caller to set the Public Key RSA Verification Callback
    Context to ctx.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaVerifyCb
    \sa wolfSSL_GetRsaVerifyCtx
*/
void  wolfSSL_SetRsaVerifyCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Public Key RSA Verification Callback
    Context previously stored with wolfSSL_SetRsaVerifyCtx().

    \return pointer If successful the call will return a valid pointer to
    the context.
    \return NULL will be returned for a blank context.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaVerifyCb
    \sa wolfSSL_SetRsaVerifyCtx
*/
void* wolfSSL_GetRsaVerifyCtx(WOLFSSL* ssl);

/*!
    \brief Allows caller to set the Public Key Callback for RSA Public
    Encrypt.  The callback should return 0 for success or < 0 for an error.
    The ssl and ctx pointers are available for the user’s convenience.
    in is the input buffer to encrypt while inSz denotes the length of
    the input.  out is the output buffer where the result of the encryption
    should be stored.  outSz is an input/output variable that specifies
    the size of the output buffer upon invocation and the actual size of
    the encryption should be stored there before returning.  keyDer is the
    RSA Public key in ASN1 format and keySz is the length of the key in
    bytes. An example callback can be found wolfssl/test.h myRsaEnc().

    \return none No returns.

    \param none No parameters.

    _Examples_
    \code
    none
    \endcode

    \sa wolfSSL_SetRsaEncCtx
    \sa wolfSSL_GetRsaEncCtx
*/
void  wolfSSL_CTX_SetRsaEncCb(WOLFSSL_CTX* ctx, CallbackRsaEnc cb);

/*!
    \brief Allows caller to set the Public Key RSA Public Encrypt
    Callback Context to ctx.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaEncCb
    \sa wolfSSL_GetRsaEncCtx
*/
void  wolfSSL_SetRsaEncCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Public Key RSA Public Encrypt
    Callback Context previously stored with wolfSSL_SetRsaEncCtx().

    \return pointer If successful the call will return a valid pointer
    to the context.
    \return NULL will be returned for a blank context.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaEncCb
    \sa wolfSSL_SetRsaEncCtx
*/
void* wolfSSL_GetRsaEncCtx(WOLFSSL* ssl);

/*!
    \brief Allows caller to set the Public Key Callback for RSA Private
    Decrypt.  The callback should return the number of plaintext bytes
    for success or < 0 for an error.  The ssl and ctx pointers are available
    for the user’s convenience.  in is the input buffer to decrypt and inSz
    denotes the length of the input.  out should be set to the beginning
    of the decryption buffer after the decryption process and any padding.
    keyDer is the RSA Private key in ASN1 format and keySz is the length
    of the key in bytes.  An example callback can be found
    wolfssl/test.h myRsaDec().

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_SetRsaDecCtx
    \sa wolfSSL_GetRsaDecCtx
*/
void  wolfSSL_CTX_SetRsaDecCb(WOLFSSL_CTX* ctx, CallbackRsaDec cb);

/*!
    \brief Allows caller to set the Public Key RSA Private Decrypt
    Callback Context to ctx.

    \return none No returns.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaDecCb
    \sa wolfSSL_GetRsaDecCtx
*/
void  wolfSSL_SetRsaDecCtx(WOLFSSL* ssl, void *ctx);

/*!
    \brief Allows caller to retrieve the Public Key RSA Private Decrypt
    Callback Context previously stored with wolfSSL_SetRsaDecCtx().

    \return pointer If successful the call will return a valid pointer
    to the context.
    \return NULL will be returned for a blank context.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_SetRsaDecCb
    \sa wolfSSL_SetRsaDecCtx
*/
void* wolfSSL_GetRsaDecCtx(WOLFSSL* ssl);

/*!
    \brief This function registers a callback with the SSL context
    (WOLFSSL_CTX) to be called when a new CA certificate is loaded
    into wolfSSL.  The callback is given a buffer with the DER-encoded
    certificate.

    \return none No return.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param callback function to be registered as the CA callback for the
    wolfSSL context, ctx. The signature of this function must follow that
    as shown above in the Synopsis section.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;

    // CA callback prototype
    int MyCACallback(unsigned char *der, int sz, int type);

    // Register the custom CA callback with the SSL context
    wolfSSL_CTX_SetCACb(ctx, MyCACallback);

    int MyCACallback(unsigned char* der, int sz, int type)
    {
    	// custom CA callback function, DER-encoded cert
        // located in “der” of size “sz” with type “type”
    }
    \endcode

    \sa wolfSSL_CTX_load_verify_locations
*/
void wolfSSL_CTX_SetCACb(WOLFSSL_CTX* ctx, CallbackCACache cb);

/*!
    \ingroup CertManager
    \brief Allocates and initializes a new Certificate Manager context.
    This context may be used independent of SSL needs.  It may be used to
    load certificates, verify certificates, and check the revocation status.

    \return WOLFSSL_CERT_MANAGER If successful the call will return a valid
    WOLFSSL_CERT_MANAGER pointer.
    \return NULL will be returned for an error state.

    \param none No parameters.

    \sa wolfSSL_CertManagerFree
*/
WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew_ex(void* heap);

/*!
    \ingroup CertManager
    \brief Allocates and initializes a new Certificate Manager context.
    This context may be used independent of SSL needs.  It may be used to
    load certificates, verify certificates, and check the revocation status.

    \return WOLFSSL_CERT_MANAGER If successful the call will return a
    valid WOLFSSL_CERT_MANAGER pointer.
    \return NULL will be returned for an error state.

    \param none No parameters.

    _Example_
    \code
    #import <wolfssl/ssl.h>

    WOLFSSL_CERT_MANAGER* cm;
    cm = wolfSSL_CertManagerNew();
    if (cm == NULL) {
	// error creating new cert manager
    }
    \endcode

    \sa wolfSSL_CertManagerFree
*/
WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew(void);

/*!
    \ingroup CertManager
    \brief Frees all resources associated with the Certificate Manager
    context.  Call this when you no longer need to use the Certificate Manager.

    \return none

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CERT_MANAGER* cm;
    ...
    wolfSSL_CertManagerFree(cm);
    \endcode

    \sa wolfSSL_CertManagerNew
*/
void wolfSSL_CertManagerFree(WOLFSSL_CERT_MANAGER*);

/*!
    \ingroup CertManager
    \brief Specifies the locations for CA certificate loading into the
    manager context.  The PEM certificate CAfile may contain several
    trusted CA certificates.  If CApath is not NULL it specifies a
    directory containing CA certificates in PEM format.

    \return SSL_SUCCESS If successful the call will return.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist,
    can’t be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BAD_FUNC_ARG is the error that will be returned if a
    pointer is not provided.
    \return SSL_FATAL_ERROR - will be returned upon failure.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created
    using wolfSSL_CertManagerNew().
    \param file pointer to the name of the file containing CA
    certificates to load.
    \param path pointer to the name of a directory path containing CA c
    ertificates to load.  The NULL pointer may be used if no
    certificate directory is desired.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int ret = 0;
    WOLFSSL_CERT_MANAGER* cm;
    ...
    ret = wolfSSL_CertManagerLoadCA(cm, “path/to/cert-file.pem”, 0);
    if (ret != SSL_SUCCESS) {
	// error loading CA certs into cert manager
    }
    \endcode

    \sa wolfSSL_CertManagerVerify
*/
int wolfSSL_CertManagerLoadCA(WOLFSSL_CERT_MANAGER* cm, const char* f,
                                                                 const char* d);

/*!
    \ingroup CertManager
    \brief Loads the CA Buffer by calling wolfSSL_CTX_load_verify_buffer and
    returning that result using a temporary cm so as not to lose the information
    in the cm passed into the function.

    \return SSL_FATAL_ERROR is returned if the WOLFSSL_CERT_MANAGER struct is
    NULL or if wolfSSL_CTX_new() returns NULL.
    \return SSL_SUCCESS is returned for a successful execution.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param in buffer for cert information.
    \param sz length of the buffer.
    \param format certificate format, either PEM or DER.

    _Example_
    \code
    WOLFSSL_CERT_MANAGER* cm = (WOLFSSL_CERT_MANAGER*)vp;
    …
    const unsigned char* in;
    long sz;
    int format;
    …
    if(wolfSSL_CertManagerLoadCABuffer(vp, sz, format) != SSL_SUCCESS){
	    Error returned. Failure case code block.
    }
    \endcode

    \sa wolfSSL_CTX_load_verify_buffer
    \sa ProcessChainBuffer
    \sa ProcessBuffer
    \sa cm_pick_method
*/
int wolfSSL_CertManagerLoadCABuffer(WOLFSSL_CERT_MANAGER* cm,
                                  const unsigned char* in, long sz, int format);

/*!
    \ingroup CertManager
    \brief This function unloads the CA signer list.

    \return SSL_SUCCESS returned on successful execution of the function.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER is NULL.
    \return BAD_MUTEX_E returned if there was a mutex error.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure,
    created using wolfSSL_CertManagerNew().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(protocol method);
    WOLFSSL_CERT_MANAGER* cm = wolfSSL_CertManagerNew();
    ...
    if(wolfSSL_CertManagerUnloadCAs(ctx->cm) != SSL_SUCCESS){
    	Failure case.
    }
    \endcode

    \sa FreeSignerTable
    \sa UnlockMutex
*/
int wolfSSL_CertManagerUnloadCAs(WOLFSSL_CERT_MANAGER* cm);

/*!
    \ingroup CertManager
    \brief The function will free the Trusted Peer linked list and unlocks
    the trusted peer list.

    \return SSL_SUCCESS if the function completed normally.
    \return BAD_FUNC_ARG if the WOLFSSL_CERT_MANAGER is NULL.
    \return BAD_MUTEX_E mutex  error if tpLock, a member of the
    WOLFSSL_CERT_MANAGER struct, is 0 (nill).

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new(Protocol define);
    WOLFSSL_CERT_MANAGER* cm = wolfSSL_CertManagerNew();
    ...
    if(wolfSSL_CertManagerUnload_trust_peers(cm) != SSL_SUCCESS){
	    The function did not execute successfully.
    }
    \endcode

    \sa UnLockMutex
*/
int wolfSSL_CertManagerUnload_trust_peers(WOLFSSL_CERT_MANAGER* cm);

/*!
    \ingroup CertManager
    \brief Specifies the certificate to verify with the Certificate Manager
    context.  The format can be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1.

    \return SSL_SUCCESS If successful.
    \return ASN_SIG_CONFIRM_E will be returned if the signature could not be
    verified.
    \return ASN_SIG_OID_E will be returned if the signature type is not
    supported.
    \return CRL_CERT_REVOKED is an error that is returned if this certificate
    has been revoked.
    \return CRL_MISSING is an error that is returned if a current issuer CRL is
    not available.
    \return ASN_BEFORE_DATE_E will be returned if the current date is before the
    before date.
    \return ASN_AFTER_DATE_E will be returned if the current date is after the
    after date.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t be
    read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BAD_FUNC_ARG is the error that will be returned if a pointer is
    not provided.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param fname pointer to the name of the file containing the certificates
    to verify.
    \param format format of the certificate to verify - either
    SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CERT_MANAGER* cm;
    ...

    ret = wolfSSL_CertManagerVerify(cm, “path/to/cert-file.pem”,
    SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
	    error verifying certificate
    }
    \endcode

    \sa wolfSSL_CertManagerLoadCA
    \sa wolfSSL_CertManagerVerifyBuffer
*/
int wolfSSL_CertManagerVerify(WOLFSSL_CERT_MANAGER* cm, const char* f,
                                                                    int format);

/*!
    \ingroup CertManager
    \brief Specifies the certificate buffer to verify with the Certificate
    Manager context.  The format can be SSL_FILETYPE_PEM or SSL_FILETYPE_ASN1.

    \return SSL_SUCCESS If successful.
    \return ASN_SIG_CONFIRM_E will be returned if the signature could not
    be verified.
    \return ASN_SIG_OID_E will be returned if the signature type is not
    supported.
    \return CRL_CERT_REVOKED is an error that is returned if this certificate
    has been revoked.
    \return CRL_MISSING is an error that is returned if a current issuer CRL
    is not available.
    \return ASN_BEFORE_DATE_E will be returned if the current date is before
    the before date.
    \return ASN_AFTER_DATE_E will be returned if the current date is after
    the after date.
    \return SSL_BAD_FILETYPE will be returned if the file is the wrong format.
    \return SSL_BAD_FILE will be returned if the file doesn’t exist, can’t
    be read, or is corrupted.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return ASN_INPUT_E will be returned if Base16 decoding fails on the file.
    \return BAD_FUNC_ARG is the error that will be returned if a pointer
    is not provided.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param buff buffer containing the certificates to verify.
    \param sz size of the buffer, buf.
    \param format format of the certificate to verify, located in buf - either
    SSL_FILETYPE_ASN1 or SSL_FILETYPE_PEM.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int ret = 0;
    int sz = 0;
    WOLFSSL_CERT_MANAGER* cm;
    byte certBuff[...];
    ...

    ret = wolfSSL_CertManagerVerifyBuffer(cm, certBuff, sz, SSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
    	error verifying certificate
    }

    \endcode

    \sa wolfSSL_CertManagerLoadCA
    \sa wolfSSL_CertManagerVerify
*/
int wolfSSL_CertManagerVerifyBuffer(WOLFSSL_CERT_MANAGER* cm,
                                const unsigned char* buff, long sz, int format);

/*!
    \ingroup CertManager
    \brief The function sets the verifyCallback function in the Certificate
    Manager. If present, it will be called for each cert loaded. If there is
    a verification error, the verify callback can be used to over-ride the
    error.

    \return none No return.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param vc a VerifyCallback function pointer to the callback routine

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int myVerify(int preverify, WOLFSSL_X509_STORE_CTX* store)
    { // do custom verification of certificate }

    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new(Protocol define);
    WOLFSSL_CERT_MANAGER* cm = wolfSSL_CertManagerNew();
    ...
    wolfSSL_CertManagerSetVerify(cm, myVerify);

    \endcode

    \sa wolfSSL_CertManagerVerify
*/
void wolfSSL_CertManagerSetVerify(WOLFSSL_CERT_MANAGER* cm,
                                                             VerifyCallback vc);

/*!
    \brief Check CRL if the option is enabled and compares the cert to the
    CRL list.

    \return SSL_SUCCESS returns if the function returned as expected. If
    the crlEnabled member of the WOLFSSL_CERT_MANAGER struct is turned on.
    \return MEMORY_E returns if the allocated memory failed.
    \return BAD_FUNC_ARG if the WOLFSSL_CERT_MANAGER is NULL.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER struct.
    \param der pointer to a DER formatted certificate.
    \param sz size of the certificate.

    _Example_
    \code
    WOLFSSL_CERT_MANAGER* cm;
    byte* der;
    int sz; // size of der
    ...
    if(wolfSSL_CertManagerCheckCRL(cm, der, sz) != SSL_SUCCESS){
    	// Error returned. Deal with failure case.
    }
    \endcode

    \sa CheckCertCRL
    \sa ParseCertRelative
    \sa wolfSSL_CertManagerSetCRL_CB
    \sa InitDecodedCert
*/
int wolfSSL_CertManagerCheckCRL(WOLFSSL_CERT_MANAGER* cm,
                                unsigned char* der, int sz);

/*!
    \ingroup CertManager
    \brief Turns on Certificate Revocation List checking when verifying
    certificates with the Certificate Manager.  By default, CRL checking
    is off.  options include WOLFSSL_CRL_CHECKALL which performs CRL
    checking on each certificate in the chain versus the Leaf certificate
    only which is the default.

    \return SSL_SUCCESS If successful the call will return.
    \return NOT_COMPILED_IN will be returned if wolfSSL was not built with
    CRL enabled.
    \return MEMORY_E will be returned if an out of memory condition occurs.
    \return BAD_FUNC_ARG is the error that will be returned if a pointer
    is not provided.
    \return SSL_FAILURE will be returned if the CRL context cannot be
    initialized properly.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param options options to use when enabling the Certification Manager, cm.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int ret = 0;
    WOLFSSL_CERT_MANAGER* cm;
    ...

    ret = wolfSSL_CertManagerEnableCRL(cm, 0);
    if (ret != SSL_SUCCESS) {
    	error enabling cert manager
    }

    ...
    \endcode

    \sa wolfSSL_CertManagerDisableCRL
*/
int wolfSSL_CertManagerEnableCRL(WOLFSSL_CERT_MANAGER* cm,
                                                                   int options);

/*!
    \ingroup CertManager
    \brief Turns off Certificate Revocation List checking when verifying
    certificates with the Certificate Manager.  By default, CRL checking is
    off.  You can use this function to temporarily or permanently disable CRL
    checking with this Certificate Manager context that previously had CRL
    checking enabled.

    \return SSL_SUCCESS If successful the call will return.
    \return BAD_FUNC_ARG is the error that will be returned if a function
    pointer is not provided.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int ret = 0;
    WOLFSSL_CERT_MANAGER* cm;
    ...
    ret = wolfSSL_CertManagerDisableCRL(cm);
    if (ret != SSL_SUCCESS) {
    	error disabling cert manager
    }
    ...
    \endcode

    \sa wolfSSL_CertManagerEnableCRL
*/
int wolfSSL_CertManagerDisableCRL(WOLFSSL_CERT_MANAGER*);

/*!
    \ingroup CertManager
    \brief Error checks and passes through to LoadCRL() in order to load the
    cert into the CRL for revocation checking. An updated CRL can be loaded by
    first calling wolfSSL_CertManagerFreeCRL, then loading the new CRL.

    \return SSL_SUCCESS if there is no error in wolfSSL_CertManagerLoadCRL and
    if LoadCRL returns successfully.
    \return BAD_FUNC_ARG if the WOLFSSL_CERT_MANAGER struct is NULL.
    \return SSL_FATAL_ERROR if wolfSSL_CertManagerEnableCRL returns anything
    other than SSL_SUCCESS.
    \return BAD_PATH_ERROR if the path is NULL.
    \return MEMORY_E if LoadCRL fails to allocate heap memory.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param path a constant char pointer holding the CRL path.
    \param type type of certificate to be loaded.
    \param monitor requests monitoring in LoadCRL().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    int wolfSSL_LoadCRL(WOLFSSL* ssl, const char* path, int type,
    int monitor);
    …
    wolfSSL_CertManagerLoadCRL(SSL_CM(ssl), path, type, monitor);
    \endcode

    \sa wolfSSL_CertManagerEnableCRL
    \sa wolfSSL_LoadCRL
    \sa wolfSSL_CertManagerFreeCRL
*/
int wolfSSL_CertManagerLoadCRL(WOLFSSL_CERT_MANAGER* cm,
                               const char* path, int type, int monitor);

/*!
    \ingroup CertManager
    \brief The function loads the CRL file by calling BufferLoadCRL.

    \return SSL_SUCCESS returned if the function completed without errors.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER is NULL.
    \return SSL_FATAL_ERROR returned if there is an error associated
    with the WOLFSSL_CERT_MANAGER.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure.
    \param buff a constant byte type and is the buffer.
    \param sz a long int representing the size of the buffer.
    \param type a long integer that holds the certificate type.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CERT_MANAGER* cm;
    const unsigned char* buff;
    long sz; size of buffer
    int type;  cert type
    ...
    int ret = wolfSSL_CertManagerLoadCRLBuffer(cm, buff, sz, type);
    if(ret == SSL_SUCCESS){
	return ret;
    } else {
    	Failure case.
    }
    \endcode

    \sa BufferLoadCRL
    \sa wolfSSL_CertManagerEnableCRL
*/
int wolfSSL_CertManagerLoadCRLBuffer(WOLFSSL_CERT_MANAGER* cm,
                                     const unsigned char* buff, long sz,
                                     int type);

/*!
    \ingroup CertManager
    \brief This function sets the CRL Certificate Manager callback. If
    HAVE_CRL is defined and a matching CRL record is not found then the
    cbMissingCRL is called (set via wolfSSL_CertManagerSetCRL_Cb). This
    allows you to externally retrieve the CRL and load it.

    \return SSL_SUCCESS returned upon successful execution of the function and
    subroutines.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER structure is NULL.

    \param cm the WOLFSSL_CERT_MANAGER structure holding the information for
    the certificate.
    \param cb a function pointer to (*CbMissingCRL) that is set to the
    cbMissingCRL member of the WOLFSSL_CERT_MANAGER.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(protocol method);
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    void cb(const char* url){
	    Function body.
    }
    …
    CbMissingCRL cb = CbMissingCRL;
    …
    if(ctx){
        return wolfSSL_CertManagerSetCRL_Cb(SSL_CM(ssl), cb);
    }
    \endcode

    \sa CbMissingCRL
    \sa wolfSSL_SetCRL_Cb
*/
int wolfSSL_CertManagerSetCRL_Cb(WOLFSSL_CERT_MANAGER* cm,
                                 CbMissingCRL cb);

/*!
    \ingroup CertManager
    \brief This function frees the CRL stored in the Cert Manager. An
    application can update the CRL by calling wolfSSL_CertManagerFreeCRL
    and then loading the new CRL.

    \return SSL_SUCCESS returned upon successful execution of the function and
    subroutines.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER structure is NULL.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    const char* crl1     = "./certs/crl/crl.pem";
    WOLFSSL_CERT_MANAGER* cm = NULL;

    cm = wolfSSL_CertManagerNew();
    wolfSSL_CertManagerLoadCRL(cm, crl1, WOLFSSL_FILETYPE_PEM, 0);
    …
    wolfSSL_CertManagerFreeCRL(cm);
    \endcode

    \sa wolfSSL_CertManagerLoadCRL
*/
int wolfSSL_CertManagerFreeCRL(WOLFSSL_CERT_MANAGER* cm);

/*!
    \ingroup CertManager
    \brief The function enables the WOLFSSL_CERT_MANAGER’s member, ocspEnabled
    to signify that the OCSP check option is enabled.

    \return SSL_SUCCESS returned on successful execution of the function. The
    ocspEnabled member of the WOLFSSL_CERT_MANAGER is enabled.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER structure is
    NULL or if an argument value that is not allowed is passed to a subroutine.
    \return MEMORY_E returned if there is an error allocating memory within
    this function or a subroutine.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param der a byte pointer to the certificate.
    \param sz an int type representing the size of the DER cert.

    _Example_
    \code
    #import <wolfssl/ssl.h>

    WOLFSSL* ssl = wolfSSL_new(ctx);
    byte* der;
    int sz; size of der
    ...
    if(wolfSSL_CertManagerCheckOCSP(cm, der, sz) != SSL_SUCCESS){
	 Failure case.
    }
    \endcode

    \sa ParseCertRelative
    \sa CheckCertOCSP
*/
int wolfSSL_CertManagerCheckOCSP(WOLFSSL_CERT_MANAGER* cm,
                                 unsigned char* der, int sz);

/*!
    \ingroup CertManager
    \brief Turns on OCSP if it’s turned off and if compiled with the
    set option available.

    \return SSL_SUCCESS returned if the function call is successful.
    \return BAD_FUNC_ARG if cm struct is NULL.
    \return MEMORY_E if WOLFSSL_OCSP struct value is NULL.
    \return SSL_FAILURE initialization of WOLFSSL_OCSP struct fails
    to initialize.
    \return NOT_COMPILED_IN build not compiled with correct feature enabled.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, created using
    wolfSSL_CertManagerNew().
    \param options used to set values in WOLFSSL_CERT_MANAGER struct.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(protocol method);
    WOLFSSL* ssl = wolfSSL_new(ctx);
    WOLFSSL_CERT_MANAGER* cm = wolfSSL_CertManagerNew();
    int options;
    …
    if(wolfSSL_CertManagerEnableOCSP(SSL_CM(ssl), options) != SSL_SUCCESS){
	    Failure case.
    }
    \endcode

    \sa wolfSSL_CertManagerNew
*/
int wolfSSL_CertManagerEnableOCSP(WOLFSSL_CERT_MANAGER* cm,
                                                                   int options);

/*!
    \ingroup CertManager
    \brief Disables OCSP certificate revocation.

    \return SSL_SUCCESS wolfSSL_CertMangerDisableCRL successfully disabled the
    crlEnabled member of the WOLFSSL_CERT_MANAGER structure.
    \return BAD_FUNC_ARG the WOLFSSL structure was NULL.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(method);
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_CertManagerDisableOCSP(ssl) != SSL_SUCCESS){
	    Fail case.
    }
    \endcode

    \sa wolfSSL_DisableCRL
*/
int wolfSSL_CertManagerDisableOCSP(WOLFSSL_CERT_MANAGER*);

/*!
    \ingroup CertManager
    \brief The function copies the url to the ocspOverrideURL member of the
    WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS the function was able to execute as expected.
    \return BAD_FUNC_ARG the WOLFSSL_CERT_MANAGER struct is NULL.
    \return MEMEORY_E Memory was not able to be allocated for the
    ocspOverrideURL member of the certificate manager.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    #include <wolfssl/ssl.h>
    WOLFSSL_CERT_MANAGER* cm = wolfSSL_CertManagerNew();
    const char* url;
    …
    int wolfSSL_SetOCSP_OverrideURL(WOLFSSL* ssl, const char* url)
    …
    if(wolfSSL_CertManagerSetOCSPOverrideURL(SSL_CM(ssl), url) != SSL_SUCCESS){
	    Failure case.
    }
    \endcode

    \sa ocspOverrideURL
    \sa wolfSSL_SetOCSP_OverrideURL
*/
int wolfSSL_CertManagerSetOCSPOverrideURL(WOLFSSL_CERT_MANAGER* cm,
                                          const char* url);

/*!
    \ingroup CertManager
    \brief The function sets the OCSP callback in the WOLFSSL_CERT_MANAGER.

    \return SSL_SUCCESS returned on successful execution. The arguments are
    saved in the WOLFSSL_CERT_MANAGER structure.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER is NULL.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure.
    \param ioCb a function pointer of type CbOCSPIO.
    \param respFreeCb - a function pointer of type CbOCSPRespFree.
    \param ioCbCtx - a void pointer variable to the I/O callback user
    registered context.

    _Example_
    \code
    #include <wolfssl/ssl.h>

    wolfSSL_SetOCSP_Cb(WOLFSSL* ssl, CbOCSPIO ioCb,
    CbOCSPRespFree respFreeCb, void* ioCbCtx){
    …
    return wolfSSL_CertManagerSetOCSP_Cb(SSL_CM(ssl), ioCb, respFreeCb, ioCbCtx);
    \endcode

    \sa wolfSSL_CertManagerSetOCSPOverrideURL
    \sa wolfSSL_CertManagerCheckOCSP
    \sa wolfSSL_CertManagerEnableOCSPStapling
    \sa wolfSSL_ENableOCSP
    \sa wolfSSL_DisableOCSP
    \sa wolfSSL_SetOCSP_Cb
*/
int wolfSSL_CertManagerSetOCSP_Cb(WOLFSSL_CERT_MANAGER* cm,
                                  CbOCSPIO ioCb, CbOCSPRespFree respFreeCb,
                                  void* ioCbCtx);

/*!
    \ingroup CertManager
    \brief This function turns on OCSP stapling if it is not turned on as well
    as set the options.

    \return SSL_SUCCESS returned if there were no errors and the function
    executed successfully.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CERT_MANAGER structure is
    NULL or otherwise if there was a unpermitted argument value passed to
    a subroutine.
    \return MEMORY_E returned if there was an issue allocating memory.
    \return SSL_FAILURE returned if the initialization of the OCSP
    structure failed.
    \return NOT_COMPILED_IN returned if wolfSSL was not compiled with
    HAVE_CERTIFICATE_STATUS_REQUEST option.

    \param cm a pointer to a WOLFSSL_CERT_MANAGER structure, a member of the
    WOLFSSL_CTX structure.

    _Example_
    \code
    int wolfSSL_CTX_EnableOCSPStapling(WOLFSSL_CTX* ctx){
    …
    return wolfSSL_CertManagerEnableOCSPStapling(ctx->cm);
    \endcode

    \sa wolfSSL_CTX_EnableOCSPStapling
*/
int wolfSSL_CertManagerEnableOCSPStapling(
                                                      WOLFSSL_CERT_MANAGER* cm);

/*!
    \brief Enables CRL certificate revocation.

    \return SSL_SUCCESS the function and subroutines returned with no errors.
    \return BAD_FUNC_ARG returned if the WOLFSSL structure is NULL.
    \return MEMORY_E returned if the allocation of memory failed.
    \return SSL_FAILURE returned if the InitCRL function does not return
    successfully.
    \return NOT_COMPILED_IN HAVE_CRL was not enabled during the compiling.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param options an integer that is used to determine the setting of
    crlCheckAll member of the WOLFSSL_CERT_MANAGER structure.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    if (wolfSSL_EnableCRL(ssl, WOLFSSL_CRL_CHECKALL) != SSL_SUCCESS){
	    // Failure case. SSL_SUCCESS was not returned by this function or
    a subroutine
    }
    \endcode

    \sa wolfSSL_CertManagerEnableCRL
    \sa InitCRL
*/
int wolfSSL_EnableCRL(WOLFSSL* ssl, int options);

/*!
    \brief Disables CRL certificate revocation.

    \return SSL_SUCCESS wolfSSL_CertMangerDisableCRL successfully disabled
    the crlEnabled member of the WOLFSSL_CERT_MANAGER structure.
    \return BAD_FUNC_ARG the WOLFSSL structure was NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_DisableCRL(ssl) != SSL_SUCCESS){
    	// Failure case
    }
    \endcode

    \sa wolfSSL_CertManagerDisableCRL
    \sa wolfSSL_CertManagerDisableOCSP
*/
int wolfSSL_DisableCRL(WOLFSSL* ssl);

/*!
    \brief A wrapper function that ends up calling LoadCRL to load the
    certificate for revocation checking.

    \return WOLFSSL_SUCCESS returned if the function and all of the
    subroutines executed without error.
    \return SSL_FATAL_ERROR returned if one of the subroutines does not
    return successfully.
    \return BAD_FUNC_ARG if the WOLFSSL_CERT_MANAGER or the WOLFSSL
    structure are NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param path a constant character pointer that holds the path to the
    crl file.
    \param type an integer representing the type of certificate.
    \param monitor an integer variable used to verify the monitor path if
    requested.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    const char* crlPemDir;
    …
    if(wolfSSL_LoadCRL(ssl, crlPemDir, SSL_FILETYPE_PEM, 0) != SSL_SUCCESS){
    	// Failure case. Did not return SSL_SUCCESS.
    }
    \endcode

    \sa wolfSSL_CertManagerLoadCRL
    \sa wolfSSL_CertManagerEnableCRL
    \sa LoadCRL
*/
int wolfSSL_LoadCRL(WOLFSSL* ssl, const char* path, int type, int monitor);

/*!
    \brief Sets the CRL callback in the WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS returned if the function or subroutine executes
    without error. The cbMissingCRL member of the WOLFSSL_CERT_MANAGER is set.
    \return BAD_FUNC_ARG returned if the WOLFSSL or WOLFSSL_CERT_MANAGER
    structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a function pointer to CbMissingCRL.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    void cb(const char* url) // required signature
    {
    	// Function body
    }
    …
    int crlCb = wolfSSL_SetCRL_Cb(ssl, cb);
    if(crlCb != SSL_SUCCESS){
    	// The callback was not set properly
    }
    \endcode

    \sa CbMissingCRL
    \sa wolfSSL_CertManagerSetCRL_Cb
*/
int wolfSSL_SetCRL_Cb(WOLFSSL* ssl, CbMissingCRL cb);

/*!
    \brief This function enables OCSP certificate verification.

    \return SSL_SUCCESS returned if the function and subroutines executes
    without errors.
    \return BAD_FUNC_ARG returned if an argument in this function or any
    subroutine receives an invalid argument value.
    \return MEMORY_E returned if there was an error allocating memory for
    a structure or other variable.
    \return NOT_COMPILED_IN returned if wolfSSL was not compiled with the
    HAVE_OCSP option.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param options an integer type passed to wolfSSL_CertMangerENableOCSP()
    used for settings check.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    int options; // initialize to option constant
    …
    int ret = wolfSSL_EnableOCSP(ssl, options);
    if(ret != SSL_SUCCESS){
    	// OCSP is not enabled
    }
    \endcode

    \sa wolfSSL_CertManagerEnableOCSP
*/
int wolfSSL_EnableOCSP(WOLFSSL* ssl, int options);

/*!
    \brief Disables the OCSP certificate revocation option.

    \return SSL_SUCCESS returned if the function and its subroutine return with
    no errors. The ocspEnabled member of the WOLFSSL_CERT_MANAGER structure was
    successfully set.
    \return BAD_FUNC_ARG returned if the WOLFSSL structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    if(wolfSSL_DisableOCSP(ssl) != SSL_SUCCESS){
	    // Returned with an error. Failure case in this block.
    }
    \endcode

    \sa wolfSSL_CertManagerDisableOCSP
*/
int wolfSSL_DisableOCSP(WOLFSSL*);

/*!
    \brief This function sets the ocspOverrideURL member in the
    WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS returned on successful execution of the function.
    \return BAD_FUNC_ARG returned if the WOLFSSL struct is NULL or if a
    unpermitted argument was passed to a subroutine.
    \return MEMORY_E returned if there was an error allocating memory in the
    subroutine.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param url a constant char pointer to the url that will be stored in the
    ocspOverrideURL member of the WOLFSSL_CERT_MANAGER structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    char url[URLSZ];
    ...
    if(wolfSSL_SetOCSP_OverrideURL(ssl, url)){
    	// The override url is set to the new value
    }
    \endcode

    \sa wolfSSL_CertManagerSetOCSPOverrideURL
*/
int wolfSSL_SetOCSP_OverrideURL(WOLFSSL* ssl, const char* url);

/*!
    \brief This function sets the OCSP callback in the
    WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS returned if the function executes without error.
    The ocspIOCb, ocspRespFreeCb, and ocspIOCtx members of the CM are set.
    \return BAD_FUNC_ARG returned if the WOLFSSL or WOLFSSL_CERT_MANAGER
    structures are NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param ioCb a function pointer to type CbOCSPIO.
    \param respFreeCb a function pointer to type CbOCSPRespFree which is the
    call to free the response memory.
    \param ioCbCtx a void pointer that will be held in the ocspIOCtx member
    of the CM.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    int OCSPIO_CB(void* , const char*, int , unsigned char* , int,
    unsigned char**){  // must have this signature
    // Function Body
    }
    …
    void OCSPRespFree_CB(void* , unsigned char* ){ // must have this signature
    	// function body
    }
    …
    void* ioCbCtx;
    CbOCSPRespFree CB_OCSPRespFree;

    if(wolfSSL_SetOCSP_Cb(ssl, OCSPIO_CB( pass args ), CB_OCSPRespFree,
				ioCbCtx) != SSL_SUCCESS){
	    // Callback not set
    }
    \endcode

    \sa wolfSSL_CertManagerSetOCSP_Cb
    \sa CbOCSPIO
    \sa CbOCSPRespFree
*/
int wolfSSL_SetOCSP_Cb(WOLFSSL* ssl, CbOCSPIO ioCb, CbOCSPRespFree respFreeCb,
                       void* ioCbCtx);

/*!
    \brief Enables CRL certificate verification through the CTX.

    \return SSL_SUCCESS returned if this function and it’s subroutines
    execute without errors.
    \return BAD_FUNC_ARG returned if the CTX struct is NULL or there
    was otherwise an invalid argument passed in a subroutine.
    \return MEMORY_E returned if there was an error allocating
    memory during execution of the function.
    \return SSL_FAILURE returned if the crl member of the
    WOLFSSL_CERT_MANAGER fails to initialize correctly.
    \return NOT_COMPILED_IN wolfSSL was not compiled with the HAVE_CRL option.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_CTX_EnableCRL(ssl->ctx, options) != SSL_SUCCESS){
    	// The function failed
    }
    \endcode

    \sa wolfSSL_CertManagerEnableCRL
    \sa InitCRL
    \sa wolfSSL_CTX_DisableCRL
*/
int wolfSSL_CTX_EnableCRL(WOLFSSL_CTX* ctx, int options);

/*!
    \brief This function disables CRL verification in the CTX structure.

    \return SSL_SUCCESS returned if the function executes without error.
    The crlEnabled member of the WOLFSSL_CERT_MANAGER struct is set to 0.
    \return BAD_FUNC_ARG returned if either the CTX struct or the CM
    struct has a NULL value.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_CTX_DisableCRL(ssl->ctx) != SSL_SUCCESS){
    	// Failure case.
    }
    \endcode

    \sa wolfSSL_CertManagerDisableCRL
*/
int wolfSSL_CTX_DisableCRL(WOLFSSL_CTX* ctx);

/*!
    \brief This function loads CRL into the WOLFSSL_CTX structure through
    wolfSSL_CertManagerLoadCRL().

    \return SSL_SUCCESS - returned if the function and its subroutines
    execute without error.
    \return BAD_FUNC_ARG - returned if this function or any subroutines
    are passed NULL structures.
    \return BAD_PATH_ERROR - returned if the path variable opens as NULL.
    \return MEMORY_E - returned if an allocation of memory failed.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param path the path to the certificate.
    \param type an integer variable holding the type of certificate.
    \param monitor an integer variable used to determine if the monitor
    path is requested.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    const char* path;
    …
    return wolfSSL_CTX_LoadCRL(ctx, path, SSL_FILETYPE_PEM, 0);
    \endcode

    \sa wolfSSL_CertManagerLoadCRL
    \sa LoadCRL
*/
int wolfSSL_CTX_LoadCRL(WOLFSSL_CTX* ctx, const char* path, int type, int monitor);

/*!
    \brief This function will set the callback argument to the cbMissingCRL
    member of the WOLFSSL_CERT_MANAGER structure by calling
    wolfSSL_CertManagerSetCRL_Cb.

    \return SSL_SUCCESS returned for a successful execution. The
    WOLFSSL_CERT_MANAGER structure’s member cbMssingCRL was successfully
    set to cb.
    \return BAD_FUNC_ARG returned if WOLFSSL_CTX or WOLFSSL_CERT_MANAGER
    are NULL.

    \param ctx a pointer to a WOLFSSL_CTX structure, created with
    wolfSSL_CTX_new().
    \param cb a pointer to a callback function of type CbMissingCRL.
    Signature requirement:
	void (*CbMissingCRL)(const char* url);

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    …
    void cb(const char* url) // Required signature
    {
    	// Function body
    }
    …
    if (wolfSSL_CTX_SetCRL_Cb(ctx, cb) != SSL_SUCCESS){
    	// Failure case, cb was not set correctly.
    }
    \endcode

    \sa wolfSSL_CertManagerSetCRL_Cb
    \sa CbMissingCRL
*/
int wolfSSL_CTX_SetCRL_Cb(WOLFSSL_CTX* ctx, CbMissingCRL cb);

/*!
    \brief This function sets options to configure behavior of OCSP
    functionality in wolfSSL.  The value of options if formed by or’ing
    one or more of the following options:
    WOLFSSL_OCSP_ENABLE - enable OCSP lookups	WOLFSSL_OCSP_URL_OVERRIDE -
    use the override URL instead of the URL in certificates. The override URL
    is specified using the wolfSSL_CTX_SetOCSP_OverrideURL() function. This
    function only sets the OCSP options when wolfSSL has been compiled with
    OCSP support (--enable-ocsp, #define HAVE_OCSP).

    \return SSL_SUCCESS is returned upon success.
    \return SSL_FAILURE is returned upon failure.
    \return NOT_COMPILED_IN is returned when this function has been called,
    but OCSP support was not enabled when wolfSSL was compiled.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param options value used to set the OCSP options.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    wolfSSL_CTX_OCSP_set_options(ctx, WOLFSSL_OCSP_ENABLE);
    \endcode

    \sa wolfSSL_CTX_OCSP_set_override_url
*/
int wolfSSL_CTX_EnableOCSP(WOLFSSL_CTX* ctx, int options);

/*!
    \brief This function disables OCSP certificate revocation checking by
    affecting the ocspEnabled member of the WOLFSSL_CERT_MANAGER structure.

    \return SSL_SUCCESS returned if the function executes without error.
    The ocspEnabled member of the CM has been disabled.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX structure is NULL.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(!wolfSSL_CTX_DisableOCSP(ssl->ctx)){
    	// OCSP is not disabled
    }
    \endcode

    \sa wolfSSL_DisableOCSP
    \sa wolfSSL_CertManagerDisableOCSP
*/
int wolfSSL_CTX_DisableOCSP(WOLFSSL_CTX*);

/*!
    \brief This function manually sets the URL for OCSP to use. By default,
    OCSP will use the URL found in the individual certificate unless the
    WOLFSSL_OCSP_URL_OVERRIDE option is set using the wolfSSL_CTX_EnableOCSP.

    \return SSL_SUCCESS is returned upon success.
    \return SSL_FAILURE is returned upon failure.
    \return NOT_COMPILED_IN is returned when this function has been called,
    but OCSP support was not enabled when wolfSSL was compiled.

    \param ctx pointer to the SSL context, created with wolfSSL_CTX_new().
    \param url pointer to the OCSP URL for wolfSSL to use.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    wolfSSL_CTX_OCSP_set_override_url(ctx, “custom-url-here”);
    \endcode

    \sa wolfSSL_CTX_OCSP_set_options
*/
int wolfSSL_CTX_SetOCSP_OverrideURL(WOLFSSL_CTX* ctx, const char* url);

/*!
    \brief Sets the callback for the OCSP in the WOLFSSL_CTX structure.

    \return SSL_SUCCESS returned if the function executed successfully. The
    ocspIOCb, ocspRespFreeCb, and ocspIOCtx members in the CM were
    successfully set.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX or
    WOLFSSL_CERT_MANAGER structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param ioCb a CbOCSPIO type that is a function pointer.
    \param respFreeCb a CbOCSPRespFree type that is a function pointer.
    \param ioCbCtx a void pointer that will be held in the WOLFSSL_CERT_MANAGER.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    …
    CbOCSPIO ocspIOCb;
    CbOCSPRespFree ocspRespFreeCb;
    …
    void* ioCbCtx;

    int isSetOCSP = wolfSSL_CTX_SetOCSP_Cb(ctx, ocspIOCb,
    ocspRespFreeCb, ioCbCtx);

    if(isSetOCSP != SSL_SUCCESS){
    	// The function did not return successfully.
    }
    \endcode

    \sa wolfSSL_CertManagerSetOCSP_Cb
    \sa CbOCSPIO
    \sa CbOCSPRespFree
*/
int wolfSSL_CTX_SetOCSP_Cb(WOLFSSL_CTX* ctx,
                           CbOCSPIO ioCb, CbOCSPRespFree respFreeCb,
                           void* ioCbCtx);

/*!
    \brief This function enables OCSP stapling by calling
    wolfSSL_CertManagerEnableOCSPStapling().

    \return SSL_SUCCESS returned if there were no errors and the function
    executed successfully.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX structure is NULL or
    otherwise if there was a unpermitted argument value passed to a subroutine.
    \return MEMORY_E returned if there was an issue allocating memory.
    \return SSL_FAILURE returned if the initialization of the OCSP
    structure failed.
    \return NOT_COMPILED_IN returned if wolfSSL was not compiled with
    HAVE_CERTIFICATE_STATUS_REQUEST option.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().

    _Example_
    \code
    WOLFSSL* ssl = WOLFSSL_new();
    ssl->method.version; // set to desired protocol
    ...
    if(!wolfSSL_CTX_EnableOCSPStapling(ssl->ctx)){
    	// OCSP stapling is not enabled
    }
    \endcode

    \sa wolfSSL_CertManagerEnableOCSPStapling
    \sa InitOCSP
*/
int wolfSSL_CTX_EnableOCSPStapling(WOLFSSL_CTX*);

/*!
    \ingroup CertsKeys

    \brief Normally, at the end of the SSL handshake, wolfSSL frees
    temporary arrays.  Calling this function before the handshake begins
    will prevent wolfSSL from freeing temporary arrays.  Temporary arrays
    may be needed for things such as wolfSSL_get_keys() or PSK hints.
    When the user is done with temporary arrays, either wolfSSL_FreeArrays()
    may be called to free the resources immediately, or alternatively the
    resources will be freed when the associated SSL object is freed.

    \return none No return.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl;
    ...
    wolfSSL_KeepArrays(ssl);
    \endcode

    \sa wolfSSL_FreeArrays
*/
void wolfSSL_KeepArrays(WOLFSSL*);

/*!
    \ingroup CertsKeys

    \brief Normally, at the end of the SSL handshake, wolfSSL frees temporary
    arrays.  If wolfSSL_KeepArrays() has been called before the handshake,
    wolfSSL will not free temporary arrays.  This function explicitly frees
    temporary arrays and should be called when the user is done with temporary
    arrays and does not want to wait for the SSL object to be freed to free
    these resources.

    \return none No return.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl;
    ...
    wolfSSL_FreeArrays(ssl);
    \endcode

    \sa wolfSSL_KeepArrays
*/
void wolfSSL_FreeArrays(WOLFSSL*);

/*!
    \brief This function enables the use of Server Name Indication in the SSL
    object passed in the 'ssl' parameter. It means that the SNI extension will
    be sent on ClientHello by wolfSSL client and wolfSSL server will respond
    ClientHello + SNI with either ServerHello + blank SNI or alert fatal in
    case of SNI mismatch.

    \return WOLFSSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of these
    cases: ssl is NULL, data is NULL, type is a unknown value. (see below)
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ssl pointer to a SSL object, created with wolfSSL_new().
    \param type indicates which type of server name is been passed in data.
    The known types are: enum { WOLFSSL_SNI_HOST_NAME = 0 };
    \param data pointer to the server name data.
    \param size size of the server name data.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        // ssl creation failed
    }
    ret = wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, "www.yassl.com",
        strlen("www.yassl.com"));
    if (ret != WOLFSSL_SUCCESS) {
        // sni usage failed
    }
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_UseSNI
*/
int wolfSSL_UseSNI(WOLFSSL* ssl, unsigned char type,
                                         const void* data, unsigned short size);

/*!
    \brief This function enables the use of Server Name Indication for SSL
    objects created from the SSL context passed in the 'ctx' parameter. It
    means that the SNI extension will be sent on ClientHello by wolfSSL
    clients and wolfSSL servers will respond ClientHello + SNI with either
    ServerHello + blank SNI or alert fatal in case of SNI mismatch.

    \return WOLFSSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of these
    cases: ctx is NULL, data is NULL, type is a unknown value. (see below)
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ctx pointer to a SSL context, created with wolfSSL_CTX_new().
    \param type indicates which type of server name is been passed in data.
    The known types are: enum { WOLFSSL_SNI_HOST_NAME = 0 };
    \param data pointer to the server name data.
    \param size size of the server name data.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ret = wolfSSL_CTX_UseSNI(ctx, WOLFSSL_SNI_HOST_NAME, "www.yassl.com",
        strlen("www.yassl.com"));
    if (ret != WOLFSSL_SUCCESS) {
        // sni usage failed
    }
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_UseSNI
*/
int wolfSSL_CTX_UseSNI(WOLFSSL_CTX* ctx, unsigned char type,
                                         const void* data, unsigned short size);

/*!
    \brief This function is called on the server side to configure the
    behavior of the SSL session using Server Name Indication in the SSL
    object passed in the 'ssl' parameter. The options are explained below.

    \return none No returns.

    \param ssl pointer to a SSL object, created with wolfSSL_new().
    \param type indicates which type of server name is been passed in data.
    The known types are: enum { WOLFSSL_SNI_HOST_NAME = 0 };
    \param options a bitwise semaphore with the chosen options. The available
    options are: enum { WOLFSSL_SNI_CONTINUE_ON_MISMATCH = 0x01,
    WOLFSSL_SNI_ANSWER_ON_MISMATCH = 0x02 }; Normally the server will abort the
    handshake by sending a fatal-level unrecognized_name(112) alert if the
    hostname provided by the client mismatch with the servers.
    \param WOLFSSL_SNI_CONTINUE_ON_MISMATCH With this option set, the server
    will not send a SNI response instead of aborting the session.
    \param WOLFSSL_SNI_ANSWER_ON_MISMATCH - With this option set, the server
    will send a SNI response as if the host names match instead of aborting
    the session.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        // ssl creation failed
    }
    ret = wolfSSL_UseSNI(ssl, 0, "www.yassl.com", strlen("www.yassl.com"));
    if (ret != WOLFSSL_SUCCESS) {
        // sni usage failed
    }
    wolfSSL_SNI_SetOptions(ssl, WOLFSSL_SNI_HOST_NAME,
        WOLFSSL_SNI_CONTINUE_ON_MISMATCH);
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_UseSNI
    \sa wolfSSL_CTX_SNI_SetOptions
*/
void wolfSSL_SNI_SetOptions(WOLFSSL* ssl, unsigned char type,
                                                         unsigned char options);

/*!
    \brief This function is called on the server side to configure the behavior
    of the SSL sessions using Server Name Indication for SSL objects created
    from the SSL context passed in the 'ctx' parameter. The options are
    explained below.

    \return none No returns.

    \param ctx pointer to a SSL context, created with wolfSSL_CTX_new().
    \param type indicates which type of server name is been passed in data.
    The known types are: enum { WOLFSSL_SNI_HOST_NAME = 0 };
    \param options a bitwise semaphore with the chosen options. The available
    options are: enum { WOLFSSL_SNI_CONTINUE_ON_MISMATCH = 0x01,
    WOLFSSL_SNI_ANSWER_ON_MISMATCH = 0x02 }; Normally the server will abort
    the handshake by sending a fatal-level unrecognized_name(112) alert if the
    hostname provided by the client mismatch with the servers.
    \param WOLFSSL_SNI_CONTINUE_ON_MISMATCH With this option set, the
    server will not send a SNI response instead of aborting the session.
    \param WOLFSSL_SNI_ANSWER_ON_MISMATCH With this option set, the server
    will send a SNI response as if the host names match instead of aborting
    the session.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
       // context creation failed
    }
    ret = wolfSSL_CTX_UseSNI(ctx, 0, "www.yassl.com", strlen("www.yassl.com"));
    if (ret != WOLFSSL_SUCCESS) {
        // sni usage failed
    }
    wolfSSL_CTX_SNI_SetOptions(ctx, WOLFSSL_SNI_HOST_NAME,
    WOLFSSL_SNI_CONTINUE_ON_MISMATCH);
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_UseSNI
    \sa wolfSSL_SNI_SetOptions
*/
void wolfSSL_CTX_SNI_SetOptions(WOLFSSL_CTX* ctx,
                                     unsigned char type, unsigned char options);

/*!
    \brief This function is called on the server side to retrieve the Server
    Name Indication provided by the client from the Client Hello message sent
    by the client to start a session. It does not requires context or session
    setup to retrieve the SNI.

    \return WOLFSSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of this
    cases: buffer is NULL, bufferSz <= 0, sni is NULL, inOutSz is NULL or <= 0
    \return BUFFER_ERROR is the error returned when there is a malformed
    Client Hello message.
    \return INCOMPLETE_DATA is the error returned when there is not enough
    data to complete the extraction.

    \param buffer pointer to the data provided by the client (Client Hello).
    \param bufferSz size of the Client Hello message.
    \param type indicates which type of server name is been retrieved
    from the buffer. The known types are: enum { WOLFSSL_SNI_HOST_NAME = 0 };
    \param sni pointer to where the output is going to be stored.
    \param inOutSz pointer to the output size, this value will be updated
    to MIN("SNI's length", inOutSz).

    _Example_
    \code
    unsigned char buffer[1024] = {0};
    unsigned char result[32]   = {0};
    int           length       = 32;
    // read Client Hello to buffer...
    ret = wolfSSL_SNI_GetFromBuffer(buffer, sizeof(buffer), 0, result, &length));
    if (ret != WOLFSSL_SUCCESS) {
        // sni retrieve failed
    }
    \endcode

    \sa wolfSSL_UseSNI
    \sa wolfSSL_CTX_UseSNI
    \sa wolfSSL_SNI_GetRequest
*/
int wolfSSL_SNI_GetFromBuffer(
                 const unsigned char* clientHello, unsigned int helloSz,
                 unsigned char type, unsigned char* sni, unsigned int* inOutSz);

/*!
    \ingroup IO

    \brief This function gets the status of an SNI object.

    \return value This function returns the byte value of the SNI struct’s
    status member if the SNI is not NULL.
    \return 0 if the SNI object is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param type the SNI type.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    #define AssertIntEQ(x, y) AssertInt(x, y, ==, !=)
    …
    Byte type = WOLFSSL_SNI_HOST_NAME;
    char* request = (char*)&type;
    AssertIntEQ(WOLFSSL_SNI_NO_MATCH, wolfSSL_SNI_Status(ssl, type));
    …
    \endcode

    \sa TLSX_SNI_Status
    \sa TLSX_SNI_find
    \sa TLSX_Find
*/
unsigned char wolfSSL_SNI_Status(WOLFSSL* ssl, unsigned char type);

/*!
    \brief This function is called on the server side to retrieve the
    Server Name Indication provided by the client in a SSL session.

    \return size the size of the provided SNI data.

    \param ssl pointer to a SSL object, created with wolfSSL_new().
    \param type indicates which type of server name is been retrieved in
    data. The known types are: enum { WOLFSSL_SNI_HOST_NAME = 0 };
    \param data pointer to the data provided by the client.

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        // ssl creation failed
    }
    ret = wolfSSL_UseSNI(ssl, 0, "www.yassl.com", strlen("www.yassl.com"));
    if (ret != WOLFSSL_SUCCESS) {
        // sni usage failed
    }
    if (wolfSSL_accept(ssl) == SSL_SUCCESS) {
        void *data = NULL;
        unsigned short size = wolfSSL_SNI_GetRequest(ssl, 0, &data);
    }
    \endcode

    \sa wolfSSL_UseSNI
    \sa wolfSSL_CTX_UseSNI
*/
unsigned short wolfSSL_SNI_GetRequest(WOLFSSL *ssl,
                                               unsigned char type, void** data);

/*!
    \ingroup Setup

    \brief Setup ALPN use for a wolfSSL session.

    \return WOLFSSL_SUCCESS: upon success.
    \return BAD_FUNC_ARG Returned if ssl or protocol_name_list
    is null or protocol_name_listSz is too large or options
    contain something not supported.
    \return MEMORY_ERROR Error allocating memory for protocol list.
    \return SSL_FAILURE upon failure.

    \param ssl The wolfSSL session to use.
    \param protocol_name_list List of protocol names to use.
    Comma delimited string is required.
    \param protocol_name_listSz Size of the list of protocol names.
    \param options WOLFSSL_ALPN_CONTINUE_ON_MISMATCH or
    WOLFSSL_ALPN_FAILED_ON_MISMATCH.

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    WOLFSSL_METHOD method = // Some wolfSSL method
    ctx = wolfSSL_CTX_new(method);
    ssl = wolfSSL_new(ctx);

    char alpn_list[] = {};

    if (wolfSSL_UseALPN(ssl, alpn_list, sizeof(alpn_list),
        WOLFSSL_APN_FAILED_ON_MISMATCH) != WOLFSSL_SUCCESS)
    {
       // Error setting session ticket
    }
    \endcode

    \sa TLSX_UseALPN
*/
int wolfSSL_UseALPN(WOLFSSL* ssl, char *protocol_name_list,
                                unsigned int protocol_name_listSz,
                                unsigned char options);

/*!
    \ingroup TLS

    \brief This function gets the protocol name set by the server.

    \return SSL_SUCCESS returned on successful execution where no
    errors were thrown.
    \return SSL_FATAL_ERROR returned if the extension was not found or
    if there was no protocol match with peer. There will also be an
    error thrown if there is more than one protocol name accepted.
    \return SSL_ALPN_NOT_FOUND returned signifying that no protocol
    match with peer was found.
    \return BAD_FUNC_ARG returned if there was a NULL argument passed
    into the function.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param protocol_name a pointer to a char that represents the protocol
    name and will be held in the ALPN structure.
    \param size a word16 type that represents the size of the protocol_name.

    _Example_
    \code
    WOLFSSL_CTX* ctx = WOLFSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new(ctx);
    ...
    int err;
    char* protocol_name = NULL;
    Word16 protocol_nameSz = 0;
    err = wolfSSL_ALPN_GetProtocol(ssl, &protocol_name, &protocol_nameSz);

    if(err == SSL_SUCCESS){
	    // Sent ALPN protocol
    }
    \endcode

    \sa TLSX_ALPN_GetRequest
    \sa TLSX_Find
*/
int wolfSSL_ALPN_GetProtocol(WOLFSSL* ssl, char **protocol_name,
                                         unsigned short *size);

/*!
    \ingroup TLS

    \brief This function copies the alpn_client_list data from the SSL
    object to the buffer.

    \return SSL_SUCCESS returned if the function executed without error. The
    alpn_client_list member of the SSL object has been copied to the
    list parameter.
    \return BAD_FUNC_ARG returned if the list or listSz parameter is NULL.
    \return BUFFER_ERROR returned if there will be a problem with the
    list buffer (either it’s NULL or the size is 0).
    \return MEMORY_ERROR returned if there was a problem dynamically
    allocating memory.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param list a pointer to the buffer. The data from the SSL object will
    be copied into it.
    \param listSz the buffer size.

    _Example_
    \code
    #import <wolfssl/ssl.h>

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method);
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    #ifdef HAVE_ALPN
    char* list = NULL;
    word16 listSz = 0;
    …
    err = wolfSSL_ALPN_GetPeerProtocol(ssl, &list, &listSz);

    if(err == SSL_SUCCESS){
	    List of protocols names sent by client
    }
    \endcode

    \sa wolfSSL_UseALPN
*/
int wolfSSL_ALPN_GetPeerProtocol(WOLFSSL* ssl, char **list,
                                             unsigned short *listSz);

/*!
    \brief This function is called on the client side to enable the use of
    Maximum Fragment Length in the SSL object passed in the 'ssl' parameter.
    It means that the Maximum Fragment Length extension will be sent on
    ClientHello by wolfSSL clients.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of
    these cases: ssl is NULL, mfl is out of range.
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ssl pointer to a SSL object, created with wolfSSL_new().
    \param mfl indicates witch is the Maximum Fragment Length requested for the
    session. The available options are: enum { WOLFSSL_MFL_2_9  = 1, 512 bytes
    WOLFSSL_MFL_2_10 = 2, 1024 bytes WOLFSSL_MFL_2_11 = 3, 2048 bytes
    WOLFSSL_MFL_2_12 = 4, 4096 bytes WOLFSSL_MFL_2_13 = 5, 8192
    bytes wolfSSL ONLY!!! };

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        // ssl creation failed
    }
    ret = wolfSSL_UseMaxFragment(ssl, WOLFSSL_MFL_2_11);
    if (ret != 0) {
        // max fragment usage failed
    }
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_UseMaxFragment
*/
int wolfSSL_UseMaxFragment(WOLFSSL* ssl, unsigned char mfl);

/*!
    \brief This function is called on the client side to enable the use
    of Maximum Fragment Length for SSL objects created from the SSL context
    passed in the 'ctx' parameter. It means that the Maximum Fragment Length
    extension will be sent on ClientHello by wolfSSL clients.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of
    these cases: ctx is NULL, mfl is out of range.
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ctx pointer to a SSL context, created with wolfSSL_CTX_new().
    \param mfl indicates which is the Maximum Fragment Length requested
    for the session. The available options are:
    enum { WOLFSSL_MFL_2_9  = 1 512 bytes, WOLFSSL_MFL_2_10 = 2 1024 bytes,
           WOLFSSL_MFL_2_11 = 3 2048 bytes WOLFSSL_MFL_2_12 = 4 4096 bytes,
           WOLFSSL_MFL_2_13 = 5 8192 bytes wolfSSL ONLY!!!,
           WOLFSSL_MFL_2_13 = 6  256 bytes wolfSSL ONLY!!!
    };

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ret = wolfSSL_CTX_UseMaxFragment(ctx, WOLFSSL_MFL_2_11);
    if (ret != 0) {
        // max fragment usage failed
    }
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_UseMaxFragment
*/
int wolfSSL_CTX_UseMaxFragment(WOLFSSL_CTX* ctx, unsigned char mfl);

/*!
    \brief This function is called on the client side to enable the use of
    Truncated HMAC in the SSL object passed in the 'ssl' parameter. It
    means that the Truncated HMAC extension will be sent on ClientHello
    by wolfSSL clients.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of
    these cases: ssl is NULL
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ssl pointer to a SSL object, created with wolfSSL_new()

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        // ssl creation failed
    }
    ret = wolfSSL_UseTruncatedHMAC(ssl);
    if (ret != 0) {
        // truncated HMAC usage failed
    }
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_CTX_UseMaxFragment
*/
int wolfSSL_UseTruncatedHMAC(WOLFSSL* ssl);

/*!
    \brief This function is called on the client side to enable the use of
    Truncated HMAC for SSL objects created from the SSL context passed in
    the 'ctx' parameter. It means that the Truncated HMAC extension will
    be sent on ClientHello by wolfSSL clients.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of
    these cases: ctx is NULL
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ctx pointer to a SSL context, created with wolfSSL_CTX_new().

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ret = wolfSSL_CTX_UseTruncatedHMAC(ctx);
    if (ret != 0) {
        // truncated HMAC usage failed
    }
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_UseMaxFragment
*/
int wolfSSL_CTX_UseTruncatedHMAC(WOLFSSL_CTX* ctx);

/*!
    \brief Stapling eliminates the need to contact the CA. Stapling
    lowers the cost of certificate revocation check presented in OCSP.

    \return SSL_SUCCESS returned if TLSX_UseCertificateStatusRequest
    executes without error.
    \return MEMORY_E returned if there is an error with the allocation
    of memory.
    \return BAD_FUNC_ARG returned if there is an argument that has a
    NULL or otherwise unacceptable value passed into the function.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param status_type a byte type that is passed through to
    TLSX_UseCertificateStatusRequest() and stored in the
    CertificateStatusRequest structure.
    \param options a byte type that is passed through to
    TLSX_UseCertificateStatusRequest() and stored in the
    CertificateStatusRequest structure.

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    if (wolfSSL_UseOCSPStapling(ssl, WOLFSSL_CSR2_OCSP,
    WOLFSSL_CSR2_OCSP_USE_NONCE) != SSL_SUCCESS){
	    // Failed case.
    }
    \endcode

    \sa TLSX_UseCertificateStatusRequest
    \sa wolfSSL_CTX_UseOCSPStapling
*/
int wolfSSL_UseOCSPStapling(WOLFSSL* ssl,
                              unsigned char status_type, unsigned char options);

/*!
    \brief This function requests the certificate status during the handshake.

    \return SSL_SUCCESS returned if the function and subroutines execute
    without error.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX structure is NULL or
    otherwise if a unpermitted value is passed to a subroutine.
    \return MEMORY_E returned if the function or subroutine failed to properly
    allocate memory.

    \param ctx a pointer to a WOLFSSL_CTX structure,
    created using wolfSSL_CTX_new().
    \param status_type a byte type that is passed through to
    TLSX_UseCertificateStatusRequest() and stored in the
    CertificateStatusRequest structure.
    \param options a byte type that is passed through to
    TLSX_UseCertificateStatusRequest() and stored in the
    CertificateStatusRequest structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    byte statusRequest = 0; // Initialize status request
    …
    switch(statusRequest){
    	case WOLFSSL_CSR_OCSP:
    		if(wolfSSL_CTX_UseOCSPStapling(ssl->ctx, WOLFSSL_CSR_OCSP,
    WOLF_CSR_OCSP_USE_NONCE) != SSL_SUCCESS){
    // UseCertificateStatusRequest failed
    }
    // Continue switch cases
    \endcode

    \sa wolfSSL_UseOCSPStaplingV2
    \sa wolfSSL_UseOCSPStapling
    \sa TLSX_UseCertificateStatusRequest
*/
int wolfSSL_CTX_UseOCSPStapling(WOLFSSL_CTX* ctx,
                              unsigned char status_type, unsigned char options);

/*!
    \brief The function sets the status type and options for OCSP.

    \return SSL_SUCCESS - returned if the function and subroutines
    executed without error.
    \return MEMORY_E - returned if there was an allocation of memory error.
    \return BAD_FUNC_ARG - returned if a NULL or otherwise unaccepted
    argument was passed to the function or a subroutine.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param status_type a byte type that loads the OCSP status type.
    \param options a byte type that holds the OCSP options, set in
    wolfSSL_SNI_SetOptions() and wolfSSL_CTX_SNI_SetOptions().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if (wolfSSL_UseOCSPStaplingV2(ssl, WOLFSSL_CSR2_OCSP_MULTI, 0) != SSL_SUCCESS){
    	// Did not execute properly. Failure case code block.
    }
    \endcode

    \sa TLSX_UseCertificatStatusRequestV2
    \sa wolfSSL_SNI_SetOptions
    \sa wolfSSL_CTX_SNI_SetOptions
*/
int wolfSSL_UseOCSPStaplingV2(WOLFSSL* ssl,
                              unsigned char status_type, unsigned char options);

/*!
    \brief Creates and initializes the certificate status request
    for OCSP Stapling.

    \return SSL_SUCCESS if the function and subroutines executed without error.
    \return BAD_FUNC_ARG returned if the WOLFSSL_CTX structure is NULL or if
    the side variable is not client side.
    \return MEMORY_E returned if the allocation of memory failed.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param status_type a byte type that is located in the
    CertificatStatusRequest structure and must be either WOLFSSL_CSR2_OCSP
    or WOLFSSL_CSR2_OCSP_MULTI.
    \param options a byte type that will be held in
    CertificateStatusRequestItemV2 struct.

    _Example_
    \code
    WOLFSSL_CTX* ctx  = wolfSSL_CTX_new( protocol method );
    byte status_type;
    byte options;
    ...
    if(wolfSSL_CTX_UseOCSPStaplingV2(ctx, status_type, options); != SSL_SUCCESS){
    	// Failure case.
    }
    \endcode

    \sa TLSX_UseCertificateStatusRequestV2
    \sa wc_RNG_GenerateBlock
    \sa TLSX_Push
*/
int wolfSSL_CTX_UseOCSPStaplingV2(WOLFSSL_CTX* ctx,
                              unsigned char status_type, unsigned char options);

/*!
    \brief This function is called on the client side to enable the use of
    Supported Elliptic Curves Extension in the SSL object passed in the 'ssl'
    parameter. It means that the supported curves enabled will be sent on
    ClientHello by wolfSSL clients. This function can be called more than
    one time to enable multiple curves.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of these
    cases: ssl is NULL, name is a unknown value. (see below)
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ssl pointer to a SSL object, created with wolfSSL_new().
    \param name indicates which curve will be supported for the session. The
    available options are: enum { WOLFSSL_ECC_SECP160R1 = 0x10,
    WOLFSSL_ECC_SECP192R1 = 0x13, WOLFSSL_ECC_SECP224R1 = 0x15,
    WOLFSSL_ECC_SECP256R1 = 0x17, WOLFSSL_ECC_SECP384R1 = 0x18,
    WOLFSSL_ECC_SECP521R1 = 0x19 };

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        // ssl creation failed
    }
    ret = wolfSSL_UseSupportedCurve(ssl, WOLFSSL_ECC_SECP256R1);
    if (ret != 0) {
        // Elliptic Curve Extension usage failed
    }
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_UseSupportedCurve
*/
int wolfSSL_UseSupportedCurve(WOLFSSL* ssl, word16 name);

/*!
    \brief This function is called on the client side to enable the use of
    Supported Elliptic Curves Extension for SSL objects created from the SSL
    context passed in the 'ctx' parameter. It means that the supported curves
    enabled will be sent on ClientHello by wolfSSL clients. This function can
    be called more than one time to enable multiple curves.

    \return SSL_SUCCESS upon success.
    \return BAD_FUNC_ARG is the error that will be returned in one of these
    cases: ctx is NULL, name is a unknown value. (see below)
    \return MEMORY_E is the error returned when there is not enough memory.

    \param ctx pointer to a SSL context, created with wolfSSL_CTX_new().
    \param name indicates which curve will be supported for the session.
    The available options are: enum { WOLFSSL_ECC_SECP160R1 = 0x10,
    WOLFSSL_ECC_SECP192R1 = 0x13, WOLFSSL_ECC_SECP224R1 = 0x15,
    WOLFSSL_ECC_SECP256R1 = 0x17, WOLFSSL_ECC_SECP384R1 = 0x18,
    WOLFSSL_ECC_SECP521R1 = 0x19 };

    _Example_
    \code
    int ret = 0;
    WOLFSSL_CTX* ctx = 0;
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        // context creation failed
    }
    ret = wolfSSL_CTX_UseSupportedCurve(ctx, WOLFSSL_ECC_SECP256R1);
    if (ret != 0) {
        // Elliptic Curve Extension usage failed
    }
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_UseSupportedCurve
*/
int wolfSSL_CTX_UseSupportedCurve(WOLFSSL_CTX* ctx,
                                                           word16 name);

/*!
    \ingroup IO

    \brief This function forces secure renegotiation for the supplied
    WOLFSSL structure.  This is not recommended.

    \return SSL_SUCCESS Successfully set secure renegotiation.
    \return BAD_FUNC_ARG Returns error if ssl is null.
    \return MEMORY_E Returns error if unable to allocate memory for secure
    renegotiation.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    WOLFSSL_METHOD method = // Some wolfSSL method
    ctx = wolfSSL_CTX_new(method);
    ssl = wolfSSL_new(ctx);

    if(wolfSSL_UseSecureRenegotiation(ssl) != SSL_SUCCESS)
    {
        // Error setting secure renegotiation
    }
    \endcode

    \sa TLSX_Find
    \sa TLSX_UseSecureRenegotiation
*/
int wolfSSL_UseSecureRenegotiation(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function executes a secure renegotiation handshake; this is user
    forced as wolfSSL discourages this functionality.

    \return SSL_SUCCESS returned if the function executed without error.
    \return BAD_FUNC_ARG returned if the WOLFSSL structure was NULL or otherwise
    if an unacceptable argument was passed in a subroutine.
    \return SECURE_RENEGOTIATION_E returned if there was an error with
    renegotiating the handshake.
    \return SSL_FATAL_ERROR returned if there was an error with the
    server or client configuration and the renegotiation could
    not be completed. See wolfSSL_negotiate().

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    if(wolfSSL_Rehandshake(ssl) != SSL_SUCCESS){
	    // There was an error and the rehandshake is not successful.
    }
    \endcode

    \sa wolfSSL_negotiate
    \sa wc_InitSha512
    \sa wc_InitSha384
    \sa wc_InitSha256
    \sa wc_InitSha
    \sa wc_InitMd5
*/
int wolfSSL_Rehandshake(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief Force provided WOLFSSL structure to use session ticket. The
    constant HAVE_SESSION_TICKET should be defined and the constant
    NO_WOLFSSL_CLIENT should not be defined to use this function.

    \return SSL_SUCCESS Successfully set use session ticket.
    \return BAD_FUNC_ARG Returned if ssl is null.
    \return MEMORY_E Error allocating memory for setting session ticket.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    WOLFSSL_METHOD method = // Some wolfSSL method
    ctx = wolfSSL_CTX_new(method);
    ssl = wolfSSL_new(ctx);

    if(wolfSSL_UseSessionTicket(ssl) != SSL_SUCCESS)
    {
        // Error setting session ticket
    }
    \endcode

    \sa TLSX_UseSessionTicket
*/
int wolfSSL_UseSessionTicket(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets wolfSSL context to use a session ticket.

    \return SSL_SUCCESS Function executed successfully.
    \return BAD_FUNC_ARG Returned if ctx is null.
    \return MEMORY_E Error allocating memory in internal function.

    \param ctx The WOLFSSL_CTX structure to use.

    _Example_
    \code
    wolfSSL_Init();
    WOLFSSL_CTX* ctx;
    WOLFSSL_METHOD method = // Some wolfSSL method ;
    ctx = wolfSSL_CTX_new(method);

    if(wolfSSL_CTX_UseSessionTicket(ctx) != SSL_SUCCESS)
    {
        // Error setting session ticket
    }
    \endcode

    \sa TLSX_UseSessionTicket
*/
int wolfSSL_CTX_UseSessionTicket(WOLFSSL_CTX* ctx);

/*!
    \ingroup IO

    \brief This function copies the ticket member of the Session structure to
    the buffer.

    \return SSL_SUCCESS returned if the function executed without error.
    \return BAD_FUNC_ARG returned if one of the arguments was NULL or if the
    bufSz argument was 0.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf a byte pointer representing the memory buffer.
    \param bufSz a word32 pointer representing the buffer size.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    byte* buf;
    word32 bufSz;  // Initialize with buf size
    …
    if(wolfSSL_get_SessionTicket(ssl, buf, bufSz) <= 0){
	    // Nothing was written to the buffer
    } else {
	    // the buffer holds the content from ssl->session->ticket
    }
    \endcode

    \sa wolfSSL_UseSessionTicket
    \sa wolfSSL_set_SessionTicket
*/
int wolfSSL_get_SessionTicket(WOLFSSL* ssl, unsigned char* buf, word32* bufSz);

/*!
    \ingroup IO

    \brief This function sets the ticket member of the WOLFSSL_SESSION
    structure within the WOLFSSL struct. The buffer passed into the function
    is copied to memory.

    \return SSL_SUCCESS returned on successful execution of the function.
    The function returned without errors.
    \return BAD_FUNC_ARG returned if the WOLFSSL structure is NULL. This will
    also be thrown if the buf argument is NULL but the bufSz argument
    is not zero.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param buf a byte pointer that gets loaded into the ticket member
    of the session structure.
    \param bufSz a word32 type that represents the size of the buffer.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    byte* buffer; // File to load
    word32 bufSz;
    ...
    if(wolfSSL_KeepArrays(ssl, buffer, bufSz) != SSL_SUCCESS){
    	// There was an error loading the buffer to memory.
    }
    \endcode

    \sa wolfSSL_set_SessionTicket_cb
*/
int wolfSSL_set_SessionTicket(WOLFSSL* ssl, const unsigned char* buf,
                              word32 bufSz);

/*!
    \brief This function sets the session ticket callback. The type
    CallbackSessionTicket is a function pointer with the signature of:
    int (*CallbackSessionTicket)(WOLFSSL*, const unsigned char*, int, void*)

    \return SSL_SUCCESS returned if the function executed without error.
    \return BAD_FUNC_ARG returned if the WOLFSSL structure is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a function pointer to the type CallbackSessionTicket.
    \param ctx a void pointer to the session_ticket_ctx member of the
    WOLFSSL structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    int sessionTicketCB(WOLFSSL* ssl, const unsigned char* ticket, int ticketSz,
				void* ctx){ … }
    wolfSSL_set_SessionTicket_cb(ssl, sessionTicketCB, (void*)”initial session”);
    \endcode

    \sa wolfSSL_get_SessionTicket
    \sa CallbackSessionTicket
    \sa sessionTicketCB
*/
int wolfSSL_set_SessionTicket_cb(WOLFSSL* ssl,
                                 CallbackSessionTicket cb, void* ctx);

/*!
    \brief This function sends a session ticket to the client after a TLS v1.3
    handhsake has been established.

    \return WOLFSSL_SUCCESS returned if a new session ticket was sent.
    \return BAD_FUNC_ARG returned if WOLFSSL structure is NULL, or not using
    TLS v1.3.
    \return SIDE_ERROR returned if not a server.
    \return NOT_READY_ERROR returned if the handshake has not completed.
    \return WOLFSSL_FATAL_ERROR returned if creating or sending message fails.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    ret = wolfSSL_send_SessionTicket(ssl);
    if (ret != WOLFSSL_SUCCESS) {
        // New session ticket not sent.
    }
    \endcode

    \sa wolfSSL_get_SessionTicket
    \sa CallbackSessionTicket
    \sa sessionTicketCB
 */
int wolfSSL_send_SessionTicket(WOLFSSL* ssl);

/*!
    \brief This function sets the session ticket key encrypt callback function
    for a server to support session tickets as specified in RFC 5077.

    \return SSL_SUCCESS will be returned upon successfully setting the session.
    \return BAD_FUNC_ARG will be returned on failure. This is caused by passing
    invalid arguments to the function.

    \param ctx pointer to the WOLFSSL_CTX object, created with wolfSSL_CTX_new().
    \param cb user callback function to encrypt/decrypt session tickets
    \param ssl(Callback) pointer to the WOLFSSL object, created with
    wolfSSL_new()
    \param key_name(Callback) unique key name for this ticket context, should
    be randomly generated
    \param iv(Callback) unique IV for this ticket, up to 128 bits, should
    be randomly generated
    \param mac(Callback) up to 256 bit mac for this ticket
    \param enc(Callback) if this encrypt parameter is true the user should fill
    in key_name, iv, mac, and encrypt the ticket in-place of length inLen and
    set the resulting output length in *outLen.  Returning WOLFSSL_TICKET_RET_OK
    tells wolfSSL that the encryption was successful. If this encrypt parameter
    is false, the user should perform a decrypt of the ticket in-place of length
    inLen using key_name, iv, and mac. The resulting decrypt length should be
    set in *outLen. Returning WOLFSSL_TICKET_RET_OK tells wolfSSL to proceed
    using the decrypted ticket. Returning WOLFSSL_TICKET_RET_CREATE tells
    wolfSSL to use the decrypted ticket but also to generate a new one to
    send to the client, helpful if recently rolled keys and don’t want to
    force a full handshake.  Returning WOLFSSL_TICKET_RET_REJECT tells
    wolfSSL to reject this ticket, perform a full handshake, and create
    a new standard session ID for normal session resumption. Returning
    WOLFSSL_TICKET_RET_FATAL tells wolfSSL to end the connection
    attempt with a fatal error.
    \param ticket(Callback) the input/output buffer for the encrypted ticket.
    See the enc parameter
    \param inLen(Callback) the input length of the ticket parameter
    \param outLen(Callback) the resulting output length of the ticket parameter.
    When entering the callback outLen will indicate the maximum size available
    in the ticket buffer.
    \param userCtx(Callback) the user context set with
    wolfSSL_CTX_set_TicketEncCtx()

    _Example_
    \code
    See wolfssl/test.h myTicketEncCb() used by the example
    server and example echoserver.
    \endcode

    \sa wolfSSL_CTX_set_TicketHint
    \sa wolfSSL_CTX_set_TicketEncCtx
*/
int wolfSSL_CTX_set_TicketEncCb(WOLFSSL_CTX* ctx,
                                            SessionTicketEncCb);

/*!
    \brief This function sets the session ticket hint relayed to the client.
    For server side use.

    \return SSL_SUCCESS will be returned upon successfully setting the session.
    \return BAD_FUNC_ARG will be returned on failure.  This is caused by passing
    invalid arguments to the function.

    \param ctx pointer to the WOLFSSL_CTX object, created with wolfSSL_CTX_new().
    \param hint number of seconds the ticket might be valid for.  Hint to client.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_set_TicketEncCb
*/
int wolfSSL_CTX_set_TicketHint(WOLFSSL_CTX* ctx, int);

/*!
    \brief This function sets the session ticket encrypt user context for the
    callback.  For server side use.

    \return SSL_SUCCESS will be returned upon successfully setting the session.
    \return BAD_FUNC_ARG will be returned on failure.  This is caused by
    passing invalid arguments to the function.

    \param ctx pointer to the WOLFSSL_CTX object, created
    with wolfSSL_CTX_new().
    \param userCtx the user context for the callback

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_set_TicketEncCb
*/
int wolfSSL_CTX_set_TicketEncCtx(WOLFSSL_CTX* ctx, void*);

/*!
    \brief This function gets the session ticket encrypt user context for the
    callback.  For server side use.

    \return userCtx will be returned upon successfully getting the session.
    \return NULL will be returned on failure.  This is caused by
    passing invalid arguments to the function, or when the user context has
    not been set.

    \param ctx pointer to the WOLFSSL_CTX object, created
    with wolfSSL_CTX_new().

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_CTX_set_TicketEncCtx
*/
void* wolfSSL_CTX_get_TicketEncCtx(WOLFSSL_CTX* ctx);

/*!
    \brief This function sets the handshake done callback. The hsDoneCb and
    hsDoneCtx members of the WOLFSSL structure are set in this function.

    \return SSL_SUCCESS returned if the function executed without an error.
    The hsDoneCb and hsDoneCtx members of the WOLFSSL struct are set.
    \return BAD_FUNC_ARG returned if the WOLFSSL struct is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param cb a function pointer of type HandShakeDoneCb with the signature of
    the form: int (*HandShakeDoneCb)(WOLFSSL*, void*);
    \param user_ctx a void pointer to the user registered context.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    …
    int myHsDoneCb(WOLFSSL* ssl, void* user_ctx){
        // callback function
    }
    …
    wolfSSL_SetHsDoneCb(ssl, myHsDoneCb, NULL);
    \endcode

    \sa HandShakeDoneCb
*/
int wolfSSL_SetHsDoneCb(WOLFSSL* ssl, HandShakeDoneCb cb, void* user_ctx);

/*!
    \ingroup IO

    \brief This function prints the statistics from the session.

    \return SSL_SUCCESS returned if the function and subroutines return without
    error. The session stats have been successfully retrieved and printed.
    \return BAD_FUNC_ARG returned if the subroutine wolfSSL_get_session_stats()
    was passed an unacceptable argument.
    \return BAD_MUTEX_E returned if there was a mutex error in the subroutine.

    \param none No parameters.

    _Example_
    \code
    // You will need to have a session object to retrieve stats from.
    if(wolfSSL_PrintSessionStats(void) != SSL_SUCCESS	){
        // Did not print session stats
    }

    \endcode

    \sa wolfSSL_get_session_stats
*/
int wolfSSL_PrintSessionStats(void);

/*!
    \ingroup IO

    \brief This function gets the statistics for the session.

    \return SSL_SUCCESS returned if the function and subroutines return without
    error. The session stats have been successfully retrieved and printed.
    \return BAD_FUNC_ARG returned if the subroutine wolfSSL_get_session_stats()
    was passed an unacceptable argument.
    \return BAD_MUTEX_E returned if there was a mutex error in the subroutine.

    \param active a word32 pointer representing the total current sessions.
    \param total a word32 pointer representing the total sessions.
    \param peak a word32 pointer representing the peak sessions.
    \param maxSessions a word32 pointer representing the maximum sessions.

    _Example_
    \code
    int wolfSSL_PrintSessionStats(void){
    …
    ret = wolfSSL_get_session_stats(&totalSessionsNow,
    &totalSessionsSeen, &peak, &maxSessions);
    …
    return ret;
    \endcode

    \sa wolfSSL_PrintSessionStats
*/
int wolfSSL_get_session_stats(unsigned int* active,
                                          unsigned int* total,
                                          unsigned int* peak,
                                          unsigned int* maxSessions);

/*!
    \ingroup TLS

    \brief This function copies the values of cr and sr then passes through to
    wc_PRF (pseudo random function) and returns that value.

    \return 0 on success
    \return BUFFER_E returned if there will be an error
    with the size of the buffer.
    \return MEMORY_E returned if a subroutine failed
    to allocate dynamic memory.

    \param ms the master secret held in the Arrays structure.
    \param msLen the length of the master secret.
    \param pms the pre-master secret held in the Arrays structure.
    \param pmsLen the length of the pre-master secret.
    \param cr the client random.
    \param sr the server random.
    \param tls1_2 signifies that the version is at least tls version 1.2.
    \param hash_type signifies the hash type.

    _Example_
    \code
    WOLFSSL* ssl;

    called in MakeTlsMasterSecret and retrieves the necessary
    information as follows:

    int MakeTlsMasterSecret(WOLFSSL* ssl){
	int ret;
	ret = wolfSSL_makeTlsMasterSecret(ssl->arrays->masterSecret, SECRET_LEN,
    ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz,
    ssl->arrays->clientRandom, ssl->arrays->serverRandom,
    IsAtLeastTLSv1_2(ssl), ssl->specs.mac_algorithm);
    …
    return ret;

    }
    \endcode

    \sa wc_PRF
    \sa MakeTlsMasterSecret
*/

int wolfSSL_MakeTlsMasterSecret(unsigned char* ms, word32 msLen,
                               const unsigned char* pms, word32 pmsLen,
                               const unsigned char* cr, const unsigned char* sr,
                               int tls1_2, int hash_type);

/*!
    \ingroup CertsKeys

    \brief An external facing wrapper to derive TLS Keys.

    \return 0 returned on success.
    \return BUFFER_E returned if the sum of labLen and
    seedLen (computes total size) exceeds the maximum size.
    \return MEMORY_E returned if the allocation of memory failed.

    \param key_data a byte pointer that is allocateded in DeriveTlsKeys
    and passed through to wc_PRF to hold the final hash.
    \param keyLen a word32 type that is derived in DeriveTlsKeys
    from the WOLFSSL structure’s specs member.
    \param ms a constant pointer type holding the master secret
    held in the arrays structure within the WOLFSSL structure.
    \param msLen a word32 type that holds the length of the
    master secret in an enumerated define, SECRET_LEN.
    \param sr a constant byte pointer to the serverRandom
    member of the arrays structure within the WOLFSSL structure.
    \param cr a constant byte pointer to the clientRandom
    member of the arrays structure within the WOLFSSL structure.
    \param tls1_2 an integer type returned from IsAtLeastTLSv1_2().
    \param hash_type an integer type held in the WOLFSSL structure.

    _Example_
    \code
    int DeriveTlsKeys(WOLFSSL* ssl){
    int ret;
    …
    ret = wolfSSL_DeriveTlsKeys(key_data, length, ssl->arrays->masterSecret,
    SECRET_LEN, ssl->arrays->clientRandom,
    IsAtLeastTLSv1_2(ssl), ssl->specs.mac_algorithm);
    …
    }
    \endcode

    \sa wc_PRF
    \sa DeriveTlsKeys
    \sa IsAtLeastTLSv1_2
*/

int wolfSSL_DeriveTlsKeys(unsigned char* key_data, word32 keyLen,
                               const unsigned char* ms, word32 msLen,
                               const unsigned char* sr, const unsigned char* cr,
                               int tls1_2, int hash_type);

/*!
    \brief wolfSSL_connect_ex() is an extension that allows
    a HandShake Callback to be set. This can be useful in
    embedded systems for debugging support when a debugger isn’t
    available and sniffing is impractical. The HandShake Callback
    will be called whether or not a handshake error occurred.
    No dynamic memory is used since the maximum number of SSL
    packets is known.  Packet names can be accessed through packetNames[].
    The connect extension also allows a Timeout Callback to be set along
    with a timeout value.  This is useful if the user doesn’t want
    to wait for the TCP stack to timeout. This extension can be called
    with either, both, or neither callbacks.

    \return SSL_SUCCESS upon success.
    \return GETTIME_ERROR will be returned if gettimeofday()
    encountered an error.
    \return SETITIMER_ERROR will be returned if setitimer()
    encountered an error.
    \return SIGACT_ERROR will be returned if sigaction() encountered an error.
    \return SSL_FATAL_ERROR will be returned if the underlying SSL_connect()
    call encountered an error.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_accept_ex
*/
int wolfSSL_connect_ex(WOLFSSL* ssl, HandShakeCallBack hsCb,
                       TimeoutCallBack toCb, WOLFSSL_TIMEVAL timeout);

/*!
    \brief wolfSSL_accept_ex() is an extension that allows a HandShake Callback
    to be set. This can be useful in embedded systems for debugging support
    when a debugger isn’t available and sniffing is impractical. The HandShake
    Callback will be called whether or not a handshake error occurred.
    No dynamic memory is used since the maximum number of SSL packets is known.
    Packet names can be accessed through packetNames[]. The connect extension
    also allows a Timeout Callback to be set along with a timeout value.
    This is useful if the user doesn’t want to wait for the TCP stack to timeout.
    This extension can be called with either, both, or neither callbacks.

    \return SSL_SUCCESS upon success.
    \return GETTIME_ERROR will be returned if gettimeofday()
    encountered an error.
    \return SETITIMER_ERROR will be returned if setitimer()
    encountered an error.
    \return SIGACT_ERROR will be returned if sigaction() encountered an error.
    \return SSL_FATAL_ERROR will be returned if the underlying
    SSL_accept() call encountered an error.

    \param none No parameters.

    _Example_
    \code
    none
    \endcode

    \sa wolfSSL_connect_ex
*/
int wolfSSL_accept_ex(WOLFSSL* ssl, HandShakeCallBacki hsCb,
                      TimeoutCallBack toCb, WOLFSSL_TIMEVAL timeout);

/*!
    \ingroup IO

    \brief This is used to set the internal file pointer for a BIO.

    \return SSL_SUCCESS On successfully setting file pointer.
    \return SSL_FAILURE If an error case was encountered.

    \param bio WOLFSSL_BIO structure to set pair.
    \param fp file pointer to set in bio.
    \param c close file behavior flag.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    XFILE fp;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
    ret  = wolfSSL_BIO_set_fp(bio, fp, BIO_CLOSE);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_get_fp
    \sa wolfSSL_BIO_free
*/
long wolfSSL_BIO_set_fp(WOLFSSL_BIO *bio, XFILE fp, int c);

/*!
    \ingroup IO

    \brief This is used to get the internal file pointer for a BIO.

    \return SSL_SUCCESS On successfully getting file pointer.
    \return SSL_FAILURE If an error case was encountered.

    \param bio WOLFSSL_BIO structure to set pair.
    \param fp file pointer to set in bio.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    XFILE fp;
    int ret;
    bio  = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
    ret  = wolfSSL_BIO_get_fp(bio, &fp);
    // check ret value
    \endcode

    \sa wolfSSL_BIO_new
    \sa wolfSSL_BIO_s_mem
    \sa wolfSSL_BIO_set_fp
    \sa wolfSSL_BIO_free
*/
long wolfSSL_BIO_get_fp(WOLFSSL_BIO *bio, XFILE* fp);

/*!
    \ingroup Setup

    \brief This function checks that the private key is a match
    with the certificate being used.

    \return SSL_SUCCESS On successfully match.
    \return SSL_FAILURE If an error case was encountered.
    \return <0 All error cases other than SSL_FAILURE are negative values.

    \param ssl WOLFSSL structure to check.

    _Example_
    \code
    WOLFSSL* ssl;
    int ret;
    // create and set up ssl
    ret  = wolfSSL_check_private_key(ssl);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int wolfSSL_check_private_key(const WOLFSSL* ssl);

/*!
    \ingroup CertsKeys

    \brief This function looks for and returns the extension index
    matching the passed in NID value.

    \return >= 0 If successful the extension index is returned.
    \return -1 If extension is not found or error is encountered.

    \param x509 certificate to get parse through for extension.
    \param nid extension OID to be found.
    \param lastPos start search from extension after lastPos.
                   Set to -1 initially.

    _Example_
    \code
    const WOLFSSL_X509* x509;
    int lastPos = -1;
    int idx;

    idx = wolfSSL_X509_get_ext_by_NID(x509, NID_basic_constraints, lastPos);
    \endcode

*/
int wolfSSL_X509_get_ext_by_NID(const WOLFSSL_X509* x509,
                                             int nid, int lastPos);

/*!
    \ingroup CertsKeys

    \brief This function looks for and returns the extension
    matching the passed in NID value.

    \return pointer If successful a STACK_OF(WOLFSSL_ASN1_OBJECT)
    pointer is returned.
    \return NULL If extension is not found or error is encountered.

    \param x509 certificate to get parse through for extension.
    \param nid extension OID to be found.
    \param c if not NULL is set to -2 for multiple extensions found -1
    if not found, 0 if found and not critical and 1 if found and critical.
    \param idx if NULL return first extension matched otherwise if not
    stored in x509 start at idx.

    _Example_
    \code
    const WOLFSSL_X509* x509;
    int c;
    int idx = 0;
    STACK_OF(WOLFSSL_ASN1_OBJECT)* sk;

    sk = wolfSSL_X509_get_ext_d2i(x509, NID_basic_constraints, &c, &idx);
    //check sk for NULL and then use it. sk needs freed after done.
    \endcode

    \sa wolfSSL_sk_ASN1_OBJECT_free
*/
void* wolfSSL_X509_get_ext_d2i(const WOLFSSL_X509* x509,
                                                     int nid, int* c, int* idx);

/*!
    \ingroup CertsKeys

    \brief This function returns the hash of the DER certificate.

    \return SSL_SUCCESS On successfully creating a hash.
    \return SSL_FAILURE Returned on bad input or unsuccessful hash.

    \param x509 certificate to get the hash of.
    \param digest the hash algorithm to use.
    \param buf buffer to hold hash.
    \param len length of buffer.

    _Example_
    \code
    WOLFSSL_X509* x509;
    unsigned char buffer[64];
    unsigned int bufferSz;
    int ret;

    ret = wolfSSL_X509_digest(x509, wolfSSL_EVP_sha256(), buffer, &bufferSz);
    //check ret value
    \endcode

    \sa none
*/
int wolfSSL_X509_digest(const WOLFSSL_X509* x509,
        const WOLFSSL_EVP_MD* digest, unsigned char* buf, unsigned int* len);

/*!
    \ingroup Setup

    \brief his is used to set the certificate for WOLFSSL structure to use
    during a handshake.

    \return SSL_SUCCESS On successful setting argument.
    \return SSL_FAILURE If a NULL argument passed in.

    \param ssl WOLFSSL structure to set certificate in.
    \param x509 certificate to use.

    _Example_
    \code WOLFSSL* ssl;
    WOLFSSL_X509* x509
    int ret;
    // create ssl object and x509
    ret  = wolfSSL_use_certificate(ssl, x509);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int wolfSSL_use_certificate(WOLFSSL* ssl, WOLFSSL_X509* x509);

/*!
    \ingroup Setup

    \brief This is used to set the certificate for WOLFSSL structure
    to use during a handshake. A DER formatted buffer is expected.

    \return SSL_SUCCESS On successful setting argument.
    \return SSL_FAILURE If a NULL argument passed in.

    \param ssl WOLFSSL structure to set certificate in.
    \param der DER certificate to use.
    \param derSz size of the DER buffer passed in.

    _Example_
    \code
    WOLFSSL* ssl;
    unsigned char* der;
    int derSz;
    int ret;
    // create ssl object and set DER variables
    ret  = wolfSSL_use_certificate_ASN1(ssl, der, derSz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int wolfSSL_use_certificate_ASN1(WOLFSSL* ssl, unsigned char* der,
                                                                     int derSz);

/*!
    \ingroup CertsKeys

    \brief This is used to set the private key for the WOLFSSL structure.

    \return SSL_SUCCESS On successful setting argument.
    \return SSL_FAILURE If a NULL ssl passed in. All error
    cases will be negative values.

    \param ssl WOLFSSL structure to set argument in.
    \param pkey private key to use.

    _Example_
    \code
    WOLFSSL* ssl;
    WOLFSSL_EVP_PKEY* pkey;
    int ret;
    // create ssl object and set up private key
    ret  = wolfSSL_use_PrivateKey(ssl, pkey);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int wolfSSL_use_PrivateKey(WOLFSSL* ssl, WOLFSSL_EVP_PKEY* pkey);

/*!
    \ingroup CertsKeys

    \brief This is used to set the private key for the WOLFSSL
    structure. A DER formatted key buffer is expected.

    \return SSL_SUCCESS On successful setting parsing and
    setting the private key.
    \return SSL_FAILURE If an NULL ssl passed in. All error cases
    will be negative values.

    \param pri type of private key.
    \param ssl WOLFSSL structure to set argument in.
    \param der buffer holding DER key.
    \param derSz size of der buffer.

    _Example_
    \code
    WOLFSSL* ssl;
    unsigned char* pkey;
    long pkeySz;
    int ret;
    // create ssl object and set up private key
    ret  = wolfSSL_use_PrivateKey_ASN1(1, ssl, pkey, pkeySz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
    \sa wolfSSL_use_PrivateKey
*/
int wolfSSL_use_PrivateKey_ASN1(int pri, WOLFSSL* ssl,
                                            unsigned char* der, long derSz);

/*!
    \ingroup CertsKeys

    \brief This is used to set the private key for the WOLFSSL
    structure. A DER formatted RSA key buffer is expected.

    \return SSL_SUCCESS On successful setting parsing and setting
    the private key.
    \return SSL_FAILURE If an NULL ssl passed in. All error cases
    will be negative values.

    \param ssl WOLFSSL structure to set argument in.
    \param der buffer holding DER key.
    \param derSz size of der buffer.

    _Example_
    \code
    WOLFSSL* ssl;
    unsigned char* pkey;
    long pkeySz;
    int ret;
    // create ssl object and set up RSA private key
    ret  = wolfSSL_use_RSAPrivateKey_ASN1(ssl, pkey, pkeySz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
    \sa wolfSSL_use_PrivateKey
*/
int wolfSSL_use_RSAPrivateKey_ASN1(WOLFSSL* ssl, unsigned char* der,
                                                                long derSz);

/*!
    \ingroup CertsKeys

    \brief This function duplicates the parameters in dsa to a
    newly created WOLFSSL_DH structure.

    \return WOLFSSL_DH If duplicated returns WOLFSSL_DH structure
    \return NULL upon failure

    \param dsa WOLFSSL_DSA structure to duplicate.

    _Example_
    \code
    WOLFSSL_DH* dh;
    WOLFSSL_DSA* dsa;
    // set up dsa
    dh = wolfSSL_DSA_dup_DH(dsa);

    // check dh is not null
    \endcode

    \sa none
*/
WOLFSSL_DH *wolfSSL_DSA_dup_DH(const WOLFSSL_DSA *r);

/*!
    \ingroup Setup

    \brief This is used to get the master key after completing a handshake.

    \return >0 On successfully getting data returns a value greater than 0
    \return 0  If no random data buffer or an error state returns 0
    \return max If outSz passed in is 0 then the maximum buffer
    size needed is returned

    \param ses WOLFSSL_SESSION structure to get master secret buffer from.
    \param out buffer to hold data.
    \param outSz size of out buffer passed in. (if 0 function will
    return max buffer size needed)

    _Example_
    \code
    WOLFSSL_SESSION ssl;
    unsigned char* buffer;
    size_t bufferSz;
    size_t ret;
    // complete handshake and get session structure
    bufferSz  = wolfSSL_SESSION_get_master_secret(ses, NULL, 0);
    buffer = malloc(bufferSz);
    ret  = wolfSSL_SESSION_get_master_secret(ses, buffer, bufferSz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int wolfSSL_SESSION_get_master_key(const WOLFSSL_SESSION* ses,
        unsigned char* out, int outSz);

/*!
    \ingroup Setup

    \brief This is used to get the master secret key length.

    \return size Returns master secret key size.

    \param ses WOLFSSL_SESSION structure to get master secret buffer from.

    _Example_
    \code
    WOLFSSL_SESSION ssl;
    unsigned char* buffer;
    size_t bufferSz;
    size_t ret;
    // complete handshake and get session structure
    bufferSz  = wolfSSL_SESSION_get_master_secret_length(ses);
    buffer = malloc(bufferSz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
int wolfSSL_SESSION_get_master_key_length(const WOLFSSL_SESSION* ses);

/*!
    \ingroup Setup

    \brief This is a setter function for the WOLFSSL_X509_STORE
    structure in ctx.

    \return none No return.

    \param ctx pointer to the WOLFSSL_CTX structure for setting
    cert store pointer.
    \param str pointer to the WOLFSSL_X509_STORE to set in ctx.

    _Example_
    \code
    WOLFSSL_CTX ctx;
    WOLFSSL_X509_STORE* st;
    // setup ctx and st
    st = wolfSSL_CTX_set_cert_store(ctx, st);
    //use st
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
*/
void wolfSSL_CTX_set_cert_store(WOLFSSL_CTX* ctx,
                                                       WOLFSSL_X509_STORE* str);

/*!
    \ingroup CertsKeys

    \brief This function get the DER buffer from bio and converts it
    to a WOLFSSL_X509 structure.

    \return pointer returns a WOLFSSL_X509 structure pointer on success.
    \return Null returns NULL on failure

    \param bio pointer to the WOLFSSL_BIO structure that has the DER
    certificate buffer.
    \param x509 pointer that get set to new WOLFSSL_X509 structure created.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    WOLFSSL_X509* x509;
    // load DER into bio
    x509 = wolfSSL_d2i_X509_bio(bio, NULL);
    Or
    wolfSSL_d2i_X509_bio(bio, &x509);
    // use x509 returned (check for NULL)
    \endcode

    \sa none
*/
WOLFSSL_X509* wolfSSL_d2i_X509_bio(WOLFSSL_BIO* bio, WOLFSSL_X509** x509);

/*!
    \ingroup Setup

    \brief This is a getter function for the WOLFSSL_X509_STORE
    structure in ctx.

    \return WOLFSSL_X509_STORE* On successfully getting the pointer.
    \return NULL Returned if NULL arguments are passed in.

    \param ctx pointer to the WOLFSSL_CTX structure for getting cert
    store pointer.

    _Example_
    \code
    WOLFSSL_CTX ctx;
    WOLFSSL_X509_STORE* st;
    // setup ctx
    st = wolfSSL_CTX_get_cert_store(ctx);
    //use st
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
    \sa wolfSSL_CTX_set_cert_store
*/
WOLFSSL_X509_STORE* wolfSSL_CTX_get_cert_store(WOLFSSL_CTX* ctx);

/*!
    \ingroup IO

    \brief Gets the number of pending bytes to read. If BIO type is BIO_BIO
    then is the number to read from pair. If BIO contains an SSL object then
    is pending data from SSL object (wolfSSL_pending(ssl)). If is BIO_MEMORY
    type then returns the size of memory buffer.

    \return >=0 number of pending bytes.

    \param bio pointer to the WOLFSSL_BIO structure that has already
    been created.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    int pending;
    bio = wolfSSL_BIO_new();
    …
    pending = wolfSSL_BIO_ctrl_pending(bio);
    \endcode

    \sa wolfSSL_BIO_make_bio_pair
    \sa wolfSSL_BIO_new
*/
size_t wolfSSL_BIO_ctrl_pending(WOLFSSL_BIO *b);

/*!
    \ingroup Setup

    \brief This is used to get the random data sent by the server
    during the handshake.

    \return >0 On successfully getting data returns a value greater than 0
    \return 0  If no random data buffer or an error state returns 0
    \return max If outSz passed in is 0 then the maximum buffer size
    needed is returned

    \param ssl WOLFSSL structure to get clients random data buffer from.
    \param out buffer to hold random data.
    \param outSz size of out buffer passed in. (if 0 function will return max
    buffer size needed)

    _Example_
    \code
    WOLFSSL ssl;
    unsigned char* buffer;
    size_t bufferSz;
    size_t ret;
    bufferSz  = wolfSSL_get_server_random(ssl, NULL, 0);
    buffer = malloc(bufferSz);
    ret  = wolfSSL_get_server_random(ssl, buffer, bufferSz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
size_t wolfSSL_get_server_random(const WOLFSSL *ssl,
                                             unsigned char *out, size_t outlen);

/*!
    \ingroup Setup

    \brief This is used to get the random data sent by the client during
    the handshake.

    \return >0 On successfully getting data returns a value greater than 0
    \return 0 If no random data buffer or an error state returns 0
    \return max If outSz passed in is 0 then the maximum buffer size needed
    is returned

    \param ssl WOLFSSL structure to get clients random data buffer from.
    \param out buffer to hold random data.
    \param outSz size of out buffer passed in. (if 0 function will return max
    buffer size needed)

    _Example_
    \code
    WOLFSSL ssl;
    unsigned char* buffer;
    size_t bufferSz;
    size_t ret;
    bufferSz  = wolfSSL_get_client_random(ssl, NULL, 0);
    buffer = malloc(bufferSz);
    ret  = wolfSSL_get_client_random(ssl, buffer, bufferSz);
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
size_t wolfSSL_get_client_random(const WOLFSSL* ssl,
                                              unsigned char* out, size_t outSz);

/*!
    \ingroup Setup

    \brief This is a getter function for the password callback set in ctx.

    \return func On success returns the callback function.
    \return NULL If ctx is NULL then NULL is returned.

    \param ctx WOLFSSL_CTX structure to get call back from.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    wc_pem_password_cb cb;
    // setup ctx
    cb = wolfSSL_CTX_get_default_passwd_cb(ctx);
    //use cb
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
*/
wc_pem_password_cb* wolfSSL_CTX_get_default_passwd_cb(WOLFSSL_CTX*
                                                                  ctx);

/*!
    \ingroup Setup

    \brief This is a getter function for the password callback user
    data set in ctx.

    \return pointer On success returns the user data pointer.
    \return NULL If ctx is NULL then NULL is returned.

    \param ctx WOLFSSL_CTX structure to get user data from.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    void* data;
    // setup ctx
    data = wolfSSL_CTX_get_default_passwd_cb(ctx);
    //use data
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_CTX_free
*/
void *wolfSSL_CTX_get_default_passwd_cb_userdata(WOLFSSL_CTX *ctx);

/*!
    \ingroup CertsKeys

    \brief This function behaves the same as wolfSSL_PEM_read_bio_X509.
    AUX signifies containing extra information such as trusted/rejected use
    cases and friendly name for human readability.

    \return WOLFSSL_X509 on successfully parsing the PEM buffer a WOLFSSL_X509
    structure is returned.
    \return Null if failed to parse PEM buffer.

    \param bp WOLFSSL_BIO structure to get PEM buffer from.
    \param x if setting WOLFSSL_X509 by function side effect.
    \param cb password callback.
    \param u NULL terminated user password.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    WOLFSSL_X509* x509;
    // setup bio
    X509 = wolfSSL_PEM_read_bio_X509_AUX(bio, NULL, NULL, NULL);
    //check x509 is not null and then use it
    \endcode

    \sa wolfSSL_PEM_read_bio_X509
*/
WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509_AUX
        (WOLFSSL_BIO *bp, WOLFSSL_X509 **x, wc_pem_password_cb *cb, void *u);

/*!
    \ingroup CertsKeys

    \brief Initializes the WOLFSSL_CTX structure’s dh member with the
    Diffie-Hellman parameters.

    \return SSL_SUCCESS returned if the function executed successfully.
    \return BAD_FUNC_ARG returned if the ctx or dh structures are NULL.
    \return SSL_FATAL_ERROR returned if there was an error setting a
    structure value.
    \return MEMORY_E returned if their was a failure to allocate memory.

    \param ctx a pointer to a WOLFSSL_CTX structure, created using
    wolfSSL_CTX_new().
    \param dh a pointer to a WOLFSSL_DH structure.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL_DH* dh;
    …
    return wolfSSL_CTX_set_tmp_dh(ctx, dh);
    \endcode

    \sa wolfSSL_BN_bn2bin
*/
long wolfSSL_CTX_set_tmp_dh(WOLFSSL_CTX* ctx, WOLFSSL_DH* dh);

/*!
    \ingroup CertsKeys

    \brief This function get the DSA parameters from a PEM buffer in bio.

    \return WOLFSSL_DSA on successfully parsing the PEM buffer a WOLFSSL_DSA
    structure is created and returned.
    \return Null if failed to parse PEM buffer.

    \param bio pointer to the WOLFSSL_BIO structure for getting PEM
    memory pointer.
    \param x pointer to be set to new WOLFSSL_DSA structure.
    \param cb password callback function.
    \param u null terminated password string.

    _Example_
    \code
    WOLFSSL_BIO* bio;
    WOLFSSL_DSA* dsa;
    // setup bio
    dsa = wolfSSL_PEM_read_bio_DSAparams(bio, NULL, NULL, NULL);

    // check dsa is not NULL and then use dsa
    \endcode

    \sa none
*/
WOLFSSL_DSA *wolfSSL_PEM_read_bio_DSAparams(WOLFSSL_BIO *bp,
    WOLFSSL_DSA **x, wc_pem_password_cb *cb, void *u);

/*!
    \ingroup Debug

    \brief This function returns the absolute value of the last error from
    WOLFSSL_ERROR encountered.

    \return error Returns absolute value of last error.

    \param none No parameters.

    _Example_
    \code
    unsigned long err;
    ...
    err = wolfSSL_ERR_peek_last_error();
    // inspect err value
    \endcode

    \sa wolfSSL_ERR_print_errors_fp
*/
unsigned long wolfSSL_ERR_peek_last_error(void);

/*!
    \ingroup CertsKeys

    \brief This function gets the peer’s certificate chain.

    \return pointer returns a pointer to the peer’s Certificate stack.
    \return NULL returned if no peer certificate.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( method );
    WOLFSSL* ssl = wolfSSL_new(ctx);
    ...
    wolfSSL_connect(ssl);
    STACK_OF(WOLFSSL_X509)* chain = wolfSSL_get_peer_cert_chain(ssl);
    ifchain){
	    // You have a pointer to the peer certificate chain
    }
    \endcode

    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_subject_name
    \sa wolfSSL_X509_get_isCA
*/
WOLF_STACK_OF(WOLFSSL_X509)* wolfSSL_get_peer_cert_chain(const WOLFSSL*);

/*!
    \ingroup Setup

    \brief This function resets option bits of WOLFSSL_CTX object.

    \return option new option bits

    \param ctx pointer to the SSL context.

    _Example_
    \code
    WOLFSSL_CTX* ctx = 0;
    ...
    wolfSSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1);
    \endcode

    \sa wolfSSL_CTX_new
    \sa wolfSSL_new
    \sa wolfSSL_free
*/
long wolfSSL_CTX_clear_options(WOLFSSL_CTX* ctx, long opt);

/*!
    \ingroup IO

    \brief This function sets the jObjectRef member of the WOLFSSL structure.

    \return SSL_SUCCESS returned if jObjectRef is properly set to objPtr.
    \return SSL_FAILURE returned if the function did not properly execute and
    jObjectRef is not set.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param objPtr a void pointer that will be set to jObjectRef.

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = WOLFSSL_new();
    void* objPtr = &obj;
    ...
    if(wolfSSL_set_jobject(ssl, objPtr)){
    	// The success case
    }
    \endcode

    \sa wolfSSL_get_jobject
*/
int wolfSSL_set_jobject(WOLFSSL* ssl, void* objPtr);

/*!
    \ingroup IO

    \brief This function returns the jObjectRef member of the WOLFSSL structure.

    \return value If the WOLFSSL struct is not NULL, the function returns the
    jObjectRef value.
    \return NULL returned if the WOLFSSL struct is NULL.

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new( protocol method );
    WOLFSSL* ssl = wolfSSL(ctx);
    ...
    void* jobject = wolfSSL_get_jobject(ssl);

    if(jobject != NULL){
    	// Success case
    }
    \endcode

    \sa wolfSSL_set_jobject
*/
void* wolfSSL_get_jobject(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets a callback in the ssl. The callback is to
    observe handshake messages. NULL value of cb resets the callback.

    \return SSL_SUCCESS On success.
    \return SSL_FAILURE If an NULL ssl passed in.

    \param ssl WOLFSSL structure to set callback argument.

    _Example_
    \code
    static cb(int write_p, int version, int content_type,
    const void *buf, size_t len, WOLFSSL *ssl, void *arg)
    …
    WOLFSSL* ssl;
    ret  = wolfSSL_set_msg_callback(ssl, cb);
    // check ret
    \endcode

    \sa wolfSSL_set_msg_callback_arg
*/
int wolfSSL_set_msg_callback(WOLFSSL *ssl, SSL_Msg_Cb cb);

/*!
    \ingroup Setup

    \brief This function sets associated callback context value in the ssl.
    The value is handed over to the callback argument.

    \return none No return.

    \param ssl WOLFSSL structure to set callback argument.

    _Example_
    \code
    static cb(int write_p, int version, int content_type,
    const void *buf, size_t len, WOLFSSL *ssl, void *arg)
    …
    WOLFSSL* ssl;
    ret  = wolfSSL_set_msg_callback(ssl, cb);
    // check ret
    wolfSSL_set_msg_callback(ssl, arg);
    \endcode

    \sa wolfSSL_set_msg_callback
*/
int wolfSSL_set_msg_callback_arg(WOLFSSL *ssl, void* arg);

/*!
    \ingroup CertsKeys

    \brief This function returns the next, if any, altname from the peer certificate.

    \return NULL if there is not a next altname.
    \return cert->altNamesNext->name from the WOLFSSL_X509 structure that is a
    string value from the altName list is returned if it exists.

    \param cert a pointer to the wolfSSL_X509 structure.

    _Example_
    \code
    WOLFSSL_X509 x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
                                                        DYNAMIC_TYPE_X509);
    …
    int x509NextAltName = wolfSSL_X509_get_next_altname(x509);
    if(x509NextAltName == NULL){
            //There isn’t another alt name
    }
    \endcode

    \sa wolfSSL_X509_get_issuer_name
    \sa wolfSSL_X509_get_subject_name
*/
char* wolfSSL_X509_get_next_altname(WOLFSSL_X509*);

/*!
    \ingroup CertsKeys

    \brief The function checks to see if x509 is NULL and if it’s not, it
    returns the notBefore member of the x509 struct.

    \return pointer to struct with ASN1_TIME to the notBefore
        member of the x509 struct.
    \return NULL the function returns NULL if the x509 structure is NULL.

    \param x509 a pointer to the WOLFSSL_X509 struct.

    _Example_
    \code
    WOLFSSL_X509* x509 = (WOLFSSL_X509)XMALLOC(sizeof(WOLFSSL_X509), NULL,
    DYNAMIC_TYPE_X509) ;
    …
    const WOLFSSL_ASN1_TIME* notAfter = wolfSSL_X509_get_notBefore(x509);
    if(notAfter == NULL){
            //The x509 object was NULL
    }
    \endcode

    \sa wolfSSL_X509_get_notAfter
*/
WOLFSSL_ASN1_TIME* wolfSSL_X509_get_notBefore(WOLFSSL_X509*);

/*!
    \ingroup IO

    \brief This function is called on the client side and initiates an SSL/TLS
    handshake with a server.  When this function is called, the underlying
    communication channel has already been set up.
    wolfSSL_connect() works with both blocking and non-blocking I/O.  When the
    underlying I/O is non-blocking, wolfSSL_connect() will return when the
    underlying I/O could not satisfy the needs of wolfSSL_connect to continue
    the handshake.  In this case, a call to wolfSSL_get_error() will yield
    either SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE.  The calling process
    must then repeat the call to wolfSSL_connect() when the underlying I/O is
    ready and wolfSSL will pick up where it left off. When using a non-blocking
    socket, nothing needs to be done, but select() can be used to check for the
    required condition.
    If the underlying I/O is blocking, wolfSSL_connect() will only return once
    the handshake has been finished or an error occurred.
    wolfSSL takes a different approach to certificate verification than OpenSSL
    does.  The default policy for the client is to verify the server, this
    means that if you don't load CAs to verify the server you'll get a connect
    error, unable to verify (-155).  It you want to mimic OpenSSL behavior of
    having SSL_connect succeed even if verifying the server fails and reducing
    security you can do this by calling:
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0); before calling SSL_new();
    Though it's not recommended.

    \return SSL_SUCCESS If successful.
    \return SSL_FATAL_ERROR will be returned if an error occurred.  To get a
    more detailed error code, call wolfSSL_get_error().

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...
    ret = wolfSSL_connect(ssl);
    if (ret != SSL_SUCCESS) {
    err = wolfSSL_get_error(ssl, ret);
    printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_accept
*/
int  wolfSSL_connect(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function is called on the server side to indicate that a
    HelloRetryRequest message must contain a Cookie and, in case of using
    protocol DTLS v1.3, that the handshake will always include a cookie
    exchange. Please note that when using protocol DTLS v1.3, the cookie
    exchange is enabled by default. The Cookie holds a hash of the current
    transcript so that another server process can handle the ClientHello in
    reply.  The secret is used when generating the integrity check on the Cookie
    data.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] secret a pointer to a buffer holding the secret.
    Passing NULL indicates to generate a new random secret.
    \param [in] secretSz Size of the secret in bytes.
    Passing 0 indicates to use the default size: WC_SHA256_DIGEST_SIZE (or WC_SHA_DIGEST_SIZE when SHA-256 not available).

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return SIDE_ERROR if called with a client.
    \return WOLFSSL_SUCCESS if successful.
    \return MEMORY_ERROR if allocating dynamic memory for storing secret failed.
    \return Another -ve value on internal error.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    char secret[32];
    ...
    ret = wolfSSL__send_hrr_cookie(ssl, secret, sizeof(secret));
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set use of Cookie and secret
    }
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_disable_hrr_cookie
*/
int  wolfSSL_send_hrr_cookie(WOLFSSL* ssl,
    const unsigned char* secret, unsigned int secretSz);

/*!

    \ingroup Setup

    \brief This function is called on the server side to indicate that a
    HelloRetryRequest message must NOT contain a Cookie and that, if using
    protocol DTLS v1.3, a cookie exchange will not be included in the
    handshake. Please note that not doing a cookie exchange when using protocol
    DTLS v1.3 can make the server susceptible to DoS/Amplification attacks.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return WOLFSSL_SUCCESS if successful
    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3
    \return SIDE_ERROR if invoked on client

    \sa wolfSSL_send_hrr_cookie
*/
int wolfSSL_disable_hrr_cookie(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function is called on the server to stop it from sending
    a resumption session ticket once the handshake is complete.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().

    \return BAD_FUNC_ARG if ctx is NULL or not using TLS v1.3.
    \return SIDE_ERROR if called with a client.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_no_ticket_TLSv13(ctx);
    if (ret != 0) {
        // failed to set no ticket
    }
    \endcode

    \sa wolfSSL_no_ticket_TLSv13
*/
int  wolfSSL_CTX_no_ticket_TLSv13(WOLFSSL_CTX* ctx);

/*!
    \ingroup Setup

    \brief This function is called on the server to stop it from sending
    a resumption session ticket once the handshake is complete.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return SIDE_ERROR if called with a client.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_no_ticket_TLSv13(ssl);
    if (ret != 0) {
        // failed to set no ticket
    }
    \endcode

    \sa wolfSSL_CTX_no_ticket_TLSv13
*/
int  wolfSSL_no_ticket_TLSv13(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function is called on a TLS v1.3 wolfSSL context to disallow
    Diffie-Hellman (DH) style key exchanges when handshakes are using
    pre-shared keys for authentication.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().

    \return BAD_FUNC_ARG if ctx is NULL or not using TLS v1.3.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_no_dhe_psk(ctx);
    if (ret != 0) {
        // failed to set no DHE for PSK handshakes
    }
    \endcode

    \sa wolfSSL_no_dhe_psk
*/
int  wolfSSL_CTX_no_dhe_psk(WOLFSSL_CTX* ctx);

/*!
    \ingroup Setup

    \brief This function is called on a TLS v1.3 client or server wolfSSL to
    disallow Diffie-Hellman (DH) style key exchanges when handshakes are using
    pre-shared keys for authentication.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_no_dhe_psk(ssl);
    if (ret != 0) {
        // failed to set no DHE for PSK handshakes
    }
    \endcode

    \sa wolfSSL_CTX_no_dhe_psk
*/
int  wolfSSL_no_dhe_psk(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function is called on a TLS v1.3 client or server wolfSSL to
    force the rollover of keys. A KeyUpdate message is sent to the peer and
    new keys are calculated for encryption. The peer will send back a KeyUpdate
    message and the new decryption keys will then be calculated.
    This function can only be called after a handshake has been completed.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return WANT_WRITE if the writing is not ready.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_update_keys(ssl);
    if (ret == WANT_WRITE) {
        // need to call again when I/O ready
    }
    else if (ret != WOLFSSL_SUCCESS) {
        // failed to send key update
    }
    \endcode

    \sa wolfSSL_write
*/
int  wolfSSL_update_keys(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function is called on a TLS v1.3 client or server wolfSSL to
    determine whether a rollover of keys is in progress. When
    wolfSSL_update_keys() is called, a KeyUpdate message is sent and the
    encryption key is updated. The decryption key is updated when the response
    is received.

    \param [in] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [out] required   0 when no key update response required. 1 when no key update response required.

    \return 0 on successful.
    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    int required;
    ...
    ret = wolfSSL_key_update_response(ssl, &required);
    if (ret != 0) {
        // bad parameters
    }
    if (required) {
        // encrypt Key updated, awaiting response to change decrypt key
    }
    \endcode

    \sa wolfSSL_update_keys
*/
int  wolfSSL_key_update_response(WOLFSSL* ssl, int* required);

/*!
    \ingroup Setup

    \brief This function is called on a TLS v1.3 client wolfSSL context to allow
    a client certificate to be sent post handshake upon request from server.
    This is useful when connecting to a web server that has some pages that
    require client authentication and others that don't.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().

    \return BAD_FUNC_ARG if ctx is NULL or not using TLS v1.3.
    \return SIDE_ERROR if called with a server.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_allow_post_handshake_auth(ctx);
    if (ret != 0) {
        // failed to allow post handshake authentication
    }
    \endcode

    \sa wolfSSL_allow_post_handshake_auth
    \sa wolfSSL_request_certificate
*/
int  wolfSSL_CTX_allow_post_handshake_auth(WOLFSSL_CTX* ctx);

/*!
    \ingroup Setup

    \brief This function is called on a TLS v1.3 client wolfSSL to allow
    a client certificate to be sent post handshake upon request from server.
    A Post-Handshake Client Authentication extension is sent in the ClientHello.
    This is useful when connecting to a web server that has some pages that
    require client authentication and others that don't.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return SIDE_ERROR if called with a server.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_allow_post_handshake_auth(ssl);
    if (ret != 0) {
        // failed to allow post handshake authentication
    }
    \endcode

    \sa wolfSSL_CTX_allow_post_handshake_auth
    \sa wolfSSL_request_certificate
*/
int  wolfSSL_allow_post_handshake_auth(WOLFSSL* ssl);

/*!
    \ingroup IO

    \brief This function requests a client certificate from the TLS v1.3 client.
    This is useful when a web server is serving some pages that require client
    authentication and others that don't.
    A maximum of 256 requests can be sent on a connection.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return WANT_WRITE if the writing is not ready.
    \return SIDE_ERROR if called with a client.
    \return NOT_READY_ERROR if called when the handshake is not finished.
    \return POST_HAND_AUTH_ERROR if posthandshake authentication is disallowed.
    \return MEMORY_E if dynamic memory allocation fails.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_request_certificate(ssl);
    if (ret == WANT_WRITE) {
        // need to call again when I/O ready
    }
    else if (ret != WOLFSSL_SUCCESS) {
        // failed to request a client certificate
    }
    \endcode

    \sa wolfSSL_allow_post_handshake_auth
    \sa wolfSSL_write
*/
int  wolfSSL_request_certificate(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets the list of elliptic curve groups to allow on
    a wolfSSL context in order of preference.
    The list is a null-terminated text string, and a colon-delimited list.
    Call this function to set the key exchange elliptic curve parameters to
    use with the TLS v1.3 connections.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param [in] list a string that is a colon-delimited list of elliptic curve
    groups.

    \return WOLFSSL_FAILURE if pointer parameters are NULL, there are more than
    WOLFSSL_MAX_GROUP_COUNT groups, a group name is not recognized or not
    using TLS v1.3.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx;
    const char* list = "P-384:P-256";
    ...
    ret = wolfSSL_CTX_set1_groups_list(ctx, list);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set group list
    }
    \endcode

    \sa wolfSSL_set1_groups_list
    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_set_groups
    \sa wolfSSL_UseKeyShare
    \sa wolfSSL_preferred_group
*/
int  wolfSSL_CTX_set1_groups_list(WOLFSSL_CTX *ctx, char *list);

/*!
    \ingroup Setup

    \brief This function sets the list of elliptic curve groups to allow on
    a wolfSSL in order of preference.
    The list is a null-terminated text string, and a colon-delimited list.
    Call this function to set the key exchange elliptic curve parameters to
    use with the TLS v1.3 connections.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] list a string that is a colon separated list of key exchange
    groups.

    \return WOLFSSL_FAILURE if pointer parameters are NULL, there are more than
    WOLFSSL_MAX_GROUP_COUNT groups, a group name is not recognized or not
    using TLS v1.3.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    const char* list = "P-384:P-256";
    ...
    ret = wolfSSL_CTX_set1_groups_list(ssl, list);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set group list
    }
    \endcode

    \sa wolfSSL_CTX_set1_groups_list
    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_set_groups
    \sa wolfSSL_UseKeyShare
    \sa wolfSSL_preferred_group
*/
int  wolfSSL_set1_groups_list(WOLFSSL *ssl, char *list);

/*!
    \ingroup TLS

    \brief This function returns the key exchange group the client prefers to
    use in the TLS v1.3 handshake.
    Call this function to after a handshake is complete to determine which
    group the server prefers so that this information can be used in future
    connections to pre-generate a key pair for key exchange.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return SIDE_ERROR if called with a server.
    \return NOT_READY_ERROR if called before handshake is complete.
    \return Group identifier if successful.

    _Example_
    \code
    int ret;
    int group;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_CTX_set1_groups_list(ssl)
    if (ret < 0) {
        // failed to get group
    }
    group = ret;
    \endcode

    \sa wolfSSL_UseKeyShare
    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_set_groups
    \sa wolfSSL_CTX_set1_groups_list
    \sa wolfSSL_set1_groups_list
*/
int  wolfSSL_preferred_group(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets the list of elliptic curve groups to allow on
    a wolfSSL context in order of preference.
    The list is an array of group identifiers with the number of identifiers
    specified in count.
    Call this function to set the key exchange elliptic curve parameters to
    use with the TLS v1.3 connections.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param [in] groups a list of key exchange groups by identifier.
    \param [in] count the number of key exchange groups in groups.

    \return BAD_FUNC_ARG if a pointer parameter is null, the number of groups
    exceeds WOLFSSL_MAX_GROUP_COUNT or not using TLS v1.3.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx;
    int* groups = { WOLFSSL_ECC_X25519, WOLFSSL_ECC_SECP256R1 };
    int count = 2;
    ...
    ret = wolfSSL_CTX_set1_groups_list(ctx, groups, count);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set group list
    }
    \endcode

    \sa wolfSSL_set_groups
    \sa wolfSSL_UseKeyShare
    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_set_groups
    \sa wolfSSL_CTX_set1_groups_list
    \sa wolfSSL_set1_groups_list
    \sa wolfSSL_preferred_group
*/
int  wolfSSL_CTX_set_groups(WOLFSSL_CTX* ctx, int* groups,
    int count);

/*!
    \ingroup Setup

    \brief This function sets the list of elliptic curve groups to allow on
    a wolfSSL.
    The list is an array of group identifiers with the number of identifiers
    specified in count.
    Call this function to set the key exchange elliptic curve parameters to
    use with the TLS v1.3 connections.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] groups a list of key exchange groups by identifier.
    \param [in] count the number of key exchange groups in groups.

    \return BAD_FUNC_ARG if a pointer parameter is null, the number of groups
    exceeds WOLFSSL_MAX_GROUP_COUNT, any of the identifiers are unrecognized or
    not using TLS v1.3.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    int* groups = { WOLFSSL_ECC_X25519, WOLFSSL_ECC_SECP256R1 };
    int count = 2;
    ...
    ret = wolfSSL_set_groups(ssl, groups, count);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set group list
    }
    \endcode

    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_UseKeyShare
    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_set_groups
    \sa wolfSSL_CTX_set1_groups_list
    \sa wolfSSL_set1_groups_list
    \sa wolfSSL_preferred_group
*/
int  wolfSSL_set_groups(WOLFSSL* ssl, int* groups, int count);

/*!
    \ingroup IO

    \brief This function is called on the client side and initiates a
    TLS v1.3 handshake with a server.  When this function is called, the
    underlying communication channel has already been set up.
    wolfSSL_connect() works with both blocking and non-blocking I/O.
    When the underlying I/O is non-blocking, wolfSSL_connect() will return
    when the underlying I/O could not satisfy the needs of wolfSSL_connect
    to continue the handshake.  In this case, a call to wolfSSL_get_error()
    will yield either SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE. The
    calling process must then repeat the call to wolfSSL_connect() when
    the underlying I/O is ready and wolfSSL will pick up where it left off.
    When using a non-blocking socket, nothing needs to be done, but select()
    can be used to check for the required condition. If the underlying I/O is
    blocking, wolfSSL_connect() will only return once the handshake has been
    finished or an error occurred. wolfSSL takes a different approach to
    certificate verification than OpenSSL does.  The default policy for the
    client is to verify the server, this means that if you don't load CAs to
    verify the server you'll get a connect error, unable to verify (-155). It
    you want to mimic OpenSSL behavior of having SSL_connect succeed even if
    verifying the server fails and reducing security you can do this by
    calling: SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0); before calling
    SSL_new();  Though it's not recommended.

    \return SSL_SUCCESS upon success.
    \return SSL_FATAL_ERROR will be returned if an error occurred.  To get a
    more detailed error code, call wolfSSL_get_error().

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...

    ret = wolfSSL_connect_TLSv13(ssl);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_connect
    \sa wolfSSL_accept_TLSv13
    \sa wolfSSL_accept
*/
int  wolfSSL_connect_TLSv13(WOLFSSL*);

/*!
    \ingroup IO

    \brief This function is called on the server side and waits for a SSL/TLS
    client to initiate the SSL/TLS handshake.  When this function is called,
    the underlying communication channel has already been set up.
    wolfSSL_accept() works with both blocking and non-blocking I/O.
    When the underlying I/O is non-blocking, wolfSSL_accept() will return
    when the underlying I/O could not satisfy the needs of wolfSSL_accept
    to continue the handshake.  In this case, a call to wolfSSL_get_error()
    will yield either SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE.
    The calling process must then repeat the call to wolfSSL_accept when
    data is available to read and wolfSSL will pick up where it left off.
    When using a non-blocking socket, nothing needs to be done, but select()
    can be used to check for the required condition. If the underlying I/O
    is blocking, wolfSSL_accept() will only return once the handshake has
    been finished or an error occurred.
    Call this function when expecting a TLS v1.3 connection though older
    version ClientHello messages are supported.

    \return SSL_SUCCESS upon success.
    \return SSL_FATAL_ERROR will be returned if an error occurred. To get a
    more detailed error code, call wolfSSL_get_error().

    \param ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    char buffer[80];
    ...

    ret = wolfSSL_accept_TLSv13(ssl);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_get_error
    \sa wolfSSL_connect_TLSv13
    \sa wolfSSL_connect
    \sa wolfSSL_accept_TLSv13
    \sa wolfSSL_accept
*/
wolfSSL_accept_TLSv13(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function sets the maximum amount of early data that a
    TLS v1.3 client or server is willing to exchange using the wolfSSL context.
    Call this function to limit the amount of early data to process to mitigate
    replay attacks. Early data is protected by keys derived from those of the
    connection that the session ticket was sent and therefore will be the same
    every time a session ticket is used in resumption.
    The value is included in the session ticket for resumption.
    A server value of zero indicates no early data is to be sent by client using
    session tickets. A client value of zero indicates that the client will
    not send any early data.
    It is recommended that the number of early data bytes be kept as low as
    practically possible in the application.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param [in] sz the amount of early data to accept in bytes.

    \return BAD_FUNC_ARG if ctx is NULL or not using TLS v1.3.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL_CTX* ctx;
    ...
    ret = wolfSSL_CTX_set_max_early_data(ctx, 128);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set group list
    }
    \endcode

    \sa wolfSSL_set_max_early_data
    \sa wolfSSL_write_early_data
    \sa wolfSSL_read_early_data
*/
int  wolfSSL_CTX_set_max_early_data(WOLFSSL_CTX* ctx,
    unsigned int sz);

/*!
    \ingroup Setup

    \brief This function sets the maximum amount of early data that a
    TLS v1.3 client or server is willing to exchange.
    Call this function to limit the amount of early data to process to mitigate
    replay attacks. Early data is protected by keys derived from those of the
    connection that the session ticket was sent and therefore will be the same
    every time a session ticket is used in resumption.
    The value is included in the session ticket for resumption.
    A server value of zero indicates no early data is to be sent by client using
    session tickets. A client value of zero indicates that the client will
    not send any early data.
    It is recommended that the number of early data bytes be kept as low as
    practically possible in the application.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] sz the amount of early data to accept from client in bytes.

    \return BAD_FUNC_ARG if ssl is NULL or not using TLS v1.3.
    \return 0 if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_set_max_early_data(ssl, 128);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set group list
    }
    \endcode

    \sa wolfSSL_CTX_set_max_early_data
    \sa wolfSSL_write_early_data
    \sa wolfSSL_read_early_data
*/
int  wolfSSL_set_max_early_data(WOLFSSL* ssl, unsigned int sz);

/*!
    \ingroup IO

    \brief This function writes early data to the server on resumption.
    Call this function instead of wolfSSL_connect() or wolfSSL_connect_TLSv13()
    to connect to the server and send the data in the handshake.
    This function is only used with clients.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] data the buffer holding the early data to write to server.
    \param [in] sz the amount of early data to write in bytes.
    \param [out] outSz the amount of early data written in bytes.

    \return BAD_FUNC_ARG if a pointer parameter is NULL, sz is less than 0 or
    not using TLSv1.3.
    \return SIDE_ERROR if called with a server.
    \return WOLFSSL_FATAL_ERROR if the connection is not made.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    byte earlyData[] = { early data };
    int outSz;
    char buffer[80];
    ...

    ret = wolfSSL_write_early_data(ssl, earlyData, sizeof(earlyData), &outSz);
    if (ret != WOLFSSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
        goto err_label;
    }
    if (outSz < sizeof(earlyData)) {
        // not all early data was sent
    }
    ret = wolfSSL_connect_TLSv13(ssl);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_read_early_data
    \sa wolfSSL_connect
    \sa wolfSSL_connect_TLSv13
*/
int  wolfSSL_write_early_data(OLFSSL* ssl, const void* data,
    int sz, int* outSz);

/*!
    \ingroup IO

    \brief This function reads any early data from a client on resumption.
    Call this function instead of wolfSSL_accept() or wolfSSL_accept_TLSv13()
    to accept a client and read any early data in the handshake.
    If there is no early data than the handshake will be processed as normal.
    This function is only used with servers.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [out] data a buffer to hold the early data read from client.
    \param [in] sz size of the buffer in bytes.
    \param [out] outSz number of bytes of early data read.

    \return BAD_FUNC_ARG if a pointer parameter is NULL, sz is less than 0 or
    not using TLSv1.3.
    \return SIDE_ERROR if called with a client.
    \return WOLFSSL_FATAL_ERROR if accepting a connection fails.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret = 0;
    int err = 0;
    WOLFSSL* ssl;
    byte earlyData[128];
    int outSz;
    char buffer[80];
    ...

    ret = wolfSSL_read_early_data(ssl, earlyData, sizeof(earlyData), &outSz);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    if (outSz > 0) {
        // early data available
    }
    ret = wolfSSL_accept_TLSv13(ssl);
    if (ret != SSL_SUCCESS) {
        err = wolfSSL_get_error(ssl, ret);
        printf(“error = %d, %s\n”, err, wolfSSL_ERR_error_string(err, buffer));
    }
    \endcode

    \sa wolfSSL_write_early_data
    \sa wolfSSL_accept
    \sa wolfSSL_accept_TLSv13
*/
int  wolfSSL_read_early_data(WOLFSSL* ssl, void* data, int sz,
    int* outSz);

/*!
    \ingroup Setup

    \brief This function sets the Pre-Shared Key (PSK) client side callback
    for TLS v1.3 connections.
    The callback is used to find a PSK identity and return its key and
    the name of the cipher to use for the handshake.
    The function sets the client_psk_tls13_cb member of the
    WOLFSSL_CTX structure.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param [in] cb a Pre-Shared Key (PSK) callback for a TLS 1.3 client.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    ...
    wolfSSL_CTX_set_psk_client_tls13_callback(ctx, my_psk_client_tls13_cb);
    \endcode

    \sa wolfSSL_set_psk_client_tls13_callback
    \sa wolfSSL_CTX_set_psk_server_tls13_callback
    \sa wolfSSL_set_psk_server_tls13_callback
*/
void wolfSSL_CTX_set_psk_client_tls13_callback(WOLFSSL_CTX* ctx,
    wc_psk_client_tls13_callback cb);

/*!
    \ingroup Setup

    \brief This function sets the Pre-Shared Key (PSK) client side callback
    for TLS v1.3 connections.
    The callback is used to find a PSK identity and return its key and
    the name of the cipher to use for the handshake.
    The function sets the client_psk_tls13_cb member of the options field in
    WOLFSSL structure.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] cb a Pre-Shared Key (PSK) callback for a TLS 1.3 client.

    _Example_
    \code
    WOLFSSL* ssl;
    ...
    wolfSSL_set_psk_client_tls13_callback(ssl, my_psk_client_tls13_cb);
    \endcode

    \sa wolfSSL_CTX_set_psk_client_tls13_callback
    \sa wolfSSL_CTX_set_psk_server_tls13_callback
    \sa wolfSSL_set_psk_server_tls13_callback
*/
void wolfSSL_set_psk_client_tls13_callback(WOLFSSL* ssl,
    wc_psk_client_tls13_callback cb);

/*!
    \ingroup Setup

    \brief This function sets the Pre-Shared Key (PSK) server side callback
    for TLS v1.3 connections.
    The callback is used to find a PSK identity and return its key and
    the name of the cipher to use for the handshake.
    The function sets the server_psk_tls13_cb member of the
    WOLFSSL_CTX structure.

    \param [in,out] ctx a pointer to a WOLFSSL_CTX structure, created
    with wolfSSL_CTX_new().
    \param [in] cb a Pre-Shared Key (PSK) callback for a TLS 1.3 server.

    _Example_
    \code
    WOLFSSL_CTX* ctx;
    ...
    wolfSSL_CTX_set_psk_server_tls13_callback(ctx, my_psk_client_tls13_cb);
    \endcode

    \sa wolfSSL_CTX_set_psk_client_tls13_callback
    \sa wolfSSL_set_psk_client_tls13_callback
    \sa wolfSSL_set_psk_server_tls13_callback
*/
void wolfSSL_CTX_set_psk_server_tls13_callback(WOLFSSL_CTX* ctx,
    wc_psk_server_tls13_callback cb);

/*!
    \ingroup Setup

    \brief This function sets the Pre-Shared Key (PSK) server side callback
    for TLS v1.3 connections.
    The callback is used to find a PSK identity and return its key and
    the name of the cipher to use for the handshake.
    The function sets the server_psk_tls13_cb member of the options field in
    WOLFSSL structure.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] cb a Pre-Shared Key (PSK) callback for a TLS 1.3 server.

    _Example_
    \code
    WOLFSSL* ssl;
    ...
    wolfSSL_set_psk_server_tls13_callback(ssl, my_psk_server_tls13_cb);
    \endcode

    \sa wolfSSL_CTX_set_psk_client_tls13_callback
    \sa wolfSSL_set_psk_client_tls13_callback
    \sa wolfSSL_CTX_set_psk_server_tls13_callback
*/
void wolfSSL_set_psk_server_tls13_callback(WOLFSSL* ssl,
    wc_psk_server_tls13_callback cb);

/*!
    \ingroup Setup

    \brief This function creates a key share entry from the group including
    generating a key pair.
    The KeyShare extension contains all the generated public keys for key
    exchange. If this function is called, then only the groups specified will
    be included.
    Call this function when a preferred group has been previously established
    for the server.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param [in] group a key exchange group identifier.

    \return BAD_FUNC_ARG if ssl is NULL.
    \return MEMORY_E when dynamic memory allocation fails.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_UseKeyShare(ssl, WOLFSSL_ECC_X25519);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set key share
    }
    \endcode

    \sa wolfSSL_preferred_group
    \sa wolfSSL_CTX_set1_groups_list
    \sa wolfSSL_set1_groups_list
    \sa wolfSSL_CTX_set_groups
    \sa wolfSSL_set_groups
    \sa wolfSSL_NoKeyShares
*/
int wolfSSL_UseKeyShare(WOLFSSL* ssl, word16 group);

/*!
    \ingroup Setup

    \brief This function is called to ensure no key shares are sent in the
    ClientHello. This will force the server to respond with a HelloRetryRequest
    if a key exchange is required in the handshake.
    Call this function when the expected key exchange group is not known and
    to avoid the generation of keys unnecessarily.
    Note that an extra round-trip will be required to complete the handshake
    when a key exchange is required.

    \param [in,out] ssl a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \return BAD_FUNC_ARG if ssl is NULL.
    \return SIDE_ERROR if called with a server.
    \return WOLFSSL_SUCCESS if successful.

    _Example_
    \code
    int ret;
    WOLFSSL* ssl;
    ...
    ret = wolfSSL_NoKeyShares(ssl);
    if (ret != WOLFSSL_SUCCESS) {
        // failed to set no key shares
    }
    \endcode

    \sa wolfSSL_UseKeyShare
*/
int wolfSSL_NoKeyShares(WOLFSSL* ssl);

/*!
    \ingroup Setup

    \brief This function is used to indicate
    that the application is a server and will only support the TLS 1.3
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \param [in] heap a pointer to a buffer that the static memory allocator will use during dynamic memory allocation.

    \return If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_3_server_method_ex(NULL);
    if (method == NULL) {
        // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method
    \sa wolfDTLSv1_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_3_server_method_ex(void* heap);

/*!
    \ingroup Setup

    \brief This function is used to indicate
    that the application is a client and will only support the TLS 1.3
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \param [in] heap a pointer to a buffer that the static memory allocator will use during dynamic memory allocation.

    \return If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_3_client_method_ex(NULL);
    if (method == NULL) {
        // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method
    \sa wolfDTLSv1_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_3_client_method_ex(void* heap);

/*!
    \ingroup Setup

    \brief This function is used to indicate
    that the application is a server and will only support the TLS 1.3
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \return If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_3_server_method();
    if (method == NULL) {
        // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_server_method
    \sa wolfTLSv1_server_method
    \sa wolfTLSv1_1_server_method
    \sa wolfTLSv1_2_server_method
    \sa wolfTLSv1_3_server_method_ex
    \sa wolfDTLSv1_server_method
    \sa wolfSSLv23_server_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_3_server_method(void);

/*!
    \ingroup Setup

    \brief This function is used to indicate
    that the application is a client and will only support the TLS 1.3
    protocol. This function allocates memory for and initializes a new
    wolfSSL_METHOD structure to be used when creating the SSL/TLS context
    with wolfSSL_CTX_new().

    \return If successful, the call will return a pointer to the newly
    created WOLFSSL_METHOD structure.
    \return FAIL If memory allocation fails when calling XMALLOC, the failure
    value of the underlying malloc() implementation will be returned
    (typically NULL with errno will be set to ENOMEM).

    _Example_
    \code
    #include <wolfssl/ssl.h>

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    method = wolfTLSv1_3_client_method();
    if (method == NULL) {
        // unable to get method
    }

    ctx = wolfSSL_CTX_new(method);
    ...
    \endcode

    \sa wolfSSLv3_client_method
    \sa wolfTLSv1_client_method
    \sa wolfTLSv1_1_client_method
    \sa wolfTLSv1_2_client_method
    \sa wolfTLSv1_3_client_method_ex
    \sa wolfDTLSv1_client_method
    \sa wolfSSLv23_client_method
    \sa wolfSSL_CTX_new
*/
WOLFSSL_METHOD *wolfTLSv1_3_client_method(void);

/*!
    \ingroup Setup

    \brief This function returns a WOLFSSL_METHOD similar to
    wolfTLSv1_3_client_method except that it is not determined
    which side yet (server/client).

    \param [in] heap a pointer to a buffer that the static memory allocator will use during dynamic memory allocation.

    \return WOLFSSL_METHOD On successful creations returns a WOLFSSL_METHOD
    pointer
    \return NULL Null if memory allocation error or failure to create method

    _Example_
    \code
    WOLFSSL* ctx;
    ctx  = wolfSSL_CTX_new(wolfTLSv1_3_method_ex(NULL));
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
WOLFSSL_METHOD *wolfTLSv1_3_method_ex(void* heap);

/*!
    \ingroup Setup

    \brief This function returns a WOLFSSL_METHOD similar to
    wolfTLSv1_3_client_method except that it is not determined
    which side yet (server/client).

    \return WOLFSSL_METHOD On successful creations returns a WOLFSSL_METHOD
    pointer
    \return NULL Null if memory allocation error or failure to create method

    _Example_
    \code
    WOLFSSL* ctx;
    ctx  = wolfSSL_CTX_new(wolfTLSv1_3_method());
    // check ret value
    \endcode

    \sa wolfSSL_new
    \sa wolfSSL_free
*/
WOLFSSL_METHOD *wolfTLSv1_3_method(void);

/*!
 \ingroup SSL
 \brief This function sets a fixed / static ephemeral key for testing only
 \return 0 Key loaded successfully
 \param ctx A WOLFSSL_CTX context pointer
 \param keyAlgo enum wc_PkType like WC_PK_TYPE_DH and WC_PK_TYPE_ECDH
 \param key key file path (if keySz == 0) or actual key buffer (PEM or ASN.1)
 \param keySz key size (should be 0 for "key" arg is file path)
 \param format WOLFSSL_FILETYPE_ASN1 or WOLFSSL_FILETYPE_PEM
 \sa wolfSSL_CTX_get_ephemeral_key
 */
int wolfSSL_CTX_set_ephemeral_key(WOLFSSL_CTX* ctx, int keyAlgo, const char* key, unsigned int keySz, int format);

/*!
 \ingroup SSL
 \brief This function sets a fixed / static ephemeral key for testing only
 \return 0 Key loaded successfully
 \param ssl A WOLFSSL object pointer
 \param keyAlgo enum wc_PkType like WC_PK_TYPE_DH and WC_PK_TYPE_ECDH
 \param key key file path (if keySz == 0) or actual key buffer (PEM or ASN.1)
 \param keySz key size (should be 0 for "key" arg is file path)
 \param format WOLFSSL_FILETYPE_ASN1 or WOLFSSL_FILETYPE_PEM
 \sa wolfSSL_get_ephemeral_key
 */
int wolfSSL_set_ephemeral_key(WOLFSSL* ssl, int keyAlgo, const char* key, unsigned int keySz, int format);

/*!
 \ingroup SSL
 \brief This function returns pointer to loaded key as ASN.1/DER
 \return 0 Key returned successfully
 \param ctx A WOLFSSL_CTX context pointer
 \param keyAlgo enum wc_PkType like WC_PK_TYPE_DH and WC_PK_TYPE_ECDH
 \param key key buffer pointer
 \param keySz key size pointer
 \sa wolfSSL_CTX_set_ephemeral_key
 */
int wolfSSL_CTX_get_ephemeral_key(WOLFSSL_CTX* ctx, int keyAlgo, 
    const unsigned char** key, unsigned int* keySz);

/*!
 \ingroup SSL
 \brief This function returns pointer to loaded key as ASN.1/DER
 \return 0 Key returned successfully
 \param ssl A WOLFSSL object pointer
 \param keyAlgo enum wc_PkType like WC_PK_TYPE_DH and WC_PK_TYPE_ECDH
 \param key key buffer pointer
 \param keySz key size pointer
 \sa wolfSSL_set_ephemeral_key
 */
int wolfSSL_get_ephemeral_key(WOLFSSL* ssl, int keyAlgo, 
    const unsigned char** key, unsigned int* keySz);

/*!
 \ingroup SSL
 \brief Sign a message with the chosen message digest, padding, and RSA key
 \return WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on error
 \param type      Hash NID
 \param m         Message to sign. Most likely this will be the digest of
                  the message to sign
 \param mLen      Length of message to sign
 \param sigRet    Output buffer
 \param sigLen    On Input: length of sigRet buffer
                  On Output: length of data written to sigRet
 \param rsa       RSA key used to sign the input
 \param flag      1: Output the signature
                  0: Output the value that the unpadded signature should be
                     compared to. Note: for RSA_PKCS1_PSS_PADDING the
                     wc_RsaPSS_CheckPadding_ex function should be used to check
                     the output of a *Verify* function.
 \param padding   Padding to use. Only RSA_PKCS1_PSS_PADDING and
                  RSA_PKCS1_PADDING are currently supported for signing.
 */
int wolfSSL_RSA_sign_generic_padding(int type, const unsigned char* m,
                               unsigned int mLen, unsigned char* sigRet,
                               unsigned int* sigLen, WOLFSSL_RSA* rsa,
                               int flag, int padding);
/*!

\brief checks if DTLSv1.3 stack has some messages sent but not yet acknowledged
 by the other peer

 \return 1 if there are pending messages, 0 otherwise
 \param ssl A WOLFSSL object pointer
*/
int wolfSSL_dtls13_has_pending_msg(WOLFSSL *ssl);

/*!
    \ingroup SSL
    \brief Get the maximum size of Early Data from a session.

    \param [in] s  the WOLFSSL_SESSION instance.

    \return the value of max_early_data that was configured in the WOLFSSL* the session
    was derived from.

    \sa wolfSSL_set_max_early_data
    \sa wolfSSL_write_early_data
    \sa wolfSSL_read_early_data
 */
unsigned int wolfSSL_SESSION_get_max_early_data(const WOLFSSL_SESSION *s);

/*!
    \ingroup SSL
    \brief Get a new index for external data. This entry applies also for the
           following API:
           - wolfSSL_CTX_get_ex_new_index
           - wolfSSL_get_ex_new_index
           - wolfSSL_SESSION_get_ex_new_index
           - wolfSSL_X509_get_ex_new_index

    \param [in] All input parameters are ignored. The callback functions are not
                supported with wolfSSL.

    \return The new index value to be used with the external data API for this
            object class.
 */
int wolfSSL_CRYPTO_get_ex_new_index(int, void*, void*, void*, void*);

/*!

\brief Enable use of ConnectionID extensions for the SSL object. See RFC 9146
and RFC 9147

 \return WOLFSSL_SUCCESS on success, error code otherwise

 \param ssl A WOLFSSL object pointer

 \sa wolfSSL_dtls_cid_is_enabled
 \sa wolfSSL_dtls_cid_set
 \sa wolfSSL_dtls_cid_get_rx_size
 \sa wolfSSL_dtls_cid_get_rx
 \sa wolfSSL_dtls_cid_get_tx_size
 \sa wolfSSL_dtls_cid_get_tx
*/
int wolfSSL_dtls_cid_use(WOLFSSL* ssl);

/*!

\brief If invoked after the handshake is complete it checks if ConnectionID was
successfully negotiated for the SSL object. See RFC 9146 and RFC 9147

 \return 1 if ConnectionID was correctly negotiated, 0 otherwise

 \param ssl A WOLFSSL object pointer

 \sa wolfSSL_dtls_cid_use
 \sa wolfSSL_dtls_cid_set
 \sa wolfSSL_dtls_cid_get_rx_size
 \sa wolfSSL_dtls_cid_get_rx
 \sa wolfSSL_dtls_cid_get_tx_size
 \sa wolfSSL_dtls_cid_get_tx
*/
int wolfSSL_dtls_cid_is_enabled(WOLFSSL* ssl);

/*!

\brief Set the ConnectionID used by the other peer to send records in this
connection. See RFC 9146 and RFC 9147. The ConnectionID must be at maximum
DTLS_CID_MAX_SIZE, that is an tunable compile time define, and it can't
never be bigger than 255 bytes.

 \return WOLFSSL_SUCCESS if ConnectionID was correctly set, error code otherwise

 \param ssl A WOLFSSL object pointern
 \param cid the ConnectionID to be used
 \param size of the ConnectionID provided

 \sa wolfSSL_dtls_cid_use
 \sa wolfSSL_dtls_cid_is_enabled
 \sa wolfSSL_dtls_cid_get_rx_size
 \sa wolfSSL_dtls_cid_get_rx
 \sa wolfSSL_dtls_cid_get_tx_size
 \sa wolfSSL_dtls_cid_get_tx
*/
int wolfSSL_dtls_cid_set(WOLFSSL* ssl, unsigned char* cid,
    unsigned int size);

/*!

\brief Get the size of the ConnectionID used by the other peer to send records
in this connection. See RFC 9146 and RFC 9147. The size is stored in the
parameter size.

 \return WOLFSSL_SUCCESS if ConnectionID was correctly negotiated, error code
 otherwise

 \param ssl A WOLFSSL object pointern
 \param size a pointer to an unsigned int where the size will be stored

 \sa wolfSSL_dtls_cid_use
 \sa wolfSSL_dtls_cid_is_enabled
 \sa wolfSSL_dtls_cid_set
 \sa wolfSSL_dtls_cid_get_rx
 \sa wolfSSL_dtls_cid_get_tx_size
 \sa wolfSSL_dtls_cid_get_tx
*/
int wolfSSL_dtls_cid_get_rx_size(WOLFSSL* ssl,
    unsigned int* size);

/*!

\brief Copy the ConnectionID used by the other peer to send records in this
connection into the buffer pointed by the parameter buffer. See RFC 9146 and RFC
9147. The available space in the buffer need to be provided in bufferSz.

 \return WOLFSSL_SUCCESS if ConnectionID was correctly copied, error code
 otherwise

 \param ssl A WOLFSSL object pointern
 \param buffer A buffer where the ConnectionID will be copied
 \param bufferSz available space in buffer

 \sa wolfSSL_dtls_cid_use
 \sa wolfSSL_dtls_cid_is_enabled
 \sa wolfSSL_dtls_cid_set
 \sa wolfSSL_dtls_cid_get_rx_size
 \sa wolfSSL_dtls_cid_get_tx_size
 \sa wolfSSL_dtls_cid_get_tx
*/
int wolfSSL_dtls_cid_get_rx(WOLFSSL* ssl, unsigned char* buffer,
    unsigned int bufferSz);

/*!

\brief Get the size of the ConnectionID used to send records in this
connection. See RFC 9146 and RFC 9147. The size is stored in the parameter size.

 \return WOLFSSL_SUCCESS if ConnectionID size was correctly stored, error
 code otherwise

 \param ssl A WOLFSSL object pointern
 \param size a pointer to an unsigned int where the size will be stored

 \sa wolfSSL_dtls_cid_use
 \sa wolfSSL_dtls_cid_is_enabled
 \sa wolfSSL_dtls_cid_set
 \sa wolfSSL_dtls_cid_get_rx_size
 \sa wolfSSL_dtls_cid_get_rx
 \sa wolfSSL_dtls_cid_get_tx
*/
int wolfSSL_dtls_cid_get_tx_size(WOLFSSL* ssl, unsigned int* size);

/*!

\brief Copy the ConnectionID used when sending records in this connection into
the buffer pointer by the parameter buffer. See RFC 9146 and RFC 9147. The
available size need to be provided in bufferSz.

 \return WOLFSSL_SUCCESS if ConnectionID was correctly copied, error code
 otherwise

 \param ssl A WOLFSSL object pointern
 \param buffer A buffer where the ConnectionID will be copied
 \param bufferSz available space in buffer

 \sa wolfSSL_dtls_cid_use
 \sa wolfSSL_dtls_cid_is_enabled
 \sa wolfSSL_dtls_cid_set
 \sa wolfSSL_dtls_cid_get_rx_size
 \sa wolfSSL_dtls_cid_get_rx
 \sa wolfSSL_dtls_cid_get_tx_size
*/
int wolfSSL_dtls_cid_get_tx(WOLFSSL* ssl, unsigned char* buffer,
    unsigned int bufferSz);
