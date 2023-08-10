
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif
 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rt_sys.h>


#include <File_Config.h>

#pragma import(__use_no_semihosting_swi)

/* The following macro definitions may be used to translate this file:

  STDIO - use standard Input/Output device
          (default is NOT used)
 */

/* Standard IO device handles. */
#define STDIN   0x8001
#define STDOUT  0x8002
#define STDERR  0x8003

/* Standard IO device name defines. */
const char __stdin_name[]  = "STDIN";
const char __stdout_name[] = "STDOUT";
const char __stderr_name[] = "STDERR";

struct __FILE { int handle; /* Add whatever you need here */ };

#ifdef STDIO
extern int  SER_GetChar   (void);
extern int  SER_PutChar   (int ch);

/*-----------------------------------------------------------------------------
  Write character to the Serial Port
 *----------------------------------------------------------------------------*/
int sendchar (int c) 
{
    if (c == '\n')  {
        SER_PutChar ('\r');
    }
    SER_PutChar (c);
    return (c);
}


/*-----------------------------------------------------------------------------
  Read character from the Serial Port
 *----------------------------------------------------------------------------*/
int getkey (void) 
{
    int ch = SER_GetChar();
        #if defined (HAVE_KEIL_RTX)
        os_itv_wait ();
        #endif
    if (ch < 0) {
        return 0;
    }
    return ch;
}
#endif

/*--------------------------- _ttywrch ---------------------------------------*/

void _ttywrch (int ch) 
{
#ifdef STDIO
    sendchar (ch);
#endif
}

/*--------------------------- _sys_open --------------------------------------*/
#ifndef NO_FILESYSTEM
static int KEIL_FS_open(const char *name, int openmode) 
{
    int i ;  int ret ;
    #define PATHSIZE 100
    char path[PATHSIZE] ; char *p ;
    
    if(strlen(name) > PATHSIZE)return(-1) ;
    
    for(i = 0; i<= strlen(name); i++) {
        if(name[i] == '/')path[i] = '\\' ;
        else              path[i] = name[i] ;
    }       
    if(path[0] == '.' && path[1] == '\\') p = path + 2 ;
    else                                  p = path ;

    ret = __sys_open (p, openmode) ;
    
    return(ret) ;
}
#endif

FILEHANDLE _sys_open (const char *name, int openmode) 
{
    /* Register standard Input Output devices. */
    if (strcmp(name, "STDIN") == 0) {
        return (STDIN);
    }
    if (strcmp(name, "STDOUT") == 0) {
        return (STDOUT);
    }
    if (strcmp(name, "STDERR") == 0) {
        return (STDERR);
    }
    #ifndef NO_FILESYSTEM
    return (KEIL_FS_open(name, openmode));
    #else
    return(0) ;
    #endif
}

/*--------------------------- _sys_close -------------------------------------*/

int _sys_close (FILEHANDLE fh) 
{
    if (fh > 0x8000) {
        return (0);
    }
    #ifndef NO_FILESYSTEM
    return (__sys_close (fh));
    #else
    return(0) ;
    #endif
}

/*--------------------------- _sys_write -------------------------------------*/

int _sys_write (FILEHANDLE fh, const U8 *buf, U32 len, int mode) 
{
#ifdef STDIO
    if (fh == STDOUT) {
        /* Standard Output device. */
        for (  ; len; len--) {
            sendchar (*buf++);
        }
        return (0);
    }
#endif
    if (fh > 0x8000) {
        return (-1);
    }
    #ifndef NO_FILESYSTEM
    return (__sys_write (fh, buf, len));
    #else
    return(0) ;
    #endif
}

/*--------------------------- _sys_read --------------------------------------*/

int _sys_read (FILEHANDLE fh, U8 *buf, U32 len, int mode) 
{
#ifdef STDIO
    if (fh == STDIN) {
    /* Standard Input device. */
        int sz ;
        while((buf[0] = getkey()) == 0) ;
           ;
        for (sz = 0 ; sz <= len ; sz ++ ) {
            if(buf[sz] == 0) break ;
            else sz++ ;
            buf[sz] = getkey ();
        }
        return (sz);
    }
#endif
    if (fh > 0x8000) {
        return (-1);
    }
    #ifndef NO_FILESYSTEM
    return (__sys_read (fh, buf, len));
    #else
    return(0) ;
    #endif
}

/*--------------------------- _sys_istty -------------------------------------*/

int _sys_istty (FILEHANDLE fh) 
{
    if (fh > 0x8000) {
        return (1);
    }
    return (0);
}

/*--------------------------- _sys_seek --------------------------------------*/

int _sys_seek (FILEHANDLE fh, long pos) 
{
    if (fh > 0x8000) {
        return (-1);
    }
    #ifndef NO_FILESYSTEM
    return (__sys_seek (fh, pos));
    #else
    return(0) ;
    #endif
}

/*--------------------------- _sys_ensure ------------------------------------*/

int _sys_ensure (FILEHANDLE fh) 
{
    if (fh > 0x8000) {
        return (-1);
    }
    #ifndef NO_FILESYSTEM
    return (__sys_ensure (fh));
    #else
    return(0) ;
    #endif
}

/*--------------------------- _sys_flen --------------------------------------*/

long _sys_flen (FILEHANDLE fh) 
{
    if (fh > 0x8000) {
        return (0);
    }
    #ifndef NO_FILESYSTEM
    return (__sys_flen (fh));
    #else
    return(0) ;
    #endif
}


/*--------------------------- _sys_tmpnam ------------------------------------*/

int _sys_tmpnam (char *name, int sig, unsigned maxlen) 
{
    return (1);
}

/*--------------------------- _sys_command_string ----------------------------*/

char *_sys_command_string (char *cmd, int len) 
{
    return (cmd);
}

/*--------------------------- _sys_exit --------------------------------------*/

void _sys_exit (int return_code) 
{
#ifdef WOLFSSL_MDK_SHELL
    return ;
#else
    /* Endless loop. */
    while (1);
#endif
    
}

