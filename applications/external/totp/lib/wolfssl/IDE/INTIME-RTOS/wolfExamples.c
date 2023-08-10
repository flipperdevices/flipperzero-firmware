#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <rt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <io.h>

#include "wolfExamples.h"
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>


/*****************************************************************************
 * Globals
 ****************************************************************************/
RTHANDLE            hRootProcess;
DWORD               dwKtickInUsecs;
INIT_STRUCT         gInit;
static int gServerExit = 0;
static int gServerReady = 0;

static const char menu1[] = "\r\n"
    "\tt. WolfCrypt Test\r\n"
    "\tb. WolfCrypt Benchmark\r\n"
    "\tc. WolfSSL Client Example\r\n"
    "\ts. WolfSSL Server Example\r\n"
    "\tl. WolfSSL Localhost Client/Server Example\r\n";


/*****************************************************************************
 * Configuration
 ****************************************************************************/

#define TLS_MAXDATASIZE 4096           /* maximum acceptable amount of data */
#define TLS_PORT        11111          /* define default port number */
#define TLS_HOST_LOCAL  "127.0.0.1"
#define TLS_HOST_REMOTE "192.168.0.112"
#define SOCK_MAX_PENDING 5
#define THREAD_BASE_PRIO 150


/*****************************************************************************
 * TLS Client
 ****************************************************************************/
int wolfExample_TLSClient(const char* ip, int port)
{
    int          ret = 0;
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL*     ssl = NULL;        /* create WOLFSSL object */
    int                sockFd;      /* socket file descriptor */
    struct sockaddr_in servAddr;    /* struct for server address */
    char sendBuff[TLS_MAXDATASIZE], rcvBuff[TLS_MAXDATASIZE];

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        printf("Failed to create socket. Error: %d\n", errno);
        return errno;
    }

    memset(&servAddr, 0, sizeof(servAddr)); /* clears memory block for use */
    servAddr.sin_family = AF_INET;          /* sets addressfamily to internet*/
    servAddr.sin_port = htons(port);    /* sets port to defined port */

    /* looks for the server at the entered address (ip in the command line) */
    if (inet_pton(AF_INET, ip, &servAddr.sin_addr) < 1) {
        /* checks validity of address */
        ret = errno;
        printf("Invalid Address. Error: %d\n", ret);
        goto exit;
    }

    if (connect(sockFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        /* if socket fails to connect to the server*/
        ret = errno;
        printf("Connect error. Error: %d\n", ret);
        goto exit;
    }

    /* create and initialize WOLFSSL_CTX structure */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        printf("SSL_CTX_new error.\n");
        goto exit;
    }

    /* load CA certificates into wolfSSL_CTX. which will verify the server */
    ret = wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
        sizeof_ca_cert_der_2048, SSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        printf("Error %d loading CA cert\n", ret);
        goto exit;
    }
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        printf("wolfSSL_new error.\n");
        goto exit;
    }
    wolfSSL_set_fd(ssl, sockFd);

    ret = wolfSSL_connect(ssl);
    if (ret == SSL_SUCCESS) {
        printf("Message for server:\t");
        fgets(sendBuff, TLS_MAXDATASIZE, stdin);

        if (wolfSSL_write(ssl, sendBuff, strlen(sendBuff)) != strlen(sendBuff)) {
            /* the message is not able to send, or error trying */
            ret = wolfSSL_get_error(ssl, 0);
            printf("Write error: Error: %d\n", ret);
            goto exit;
        }

        memset(rcvBuff, 0, TLS_MAXDATASIZE);
        if (wolfSSL_read(ssl, rcvBuff, TLS_MAXDATASIZE) < 0) {
            /* the server failed to send data, or error trying */
            ret = wolfSSL_get_error(ssl, 0);
            printf("Read error. Error: %d\n", ret);
            goto exit;
        }
        printf("Received: \t%s\n", rcvBuff);
    }

exit:
    /* frees all data before client termination */
    if (sockFd != -1)
        close(sockFd);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    gServerExit = 1;

    return ret;
}

/*****************************************************************************
 * TLS Server
 ****************************************************************************/
int wolfExample_TLSServer(int port)
{
    int ret = 0;
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL* ssl = NULL;
    int sockFd, clientFd = -1;
    struct sockaddr_in serverAddr = {0}, clientAddr = {0};
    const char reply[]  = "I hear ya fa shizzle!\n";
    int addrSize        = sizeof(clientAddr);
    char buff[256];

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        printf("Failed to create socket. Error: %d\n", errno);
        return errno;
    }

    /* create and initialize WOLFSSL_CTX structure */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
        fprintf(stderr, "wolfSSL_CTX_new error.\n");
        goto exit;
    }

    /* Load server certificate into WOLFSSL_CTX */
    ret = wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_der_2048,
        sizeof_server_cert_der_2048, SSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        fprintf(stderr, "Error %d loading server-cert!\n", ret);
        goto exit;
    }

    /* Load server key into WOLFSSL_CTX */
    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key_der_2048,
        sizeof_server_key_der_2048, SSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        fprintf(stderr, "Error %d loading server-key!\n", ret);
        goto exit;
    }

    /* Initialize the server address struct to zero */
    memset((char *)&serverAddr, 0, sizeof(serverAddr));

    /* Fill the server's address family */
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(port);

    /* Attach the server socket to our port */
    if (bind(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("ERROR: failed to bind\n");
        goto exit;
    }

    printf("Waiting for a connection...\n");
    gServerReady = 1;

    /* Continuously accept connects while not in an active connection */
    while (gServerExit == 0) {
        /* listen for a new connection */
        ret = listen(sockFd, SOCK_MAX_PENDING);
        if (ret == 0) {
            /* Wait until a client connects */
            clientFd = accept(sockFd, (struct sockaddr*)&clientAddr, &addrSize);

            /* If fails to connect, loop back up and wait for a new connection */
            if (clientFd == -1) {
                printf("failed to accept the connection..\n");
            }
            /* If it connects, read in and reply to the client */
            else {
                printf("Client connected successfully\n");

                ssl = wolfSSL_new(ctx);
                if (ssl == NULL) {
                    fprintf(stderr, "wolfSSL_new error.\n");
                    break;
                }

                /* direct our ssl to our clients connection */
                wolfSSL_set_fd(ssl, clientFd);

                printf("Using Non-Blocking I/O: %d\n",
                    wolfSSL_get_using_nonblock(ssl));

                for ( ; ; ) {
                    /* Clear the buffer memory for anything  possibly left over */
                    memset(&buff, 0, sizeof(buff));

                    /* Read the client data into our buff array */
					ret = wolfSSL_read(ssl, buff, sizeof(buff) - 1);
                    if (ret > 0) {
                        /* Print any data the client sends to the console */
                        printf("Client: %s\n", buff);

                        /* Reply back to the client */
						ret = wolfSSL_write(ssl, reply, sizeof(reply) - 1);
                        if (ret < 0) {
                            printf("wolfSSL_write error = %d\n",
                                wolfSSL_get_error(ssl, ret));
                            gServerExit = 1;
                            break;
                        }
                    }
                    /* if the client disconnects break the loop */
                    else {
                        if (ret < 0)
                            printf("wolfSSL_read error = %d\n",
                                wolfSSL_get_error(ssl, ret));
                        else if (ret == 0)
                            printf("The client has closed the connection.\n");
                        gServerExit = 1;
                        break;
                    }
                }
                wolfSSL_free(ssl);           /* Free the WOLFSSL object */
                ssl = NULL;
            }
            close(clientFd);               /* close the connected socket */
            clientFd = -1;
        }
    } /* while */

exit:
    if (clientFd != -1)
        close(clientFd);
    if (sockFd != -1)
        close(sockFd);
    wolfSSL_free(ssl);       /* Free the WOLFSSL object */
    wolfSSL_CTX_free(ctx);   /* Free WOLFSSL_CTX */

    return ret;
}

/*****************************************************************************
 * TLS Local Test
 ****************************************************************************/
static void wolfSSLLocalServerThread(void* param)
{
    int port = (int)((int*)param);
    wolfExample_TLSServer(port);
}

int wolfExample_TLSLocal(int port)
{
    int ret;
    RTHANDLE srvHandle;

    /* start server thread */
    srvHandle = CreateRtThread(THREAD_BASE_PRIO + 10,
        (LPPROC)wolfSSLLocalServerThread, WOLF_EXAMPLES_STACK, (void*)port);
    if (srvHandle == BAD_RTHANDLE) {
        Fail("Cannot create server thread");
        return -1;
    }

    /* wait for server to be ready */
    while (gServerReady != 1) {
        RtSleep(0);
    }

    /* run client */
    ret = wolfExample_TLSClient(TLS_HOST_LOCAL, port);

    return ret;
}


/*****************************************************************************
 * Thread
        memset(&args, 0, sizeof(args));
 ****************************************************************************/
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

static void wolfExampleThread(void* param)
{
    func_args args;

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    /* initialize wolfSSL */
    wolfSSL_Init();

    while (1) {
        char rc;

        gServerExit = 0;
        gServerReady = 0;

        printf("\r\n\t\t\t\tMENU\r\n");
        printf(menu1);
        printf("Please select one of the above options: ");

        rc = getchar();
        switch (rc) {
            case 't':
                printf("\nCrypt Test\n");
                wolfcrypt_test(&args);
                printf("Crypt Test: Return code %d\n", args.return_code);
                break;

            case 'b':
                printf("\nBenchmark Test\n");
                benchmark_test(&args);
                printf("Benchmark Test: Return code %d\n", args.return_code);
                break;

            case 'c':
                wolfExample_TLSClient(TLS_HOST_REMOTE, TLS_PORT);
                break;

            case 's':
                wolfExample_TLSServer(TLS_PORT);
                break;

            case 'l':
                wolfExample_TLSLocal(TLS_PORT);
                break;

            // All other cases go here
            default:
                if (rc != '\r' && rc != '\n')
                    printf("\r\nSelection %c out of range\r\n", rc);
                break;
        }
    }

    wolfSSL_Cleanup();
}


/*****************************************************************************
* FUNCTION:		Catalog
*
* PARAMETERS:	1. handle of the process whose object directory must be used
*				2. the object whose handle must be cataloged
*				3. the name to be used (upto 14 characters)
*
* RETURNS:		TRUE on success
*
* DESCRIPTION:	If the given name already exists,
*				and the existing name refers to a non-existing object,
*				then the existing name is removed before cataloging.
\*****************************************************************************/
BOOLEAN Catalog(
	RTHANDLE			hProcess,
	RTHANDLE			hObject,
	LPSTR				lpszName)
{
	RTHANDLE		hOld;

	if (CatalogRtHandle(hProcess, hObject, lpszName))
		return TRUE;

	// something wrong: check for the case mentioned above
	if (((hOld = LookupRtHandle(hProcess, lpszName, NO_WAIT)) != BAD_RTHANDLE) &&
		(GetRtHandleType(hOld) == INVALID_TYPE))
	{
		// this is the case mentioned above: remove the old entry and try again
		if (UncatalogRtHandle(hProcess, lpszName))
			return (CatalogRtHandle(hProcess, hObject, lpszName));
	}
	return FALSE;
}

/*****************************************************************************
* FUNCTION:   Cleanup (local function)
*
* DESCRIPTION:
*  Tell threads to delete themselves and wait a while;
*  if any thread still exists, kill it.
*  Remove all other objects as far as they have been created.
\*****************************************************************************/
void Cleanup(void)
{
	// indicate that we are cleaning up
	gInit.state		= CLEANUP_BUSY;
	gInit.bShutdown = TRUE;

#ifdef _DEBUG
  fprintf(stderr, "wolfExamples started cleaning up\n");
#endif

	// remove our name from the root process
	if (gInit.bCataloged) {
		if (!UncatalogRtHandle(hRootProcess, "wolfExample"))
			Fail("Cannot remove my own name");
    }

#ifdef _DEBUG
	fprintf(stderr, "wolfExamples finished cleaning up\n");
#endif

	// lie down
	exit(0);
}

/*****************************************************************************
* FUNCTION:     	Fail
*
* PARAMETERS:   	same parameters as expected by printf
*
* DESCRIPTION:
*  If in debug mode, prints the message, appending a new line and the error number.
*  Then the current process is killed graciously:
*  If the current thread is the main thread, this is done directly.
*  if the current thread is another one, a terminate request is sent and
*  the function returns to the calling thread.
\*****************************************************************************/
void Fail(LPSTR lpszMessage, ...)
{
	EXCEPTION		eh;
	RTHANDLE		hDelMbx;
	DWORD			dwTerminate;

#ifdef _DEBUG
	va_list			ap;

	va_start(ap, lpszMessage);
	vfprintf(stderr, lpszMessage, ap);
	va_end(ap);
	fprintf(stderr, "\nError nr=%x %s\n", GetLastRtError(), GetRtErrorText(GetLastRtError()));
#endif

	// make sure that exceptions are returned for inline handling
	GetRtExceptionHandlerInfo(THREAD_HANDLER, &eh);
	eh.ExceptionMode = 0;
	SetRtExceptionHandler(&eh);

	// if we had not started initializing yet, just get out
	if (BEFORE_INIT == gInit.state)
		exit(0);

	if (gInit.hMain == GetRtThreadHandles(THIS_THREAD))
	{
		// this is the main thread:
		// if we are busy initializing, then do Cleanup
		if (INIT_BUSY == gInit.state)
			Cleanup();  // does not return

		// this is the main thread, but we are not initializing: just return
		return;
	}

	// this is not the main thread:
	// ask main thread to do cleanup
	// (allow some time to setup the deletion mailbox, ignore errors)
	hDelMbx			= LookupRtHandle(NULL_RTHANDLE, "R?EXIT_MBOX", 5000);
	dwTerminate		= TERMINATE;
	SendRtData(hDelMbx, &dwTerminate, 4);
}

/*****************************************************************************
*
* FUNCTION:		UsecsToKticks
*
* PARAMETERS:	1. number of usecs
*
* RETURNS:		number of low level ticks
*
* DESCRIPTION:	returns the parameter if it is WAIT_FOREVER
*				otherwise rounds up to number of low level ticks
\*****************************************************************************/
DWORD UsecsToKticks(DWORD dwUsecs)
{
	if (dwUsecs == WAIT_FOREVER)
		return WAIT_FOREVER;

	return (dwUsecs + dwKtickInUsecs - 1) / dwKtickInUsecs;
}


/*****************************************************************************
* FUNCTION:         main
*
* DESCRIPTION:
*  This is the main program module.
*  It creates global objects  and all threads.
*  The main thread then waits for notifications and acts accordingly
\*****************************************************************************/
int main(int argc, char* argv[])
{
    SYSINFO         sysinfo;
    EVENTINFO       eiEventInfo;
    RTHANDLE        taskHandle;

#ifdef _DEBUG
    fprintf(stderr, "wolfExamples started\n");
#endif

    // obtain handle of root process (cannot fail)
    hRootProcess    = GetRtThreadHandles(ROOT_PROCESS);

    // initialize the structure for cleaning up
    memset(&gInit, 0, sizeof(gInit));
    gInit.state     = BEFORE_INIT;

    // get low level tick length in usecs
    if (!CopyRtSystemInfo(&sysinfo))
        Fail("Cannot copy system info");
    dwKtickInUsecs  = 10000 / sysinfo.KernelTickRatio;
    if (dwKtickInUsecs == 0)
        Fail("Invalid low level tick length");

    // adjust process max priority (ignore error)
    // TODO adjust the 2nd parameter to a value closer to zero if you want to allow more priorities
    SetRtProcessMaxPriority(NULL_RTHANDLE, THREAD_BASE_PRIO);

    // obtain main thread's handle
    gInit.hMain     = GetRtThreadHandles(THIS_THREAD);
    gInit.state     = INIT_BUSY;

    // attempt to catalog the thread but ignore error
    Catalog(NULL_RTHANDLE, gInit.hMain, "TMain");

    // catalog the handle of this process in the root process
    if (!Catalog(hRootProcess, GetRtThreadHandles(THIS_PROCESS), "wolfExample")) {
        Fail("Cannot catalog process name");
    }
    gInit.bCataloged = TRUE;

    // create thread
    taskHandle = CreateRtThread(THREAD_BASE_PRIO + 20,
        (LPPROC)wolfExampleThread, WOLF_EXAMPLES_STACK, 0);
    if (taskHandle == BAD_RTHANDLE) {
        Fail("Cannot create thread");
    }

    // indicate that initialization has finished
    gInit.state     = INIT_DONE;
#ifdef _DEBUG
    fprintf(stderr, "wolfExamples finished initialization\n");
#endif

    // wait for notifications
    while (RtNotifyEvent(RT_SYSTEM_NOTIFICATIONS | RT_EXIT_NOTIFICATIONS,
        WAIT_FOREVER, &eiEventInfo))
    {
        switch(eiEventInfo.dwNotifyType)
        {
        case TERMINATE:
            // TODO: this process should terminate
            // cleanup the environment
            Cleanup();  // does not return

        case NT_HOST_UP:
            // TODO: react to a Windows host that has come back
            break;

        case NT_BLUESCREEN:
            // TODO: react to a Windows blue screen
            break;

        case KERNEL_STOPPING:
            // TODO: react to the INtime kernel stopping
            break;

        case NT_HOST_HIBERNATE:
            // TODO: react to the Windows host going in hibernation
            break;

        case NT_HOST_STANDBY:
            // TODO: react to the Windows host going in standby mode
            break;

        case NT_HOST_SHUTDOWN_PENDING:
            // TODO: react to a Windows host that is about to shutdown
            break;
        }
    }
    Fail("Notify failed");
    return 0;
}
