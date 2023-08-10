/* wolfssl_KEIL_RL.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


/***************************************************************************************/
/**   This file is for defining functions for specific to KEIL-RL.                **/
/***************************************************************************************/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_MDK_ARM)
        #include <stdio.h>
        #include <string.h>

        #if defined(WOLFSSL_MDK5)
            #include "cmsis_os.h"
            #include "rl_fs.h"
            #include "rl_net.h"
        #else
            #include "rtl.h"
        #endif

        #include "wolfssl_MDK_ARM.h"
#endif

#include "wolfssl_MDK_ARM.h"

#include <wolfssl/wolfcrypt/visibility.h>
#include <wolfssl/wolfcrypt/logging.h>

#if defined (WOLFSSL_CMSIS_RTOS)
        #define os_dly_wait(t)    osDelay(10*t)
#endif


/** KEIL-RL TCPnet ****/
/** TCPnet BSD socket does not have following functions. **/

#if defined(WOLFSSL_KEIL_TCP_NET)
char *inet_ntoa(struct in_addr in)
{
    #define NAMESIZE 16
    static char name[NAMESIZE] ;
    sprintf(name, "%d.%d.%d.%d", (in.s_addr>>24)&0xff, (in.s_addr>>16)&0xff, (in.s_addr>>8)&0xff, in.s_addr&0xff) ;
    return name ;
}

unsigned long inet_addr(const char *cp)
{
    unsigned int a[4] ; unsigned long ret ;
    sscanf(cp, "%u.%u.%u.%u", &a[0], &a[1], &a[2], &a[3]) ;
    ret = ((a[3]<<24) + (a[2]<<16) + (a[1]<<8) + a[0]) ;
    return(ret) ;
}


/*** tcp_connect is actually associated with following syassl_tcp_connect. ***/
int wolfssl_connect(int sd, const  struct sockaddr* sa, int sz)
{
    int ret = 0 ;
    #if defined(WOLFSSL_KEIL_TCP_NET)

    SOCKADDR_IN addr ;

    addr = *(SOCKADDR_IN *)sa ;

    do {
        #undef connect  /* Go to KEIL TCPnet connect */
        ret = connect(sd, (SOCKADDR *)&addr, sizeof(addr)) ;
        os_dly_wait(50);
    } while(ret == SCK_EWOULDBLOCK) ;
    #ifdef DEBUG_WOLFSSL
    {
        char msg[50] ;
        sprintf(msg, "BSD Connect return code: %d\n", ret) ;
        WOLFSSL_MSG(msg) ;
    }
    #endif

    #endif /* WOLFSSL_KEIL_TCP_NET */
    return(ret ) ;
}


int wolfssl_accept(int sd, struct sockaddr *addr, int *addrlen)
{
    int ret = 0 ;

    #if defined(WOLFSSL_KEIL_TCP_NET)
    while(1) {
        #undef accept  /* Go to KEIL TCPnet accept */
        ret = accept(sd, addr,  addrlen) ;
        if(ret != SCK_EWOULDBLOCK) break ;
        os_dly_wait(1);
    }
    #ifdef DEBUG_WOLFSSL
    {
        char msg[50] ;
        sprintf(msg, "BSD Accept return code: %d\n", ret) ;
        WOLFSSL_MSG(msg) ;
    }
    #endif

    #endif /* WOLFSSL_KEIL_TCP_NET */
    return(ret ) ;

}

int wolfssl_recv(int sd, void *buf, size_t len, int flags)
{
    int ret  = 0;
    #if defined(WOLFSSL_KEIL_TCP_NET)
    while(1) {
        #undef recv  /* Go to KEIL TCPnet recv */
        ret = recv(sd, buf, len,  flags) ;
        if((ret != SCK_EWOULDBLOCK) &&( ret != SCK_ETIMEOUT)) break ;
        os_dly_wait(1);
    }
    #ifdef DEBUG_WOLFSSL
    {
        char msg[50] ;
        sprintf(msg, "BSD Recv return code: %d\n", ret) ;
        WOLFSSL_MSG(msg) ;
    }
    #endif

    #endif  /* WOLFSSL_KEIL_TCP_NET */
    return(ret ) ;
}

int wolfssl_send(int sd, const void *buf, size_t len, int flags)
{
    int  ret = 0 ;

    #if defined(WOLFSSL_KEIL_TCP_NET)
    while(1) {
    #undef send  /* Go to KEIL TCPnet send */
        ret = send(sd, buf, len,  flags) ;
        if(ret != SCK_EWOULDBLOCK) break ;
        os_dly_wait(1);
    }
    #ifdef DEBUG_WOLFSSL
    {
        char msg[50] ;
        sprintf(msg, "BSD Send return code: %d\n", ret) ;
        WOLFSSL_MSG(msg) ;
    }
    #endif

#endif  /* WOLFSSL_KEIL_TCP_NET */
    return(ret) ;

}

#endif /* WOLFSSL_KEIL_TCP_NET */

#if defined(WOLFSSL_KEIL_TCP_NET)
void wolfssl_sleep(int t)
{
    #if defined(HAVE_KEIL_RTX)
    os_dly_wait(t/1000+1) ;
    #endif
}

int wolfssl_tcp_select(int sd, int timeout)
{

    return 0 ;

}
#endif

FILE * wolfSSL_fopen(const char *name, const char *openmode)
{
    int i ;  FILE * ret ;
    #define PATHSIZE 100
    char path[PATHSIZE] ; char *p ;

    if (name == NULL) return NULL;
    if(strlen(name) > PATHSIZE)return(NULL) ;

    for(i = 0; i<= strlen(name); i++) {
        if(name[i] == '/')path[i] = '\\' ;
        else              path[i] = name[i] ;
    }
    if(path[0] == '.' && path[1] == '\\') p = path + 2 ;
    else                                  p = path ;

    ret = fopen (p, openmode) ;

    return(ret) ;
}

#define getkey getchar
#define sendchar putchar

char * wolfssl_fgets ( char * str, int num, FILE * f )
{
    int i ;

    for(i = 0 ; i< num ; i++) {
            while((str[i] = getkey()) == 0) {
            #if defined (HAVE_KEIL_RTX) && !defined(WOLFSSL_CMSIS_RTOS)
                os_tsk_pass ();
            #elif defined(WOLFSSL_CMSIS_RTOS)
                osThreadYield ();
            #endif
        }
        if(str[i] == '\n' || str[i] == '\012' || str[i] == '\015')  {
            sendchar('\n') ;
            str[i++] = '\n' ;
            str[i] = '\0' ;
            break ;
        } else if(str[i] == '\010') { /* BS */
            if(i) { /* erace one char */
                sendchar('\010') ; sendchar(' ') ; sendchar('\010') ;
                i = (i>0 ? (i-2) : -1 ) ;
                continue ;
            }
        } else if(str[i] == '\033'  || str[i] == '\004' ) {  /* ESC or ^D */
            str[i] = '\0' ;
            return(0) ;
        }
        sendchar(str[i]) ;
    }
    return(str) ;
}
