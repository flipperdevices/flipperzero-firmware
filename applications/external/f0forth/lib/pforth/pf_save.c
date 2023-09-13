/* @(#) pf_save.c 98/01/26 1.3 */
/***************************************************************
** Save and Load Dictionary
** for PForth based on 'C'
**
** Compile file based version or static data based version
** depending on PF_NO_FILEIO switch.
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
****************************************************************
** 940225 PLB Fixed CodePtr save, was using NAMEREL instead of CODEREL
**            This would only work if the relative location
**            of names and code was the same when saved and reloaded.
** 940228 PLB Added PF_NO_FILEIO version
** 961204 PLB Added PF_STATIC_DIC
** 000623 PLB Cast chars as ucell_t before shifting for 16 bit systems.
***************************************************************/

#include <assert.h>

#include "pf_all.h"

/* If no File I/O, then force static dictionary. */
#ifdef PF_NO_FILEIO
    #ifndef PF_STATIC_DIC
        #define PF_STATIC_DIC
    #endif
#endif

#ifdef PF_STATIC_DIC
    #include "pfdicdat.h"
#endif

/*
Dictionary File Format based on IFF standard.
The chunk IDs, sizes, and data values are all Big Endian in conformance with the IFF standard.
The dictionaries may be big or little endian.
    'FORM'
    size
    'P4TH'  -  Form Identifier

Chunks
    'P4DI'
    size
    struct DictionaryInfoChunk

    'P4NM'
    size
    Name and Header portion of dictionary. (Big or Little Endian) (Optional)

    'P4CD'
    size
    Code portion of dictionary. (Big or Little Endian)
*/


/***************************************************************/
/* Endian-ness tools. */
ucell_t ReadCellBigEndian( const uint8_t *addr )
{
    ucell_t temp = (ucell_t)addr[0];
    temp = (temp << 8) | ((ucell_t)addr[1]);
    temp = (temp << 8) | ((ucell_t)addr[2]);
    temp = (temp << 8) | ((ucell_t)addr[3]);
    if( sizeof(ucell_t) == 8 )
    {
        temp = (temp << 8) | ((ucell_t)addr[4]);
        temp = (temp << 8) | ((ucell_t)addr[5]);
        temp = (temp << 8) | ((ucell_t)addr[6]);
        temp = (temp << 8) | ((ucell_t)addr[7]);
    }

    return temp;
}
/***************************************************************/
/* Endian-ness tools. */
uint32_t Read32BigEndian( const uint8_t *addr )
{
    uint32_t temp = (uint32_t)addr[0];
    temp = (temp << 8) | ((uint32_t)addr[1]);
    temp = (temp << 8) | ((uint32_t)addr[2]);
    temp = (temp << 8) | ((uint32_t)addr[3]);
    return temp;
}

/***************************************************************/
uint16_t Read16BigEndian( const uint8_t *addr )
{
    return (uint16_t) ((addr[0]<<8) | addr[1]);
}

/***************************************************************/
ucell_t ReadCellLittleEndian( const uint8_t *addr )
{
    ucell_t temp = 0;
    if( sizeof(ucell_t) == 8 )
    {
        temp = (temp << 8) | ((uint32_t)addr[7]);
        temp = (temp << 8) | ((uint32_t)addr[6]);
        temp = (temp << 8) | ((uint32_t)addr[5]);
        temp = (temp << 8) | ((uint32_t)addr[4]);
    }
    temp = (temp << 8) | ((uint32_t)addr[3]);
    temp = (temp << 8) | ((uint32_t)addr[2]);
    temp = (temp << 8) | ((uint32_t)addr[1]);
    temp = (temp << 8) | ((uint32_t)addr[0]);
    return temp;
}

/***************************************************************/
uint32_t Read32LittleEndian( const uint8_t *addr )
{
    uint32_t temp = (uint32_t)addr[3];
    temp = (temp << 8) | ((uint32_t)addr[2]);
    temp = (temp << 8) | ((uint32_t)addr[1]);
    temp = (temp << 8) | ((uint32_t)addr[0]);
    return temp;
}

/***************************************************************/
uint16_t Read16LittleEndian( const uint8_t *addr )
{
    const unsigned char *bp = (const unsigned char *) addr;
    return (uint16_t) ((bp[1]<<8) | bp[0]);
}

#ifdef PF_SUPPORT_FP

/***************************************************************/
static void ReverseCopyFloat( const PF_FLOAT *src, PF_FLOAT *dst );

static void ReverseCopyFloat( const PF_FLOAT *src, PF_FLOAT *dst )
{
    int i;
    unsigned char *d = (unsigned char *) dst;
    const unsigned char *s = (const unsigned char *) src;

    for( i=0; i<sizeof(PF_FLOAT); i++ )
    {
        d[i] = s[sizeof(PF_FLOAT) - 1 - i];
    }
}

/***************************************************************/
void WriteFloatBigEndian( PF_FLOAT *addr, PF_FLOAT data )
{
    if( IsHostLittleEndian() )
    {
        ReverseCopyFloat( &data, addr );
    }
    else
    {
        *addr = data;
    }
}

/***************************************************************/
PF_FLOAT ReadFloatBigEndian( const PF_FLOAT *addr )
{
    PF_FLOAT data;
    if( IsHostLittleEndian() )
    {
        ReverseCopyFloat( addr, &data );
        return data;
    }
    else
    {
        return *addr;
    }
}

/***************************************************************/
void WriteFloatLittleEndian( PF_FLOAT *addr, PF_FLOAT data )
{
    if( IsHostLittleEndian() )
    {
        *addr = data;
    }
    else
    {
        ReverseCopyFloat( &data, addr );
    }
}

/***************************************************************/
PF_FLOAT ReadFloatLittleEndian( const PF_FLOAT *addr )
{
    PF_FLOAT data;
    if( IsHostLittleEndian() )
    {
        return *addr;
    }
    else
    {
        ReverseCopyFloat( addr, &data );
        return data;
    }
}

#endif /* PF_SUPPORT_FP */

/***************************************************************/
void WriteCellBigEndian( uint8_t *addr, ucell_t data )
{
    /* Write should be in order of increasing address
     * to optimize for burst writes to DRAM. */
    if( sizeof(ucell_t) == 8 )
    {
        *addr++ = (uint8_t) (data>>56);
        *addr++ = (uint8_t) (data>>48);
        *addr++ = (uint8_t) (data>>40);
        *addr++ = (uint8_t) (data>>32);
    }
    *addr++ = (uint8_t) (data>>24);
    *addr++ = (uint8_t) (data>>16);
    *addr++ = (uint8_t) (data>>8);
    *addr = (uint8_t) (data);
}

/***************************************************************/
void Write32BigEndian( uint8_t *addr, uint32_t data )
{
    *addr++ = (uint8_t) (data>>24);
    *addr++ = (uint8_t) (data>>16);
    *addr++ = (uint8_t) (data>>8);
    *addr = (uint8_t) (data);
}

/***************************************************************/
void Write16BigEndian( uint8_t *addr, uint16_t data )
{
    *addr++ = (uint8_t) (data>>8);
    *addr = (uint8_t) (data);
}

/***************************************************************/
void WriteCellLittleEndian( uint8_t *addr, ucell_t data )
{
    /* Write should be in order of increasing address
     * to optimize for burst writes to DRAM. */
    if( sizeof(ucell_t) == 8 )
    {
        *addr++ = (uint8_t) data;  /* LSB at near end */
        data = data >> 8;
        *addr++ = (uint8_t) data;
        data = data >> 8;
        *addr++ = (uint8_t) data;
        data = data >> 8;
        *addr++ = (uint8_t) data;
        data = data >> 8;
    }
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr = (uint8_t) data;
}
/***************************************************************/
void Write32LittleEndian( uint8_t *addr, uint32_t data )
{
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr = (uint8_t) data;
}

/***************************************************************/
void Write16LittleEndian( uint8_t *addr, uint16_t data )
{
    *addr++ = (uint8_t) data;
    data = data >> 8;
    *addr = (uint8_t) data;
}

/***************************************************************/
/* Return 1 if host CPU is Little Endian */
int IsHostLittleEndian( void )
{
    static int gEndianCheck = 1;
    unsigned char *bp = (unsigned char *) &gEndianCheck;
    return (int) (*bp); /* Return byte pointed to by address. If LSB then == 1 */
}

#if defined(PF_NO_FILEIO) || defined(PF_NO_SHELL)

cell_t ffSaveForth( const char *FileName, ExecToken EntryPoint, cell_t NameSize, cell_t CodeSize, pfTaskData_t *gCurrentTask)
{
    TOUCH(FileName);
    TOUCH(EntryPoint);
    TOUCH(NameSize);
    TOUCH(CodeSize);

    pfReportError("ffSaveForth", PF_ERR_NOT_SUPPORTED);
    return -1;
}

#else /* PF_NO_FILEIO or PF_NO_SHELL */

/***************************************************************/
static int Write32ToFile( FileStream *fid, uint32_t Val )
{
    size_t numw;
    uint8_t pad[4];

    Write32BigEndian(pad,Val);
    numw = sdWriteFile( pad, 1, sizeof(pad), fid );
    if( numw != sizeof(pad) ) return -1;
    return 0;
}

/***************************************************************/
static cell_t WriteChunkToFile( FileStream *fid, cell_t ID, char *Data, int32_t NumBytes )
{
    cell_t numw;
    cell_t EvenNumW;

    EvenNumW = EVENUP(NumBytes);

    assert(ID <= UINT32_MAX);
    if( Write32ToFile( fid, (uint32_t)ID ) < 0 ) goto error;
    assert(EvenNumW <= UINT32_MAX);
    if( Write32ToFile( fid, (uint32_t)EvenNumW ) < 0 ) goto error;

    numw = sdWriteFile( Data, 1, EvenNumW, fid );
    if( numw != EvenNumW ) goto error;
    return 0;
error:
    pfReportError("WriteChunkToFile", PF_ERR_WRITE_FILE);
    return -1;
}

/* Convert dictionary info chunk between native and on-disk (big-endian). */
static void
convertDictionaryInfoWrite (DictionaryInfoChunk *sd)
{
/* Convert all fields in DictionaryInfoChunk from Native to BigEndian.
 * This assumes they are all 32-bit integers.
 */
    int   i;
    uint32_t *p = (uint32_t *) sd;
    for (i=0; i<((int)(sizeof(*sd)/sizeof(uint32_t))); i++)
    {
        Write32BigEndian( (uint8_t *)&p[i], p[i] );
    }
}

static void
convertDictionaryInfoRead (DictionaryInfoChunk *sd)
{
/* Convert all fields in structure from BigEndian to Native. */
    int   i;
    uint32_t *p = (uint32_t *) sd;
    for (i=0; i<((int)(sizeof(*sd)/sizeof(uint32_t))); i++)
    {
        p[i] = Read32BigEndian( (uint8_t *)&p[i] );
    }
}

/****************************************************************
** Save Dictionary in File.
** If EntryPoint is NULL, save as development environment.
** If EntryPoint is non-NULL, save as turnKey environment with no names.
*/
cell_t ffSaveForth( const char *FileName, ExecToken EntryPoint, cell_t NameSize, cell_t CodeSize, pfTaskData_t *gCurrentTask)
{
    FileStream *fid;
    DictionaryInfoChunk SD;
    uint32_t FormSize;
    uint32_t NameChunkSize = 0;
    uint32_t CodeChunkSize;
    uint32_t relativeCodePtr;

    fid = sdOpenFile( FileName, "wb" );
    if( fid == NULL )
    {
        pfReportError("pfSaveDictionary", PF_ERR_OPEN_FILE);
        return -1;
    }

/* Save in uninitialized form. */
    pfExecIfDefined("AUTO.TERM", gCurrentTask);

/* Write FORM Header ---------------------------- */
    if( Write32ToFile( fid, ID_FORM ) < 0 ) goto error;
    if( Write32ToFile( fid, 0 ) < 0 ) goto error;
    if( Write32ToFile( fid, ID_P4TH ) < 0 ) goto error;

/* Write P4DI Dictionary Info  ------------------ */
    SD.sd_Version = PF_FILE_VERSION;

    relativeCodePtr = ABS_TO_CODEREL(gCurrentDictionary->dic_CodePtr.Byte); /* 940225 */
    SD.sd_RelCodePtr = relativeCodePtr;
    SD.sd_UserStackSize = sizeof(cell_t) * (gCurrentTask->td_StackBase - gCurrentTask->td_StackLimit);
    SD.sd_ReturnStackSize = sizeof(cell_t) * (gCurrentTask->td_ReturnBase - gCurrentTask->td_ReturnLimit);
    SD.sd_NumPrimitives = gNumPrimitives;  /* Must match compiled dictionary. */

#ifdef PF_SUPPORT_FP
    SD.sd_FloatSize = sizeof(PF_FLOAT);  /* Must match compiled dictionary. */
#else
    SD.sd_FloatSize = 0;
#endif

    SD.sd_CellSize = sizeof(cell_t);

/* Set bit that specifies whether dictionary is BIG or LITTLE Endian. */
    {
#if defined(PF_BIG_ENDIAN_DIC)
        int eflag = SD_F_BIG_ENDIAN_DIC;
#elif defined(PF_LITTLE_ENDIAN_DIC)
        int eflag = 0;
#else
        int eflag = IsHostLittleEndian() ? 0 : SD_F_BIG_ENDIAN_DIC;
#endif
        SD.sd_Flags = eflag;
    }

    if( EntryPoint )
    {
        SD.sd_EntryPoint = EntryPoint;  /* Turnkey! */
    }
    else
    {
        SD.sd_EntryPoint = 0;
    }

/* Do we save names? */
    if( NameSize == 0 )
    {
        SD.sd_RelContext = 0;
        SD.sd_RelHeaderPtr = 0;
        SD.sd_NameSize = 0;
    }
    else
    {
        uint32_t relativeHeaderPtr;
/* Development mode. */
        SD.sd_RelContext = ABS_TO_NAMEREL(gVarContext);
        relativeHeaderPtr = ABS_TO_NAMEREL(gCurrentDictionary->dic_HeaderPtr);
        SD.sd_RelHeaderPtr = relativeHeaderPtr;

/* How much real name space is there? */
        NameChunkSize = QUADUP(relativeHeaderPtr);  /* Align */

/* NameSize must be 0 or greater than NameChunkSize + 1K */
        NameSize = QUADUP(NameSize);  /* Align */
        if( NameSize > 0 )
        {
            NameSize = MAX( (ucell_t)NameSize, (NameChunkSize + 1024) );
        }
        SD.sd_NameSize = NameSize;
    }

/* How much real code is there? */
    CodeChunkSize = QUADUP(relativeCodePtr);
    CodeSize = QUADUP(CodeSize);  /* Align */
    CodeSize = MAX( (ucell_t)CodeSize, (CodeChunkSize + 2048) );
    SD.sd_CodeSize = CodeSize;


    convertDictionaryInfoWrite (&SD);

    if( WriteChunkToFile( fid, ID_P4DI, (char *) &SD, sizeof(DictionaryInfoChunk) ) < 0 ) goto error;

/* Write Name Fields if NameSize non-zero ------- */
    if( NameSize > 0 )
    {
        if( WriteChunkToFile( fid, ID_P4NM, (char *) NAME_BASE,
            NameChunkSize ) < 0 ) goto error;
    }

/* Write Code Fields ---------------------------- */
    if( WriteChunkToFile( fid, ID_P4CD, (char *) CODE_BASE,
        CodeChunkSize ) < 0 ) goto error;

    FormSize = (uint32_t) sdTellFile( fid ) - 8;
    sdSeekFile( fid, 4, PF_SEEK_SET );
    if( Write32ToFile( fid, FormSize ) < 0 ) goto error;

    sdCloseFile( fid );

/* Restore initialization. */
    pfExecIfDefined("AUTO.INIT", gCurrentTask);
    return 0;

error:
    sdSeekFile( fid, 0, PF_SEEK_SET );
    Write32ToFile( fid, ID_BADF ); /* Mark file as bad. */
    sdCloseFile( fid );

/* Restore initialization. */
    pfExecIfDefined("AUTO.INIT", gCurrentTask);

    return -1;
}

#endif /* !PF_NO_FILEIO and !PF_NO_SHELL */


#ifndef PF_NO_FILEIO

/***************************************************************/
static int32_t Read32FromFile( FileStream *fid, uint32_t *ValPtr )
{
    cell_t numr;
    uint8_t pad[4];
    numr = sdReadFile( pad, 1, sizeof(pad), fid );
    if( numr != sizeof(pad) ) return -1;
    *ValPtr = Read32BigEndian( pad );
    return 0;
}

/***************************************************************/
PForthDictionary pfLoadDictionary( const char *FileName, ExecToken *EntryPointPtr, pfTaskData_t *gCurrentTask )
{
    pfDictionary_t *dic = NULL;
    FileStream *fid;
    DictionaryInfoChunk *sd;
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t FormSize;
    uint32_t BytesLeft;
    cell_t numr;
    int   isDicBigEndian;

DBUG(("pfLoadDictionary( %s )\n", FileName ));

/* Open file. */
    fid = sdOpenFile( FileName, "rb" );
    if( fid == NULL )
    {
        pfReportError("pfLoadDictionary", PF_ERR_OPEN_FILE);
        goto xt_error;
    }

/* Read FORM, Size, ID */
    if (Read32FromFile( fid, &ChunkID ) < 0) goto read_error;
    if( ChunkID != ID_FORM )
    {
        pfReportError("pfLoadDictionary", PF_ERR_WRONG_FILE);
        goto error;
    }

    if (Read32FromFile( fid, &FormSize ) < 0) goto read_error;
    BytesLeft = FormSize;

    if (Read32FromFile( fid, &ChunkID ) < 0) goto read_error;
    BytesLeft -= 4;
    if( ChunkID != ID_P4TH )
    {
        pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE);
        goto error;
    }

/* Scan and parse all chunks in file. */
    while( BytesLeft > 0 )
    {
        if (Read32FromFile( fid, &ChunkID ) < 0) goto read_error;
        if (Read32FromFile( fid, &ChunkSize ) < 0) goto read_error;
        BytesLeft -= 8;

        DBUG(("ChunkID = %4s, Size = %d\n", (char *)&ChunkID, ChunkSize ));

        switch( ChunkID )
        {
        case ID_P4DI:
            sd = (DictionaryInfoChunk *) pfAllocMem( ChunkSize );
            if( sd == NULL ) goto nomem_error;

            numr = sdReadFile( sd, 1, ChunkSize, fid );
            if( numr != ChunkSize ) goto read_error;
            BytesLeft -= ChunkSize;

            convertDictionaryInfoRead (sd);

            isDicBigEndian = sd->sd_Flags & SD_F_BIG_ENDIAN_DIC;

            if( !gVarQuiet )
            {
                MSG("pForth loading dictionary from file "); MSG(FileName);
                    EMIT_CR;
                MSG_NUM_D("     File format version is ", sd->sd_Version );
                MSG_NUM_D("     Name space size = ", sd->sd_NameSize );
                MSG_NUM_D("     Code space size = ", sd->sd_CodeSize );
                MSG_NUM_D("     Entry Point     = ", sd->sd_EntryPoint );
                MSG_NUM_D("     Cell Size       = ", sd->sd_CellSize );
                MSG( (isDicBigEndian ? "     Big Endian Dictionary" :
                                       "     Little  Endian Dictionary") );
                if( isDicBigEndian == IsHostLittleEndian() ) MSG(" !!!!");
                    EMIT_CR;
            }

            if( sd->sd_Version > PF_FILE_VERSION )
            {
                pfReportError("pfLoadDictionary", PF_ERR_VERSION_FUTURE );
                goto error;
            }
            if( sd->sd_Version < PF_EARLIEST_FILE_VERSION )
            {
                pfReportError("pfLoadDictionary", PF_ERR_VERSION_PAST );
                goto error;
            }
            if( sd->sd_CellSize != sizeof(cell_t) )
            {
                pfReportError("pfLoadDictionary", PF_ERR_CELL_SIZE_CONFLICT );
                goto error;
            }
            if( sd->sd_NumPrimitives > NUM_PRIMITIVES )
            {
                pfReportError("pfLoadDictionary", PF_ERR_NOT_SUPPORTED );
                goto error;
            }

/* Check to make sure that EndianNess of dictionary matches mode of pForth. */
#if defined(PF_BIG_ENDIAN_DIC)
            if(isDicBigEndian == 0)
#elif defined(PF_LITTLE_ENDIAN_DIC)
            if(isDicBigEndian == 1)
#else
            if( isDicBigEndian == IsHostLittleEndian() )
#endif
            {
                pfReportError("pfLoadDictionary", PF_ERR_ENDIAN_CONFLICT );
                goto error;
            }

/* Check for compatible float size. */
#ifdef PF_SUPPORT_FP
            if( sd->sd_FloatSize != sizeof(PF_FLOAT) )
#else
            if( sd->sd_FloatSize != 0 )
#endif
            {
                pfReportError("pfLoadDictionary", PF_ERR_FLOAT_CONFLICT );
                goto error;
            }

            dic = pfCreateDictionary( sd->sd_NameSize, sd->sd_CodeSize );
            if( dic == NULL ) goto nomem_error;
            gCurrentDictionary = dic;
            if( sd->sd_NameSize > 0 )
            {
                gVarContext = NAMEREL_TO_ABS(sd->sd_RelContext); /* Restore context. */
                gCurrentDictionary->dic_HeaderPtr = (ucell_t)(uint8_t *)
                    NAMEREL_TO_ABS(sd->sd_RelHeaderPtr);
            }
            else
            {
                gVarContext = 0;
                gCurrentDictionary->dic_HeaderPtr = (ucell_t)NULL;
            }
            gCurrentDictionary->dic_CodePtr.Byte = (uint8_t *) CODEREL_TO_ABS(sd->sd_RelCodePtr);
            gNumPrimitives = sd->sd_NumPrimitives;  /* Must match compiled dictionary. */
/* Pass EntryPoint back to caller. */
            if( EntryPointPtr != NULL ) *EntryPointPtr = sd->sd_EntryPoint;
            pfFreeMem(sd);
            break;

        case ID_P4NM:
#ifdef PF_NO_SHELL
            pfReportError("pfLoadDictionary", PF_ERR_NO_SHELL );
            goto error;
#else
            if( NAME_BASE == 0 )
            {
                pfReportError("pfLoadDictionary", PF_ERR_NO_NAMES );
                goto error;
            }
            if( gCurrentDictionary == NULL )
            {
                pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE );
                goto error;
            }
            if( ChunkSize > NAME_SIZE )
            {
                pfReportError("pfLoadDictionary", PF_ERR_TOO_BIG);
                goto error;
            }
            numr = sdReadFile( (char *) NAME_BASE, 1, ChunkSize, fid );
            if( numr != ChunkSize ) goto read_error;
            BytesLeft -= ChunkSize;
#endif /* PF_NO_SHELL */
            break;

        case ID_P4CD:
            if( gCurrentDictionary == NULL )
            {
                pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE );
                goto error;
            }
            if( ChunkSize > CODE_SIZE )
            {
                pfReportError("pfLoadDictionary", PF_ERR_TOO_BIG);
                goto error;
            }
            numr = sdReadFile( (uint8_t *) CODE_BASE, 1, ChunkSize, fid );
            if( numr != ChunkSize ) goto read_error;
            BytesLeft -= ChunkSize;
            break;

        default:
            pfReportError("pfLoadDictionary", PF_ERR_BAD_FILE );
            sdSeekFile( fid, ChunkSize, PF_SEEK_CUR );
            break;
        }
    }

    sdCloseFile( fid );

    if( NAME_BASE != 0)
    {
        cell_t Result;
/* Find special words in dictionary for global XTs. */
        if( (Result = FindSpecialXTs(gCurrentTask)) < 0 )
        {
            pfReportError("pfLoadDictionary: FindSpecialXTs", (Err)Result);
            goto error;
        }
    }

DBUG(("pfLoadDictionary: return %p\n", dic));
    return (PForthDictionary) dic;

nomem_error:
    pfReportError("pfLoadDictionary", PF_ERR_NO_MEM);
    sdCloseFile( fid );
    return NULL;

read_error:
    pfReportError("pfLoadDictionary", PF_ERR_READ_FILE);
error:
    sdCloseFile( fid );
xt_error:
    return NULL;
}

#else

PForthDictionary pfLoadDictionary( const char *FileName, ExecToken *EntryPointPtr )
{
    (void) FileName;
    (void) EntryPointPtr;
    return NULL;
}
#endif /* !PF_NO_FILEIO */



/***************************************************************/
//PForthDictionary pfLoadStaticDictionary( void )
//{
//#ifdef PF_STATIC_DIC
//    cell_t Result;
//    pfDictionary_t *dic;
//    cell_t NewNameSize, NewCodeSize;
//
//    if( IF_LITTLE_ENDIAN != IsHostLittleEndian() )
//    {
//        MSG( (IF_LITTLE_ENDIAN ?
//                 "Little Endian Dictionary on " :
//                 "Big Endian Dictionary on ") );
//        MSG( (IsHostLittleEndian() ?
//                 "Little Endian CPU" :
//                 "Big Endian CPU") );
//        EMIT_CR;
//    }
//
///* Check to make sure that EndianNess of dictionary matches mode of pForth. */
//#if defined(PF_BIG_ENDIAN_DIC)
//    if(IF_LITTLE_ENDIAN == 1)
//#elif defined(PF_LITTLE_ENDIAN_DIC)
//    if(IF_LITTLE_ENDIAN == 0)
//#else /* Code is native endian! */
//    if( IF_LITTLE_ENDIAN != IsHostLittleEndian() )
//#endif
//    {
//        pfReportError("pfLoadStaticDictionary", PF_ERR_ENDIAN_CONFLICT );
//        goto error;
//    }
//
//
//#ifndef PF_EXTRA_HEADERS
//    #define PF_EXTRA_HEADERS  (20000)
//#endif
//#ifndef PF_EXTRA_CODE
//    #define PF_EXTRA_CODE  (40000)
//#endif
//
///* Copy static const data to allocated dictionaries. */
//    NewNameSize = sizeof(MinDicNames) + PF_EXTRA_HEADERS;
//    NewCodeSize = sizeof(MinDicCode) + PF_EXTRA_CODE;
//
//    DBUG_NUM_D( "static dic name size = ", NewNameSize );
//    DBUG_NUM_D( "static dic code size = ", NewCodeSize );
//
//    gCurrentDictionary = dic = pfCreateDictionary( NewNameSize, NewCodeSize );
//    if( !dic ) goto nomem_error;
//
//    pfCopyMemory( (uint8_t *) dic->dic_HeaderBase, MinDicNames, sizeof(MinDicNames) );
//    pfCopyMemory( (uint8_t *) dic->dic_CodeBase, MinDicCode, sizeof(MinDicCode) );
//    DBUG(("Static data copied to newly allocated dictionaries.\n"));
//
//    dic->dic_CodePtr.Byte = (uint8_t *) CODEREL_TO_ABS(CODEPTR);
//    gNumPrimitives = NUM_PRIMITIVES;
//
//    if( NAME_BASE != 0)
//    {
///* Setup name space. */
//        dic->dic_HeaderPtr = (ucell_t)(uint8_t *) NAMEREL_TO_ABS(HEADERPTR);
//        gVarContext = NAMEREL_TO_ABS(RELCONTEXT); /* Restore context. */
//
///* Find special words in dictionary for global XTs. */
//        if( (Result = FindSpecialXTs()) < 0 )
//        {
//            pfReportError("pfLoadStaticDictionary: FindSpecialXTs", Result);
//            goto error;
//        }
//    }
//
//    return (PForthDictionary) dic;
//
//error:
//    return NULL;
//
//nomem_error:
//    pfReportError("pfLoadStaticDictionary", PF_ERR_NO_MEM);
//#endif /* PF_STATIC_DIC */
//
//    return NULL;
//}

PForthDictionary pfLoadStaticDictionary( pfTaskData_t *gCurrentTask )
{
#ifdef PF_STATIC_DIC
    cell_t Result;
    pfDictionary_t *dic;
    cell_t NewNameSize, NewCodeSize;

    if( IF_LITTLE_ENDIAN != IsHostLittleEndian() )
    {
        MSG( (IF_LITTLE_ENDIAN ?
                 "Little Endian Dictionary on " :
                 "Big Endian Dictionary on ") );
        MSG( (IsHostLittleEndian() ?
                 "Little Endian CPU" :
                 "Big Endian CPU") );
        EMIT_CR;
    }

/* Check to make sure that EndianNess of dictionary matches mode of pForth. */
#if defined(PF_BIG_ENDIAN_DIC)
    if(IF_LITTLE_ENDIAN == 1)
#elif defined(PF_LITTLE_ENDIAN_DIC)
    if(IF_LITTLE_ENDIAN == 0)
#else /* Code is native endian! */
    if( IF_LITTLE_ENDIAN != IsHostLittleEndian() )
#endif
    {
        pfReportError("pfLoadStaticDictionary", PF_ERR_ENDIAN_CONFLICT );
        goto error;
    }

    NewNameSize = sizeof(MinDicNames) - DIC_ALIGNMENT_SIZE;
    NewCodeSize = sizeof(MinDicCode) - DIC_ALIGNMENT_SIZE;

    DBUG_NUM_D( "static dic name size = ", NewNameSize );
    DBUG_NUM_D( "static dic code size = ", NewCodeSize );

    gCurrentDictionary = dic = pfCreateStaticDictionary( MinDicNames,
            MinDicCode, NewNameSize, NewCodeSize );
    if( !dic ) goto nomem_error;

    dic->dic_CodePtr.Byte = (uint8_t *) CODEREL_TO_ABS(CODEPTR);
    gNumPrimitives = NUM_PRIMITIVES;

    if( NAME_BASE != 0)
    {
/* Setup name space. */
        dic->dic_HeaderPtr = (ucell_t)(uint8_t *) NAMEREL_TO_ABS(HEADERPTR);
        gVarContext = NAMEREL_TO_ABS(RELCONTEXT); /* Restore context. */

/* Find special words in dictionary for global XTs. */
        if( (Result = FindSpecialXTs(gCurrentTask)) < 0 )
        {
            pfReportError("pfLoadStaticDictionary: FindSpecialXTs", Result);
            goto error;
        }
    }

    return (PForthDictionary) dic;

error:
    return NULL;

nomem_error:
    pfReportError("pfLoadStaticDictionary", PF_ERR_NO_MEM);
#endif /* PF_STATIC_DIC */

    return NULL;
}

