/* asn1.c
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

#ifndef WOLFSSL_USER_SETTINGS
    #include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <stdio.h>

#if defined(WOLFSSL_ASN_PRINT) && !defined(NO_FILESYSTEM)

/* Increment allocated data by this much. */
#define DATA_INC_LEN    256


/* File format is DER/BER. */
#define FORMAT_DER     0
/* File format is BASE64. */
#define FORMAT_BASE64  1
/* File format is PEM. */
#define FORMAT_PEM     2

/* ASN.1 print options. */
static Asn1PrintOptions opts;
/* ASN.1 parsing state. */
static Asn1 asn1;

/* Read the contents of a file into a dynamically allocated buffer.
 *
 * Uses realloc as input may be stdin.
 *
 * @param [in]  fp     File pointer to read from.
 * @param [out] pdata  Pointer to data.
 * @param [out] plen   Pointer to length.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int asn1App_ReadFile(FILE* fp, unsigned char** pdata, word32* plen)
{
    int ret = 0;
    word32 len = 0;
    size_t read_len;
    /* Allocate a minimum amount. */
    unsigned char* data = (unsigned char*)malloc(DATA_INC_LEN);

    if (data != NULL) {
        /* Read more data. */
        while ((read_len = fread(data + len, 1, DATA_INC_LEN, fp)) != 0) {
            unsigned char* p;

            /* Add read data amount to length. */
            len += (word32)read_len;

            /* Stop if we are at end-of-file. */
            if (feof(fp)) {
                break;
            }

            /* Make space for more data to be added to buffer. */
            p = (unsigned char*)realloc(data, len + DATA_INC_LEN);
            if (p == NULL) {
                /* Reallocation failed - free current buffer. */
                free(data);
                data = NULL;
                break;
            }
            /* Set data to new pointer. */
            data = p;
        }
        /* Done with file. */
        fclose(fp);
    }

    if (data != NULL) {
        /* Return data and length. */
        *pdata = data;
        *plen = len;
    }
    else {
        /* Failed to allocate data. */
        ret = MEMORY_E;
    }
    return ret;
}

/* Print ASN.1 of a file containing BER/DER data.
 *
 * @param [in] fp  File pointer to read from.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int PrintDer(FILE* fp)
{
    int ret = 0;
    word32 len = 0;
    unsigned char* data = NULL;

    /* Load DER/BER file. */
    if (asn1App_ReadFile(fp, &data, &len) != 0) {
        ret = 1;
    }

    if ((ret == 0) && (data != NULL)) {
        /* Print DER/BER. */
        ret = wc_Asn1_PrintAll(&asn1, &opts, data, len);
        /* Dispose of buffer. */
        free(data);
    }

    return ret;
}

/* Print ASN.1 of a file containing Base64 encoding of BER/DER data.
 *
 * @param [in] fp  File pointer to read from.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int PrintBase64(FILE* fp)
{
    int ret = 0;
    word32 len = 0;
    unsigned char* data = NULL;

    /* Load Base64 encoded file. */
    if (asn1App_ReadFile(fp, &data, &len) != 0) {
        ret = 1;
    }

    if ((ret == 0) && (data != NULL)) {
        /* Decode Base64. */
        if (Base64_Decode(data, len, data, &len) != 0) {
            fprintf(stderr, "Invalid Base64 encoding\n");
            ret = 1;
        }

        if (ret == 0) {
            /* Print DER/BER. */
            ret = wc_Asn1_PrintAll(&asn1, &opts, data, len);
        }
        /* Dispose of buffer. */
        free(data);
    }

    return ret;
}

/* Find the next PEM block.
 *
 * @param [in]  data    PEM data.
 * @param [in]  offset  Offset into data to start looking.
 * @param [in]  len     Length of PEM data.
 * @param [out] start   Start of Base64 encoding.
 * @param [out] end     End of Base64 encoding.
 */
static int FindPem(unsigned char* data, word32 offset, word32 len,
    word32* start, word32* end)
{
    int ret = 0;
    word32 i;
    word32 j;

    /* Find header. */
    for (i = offset; i < len; i++) {
        if ((data[i] == '-') &&
                (strncmp((char*)data + i, "-----BEGIN", 10) == 0)) {
            break;
        }
    }
    if (i == len) {
        /* Got to end without finding PEM header. */
        fprintf(stderr, "No PEM header found\n");
        ret = 1;
    }
    if (ret == 0) {
        /* Confirm header. */
        for (i += 10; i < len; i++) {
            if ((data[i] == '-') &&
                    (strncmp((char*)data + i, "-----", 5) == 0)) {
                break;
            }
        }
        if (i == len) {
            /* Got to end without finding rest of PEM header. */
            fprintf(stderr, "Invalid PEM header\n");
            ret = 1;
        }
    }
    if (ret == 0) {
        /* Find footer. */
        i += 6;
        for (j = i + 1; j < len; j++) {
            if ((data[j] == '-') &&
                    (strncmp((char*)data + j, "-----END", 8) == 0)) {
                break;
            }
        }
        if (j == len) {
            /* Got to end without finding PEM footer. */
            fprintf(stderr, "No PEM footer found\n");
            ret = 1;
        }
    }

    if (ret == 0) {
        /* Return start and end indices. */
        *start = i;
        *end = j;
    }
    return ret;
}

/* Print ASN.1 of file containing PEM.
 *
 * Only one block is printed.
 *
 * @param [in] fp        File pointer to read from.
 * @param [in] pem_skip  Number of PEM blocks to skip.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int PrintPem(FILE* fp, int pem_skip)
{
    int ret = 0;
    unsigned char* data = NULL;
    word32 len = 0;

    /* Load PEM file. */
    if (asn1App_ReadFile(fp, &data, &len) != 0) {
        ret = 1;
    }

    if ((ret == 0) && (data != NULL)) {
        word32 i = 0;
        word32 j = 0;

        /* Find PEM blocks and skip number requested. */
        do {
            /* Find start and end of PEM Base64 data. */
            ret = FindPem(data, j, len, &i, &j);
        } while ((ret == 0) && ((pem_skip--) != 0));

        /* Decode data between header and footer. */
        if ((ret == 0) && (Base64_Decode(data + i, j - i, data, &len) != 0)) {
            fprintf(stderr, "Invalid Base64 encoding\n");
            ret = 1;
        }

        if (ret == 0) {
            /* Print DER/BER. */
            ret = wc_Asn1_PrintAll(&asn1, &opts, data, len);
        }
        /* Dispose of buffer. */
        free(data);
    }

    return ret;
}

/* Usage lines to show. */
const char* usage[] = {
    "asn1 [OPTION]... [FILE]",
    "Display a human-readable version of a DER/BER encoding.",
    "",
    "Options:",
    "  -?, --help           display this help and exit",
    "  -b, --branch         draw branches before tag name",
    "  -B, --base64         file contents are Base64 encoded",
    "  -d, --dump           show all ASN.1 item data as a hex dump",
    "  -h, --headers        show all ASN.1 item headers as a hex dump",
    "  -i, --indent         indent tag name with depth",
    "  -l, --length LEN     display length bytes of data",
    "  -n, --no-text        do not show data as text",
    "  -N, --no-dump-text   do not show data as a hex dump text",
    "  -o, --offset OFFSET  start decoding from offset",
    "  -O, --oid            show wolfSSL OID value in text",
    "  -p, --pem            file contents are PEM",
    "  -s, --skip-pem NUM   number of PEM blocks to skip",
};
/* Number of usage lines. */
#define USAGE_SZ  ((int)(sizeof(usage) / sizeof(*usage)))

/* Print out usage lines.
 */
static void Usage(void)
{
    int i;

    for (i = 0; i < USAGE_SZ; i++) {
        printf("%s\n", usage[i]);
    }
}

/* Main entry of ASN.1 printing program.
 *
 * @param [in] argc  Count of command line arguments.
 * @param [in] argv  Command line arguments.
 * @return  0 on success.
 * @return  1 on failure.
 */
int main(int argc, char* argv[])
{
    int ret = 0;
    /* Default to reading STDIN. */
    FILE* fp = stdin;
    int file_format = FORMAT_DER;
    word32 indent = 0;
    int pem_skip = 0;

    /* Reset options. */
    (void)wc_Asn1PrintOptions_Init(&opts);

    /* Skip over program name. */
    argc--;
    argv++;
    while (argc > 0) {
        /* Show branches instead of indenting. */
        if ((strcmp(argv[0], "-b") == 0) ||
            (strcmp(argv[0], "--branch") == 0)) {
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_DRAW_BRANCH, 1);
        }
        /* File is Base64 encoded data. */
        else if ((strcmp(argv[0], "-b64") == 0) ||
                 (strcmp(argv[0], "--base64") == 0)) {
            file_format = FORMAT_BASE64;
        }
        /* Dump all ASN.1 item data. */
        else if ((strcmp(argv[0], "-d") == 0) ||
                 (strcmp(argv[0], "--dump") == 0)) {
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_SHOW_DATA, 1);
        }
        /* Dump ASN.1 item headers. */
        else if ((strcmp(argv[0], "-h") == 0) ||
                 (strcmp(argv[0], "--headers") == 0)) {
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_SHOW_HEADER_DATA, 1);
        }
        /* Indent to text to indicate depth. */
        else if ((strcmp(argv[0], "-i") == 0) ||
                 (strcmp(argv[0], "--indent") == 0)) {
            indent++;
            if (indent > 15) {
            }
        }
        /* Only parse the specified length of DER/BER data. */
        else if ((strcmp(argv[0], "-l") == 0) ||
                 (strcmp(argv[0], "--length") == 0)) {
            if (argc == 1) {
                printf("Missing length value\n");
                return 1;
            }
            argc--;
            argv++;
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_LENGTH,
                (word32)atoi(argv[0]));
        }
        /* Do not show text representations of ASN.1 item data. */
        else if ((strcmp(argv[0], "-n") == 0) ||
                 (strcmp(argv[0], "--no-text") == 0)) {
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_SHOW_NO_TEXT, 1);
        }
        /* Do not show hex dump text representations of ASN.1 item data. */
        else if ((strcmp(argv[0], "-N") == 0) ||
                 (strcmp(argv[0], "--no-dump-text") == 0)) {
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_SHOW_NO_DUMP_TEXT, 1);
        }
        /* Offset into DER/BER to start decoding from. */
        else if ((strcmp(argv[0], "-o") == 0) ||
                 (strcmp(argv[0], "--offset") == 0)) {
            if (argc == 1) {
                fprintf(stderr, "Missing offset value\n");
                return 1;
            }
            argc--;
            argv++;
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_OFFSET,
                (word32)atoi(argv[0]));
        }
        /* Show wolfSSL OID value for all OBJECT_IDs. */
        else if ((strcmp(argv[0], "-O") == 0) ||
                 (strcmp(argv[0], "--oid") == 0)) {
            wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_SHOW_OID, 1);
        }
        /* File contains PEM blocks. */
        else if ((strcmp(argv[0], "-p") == 0) ||
                 (strcmp(argv[0], "--pem") == 0)) {
            file_format = FORMAT_PEM;
        }
        /* Skip a number of PEM blocks. */
        else if ((strcmp(argv[0], "-s") == 0) ||
                 (strcmp(argv[0], "--skip-pem") == 0)) {
            if (argc == 1) {
                fprintf(stderr, "Missing number of PEM blocks to skip\n");
                return 1;
            }
            argc--;
            argv++;
            pem_skip = atoi(argv[0]);
            if ((pem_skip < 0) || (pem_skip > 15)) {
                fprintf(stderr, "Skip value out of range: %d\n", pem_skip);
                return 1;
            }
        }
        /* Display help/usage. */
        else if ((strcmp(argv[0], "-?") == 0) ||
                 (strcmp(argv[0], "--help") == 0)) {
            Usage();
            return 0;
        }
        /* Unknown option detection. */
        else if (argv[0][0] == '-') {
            fprintf(stderr, "Bad option: %s\n", argv[0]);
            Usage();
            return 1;
        }
        else {
            /* Name of file to read. */
            fp = fopen(argv[0], "r");
            if (fp == NULL) {
                fprintf(stderr, "File not able to be read: %s\n", argv[0]);
                return 1;
            }
        }

        /* Move on to next command line argument. */
        argc--;
        argv++;
    }

    wc_Asn1PrintOptions_Set(&opts, ASN1_PRINT_OPT_INDENT, indent);

    (void)wc_Asn1_Init(&asn1);
    (void)wc_Asn1_SetFile(&asn1, stdout);

    /* Process file based on type. */
    if (file_format == FORMAT_DER) {
        ret = PrintDer(fp);
    }
    else if (file_format == FORMAT_BASE64) {
        ret = PrintBase64(fp);
    }
    else if (file_format == FORMAT_PEM) {
        ret = PrintPem(fp, pem_skip);
    }

    if (ret != 0) {
        fprintf(stderr, "%s\n", wc_GetErrorString(ret));
    }
    return (ret == 0) ? 0 : 1;
}

#else

/* Main entry of ASN.1 printing program.
 *
 * @param [in] argc  Count of command line arguments.
 * @param [in] argv  Command line arguments.
 * @return  0 on success.
 * @return  1 on failure.
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    fprintf(stderr, "ASN.1 Parsing and Printing or file system not compiled"
                    " in.\n");
    return 0;
}

#endif /* WOLFSSL_ASN_PRINT && !defined(NO_FILESYSTEM)*/
