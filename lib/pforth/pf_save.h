/* @(#) pf_save.h 96/12/18 1.8 */
#ifndef _pforth_save_h
#define _pforth_save_h

/***************************************************************
** Include file for PForth SaveForth
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
**  941031  rdg     fix redefinition of MAKE_ID and EVENUP to be conditional
**
***************************************************************/


typedef struct DictionaryInfoChunk
{
/* All fields are stored in BIG ENDIAN format for consistency in data files.
 * All fields must be the same size for easy endian conversion.
 * All fields must be 32 bit for file compatibility with older versions.
 */
    int32_t  sd_Version;
    int32_t  sd_RelContext;      /* relative ptr to Dictionary Context */
    int32_t  sd_RelHeaderPtr;    /* relative ptr to Dictionary Header Ptr */
    int32_t  sd_RelCodePtr;      /* relative ptr to Dictionary Header Ptr */
    int32_t  sd_EntryPoint;  /* relative ptr to entry point or NULL */
    int32_t  sd_UserStackSize;   /* in bytes */
    int32_t  sd_ReturnStackSize; /* in bytes */
    int32_t  sd_NameSize;        /* in bytes */
    int32_t  sd_CodeSize;        /* in bytes */
    int32_t  sd_NumPrimitives;   /* To distinguish between primitive and secondary. */
    uint32_t sd_Flags;
    int32_t  sd_FloatSize;       /* In bytes. Must match code. 0 means no floats. */
    int32_t  sd_CellSize;        /* In bytes. Must match code. */
} DictionaryInfoChunk;

/* Bits in sd_Flags */
#define SD_F_BIG_ENDIAN_DIC    (1<<0)

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((((uint32_t)a)<<24)|(((uint32_t)b)<<16)|(((uint32_t)c)<<8)|((uint32_t)d))
#endif

#define ID_FORM MAKE_ID('F','O','R','M')
#define ID_P4TH MAKE_ID('P','4','T','H')
#define ID_P4DI MAKE_ID('P','4','D','I')
#define ID_P4NM MAKE_ID('P','4','N','M')
#define ID_P4CD MAKE_ID('P','4','C','D')
#define ID_BADF MAKE_ID('B','A','D','F')

#ifndef EVENUP
#define EVENUP(n) ((n+1)&(~1))
#endif

#ifdef __cplusplus
extern "C" {
#endif

cell_t ffSaveForth( const char *FileName, ExecToken EntryPoint, cell_t NameSize, cell_t CodeSize );

/* Endian-ness tools. */
int    IsHostLittleEndian( void );

ucell_t ReadCellBigEndian( const uint8_t *addr );
uint32_t Read32BigEndian( const uint8_t *addr );
uint16_t Read16BigEndian( const uint8_t *addr );

ucell_t ReadCellLittleEndian( const uint8_t *addr );
uint32_t Read32LittleEndian( const uint8_t *addr );
uint16_t Read16LittleEndian( const uint8_t *addr );

void WriteCellBigEndian( uint8_t *addr, ucell_t data );
void Write32BigEndian( uint8_t *addr, uint32_t data );
void Write16BigEndian( uint8_t *addr, uint16_t data );

void WriteCellLittleEndian( uint8_t *addr, ucell_t data );
void Write32LittleEndian( uint8_t *addr, uint32_t data );
void Write16LittleEndian( uint8_t *addr, uint16_t data );

#ifdef PF_SUPPORT_FP
void WriteFloatBigEndian( PF_FLOAT *addr, PF_FLOAT data );
PF_FLOAT ReadFloatBigEndian( const PF_FLOAT *addr );
void WriteFloatLittleEndian( PF_FLOAT *addr, PF_FLOAT data );
PF_FLOAT ReadFloatLittleEndian( const PF_FLOAT *addr );
#endif

#ifdef __cplusplus
}
#endif

#endif /* _pforth_save_h */
