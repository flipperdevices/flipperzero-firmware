/*shell.c
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


 /*** tiny Shell for wolfSSL apps ***/

 #ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/logging.h>

#include <stdio.h>

#if defined(WOLFSSL_MDK_ARM)
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <rtl.h>
    #include "wolfssl_MDK_ARM.h"
#endif

#ifdef WOLFSSL_KEIL_NET
#include "wolfassl/test.h"
#else
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;
#endif

#ifdef NO_ECHOCLIENT
#define echoclient_test command_not_found
#endif
#ifdef NO_ECHOSERVER
#define echoserver_test command_not_found
#endif
#ifdef NO_SIMPLE_CLIENT
#define client_test command_not_found
#endif
#ifdef NO_SIMPLE_SERVER
#define server_test command_not_found
#endif
#ifdef NO_CRYPT_BENCHMARK
#define benchmark_test command_not_found
#endif
#ifdef NO_CRYPT_TEST
#define ctaocrypt_test command_not_found
#endif

#ifndef WOLFSSL_KEIL_NET
#define ipaddr_comm command_not_found
#endif

#if !defined(HAVE_KEIL_RTX)
#define stack_comm command_not_found
#endif


#if !defined(DEBUG_WOLFSSL)
#define dbg_comm command_not_found
#endif


void command_not_found(void *argv) {
        printf("Command not found\n") ;
}

extern void echoclient_test(void *args) ;
extern void echoserver_test(void *args) ;
extern void benchmark_test(void *args) ;
extern void wolfcrypt_test(void *args) ;
extern void client_test(void *args) ;
extern void server_test(void *args) ;
extern void kill_task(void *args) ;

extern void ipaddr_comm(void *args) ;
extern void stack_comm(void *args) ;
extern void for_command(void *args) ;
extern void dbg_comm(void *arg) ;
extern void help_comm(void *arg) ;

#if !defined(NO_CRYPT_TEST)

#ifndef NO_MD5
extern void md5_test(void *arg) ;
#endif
#ifdef WOLFSSL_MD2
extern void md2_test(void *arg) ;
#endif
#ifndef NO_MD4
extern void md4_test(void *arg) ;
#endif

extern void sha_test(void *arg) ;

#ifndef NO_SHA256
extern void sha256_test(void *arg) ;
#endif
#ifdef WOLFSSL_SHA384
extern void sha384_test(void *arg) ;
#endif

#ifdef WOLFSSL_SHA512
extern void sha512_test(void *arg) ;
#endif

#ifdef WOLFSSL_RIPEMD
extern void ripemd_test(void *arg) ;
#endif
#ifndef NO_HMAC
    #ifndef NO_MD5
extern void hmac_md5_test(void *arg) ;
    #endif
extern void hmac_sha_test(void *arg) ;

    #ifndef NO_SHA256
extern void hmac_sha256_test(void *arg) ;
    #endif

    #ifdef WOLFSSL_SHA384
extern void hmac_sha384_test(void *arg) ;
    #endif
#endif
#ifndef NO_RC4
extern void arc4_test(void *arg) ;
#endif

#ifndef NO_DES3
extern void des_test(void *arg) ;
extern void des3_test(void *arg) ;
#endif

#ifndef NO_AES
extern void aes_test(void *arg) ;
#ifdef HAVE_AESGCM
extern void aesgcm_test(void *arg) ;
#endif

#ifdef HAVE_AESCCM
extern void aesccm_test(void *arg) ;
#endif
#endif

#ifdef HAVE_CAMELLIA
extern void camellia_test(void *arg) ;
#endif
extern void random_test(void *arg) ;

#ifndef NO_RSA
extern void rsa_test(void *arg) ;
#endif

#ifndef NO_DH
extern void dh_test(void *arg) ;
#endif

#ifndef NO_DSA
extern void dsa_test(void *arg) ;
#endif

#ifndef NO_PWDBASED
extern void pwdbased_test(void *arg) ;
#endif

#ifdef HAVE_ECC
extern void openssl_test(void *arg) ;
#endif

#ifdef HAVE_ECC
extern void ecc_test(void *arg) ;
#endif

#endif /* NO_CRYPT_TEST */

static struct {
  const char *command ;
    void (*func)(void *args) ;
}   commandTable[] = {
    "echoclient", echoclient_test,
    "echoserver", echoserver_test,
    "benchmark", benchmark_test,
    "test", wolfcrypt_test,
    "client", client_test,
    "server", server_test,
    "ipaddr", ipaddr_comm,      /* TBD */
    "stack", stack_comm,        /* On/Off check stack size */
    "for", for_command,         /* iterate next command X times */
    "debug", dbg_comm,          /* On/Off debug message  */
    "help", help_comm,          /* Brief description about the commands */

    /** short name **/
    "ec", echoclient_test,
    "es", echoserver_test,
    "bm", benchmark_test,
    "te", wolfcrypt_test,
    "cl", client_test,
    "sv", server_test,
    "ip", ipaddr_comm,
    "st", stack_comm,
  "dbg", dbg_comm,
    "?",    help_comm,

/*** test suites ****/
#if !defined(NO_CRYPT_TEST)
#ifndef NO_MD5
  "md5",  md5_test,
#endif
#ifdef WOLFSSL_MD2
  "md2",  md2_test,
#endif
#ifndef NO_MD4
  "md4",  md4_test,
#endif
  "sha",  sha_test,
#ifndef NO_SHA256
  "sha256",  sha256_test,
#endif
#ifdef WOLFSSL_SHA384
  "sha384",  sha384_test,
#endif
#ifdef WOLFSSL_SHA512
  "sha512",  sha512_test,
#endif
#ifdef WOLFSSL_RIPEMD
  "ripemd",  ripemd_test,
#endif
#ifndef NO_HMAC
  #ifndef NO_MD5
  "hmac_md5",  hmac_md5_test,
    #endif
  "hmac_sha",  hmac_sha_test,
    #ifndef NO_SHA256
  "hmac_sha256",  hmac_sha256_test,
    #endif
    #ifdef WOLFSSL_SHA384
  "hmac_sha384",  hmac_sha384_test,
  #endif
#endif
#ifndef NO_RC4
    "arc4",  arc4_test,
#endif
#ifndef NO_DES3
  "des",  des_test,
  "des3",  des3_test,
#endif
#ifndef NO_AES
  "aes",  aes_test,
    #ifdef HAVE_AESGCM
  "aesgcm",  aesgcm_test,
    #endif
    #ifdef HAVE_AESCCM
  "aesccm",  aesccm_test,
    #endif
#endif

#ifdef HAVE_CAMELLIA
  "camellia",  camellia_test,
#endif
  "random",  random_test,
#ifndef NO_RSA
  "rsa",  rsa_test,
#endif
#ifndef NO_DH
  "dh",  dh_test,
#endif
#ifndef NO_DSA
    "dsa",  dsa_test,
#endif
#ifndef NO_PWDBASED
  "pwdbased",  pwdbased_test,
#endif
#ifdef OPENSSL_EXTRA
  "openssl",  openssl_test,
#endif
#ifdef HAVE_ECC
  "ecc",  ecc_test,
#endif

#endif /* NO_CRYPT_TEST */

    "",  NULL
} ;

enum jobtype { FOREGROUND, BACKGROUND }  ;

#define IF_DELIMITER(ch) ((ch) == ' ' || (ch) == '\n')

/*******  Get Command Line *****************************/
static int getline(char * line, int sz, func_args *args, int*bf_flg)
{
    char * ret ;
    int i ;

    #define MAXARGS 10
    #define MAXARGLEN 30
    static char *argv[MAXARGS] ;
    args->argv = argv ;

    putchar('>') ;
    fflush(stdout) ;
    ret = fgets(line, sz, stdin) ;
    #define SHELL_ERROR_FGETS -102
    if(ret != line) return(SHELL_ERROR_FGETS) ;

    if(line[strlen(line)-2] == '&') {
        (*bf_flg) = BACKGROUND ;
        line[strlen(line)-2] = '\n' ;
    } else {
        (*bf_flg) = FOREGROUND ;
    }
    args->argc = 0 ;
    for(i=0; i<sz; i++) {
        args->argv[args->argc] = &(line[i]) ;
        while(!IF_DELIMITER(line[i])) i++ ;
        args->argc++ ;
        if(line[i] == '\n') {
            line[i]  = '\0' ;
            break ;
        } else {
            line[i]  = '\0' ;
        }
    }
    return i ;
}

static int BackGround = 0 ; /* 1: background job is running */

/************* Embedded Shell Commands **********************************/
#define IP_SIZE 16

#ifdef WOLFSSL_KEIL_NET
static void ipaddr_comm(void *args)
{
    if(((func_args *)args)->argc == 1) {
            printf("IP addr: %s, port %d\n", wolfSSLIP, wolfSSLPort) ;
    } else {
        if(BackGround != 0) {
        printf("Cannot change IP addr while background server is running\n") ;
        } else if(((func_args *)args)->argc == 3 &&
                  ((func_args *)args)->argv[1][0] == '-'&&
                  ((func_args *)args)->argv[1][1] == 'a' ) {
/*          strcpy(wolfSSLIP, ((func_args *)args)->argv[2]) ; */
        } else if(((func_args *)args)->argc == 3 &&
                  ((func_args *)args)->argv[1][0] == '-' &&
                  ((func_args *)args)->argv[1][1] == 'p' ) {
/*          yasslPort = atoi(((func_args *)args)->argv[2]) ; */
        } else printf("Invalid argument\n") ;
    }
}

#endif



#if defined(HAVE_KEIL_RTX)
static int stack_ck = 0 ;

static void stack_comm(void *args)
{
    if(stack_ck) {
        printf("Stack Check: Off\n") ;
        stack_ck = 0 ;
    } else {
        printf("Stack Check: On\n") ;
        stack_ck = 1 ;
    }
}

#define FILL_PATTERN 0xa596695a
void stack_fill(char * stack, int size)
{
    int i ;

    if(stack_ck == 0)return ;
    for(i=1; i<size/4-10; i++)
        ((int *)stack)[i] = FILL_PATTERN ;
}

void stack_check(char * stack, int size)
{
    int i ;

    if(stack_ck == 0)return ;
    if(stack_ck == 1) {
        stack_ck ++ ; return ;
    }
    for(i=1; i<size/4 ; i++) {
        if(((int *)stack)[i] != FILL_PATTERN) break ;
    }
    if(i < size/4) {
        printf("Stack is used %d bytes out of %d\n", size - i*4, size) ;
    } else {
        printf("Stack overflow. Stack size: %d\n", size) ;
    }
}

#endif /* HAVE_KEIL_RTX */

static int for_iteration = 1 ;

static void for_command(void *args)
{
    if( args == NULL || ((func_args *)args)->argc == 1) {
        printf("For %d times\n", for_iteration) ;
    } else if(((func_args *)args)->argc == 2) {
        for_iteration = atoi(((func_args *)args)->argv[1]) ;
    } else printf("Invalid argument\n") ;
}


#if defined(DEBUG_WOLFSSL)

static int wolfsslDebug = 1 ;

static void dbg_comm(void *args)
{
    if(wolfsslDebug == 1) {
        wolfsslDebug = 0 ;
        printf("Turning OFF Debug message\n") ;
        wolfSSL_Debugging_OFF() ;
    } else {
        wolfsslDebug = 1 ;
        printf("Turning ON Debug message\n") ;
        wolfSSL_Debugging_ON() ;
    }
}
#endif

static void help_comm(void *args)
{

}



#define BG_JOB_STACK_SIZE 16000
#if (!defined(NO_SIMPLE_SERVER) && !defined(NO_ECHOSERVER)) && \
                                                   defined(HAVE_KEIL_RTX)
static char bg_job_stack[BG_JOB_STACK_SIZE] ;
#endif

#define COMMAND_STACK_SIZE 16000
#if defined(HAVE_KEIL_RTX)
static char command_stack[COMMAND_STACK_SIZE] ;
#endif


#ifdef  HAVE_KEIL_RTX
static   wolfSSL_Mutex command_mutex ;
#endif

/***********    Invoke Foreground Command  *********************/
static void command_invoke(void *args)
{
    void (*func)(void * ) ;
    int i,iteration ;

    func = (void(*)(void *))((func_args *)args)->argv[0] ;
    #ifdef  HAVE_KEIL_RTX
    wc_LockMutex((wolfSSL_Mutex *)&command_mutex) ;
    #endif
    iteration = for_iteration ;
    for(i=0; i< iteration; i++) {
        if(iteration > 1) printf("--- Start for %d ---->\n", i) ;
        #if defined(HAVE_KEIL_RTX)
        stack_fill(command_stack, COMMAND_STACK_SIZE) ;
        #endif

        func(args) ;        /* invoke command */

        #if defined(HAVE_KEIL_RTX)
        stack_check(command_stack, COMMAND_STACK_SIZE) ;
        #endif
    }
    if(iteration > 1)
        for_iteration = 1 ;
    #ifdef HAVE_KEIL_RTX
    wc_UnLockMutex((wolfSSL_Mutex *)&command_mutex) ;
    os_tsk_delete_self() ;
    #endif
}

#if (!defined(NO_SIMPLE_SERVER) && !defined(NO_ECHOSERVER)) && \
                                                      defined(HAVE_KEIL_RTX)
/*******  Invoke Background Job   *******************************/
static void bg_job_invoke(void *args)
{
    void (*func)(void * ) ;
    BackGround = 1 ;
    stack_fill(bg_job_stack, BG_JOB_STACK_SIZE) ;
    func = (void(*)(void *))((func_args *)args)->argv[0] ;
    func(args) ;        /* invoke command */
    stack_check(bg_job_stack, BG_JOB_STACK_SIZE) ;
    #ifdef WOLFSSL_KEIL_NET
    init_TcpNet ();
    #endif
    BackGround = 0 ;
    os_tsk_delete_self() ; ;
}
#endif

#define LINESIZE 100
static char line[LINESIZE] ;


/********* SHEULL MAIN LOOP ***********************************/
void shell_main(void) {
    int i ;
    func_args args ;
    int bf_flg ;

    i = BackGround ;
        /* Dummy for avoiding warning: BackGround is defined but not used. */


 #if defined(HAVE_KEIL_RTX)
    wc_InitMutex(&command_mutex) ;
#endif
    printf("Starting Shell\n") ;
    while(1) {
        if(getline(line,  LINESIZE, &args, &bf_flg) > 0) {
        for(i=0; commandTable[i].func != NULL; i++) {
            if(strcmp(commandTable[i].command, args.argv[0]) == 0) {
            args.argv[0] = (char *) commandTable[i].func ;
                if(bf_flg == FOREGROUND) {
                    #ifdef  HAVE_KEIL_RTX
                    wc_UnLockMutex((wolfSSL_Mutex *)&command_mutex) ;
                    os_tsk_create_user_ex( (void(*)(void *))&command_invoke, 7,
                            command_stack, COMMAND_STACK_SIZE, &args) ;
                    #else
                    command_invoke(&args) ;
                    #endif
                    #ifdef  HAVE_KEIL_RTX
                    wc_LockMutex((wolfSSL_Mutex *)&command_mutex) ;
                    #endif
                } else {
                    #if (!defined(NO_SIMPLE_SERVER) && \
                         !defined(NO_ECHOSERVER)) && \
                         defined(HAVE_KEIL_RTX)
                    if(BackGround != 0) {
                        printf("Multiple background servers not supported.\n") ;
                    } else {
                        printf("\"%s\" is running with the background mode.\n",
                                                     commandTable[i].command) ;
                        os_tsk_create_user_ex( (void(*)(void *))&bg_job_invoke,
                                   6, bg_job_stack, BG_JOB_STACK_SIZE, &args) ;
                    }
                    #else
                    printf("Invalid Command: no background job\n") ;
                    #endif
                }
                break ;
            }
        }
        if(commandTable[i].func == NULL)
            printf("Command not found\n") ;
        }
    }
}

