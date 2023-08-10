/*!
    \brief  
    \return Success  この関数は、読み取られたバイト数を返します。
    \return WOLFSSL_CBIO_ERR_WANT_READ  最後のエラーがsocket_ewouldbolcokまたはsocket_eagainであれば、メッセージを返されます。
    \return WOLFSSL_CBIO_ERR_TIMEOUT  "Socket Timeout"メッセージを返しました。
    \return WOLFSSL_CBIO_ERR_CONN_RST  最後のエラーがsocket_econnresetの場合、 "Connection Reset"メッセージで返されます。
    \return WOLFSSL_CBIO_ERR_ISR  最後のエラーがsocket_eintrの場合、 "Socket Interrupted"メッセージが返されます。
    \return WOLFSSL_CBIO_ERR_WANT_READ  最後のエラーがsocket_econneRefusedの場合、「接続拒否」メッセージを返しました。
    \return WOLFSSL_CBIO_ERR_CONN_CLOSE  最後のエラーがSOCKET_ECONNABORTEDの場合、「接続中止」メッセージで返されます。
    \return WOLFSSL_CBIO_ERR_GENERAL  最後のエラーが指定されていない場合は、「一般的なエラー」メッセージで返されます。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
    \param buf  バッファのチャーポインタ表現。
    \param sz  バッファのサイズ。
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
    \brief  
    \return Success  この関数は送信されたバイト数を返します。
    \return WOLFSSL_CBIO_ERR_WANT_WRITE  最後のエラーがsocket_ewouldblockまたはsocket_eagainであれば、 "Block"メッセージを返します。
    \return WOLFSSL_CBIO_ERR_CONN_RST  最後のエラーがsocket_econnresetの場合、 "Connection Reset"メッセージで返されます。
    \return WOLFSSL_CBIO_ERR_ISR  最後のエラーがsocket_eintrの場合、 "Socket Interrupted"メッセージが返されます。
    \return WOLFSSL_CBIO_ERR_CONN_CLOSE  最後のエラーがsocket_epipeの場合、 "Socket Epipe"メッセージを返しました。
    \return WOLFSSL_CBIO_ERR_GENERAL  最後のエラーが指定されていない場合は、「一般的なエラー」メッセージで返されます。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
    \param buf  バッファを表す文字ポインタ。
    \param sz  バッファのサイズ。
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
    \brief  
    \return Success  この関数は、実行が成功した場合に読み込まれたNBバイトを返します。
    \return WOLFSSL_CBIO_ERR_WANT_READ  接続が拒否された場合、または「ブロック」エラーが発生した場合は機能にスローされました。
    \return WOLFSSL_CBIO_ERR_TIMEOUT  ソケットがタイムアウトした場合は返されます。
    \return WOLFSSL_CBIO_ERR_CONN_RST  接続がリセットされている場合は返されます。
    \return WOLFSSL_CBIO_ERR_ISR  ソケットが中断された場合は返されます。
    \return WOLFSSL_CBIO_ERR_GENERAL  一般的なエラーがあった場合に返されます。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
    \param buf  バッファへの定数の文字ポインタ。
    \param sz  バッファのサイズを表すint型。
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
    \brief  
    \return Success  この関数は送信されたバイト数を返します。
    \return WOLFSSL_CBIO_ERR_WANT_WRITE  最後のエラーがsocket_ewouldblockまたはsocket_eagainエラーの場合、 "Block"メッセージを返します。
    \return WOLFSSL_CBIO_ERR_CONN_RST  最後のエラーがsocket_econnresetの場合、 "Connection Reset"メッセージで返されます。
    \return WOLFSSL_CBIO_ERR_ISR  最後のエラーがsocket_eintrの場合、 "Socket Interrupted"メッセージが返されます。
    \return WOLFSSL_CBIO_ERR_CONN_CLOSE  最後のエラーがwolfssl_cbio_err_conn_croseの場合、 "Socket Epipe"メッセージを返しました。
    \return WOLFSSL_CBIO_ERR_GENERAL  最後のエラーが指定されていない場合は、「一般的なエラー」メッセージで返されます。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
    \param buf  バッファを表す文字ポインタ。
    \param sz  バッファのサイズ。
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
    \brief  
    \return Success  この関数は、バッファにコピーされたバイト数を返します。
    \return GEN_COOKIE_E  getPeernameがEmbedGenerateCookieに失敗した場合に返されます。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
    \param buf  バッファを表すバイトポインタ。xmemcpy（）からの宛先です。
    \param sz  バッファのサイズ。
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
    \brief  
    \return none  いいえ返します。
    \param ctx  ヒープヒントへのvoidポインタ。
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
    \brief  データ。デフォルトでは、WolfSSLはシステムのTCP RECV（）関数を使用するコールバックとしてEmbedReceive（）を使用します。ユーザは、メモリ、他のネットワークモジュール、またはどこからでも入力するように機能を登録できます。関数の機能とエラーコードのためのガイドとして、src / io.cの埋め込みReceive（）関数を参照してください。特に、データが準備ができていないときに、IO_ERR_WANT_READを非ブロック受信用に返す必要があります。
    \return none  いいえ返します。
    \param ctx  wolfssl_ctx_new（）で作成されたSSLコンテキストへのポインタ。
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
    \brief  コールバック関数デフォルトでは、WolfSSLは、WolfSSLがシステムのTCPライブラリを使用している場合、wolfssl_set_fd（）に渡されたファイル記述子をコンテキストとして設定します。自分の受信コールバックを登録した場合は、セッションの特定のコンテキストを設定することができます。たとえば、メモリバッファを使用している場合、コンテキストは、メモリバッファーのどこにありかを説明する構造へのポインタであり得る。
    \return none  いいえ返します。
    \param ssl  wolfssl_new（）で作成されたSSLセッションへのポインタ。
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
    \brief  コールバック関数デフォルトでは、WolfSSLは、WolfSSLがシステムのTCPライブラリを使用している場合、wolfssl_set_fd（）に渡されたファイル記述子をコンテキストとして設定します。独自の送信コールバックを登録した場合は、セッションの特定のコンテキストを設定することができます。たとえば、メモリバッファを使用している場合、コンテキストは、メモリバッファーのどこにありかを説明する構造へのポインタであり得る。
    \return none  いいえ返します。
    \param ssl  wolfssl_new（）で作成されたSSLセッションへのポインタ。
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
    \brief  この関数は、WolfSSL構造体のIOCB_READCTXメンバーを返します。
    \return pointer  この関数は、wolfssl構造体のiocb_readctxメンバーへのvoidポインタを返します。
    \return NULL  wolfssl構造体がNULLの場合に返されます。
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
    \brief  この関数は、WolfSSL構造のIOCB_WRITECTXメンバーを返します。
    \return pointer  この関数は、WolfSSL構造のIOCB_WRITECTXメンバーへのvoidポインタを返します。
    \return NULL  wolfssl構造体がNULLの場合に返されます。
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
    \brief  与えられたSSLセッション受信コールバックは、デフォルトのwolfssl埋め込み受信コールバック、またはユーザによって指定されたカスタムコールバックであり得る（wolfssl_ctx_setiorecvを参照）。デフォルトのフラグ値は、WolfSSLによってwolfsslによって0の値に設定されます。デフォルトのWolfSSL受信コールバックはRECV（）関数を使用してソケットからデータを受信します。 「Recv（）」ページから：「Recv（）関数へのflags引数は、1つ以上の値をOR処理するか、MSG_OOBプロセス帯域外データ、MSG_PEEK PEEK、MSG_PEEK PEEK、MSG_WAITALLがフルを待っています要求またはエラー。 MSG_OOBフラグは、通常のデータストリームで受信されないであろう帯域外データの受信を要求します。一部のプロトコルは通常のデータキューの先頭に迅速なデータを配置し、このフラグをそのようなプロトコルで使用することはできません。 MSG_PEEKフラグは、受信操作によって受信キューの先頭からのデータをキューから削除することなくデータを返します。したがって、以降の受信呼び出しは同じデータを返します。 MSG_WAITALLフラグは、完全な要求が満たされるまで操作ブロックを要求します。ただし、信号がキャッチされている場合は、呼び出し側よりも少ないデータが少なく、エラーまたは切断が発生するか、または受信されるデータが返されるものとは異なるタイプのデータを返します。
    \return none  いいえ返します。
    \param ssl  wolfssl_new（）で作成されたSSLセッションへのポインタ。
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
    \brief  SSLセッションを考えると送信コールバックは、デフォルトのWolfSSL EmbedEndコールバック、またはユーザーによって指定されたカスタムコールバックのいずれかです（WolfSSL_CTX_SetiosEndを参照）。デフォルトのフラグ値は、wolfsslによって0の値に設定されます。デフォルトのWolfSSL Send Callbackはsend（）関数を使用してソケットからデータを送信します。send（）manページから： "flagsパラメータには、次のうち1つ以上が含まれていてもよい。フラグMSG_OOBは、この概念（例えばSOCK_STREAM）をサポートするソケットに「帯域外」データを送信するために使用される。基礎となるプロトコルは、「帯域外」のデータもサポートする必要があります。MSG_DONTROUTEは通常、診断プログラムまたはルーティングプログラムによってのみ使用されます。」
    \return none  いいえ返します。
    \param ssl  wolfssl_new（）で作成されたSSLセッションへのポインタ。
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
    \brief  この関数は、wolfssl構造内のnxctx構造体のNxSocketメンバーとNXWAITメンバーを設定します。
    \return none  いいえ返します。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
    \param nxSocket  NXCTX構造のNXSOCTOCKメンバーに設定されているNX_TCP_SOCKETを入力するためのポインタ。
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
    \brief  wolfssl_ctx構造CallBackGencookie Typeは関数ポインタで、署名：int（* callbackgencookie）（wolfssl * ssl、unsigned char * buf、int sz、void * ctx）を持っています。
    \return none  いいえ返します。
    \param ssl  wolfssl_new（）を使用して作成されたWolfSSL構造へのポインタ。
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
    \brief  この関数は、WolfSSL構造のIOCB_COOKIECTXメンバーを返します。
    \return pointer  この関数は、iocb_cookiectxに格納されているvoidポインタ値を返します。
    \return NULL  WolfSSL構造体がNULLの場合
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
    \brief  この関数は、WolfSSLがWolfSSL_ISOTPでコンパイルされている場合に使用する場合は、WolfSSLの場合はISO-TPコンテキストを設定します。
    \return 0  成功すると、故障のwolfssl_cbio_err_general
    \param ssl  wolfsslコンテキスト
    \param ctx  ユーザーはこの関数が初期化されるISOTPコンテキストを作成しました
    \param recv_fn  ユーザーはバスを受信できます
    \param send_fn  ユーザーはバスを送ることができます
    \param delay_fn  ユーザーマイクロ秒の粒度遅延関数
    \param receive_delay  各CANバスパケットを遅らせるためのマイクロ秒のセット数
    \param receive_buffer  ユーザーがデータを受信するためのバッファーが提供され、ISOTP_DEFAULT_BUFFER_SIZEバイトに割り当てられていることをお勧めします。
    \param receive_buffer_size   -  receive_bufferのサイズ
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
