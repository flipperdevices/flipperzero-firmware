/* compress.c
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



#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_LIBZ


#include <wolfssl/wolfcrypt/compress.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <zlib.h>


/* alloc user allocs to work with zlib */
static void* myAlloc(void* opaque, unsigned int item, unsigned int size)
{
    (void)opaque;
    return (void *)XMALLOC(item * size, opaque, DYNAMIC_TYPE_LIBZ);
}


static void myFree(void* opaque, void* memory)
{
    (void)opaque;
    XFREE(memory, opaque, DYNAMIC_TYPE_LIBZ);
}


#ifdef HAVE_MCAPI
    #define DEFLATE_DEFAULT_WINDOWBITS  11
    #define DEFLATE_DEFAULT_MEMLEVEL     1
#else
    #define DEFLATE_DEFAULT_WINDOWBITS 15
    #define DEFLATE_DEFAULT_MEMLEVEL    8
#endif


/*
 * out - pointer to destination buffer
 * outSz - size of destination buffer
 * in - pointer to source buffer to compress
 * inSz - size of source to compress
 * flags - flags to control how compress operates
 *
 * return:
 *    negative - error code
 *    positive - bytes stored in out buffer
 *
 * Note, the output buffer still needs to be larger than the input buffer.
 * The right chunk of data won't compress at all, and the lookup table will
 * add to the size of the output. The libz code says the compressed
 * buffer should be srcSz + 0.1% + 12.
 */
int wc_Compress_ex(byte* out, word32 outSz, const byte* in, word32 inSz,
    word32 flags, word32 windowBits)
{
    z_stream stream;
    int result = 0;

    stream.next_in = (Bytef*)in;
    stream.avail_in = (uInt)inSz;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != inSz) return COMPRESS_INIT_E;
#endif
    stream.next_out = out;
    stream.avail_out = (uInt)outSz;
    if ((uLong)stream.avail_out != outSz) return COMPRESS_INIT_E;

    stream.zalloc = (alloc_func)myAlloc;
    stream.zfree = (free_func)myFree;
    stream.opaque = (voidpf)0;

    if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                     DEFLATE_DEFAULT_WINDOWBITS | windowBits,
                     DEFLATE_DEFAULT_MEMLEVEL,
                     flags ? Z_FIXED : Z_DEFAULT_STRATEGY) != Z_OK)
        return COMPRESS_INIT_E;

    if (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
        deflateEnd(&stream);
        return COMPRESS_E;
    }

    result = (int)stream.total_out;

    if (deflateEnd(&stream) != Z_OK)
        result = COMPRESS_E;

    return result;
}

int wc_Compress(byte* out, word32 outSz, const byte* in, word32 inSz, word32 flags)
{
    return wc_Compress_ex(out, outSz, in, inSz, flags, 0);
}


/* windowBits:
* deflateInit() and inflateInit(), as well as deflateInit2() and inflateInit2()
    with windowBits in 0..15 all process zlib-wrapped deflate data.
    (See RFC 1950 and RFC 1951.)
* deflateInit2() and inflateInit2() with negative windowBits in -1..-15 process
    raw deflate data with no header or trailer.
* deflateInit2() and inflateInit2() with windowBits in 16..31, i.e. 16
    added to 0..15, process gzip-wrapped deflate data (RFC 1952).
* inflateInit2() with windowBits in 32..47 (32 added to 0..15) will
    automatically detect either a gzip or zlib header (but not raw deflate
    data), and decompress accordingly.
*/
int wc_DeCompress_ex(byte* out, word32 outSz, const byte* in, word32 inSz,
    int windowBits)
/*
 * out - pointer to destination buffer
 * outSz - size of destination buffer
 * in - pointer to source buffer to compress
 * inSz - size of source to compress
 * windowBits - flags to control how decompress operates
 *
 * return:
 *    negative - error code
 *    positive - bytes stored in out buffer
 */
{
    z_stream stream;
    int result = 0;

    stream.next_in = (Bytef*)in;
    stream.avail_in = (uInt)inSz;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != inSz) return DECOMPRESS_INIT_E;

    stream.next_out = out;
    stream.avail_out = (uInt)outSz;
    if ((uLong)stream.avail_out != outSz) return DECOMPRESS_INIT_E;

    stream.zalloc = (alloc_func)myAlloc;
    stream.zfree = (free_func)myFree;
    stream.opaque = (voidpf)0;

    if (inflateInit2(&stream, DEFLATE_DEFAULT_WINDOWBITS | windowBits) != Z_OK)
        return DECOMPRESS_INIT_E;

    result = inflate(&stream, Z_FINISH);
    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        return DECOMPRESS_E;
    }

    result = (int)stream.total_out;

    if (inflateEnd(&stream) != Z_OK)
        result = DECOMPRESS_E;

    return result;
}


int wc_DeCompress(byte* out, word32 outSz, const byte* in, word32 inSz)
{
    return wc_DeCompress_ex(out, outSz, in, inSz, 0);
}


/* Decompress the input buffer and create output buffer. Free'ing 'out' buffer
 * is the callers responsibility on successful return.
 *
 * out gets set to the output buffer created, *out gets overwritten
 * maxSz is the max decompression multiplier, i.e if 2 then max out size created
 *     would be 2*inSz, if set to -1 then there is no limit on out buffer size
 * memoryType the memory hint to use for 'out' i.e. DYNAMIC_TYPE_TMP_BUFFER
 * in  compressed input buffer
 * inSz size of 'in' buffer
 * windowBits decompression behavior flag (can be 0)
 * heap hint to use when mallocing 'out' buffer
 *
 * return the decompressed size, creates and grows out buffer as needed
 */
int wc_DeCompressDynamic(byte** out, int maxSz, int memoryType,
        const byte* in, word32 inSz, int windowBits, void* heap)
{
    z_stream   stream;
    int result   = 0;
    int i;
    word32 tmpSz = 0;
    byte*  tmp;

    (void)memoryType;
    (void)heap;

    if (out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }
    i = (maxSz == 1)? 1 : 2; /* start with output buffer twice the size of input
                              * unless max was set to 1 */

    stream.next_in = (Bytef*)in;
    stream.avail_in = (uInt)inSz;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != inSz) return DECOMPRESS_INIT_E;

    tmpSz = inSz * i;
    tmp = (byte*)XMALLOC(tmpSz, heap, memoryType);
    if (tmp == NULL)
        return MEMORY_E;

    stream.next_out  = tmp;
    stream.avail_out = (uInt)tmpSz;
    if ((uLong)stream.avail_out != tmpSz) return DECOMPRESS_INIT_E;

    stream.zalloc = (alloc_func)myAlloc;
    stream.zfree  = (free_func)myFree;
    stream.opaque = (voidpf)0;

    if (inflateInit2(&stream, DEFLATE_DEFAULT_WINDOWBITS | windowBits) != Z_OK) {
        XFREE(tmp, heap, memoryType);
        return DECOMPRESS_INIT_E;
    }

    /*
       Wanted to use inflateGetHeader here for uncompressed size but
       structure gz_headerp does not contain the ISIZE from RFC1952

        gz_headerp header;
        inflateGetHeader(&stream, &header);
    */

    /* loop through doing the decompression block by block to get full size */
    do {
        result = inflate(&stream, Z_BLOCK);
        if (result == Z_STREAM_END) {
            /* hit end of decompression */
            break;
        }

        /* good chance output buffer ran out of space with Z_BUF_ERROR
           try increasing output buffer size */
        if (result == Z_BUF_ERROR) {
            word32 newSz;
            byte*  newTmp;

            if (maxSz > 0 && i >= maxSz) {
                WOLFSSL_MSG("Hit max decompress size!");
                break;
            }
            i++;

            newSz = tmpSz + inSz;
            newTmp = (byte*)XMALLOC(newSz, heap, memoryType);
            if (newTmp == NULL) {
                WOLFSSL_MSG("Memory error with increasing buffer size");
                break;
            }
            XMEMCPY(newTmp, tmp, tmpSz);
            XFREE(tmp, heap, memoryType);
            tmp   = newTmp;
            stream.next_out  = tmp + stream.total_out;
            stream.avail_out = stream.avail_out + (uInt)inSz;
            tmpSz  = newSz;
            result = inflate(&stream, Z_BLOCK);
        }
    } while (result == Z_OK);

    if (result == Z_STREAM_END) {
        result = (int)stream.total_out;
        *out   = (byte*)XMALLOC(result, heap, memoryType);
        if (*out != NULL) {
            XMEMCPY(*out, tmp, result);
        }
        else {
            result = MEMORY_E;
        }
    }
    else {
        result = DECOMPRESS_E;
    }

    if (inflateEnd(&stream) != Z_OK)
        result = DECOMPRESS_E;

    if (tmp != NULL) {
        XFREE(tmp, heap, memoryType);
        tmp = NULL;
    }

    return result;
}

#endif /* HAVE_LIBZ */

