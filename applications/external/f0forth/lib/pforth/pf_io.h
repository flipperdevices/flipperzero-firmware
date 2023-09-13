/* @(#) pf_io.h 98/01/26 1.2 */
#ifndef _pf_io_h
#define _pf_io_h

/***************************************************************
** Include file for PForth IO
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
***************************************************************/

#include "pf_types.h"

#define PF_CHAR_XON    (0x11)
#define PF_CHAR_XOFF   (0x13)

#ifdef __cplusplus
extern "C" {
#endif
int  sdTerminalOut( char c );
int  sdTerminalEcho( char c );
int  sdTerminalFlush( void );
int  sdTerminalIn( void );
int  sdQueryTerminal( void );
void sdTerminalInit( void );
void sdTerminalTerm( void );
cell_t sdSleepMillis( cell_t msec );
#ifdef __cplusplus
}
#endif

void ioInit( void );
void ioTerm( void );

#ifdef PF_NO_CHARIO
    void sdEnableInput( void );
    void sdDisableInput( void );

#else   /* PF_NO_CHARIO */
    #ifdef PF_USER_CHARIO
/* Get user prototypes or macros from include file.
** API must match that defined above for the stubs.
*/
/* If your sdTerminalIn echos, define PF_KEY_ECHOS. */
        #include PF_USER_CHARIO
    #else
        #define sdEnableInput()     /* sdTerminalOut( PF_CHAR_XON ) */
        #define sdDisableInput()    /* sdTerminalOut( PF_CHAR_XOFF ) */

    #endif
#endif   /* PF_NO_CHARIO */

/* Define file access modes. */
/* User can #undef and re#define using PF_USER_FILEIO if needed. */
#define PF_FAM_READ_ONLY   (0)
#define PF_FAM_READ_WRITE  (1)
#define PF_FAM_WRITE_ONLY  (2)
#define PF_FAM_BINARY_FLAG (8)

#define PF_FAM_CREATE_WO      ("w")
#define PF_FAM_CREATE_RW      ("w+")
#define PF_FAM_OPEN_RO        ("r")
#define PF_FAM_OPEN_RW        ("r+")
#define PF_FAM_BIN_CREATE_WO  ("wb")
#define PF_FAM_BIN_CREATE_RW  ("wb+")
#define PF_FAM_BIN_OPEN_RO    ("rb")
#define PF_FAM_BIN_OPEN_RW    ("rb+")

#ifdef PF_NO_FILEIO

    typedef void FileStream;

    extern FileStream *PF_STDIN;
    extern FileStream *PF_STDOUT;

    #ifdef __cplusplus
    extern "C" {
    #endif

    /* Prototypes for stubs. */
    FileStream *sdOpenFile( const char *FileName, const char *Mode );
    cell_t sdFlushFile( FileStream * Stream  );
    cell_t sdReadFile( void *ptr, cell_t Size, int32_t nItems, FileStream * Stream  );
    cell_t sdWriteFile( void *ptr, cell_t Size, int32_t nItems, FileStream * Stream  );
    cell_t sdSeekFile( FileStream * Stream, file_offset_t Position, int32_t Mode );
    cell_t sdRenameFile( const char *OldName, const char *NewName );
    cell_t sdDeleteFile( const char *FileName );
    ThrowCode sdResizeFile( FileStream *, uint64_t Size);
    file_offset_t sdTellFile( FileStream * Stream );
    cell_t sdCloseFile( FileStream * Stream );
    cell_t sdInputChar( FileStream *stream );

    #ifdef __cplusplus
    }
    #endif

    #define  PF_SEEK_SET   (0)
    #define  PF_SEEK_CUR   (1)
    #define  PF_SEEK_END   (2)
    /*
    ** printf() is only used for debugging purposes.
    ** It is not required for normal operation.
    */
    #define PRT(x) /* No printf(). */

#else

    #ifdef PF_USER_FILEIO
/* Get user prototypes or macros from include file.
** API must match that defined above for the stubs.
*/
        #include PF_USER_FILEIO

    #else
        typedef FILE FileStream;

        #define sdOpenFile      fopen
        #define sdDeleteFile    remove
        #define sdFlushFile     fflush
        #define sdReadFile      fread
        #define sdWriteFile     fwrite

        /*
         * Note that fseek() and ftell() only support a long file offset.
         * So 64-bit offsets may not be supported on some platforms.
         * At one point we supported fseeko() and ftello() but they require
         * the off_t data type, which is not very portable.
         * So we decided to sacrifice vary large file support in
         * favor of portability.
         */
        #define sdSeekFile      fseek
        #define sdTellFile      ftell

        #define sdCloseFile     fclose
        #define sdRenameFile    rename
        #define sdInputChar     fgetc

        #define PF_STDIN  ((FileStream *) stdin)
        #define PF_STDOUT ((FileStream *) stdout)

        #define  PF_SEEK_SET   (SEEK_SET)
        #define  PF_SEEK_CUR   (SEEK_CUR)
        #define  PF_SEEK_END   (SEEK_END)

        /* TODO review the Size data type. */
        ThrowCode sdResizeFile( FileStream *, uint64_t Size);

        /*
        ** printf() is only used for debugging purposes.
        ** It is not required for normal operation.
        */
        #define PRT(x) { printf x; sdFlushFile(PF_STDOUT); }
    #endif

#endif  /* PF_NO_FILEIO */


#ifdef __cplusplus
extern "C" {
#endif

cell_t ioAccept( char *Target, cell_t n1 );
cell_t ioKey( void);
void ioEmit( char c );
void ioType( const char *s, cell_t n);

#ifdef __cplusplus
}
#endif

#endif /* _pf_io_h */
