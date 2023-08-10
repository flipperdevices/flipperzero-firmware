#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <wolfssl/ssl.h>
#include <ifaddrs.h>
#include <applibs/log.h>

#define _GNU_SOURCE /* defines NI_NUMERICHOST */
#ifndef NI_MAXHOST
        #define NI_MAXHOST 256
#endif

static void util_Cleanup(int sockfd, WOLFSSL_CTX* ctx, WOLFSSL* ssl)
{
    wolfSSL_free(ssl);      /* Free the wolfSSL object                  */
    wolfSSL_CTX_free(ctx);  /* Free the wolfSSL context object          */
    wolfSSL_Cleanup();      /* Cleanup the wolfSSL environment          */
    close(sockfd);          /* Close the connection to the server       */
}

/* Displays each AF_INET interface and it's IP Address
*  Return: WOLFSSL_SUCCESS if print is successful else WOLFSSL_FAILURE
*/
static int util_PrintIfAddr(void)
{
    char host[NI_MAXHOST];
    struct ifaddrs* ifaddr, * nxt;
    int family, info, n;

    /* Get a linked list of 'struct ifaddrs*' */
    if (getifaddrs(&ifaddr) != 0) {
        fprintf(stderr, "ERROR: Getting network interface and IP address");
        return WOLFSSL_FAILURE;
    }
    printf("\nInterface   IP Address\n");
    /* Traverse ifaddr linked list using nxt */
    for (nxt = ifaddr; nxt != NULL; nxt = nxt->ifa_next) {
        if (nxt->ifa_addr == NULL)
            continue;
        family = nxt->ifa_addr->sa_family;
        /* Display the address of each AF_INET* interface */
        if (family == AF_INET) {
            info = getnameinfo(nxt->ifa_addr, sizeof(struct sockaddr_in),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (info != 0) {
                    fprintf(stderr, "Failed to getnameinfo");
                    freeifaddrs(ifaddr);
                    return WOLFSSL_FAILURE;
            }
            /* Determine amount of space, n, to justify IP Address */
            n = (int)strlen("Interface   ") - (int)strlen(nxt->ifa_name);
            n = (n > 0) ? n : 1; /* Set space to 1 if n is negative */
            printf("%s %*c%s>\n", nxt->ifa_name, n, '<', host);
        }
    }
    printf("\n");
    freeifaddrs(ifaddr);
    return WOLFSSL_SUCCESS;
}
#endif
