/* iotsafe.c
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

/* IoT-safe module for communication with IoT-safe applet on SIM card */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_IOTSAFE

#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>
#include <wolfssl/internal.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef DEBUG_IOTSAFE
#include <stdio.h>
#endif

static int wolfIoT_initialized = 0;

/* Modem APDU interface
 *
 */

/* I/O calls */
static wolfSSL_IOTSafe_CSIM_read_cb csim_read_cb = NULL;
static wolfSSL_IOTSafe_CSIM_write_cb csim_write_cb = NULL;

#define GETRAND_CMD_SIZE (24 + 2)
#define IOTSAFE_MAX_RETRIES (8)


#define AT_CSIM_CMD_SIZE 13
#define AT_CMD_HDR_SIZE  10
#define AT_CMD_INS_POS (AT_CMD_HDR_SIZE - 8)
#define AT_CMD_P1_POS  (AT_CMD_HDR_SIZE - 6)
#define AT_CMD_P2_POS  (AT_CMD_HDR_SIZE - 4)
#define AT_CMD_LC_POS  (AT_CMD_HDR_SIZE - 2)

/* Size of "\r\n + null terminator */
#define CSIM_CMD_ENDSTR_SIZE 4

/* Buffer for CSIM RX APDU */
#define MAXBUF 2048
static char csim_read_buf[MAXBUF];
static char csim_cmd[IOTSAFE_CMDSIZE_MAX];

/* APDU layer: I/O */
static int csim_read(char *buf, int len)
{
    if (csim_read_cb)
        return csim_read_cb(buf, len);
    else
        return -WOLFSSL_ERROR_SYSCALL;
}

static int csim_write(const char *buf, int len)
{
#ifdef DEBUG_IOTSAFE
    printf(">>> %s\n", buf);
#endif
    if (csim_write_cb)
        return csim_write_cb(buf, len);
    else
        return -WOLFSSL_ERROR_SYSCALL;
}

/* APDU string parser/expect
 */
static int hex_to_bytes(const char *hex, unsigned char *output, unsigned long sz)
{
    word32 i;
    for (i = 0; i < sz; i++) {
        signed char ch1, ch2;
        ch1 = HexCharToByte(hex[i * 2]);
        ch2 = HexCharToByte(hex[i * 2 + 1]);
        if ((ch1 < 0) || (ch2 < 0)) {
            WOLFSSL_MSG("hex_to_bytes: syntax error");
            return -1;
        }
        output[i] = (unsigned char)((ch1 << 4) + ch2);
    }
    return (int)sz;
}

static int bytes_to_hex(const unsigned char *bytes, char *hex, unsigned long sz)
{
    word32 i;
    for (i = 0; i < sz; i++) {
        ByteToHexStr(bytes[i], &hex[2 * i]);
    }
    return (int)(2 * sz);
}

static int expect_tok(const char *cmd, int size, const char *tok, char **repl)
{
    char *r_found = NULL;
    static char parser_line[MAXBUF / 2];
    char *reply = NULL;
    int ret;
    if (cmd) {
        ret = csim_write(cmd, size);
        if (ret <= 0)
            return ret;
    } else {
        /* Force enter the read loop on cmd == NULL */
        ret = 1;
    }
    while (ret > 0) {
        ret = csim_read(csim_read_buf, MAXBUF);
#ifdef DEBUG_IOTSAFE
        if (ret > 0)
            printf("<<< %s\n", csim_read_buf);
#endif
        if (tok && (ret > 0) && !r_found) {
            /* Mark the beginning of the match in the reply. */
            r_found = XSTRSTR(csim_read_buf, tok);
            if (r_found) {
                XSTRNCPY(parser_line, r_found, MAXBUF / 2 - 1);
                /* Do not break here! *
                 * Keep reading until csim_read returns 0, or error */
            }
        }
    }
    if ((ret >= 0) && (r_found)) {
        reply = parser_line + XSTRLEN(tok);
        /* If the reply consists of token only,
         * return the entire string.
         */
        if (XSTRLEN(reply) == 0) {
            reply = parser_line;
        }
    }
    /* Assign the pointer to the received reply
     * only if repl is not NULL
     */
    if (repl)
        *repl = reply;

    if (reply)
        ret = (int)XSTRLEN(reply);
    else
        ret = 0;
    return ret;
}

static int expect_ok(const char *cmd, int size)
{
    return expect_tok(cmd, size, "OK", NULL);
}

/* Utilities
 */

/* Conversion utility from hex string format to bytes */
static int hexbuffer_conv(char *hex_str, unsigned char *output, unsigned long sz)
{
    if (XSTRLEN(hex_str) != (2 * sz)) {
        return -1;
    }
    return (int)hex_to_bytes(hex_str, output, sz);
}

/* Search a TLV by tag in a buffer */
static char *search_tlv(const char *haystack, int size, uint8_t tag)
{
    int i = 0;
    uint8_t t;
    uint8_t l;
    while (i < size) {
        if (hex_to_bytes(&haystack[i], &t, 1) < 0)
            return NULL;
        if (hex_to_bytes(&haystack[i + 2], &l, 1) < 0)
            return NULL;
        if (tag == t) {
            return (char *)&haystack[i];
        }
        if (l + i > size)
            return NULL;
        i += 4 +  2 * l;
    }
    return NULL;
}

static int iotsafe_cmd_start(char *cmd, byte cmd_class, byte ins, byte p1, byte p2)
{
    byte lc = 0;
    char *out;
    XMEMSET(cmd, 0, IOTSAFE_CMDSIZE_MAX);
    XSTRNCPY(cmd, "AT+CSIM= 10,\"", IOTSAFE_CMDSIZE_MAX - 1);
    out = cmd + AT_CSIM_CMD_SIZE;
    bytes_to_hex(&cmd_class, out, 1);
    bytes_to_hex(&ins, out + AT_CMD_INS_POS, 1);
    bytes_to_hex(&p1, out + AT_CMD_P1_POS, 1);
    bytes_to_hex(&p2, out + AT_CMD_P2_POS, 1);
    bytes_to_hex(&lc, out + AT_CMD_LC_POS, 1);
    out[10] = 0;
    return (int)XSTRLEN(cmd);
}

static int iotsafe_cmd_add_tlv_ex(char *cmd, byte tag, uint16_t len,
        const byte *val, int taglen_size)
{
    word32 cur_csim_len;
    word32 off;
    byte cur_lc;
    char *out;
    int ret;
    byte *len_b = (byte *)&len;
    char *len_csim_str = (cmd + AT_CSIM_CMD_SIZE - 5);
    word32 cmdlen;

    cmdlen = (word32)XSTRLEN(cmd);
    if (cmdlen < AT_CSIM_CMD_SIZE) {
        return BAD_FUNC_ARG;
    }

    if ((taglen_size < 1) || (taglen_size > 2)) {
        return BAD_FUNC_ARG;
    }

    /* Read out current CSIM len from the existing string.
     * The generated command may have the format:
     *  "AT+CSIM= 68" (< 100: leading space)
     *   or
     *  "AT+CSIM=212" (>=100: same positions, leading hundreds)
     *
     */
    cur_csim_len = (word32)len_csim_str[2] - '0';
    cur_csim_len += (len_csim_str[1] - '0') * 10;
    if (len_csim_str[0] >= '0' &&
            len_csim_str[0] <= '9') {
        cur_csim_len += (len_csim_str[0] - '0') * 100;
    } else if (len_csim_str[0] != ' ') {
        return BAD_FUNC_ARG;
    }

    if ((cmdlen + cur_csim_len + CSIM_CMD_ENDSTR_SIZE) > IOTSAFE_CMDSIZE_MAX) {
        return MEMORY_E;
    }

    /* Read out current Lc parameter in the CSIM command, last byte in the
     * header
     */
    if (hex_to_bytes(cmd + AT_CSIM_CMD_SIZE + AT_CMD_LC_POS, &cur_lc, 1) < 0) {
        return BAD_FUNC_ARG;
    }

    /* Increase Lc and CSIM length according to the TLV len */
    cur_lc += 1 + taglen_size + len;
    cur_csim_len += 2 + (2 * taglen_size) + 2*len;

    /* Position the out buffer to the current end of string */
    out = cmd + XSTRLEN(cmd);
    off = 0;

    /* Write Tag */
    bytes_to_hex(&tag, out, 1);
    off += 2;

    /* Write Len */
    if (taglen_size == 2) {
        bytes_to_hex(&len_b[1], out + 2, 1);
        bytes_to_hex(&len_b[0], out + 4, 1);
        off += 4;
    } else {
        bytes_to_hex(&len_b[0], out + 2, 1);
        off += 2;
    }

    /* Write Val */
    ret = bytes_to_hex(val, out + off, len);
    off += ret;

    /* Terminate string */
    out[off] = 0;

    /* Write new CSIM command size */
    if (cur_csim_len > 99)
        len_csim_str[0] = (cur_csim_len / 100) + '0';
    else
        len_csim_str[0] = ' ';
    len_csim_str[1] = ((cur_csim_len % 100) / 10) + '0';
    len_csim_str[2] = (cur_csim_len % 10) + '0';

    /* Write new Lc value */
    bytes_to_hex(&cur_lc, cmd + AT_CSIM_CMD_SIZE + AT_CMD_LC_POS, 1);
    return off;
}

static int iotsafe_cmd_add_tlv16(char *cmd, byte tag, uint16_t len,
        const byte *val)
{
    return iotsafe_cmd_add_tlv_ex(cmd, tag, len, val, 2);
}

static int iotsafe_cmd_add_tlv(char *cmd, byte tag, byte len, const byte *val)
{
    return iotsafe_cmd_add_tlv_ex(cmd, tag, len, val, 1);
}

static void iotsafe_cmd_complete(char *cmd)
{
    word32 cmdlen = (word32)XSTRLEN(cmd);
    char *out;
    if (cmdlen + CSIM_CMD_ENDSTR_SIZE > IOTSAFE_CMDSIZE_MAX) {
        return;
    }
    out = cmd + cmdlen;
    out[0] = '"';
    out[1] = '\r';
    out[2] = '\n';
    out[3] = 0;
}

/** IOT-Safe Commands
 *
 *
 */
static int expect_csim_response(const char *cmd, word32 size, char **reply)
{
    const char csim_response_hdr[] = "+CSIM:";
    uint8_t len = 0;
    char *csim_reply;
    int ret;
    char *payload;
    char *endstr;
    ret = expect_tok(cmd, size, csim_response_hdr, &csim_reply);
    while (ret == 0) {
        ret = expect_tok("AT\r\n", 4, csim_response_hdr, &csim_reply);
    }
    if (ret < 1) {
        return ret;
    }
    payload = XSTRSTR(csim_reply, "\"");
    if (payload == NULL) {
        return -1;
    }
    payload++;
    if (XSTRNCMP(payload, "61", 2) == 0) {
        if (hex_to_bytes(payload + 2, &len, 1) == 1) {
            iotsafe_cmd_start(csim_cmd,1,IOTSAFE_INS_GETRESPONSE, 0, 0);
            bytes_to_hex(&len, csim_cmd + AT_CSIM_CMD_SIZE + AT_CMD_LC_POS, 1);
            csim_cmd[AT_CSIM_CMD_SIZE + AT_CMD_HDR_SIZE] = 0;
            iotsafe_cmd_complete(csim_cmd);
            ret = expect_tok(csim_cmd, (int)XSTRLEN(csim_cmd),
                    csim_response_hdr, reply);
            if (ret < 1)
                return -1;
            payload = XSTRSTR(*reply, "\"");
            if (payload == NULL)
                return -1;
            payload++;
        }
    }
    ret -= 2;
    if (ret >= 4) {
        endstr = XSTRSTR(payload, "9000\"");
        if (endstr == NULL) {
            endstr = XSTRSTR(payload, "\"");
        }
        if (endstr) {
            *endstr = 0;
            ret = (int)XSTRLEN(payload);
        }
    }
    payload[ret] = '\0';
    *reply = payload;
    return ret;
}

/* Internal initialization function.
 * Load the IoT-Safe applet
 */

static int iotsafe_init(void)
{
    char *reply;
    const char atcmd_load_applet_str[]=
        "AT+CSIM=24,\"01A4040007A0000005590010\"\r\n";
    int ret;

    do {
        ret = expect_ok("ATE0\r\n", 6);
        if (ret == 0)
            ret = expect_tok(NULL, 0, NULL, NULL);
    } while (ret == 0);
    if (ret < 0)
        return ret;

    WOLFSSL_MSG("ATE0 OK!");
    if (expect_csim_response(atcmd_load_applet_str,
                (word32)XSTRLEN(atcmd_load_applet_str), &reply) < 0) {
        WOLFSSL_MSG("FAIL: no Applet code response from iot-safe init");
        expect_ok("AT", 2);
    } else {
        WOLFSSL_MSG("IoT Safe Applet INIT OK");
    }
    if (expect_tok(NULL, 0, NULL, NULL) < 0)
        return -1;
    wolfIoT_initialized++;
    return 0;
}


/* internal: Read File content into a buffer */
static int iotsafe_readfile(uint8_t *file_id, uint16_t file_id_sz,
        unsigned char *content, int max_size)
{
    char *resp;
    int ret;
    uint16_t off = 0;
#ifdef IOTSAFE_NO_GETDATA
    int file_sz = max_size;
#else
    int file_sz = 0;
    char *filesz_s;

    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_GETDATA,
            IOTSAFE_GETDATA_FILE, 0);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_FILE_ID, file_id_sz, file_id);
    iotsafe_cmd_complete(csim_cmd);

    ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp);
    if (ret <= 0) {
        return ret;
    }

    filesz_s = search_tlv(resp + 4, ret, 0x20);
    if ((filesz_s) && (XSTRLEN(filesz_s)) >= 8) {
        uint8_t fs_msb, fs_lsb;
        if (hex_to_bytes(filesz_s + 4, &fs_msb, 1) < 0)
            return -1;
        if (hex_to_bytes(filesz_s + 6, &fs_lsb, 1) < 0)
            return -1;
        file_sz = (fs_msb << 8) + fs_lsb;
        WOLFSSL_MSG("Stat successful on file");
        #ifdef DEBUG_IOTSAFE
            printf("File size: %d (%04x)", file_sz, file_sz);
        #endif
    }

    if (file_sz > max_size) {
        WOLFSSL_MSG("iotsafe_readfile: insufficient space in buffer");
        return -1;
    }
#endif

    while (off < file_sz) {
        byte off_p1, off_p2;
        off_p1 = (off >> 8) & 0xff;
        off_p2 = (off & 0xff);
        iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_READ_FILE,
                off_p1, off_p2);
        iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_FILE_ID, file_id_sz, file_id);
        iotsafe_cmd_complete(csim_cmd);
        ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp);
        if (ret > 0) {
            if (ret > 2 * (file_sz - off))
                ret = 2 * (file_sz - off);
            if (hex_to_bytes(resp, content + off, (ret / 2)) < 0) {
                return -1;
            }
            off += ret/2;
#ifdef IOTSAFE_NO_GETDATA
            if (XSTRNCMP(&resp[ret-4], "0000", 4) == 0) {
                /* Strip trailing zeros */
                int idx = 0;
                for (idx = 0; idx < off-1; idx+=2) {
                    if (content[idx] == 0 && content[idx+1] == 0) {
                        off = idx;
#ifdef DEBUG_IOTSAFE
                        WOLFSSL_MSG("Stripped trailing zeros from cert buffer.");
                        WOLFSSL_BUFFER(content, off);
#endif
                        break;
                    }
                }
                break;
            }
#endif
        } else {
            WOLFSSL_MSG("IoTSafe: Error reading file.");
            return -1;
        }
    }

    return off;
}

static int iotsafe_getrandom(unsigned char* output, unsigned long sz)
{
    char *resp = NULL;
    int ret;
    int i;
    byte len = (byte)sz;
    if (sz == 0) {
        return BAD_FUNC_ARG;
    }
    if (!wolfIoT_initialized) {
        if (iotsafe_init() < 0) {
            return WC_HW_E;
        }
    }

    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_GETRANDOM,0,0);
    bytes_to_hex(&len, csim_cmd + AT_CSIM_CMD_SIZE + AT_CMD_LC_POS, 1);
    csim_cmd[AT_CSIM_CMD_SIZE + AT_CMD_HDR_SIZE] = 0;
    iotsafe_cmd_complete(csim_cmd);

    ret = expect_csim_response(csim_cmd, (word32)GETRAND_CMD_SIZE, &resp);
    if (ret <= 0) {
        WOLFSSL_MSG("Unexpected reply from RAND");
        ret = WC_HW_E;
    } else {
        ret = hexbuffer_conv(resp, output, sz);
        if (ret < 0)
            ret = WC_HW_E;
        else
            ret = 0;
    }

    /* Send an empty command until the applet is responsive again */
    for (i = 0; i < IOTSAFE_MAX_RETRIES; i++) {
        if (expect_tok(NULL, 0, NULL, NULL) < 0) {
            ret = WC_HW_E;
        }
    }
    return ret;
}



#ifdef HAVE_ECC

static int iotsafe_parse_public_key(char* resp, int len, ecc_key *key)
{
    int ret;
    char *rkey, *ktype, *payload_str;
    char Qx[IOTSAFE_ECC_KSIZE * 2 + 1], Qy[IOTSAFE_ECC_KSIZE * 2 + 1];
    if (len < IOTSAFE_TAG_ECC_KEY_FIELD_SZ + 2) {
        WOLFSSL_MSG("Response from iot-safe: too short");
        return BAD_STATE_E;
    }
    rkey = search_tlv(resp, len, IOTSAFE_TAG_ECC_KEY_FIELD);
    if (rkey == NULL) {
        WOLFSSL_MSG("IoT safe Error in rkey response");
        return MISSING_KEY;
    }
    ktype = search_tlv(rkey + 4, IOTSAFE_TAG_ECC_KEY_FIELD_SZ,
            IOTSAFE_TAG_ECC_KEY_TYPE);
    if (ktype == NULL) {
        WOLFSSL_MSG("IoT safe Error in ktype response");
        return MISSING_KEY;
    }
    payload_str = search_tlv(ktype + 4, IOTSAFE_TAG_ECC_KEY_TYPE_SZ,
            IOTSAFE_TAG_ECC_KEY_XY);
    if (payload_str == NULL) {
        WOLFSSL_MSG("IoT safe: Error in payload response");
        return MISSING_KEY;
    }
    XMEMSET(Qx, 0, sizeof(Qx));
    XMEMSET(Qy, 0, sizeof(Qy));
    ret = wc_ecc_init(key);
    if (ret < 0) {
        WOLFSSL_MSG("Cannot initialize ecc key to store IoTSafe public key");
        return -1;
    }
    XSTRNCPY(Qx, payload_str + 6, IOTSAFE_ECC_KSIZE * 2);
    XSTRNCPY(Qy, payload_str + 6 + IOTSAFE_ECC_KSIZE * 2, IOTSAFE_ECC_KSIZE * 2);

    ret = wc_ecc_import_raw_ex(key, Qx, Qy, NULL, IOTSAFE_ECC_KTYPE);
    if (ret < 0) {
        WOLFSSL_MSG("Could not import raw key into ecc key");
        return ret;
    }
    WOLFSSL_MSG("Get Public key: OK");
    return 0;
}

/* Execute GEN_KEYPAIR on the IoT-SAFE applet.
 *
 * Return -1 on error; 0 if the operation is successful, but
 * the generated public key was not yet stored in `key`; 1 if 
 * the operation is successful and the public key was found in the
 * command response and copied to the `key` structure, if not NULL.
 */
static int iotsafe_gen_keypair(byte *wr_slot, unsigned long id_size,
                               ecc_key *key)
{
    char *resp;
    int ret = WC_HW_E;
    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_GEN_KEYPAIR, 0, 0);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PRIVKEY_ID, id_size, wr_slot);
    iotsafe_cmd_complete(csim_cmd);
    ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp);
    if (ret < 1) {
        WOLFSSL_MSG("Unexpected reply from Keygen");
        ret = WC_HW_E;
    } else if (key != NULL) {
        if (iotsafe_parse_public_key(resp, ret, key) == 0) {
            /* iotsafe_parse_public_key was successful on response.
             * Return '1' here to indicate that the key is populated.
             */
            ret = 1;
        } else {
            /* The keygen operation was successful but we have not
             * retrieved the generated public key yet.
             */
            ret = 0;
        }
    } else {
        ret = 0;
    }
    return ret;
}

static int iotsafe_get_public_key(byte *pubkey_id, unsigned long id_size,
        ecc_key *key)
{
    int ret;
    char *resp;

    /* exporting generated public key */
    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_READ_KEY,0, 0);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PUBKEY_ID, id_size, pubkey_id);
    iotsafe_cmd_complete(csim_cmd);
    ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp);
    if (ret < 1) {
        WOLFSSL_MSG("Error exporting EPH public key from IoT-Safe");
        return WC_HW_E;
    }

    return iotsafe_parse_public_key(resp, ret, key);
}

#define PUT_PK_SID 0x02
static int iotsafe_put_public_key(byte *pubkey_id, unsigned long id_size,
        ecc_key *key)
{
    char *resp;
    int ret;
    int retries = 0;
    word32 qxlen = IOTSAFE_ECC_KSIZE, qylen = IOTSAFE_ECC_KSIZE;
    byte ecc_pub_raw[IOTSAFE_TAG_ECC_KEY_FIELD_SZ] =  {
        IOTSAFE_TAG_ECC_KEY_TYPE,
        IOTSAFE_TAG_ECC_KEY_TYPE_SZ,
        IOTSAFE_TAG_ECC_KEY_XY,
        IOTSAFE_TAG_ECC_KEY_XY_SZ,
        IOTSAFE_TAG_ECC_KEY_XY_HDR_BYTE
    };

    /* Export raw Qx, Qy values */
    ret = wc_ecc_export_public_raw(key,
        ecc_pub_raw + 5, &qxlen,
        ecc_pub_raw + 5 + IOTSAFE_ECC_KSIZE, &qylen);
    if (ret != 0) {
        WOLFSSL_MSG("IoT Safe: Could not export public key: Error");
        return ret;
    }

    /* Put Public Init */
    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_PUT_PUBLIC_INIT,
            0, PUT_PK_SID);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PUBKEY_ID, id_size, pubkey_id);
    iotsafe_cmd_complete(csim_cmd);
    if (expect_ok(csim_cmd, (word32)XSTRLEN(csim_cmd)) < 0) {
        WOLFSSL_MSG("Unexpected reply when storing public key");
        return WC_HW_E;
    }

    do {
        ret = expect_ok("AT\r\n", 4);
        if (ret == 0) {
            if (++retries > IOTSAFE_MAX_RETRIES)
                return WC_HW_E;
        }
    } while (ret == 0);
    retries = 0;

    if (ret > 0) {
        /* Put Public Update */
        iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_PUT_PUBLIC_UPDATE,
                IOTSAFE_DATA_LAST, PUT_PK_SID);
        iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_ECC_KEY_FIELD,
                IOTSAFE_TAG_ECC_KEY_FIELD_SZ, ecc_pub_raw);
        iotsafe_cmd_complete(csim_cmd);
        if (expect_csim_response(csim_cmd,
                    (word32)XSTRLEN(csim_cmd), &resp) < 0) {
            WOLFSSL_MSG("Unexpected reply when storing public key (update)");
            ret = WC_HW_E;
        } else {
            iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS,
                IOTSAFE_INS_PUT_PUBLIC_INIT, 1, PUT_PK_SID);
            iotsafe_cmd_complete(csim_cmd);
            if (expect_ok(csim_cmd, (word32)XSTRLEN(csim_cmd)) < 0) {
                ret = WC_HW_E;
            } else {
                ret = 0;
            }
            do {
                ret = expect_ok("AT\r\n", 4);
                if (ret == 0) {
                    if (++retries > IOTSAFE_MAX_RETRIES)
                        ret = WC_HW_E;
                }
            } while (ret == 0);
            if (ret > 0)
                ret = 0;
        }
    }
    return ret;
}
#ifdef HAVE_HKDF
static int iotsafe_hkdf_extract(byte* prk, const byte* salt, word32 saltLen,
       byte* ikm, word32 ikmLen, int digest)
{
    int ret;
    char *resp;
    uint16_t hash_algo = 0;
    int len;
    uint16_t hash_algo_be = 0;

    WOLFSSL_MSG("Enter iotsafe_hkdf_extract");
     switch (digest) {
        #ifndef NO_SHA256
        case WC_SHA256:
        hash_algo = (uint16_t)1;
        if (ikmLen == 0) {
            len = WC_SHA256_DIGEST_SIZE;
        }
            break;
        #endif
        #ifdef WOLFSSL_SHA384
        case WC_SHA384:
        hash_algo = (uint16_t)2;
        if (ikmLen == 0) {
            len = WC_SHA384_DIGEST_SIZE;
        }
            break;
        #endif
        #ifdef WOLFSSL_TLS13_SHA512
        case WC_SHA512:
        hash_algo = (uint16_t)4;
        if (ikmLen == 0) {
            len = WC_SHA512_DIGEST_SIZE;
        }
            break;
        #endif
        default:
            return BAD_FUNC_ARG;
            break;
     }

    if (ikmLen == 0) {
        ikmLen = len;
        XMEMSET(ikm, 0, len);
    }

#ifdef DEBUG_IOTSAFE
    printf("IOTSAFE PK HKDF Extract\n");
    printf("salt: ");
    for(word32 i = 0; i < saltLen; i++)
        printf("%02X", salt[i]);

    printf("\nikm: ");
    for(word32 i = 0; i < ikmLen; i++)
        printf("%02X", ikm[i]);

    printf("\nhash: %d\n", digest);
#endif

    hash_algo_be = XHTONS(hash_algo);

    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_HKDF_EXTRACT, 0, 0);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_SECRET, ikmLen, ikm);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_SALT, saltLen,salt);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_HASH_ALGO, 2, (const byte*)&hash_algo_be);
    iotsafe_cmd_complete(csim_cmd);
    if (expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp) < 1) {
        WOLFSSL_MSG("Unexpected reply from HKDF extract");
        ret = WC_HW_E;
    } else {

         ret = hexbuffer_conv(resp, prk, 32);
        if (ret < 0)
            ret = WC_HW_E;
        else
            ret = 0;
    }

    return ret;
}
#endif

static int iotsafe_sign_hash(byte *privkey_idx, uint16_t id_size,
        uint16_t hash_algo, uint8_t sign_algo, const byte *hash, word32 hashLen,
        byte *signature, word32 *sigLen)
{
    byte mode_of_operation = IOTSAFE_MOO_SIGN_ONLY;
    uint16_t hash_algo_be = XHTONS(hash_algo);
    int ret = WC_HW_E;
    char *resp;
    char R[2 * IOTSAFE_ECC_KSIZE + 1];
    char S[2 * IOTSAFE_ECC_KSIZE + 1];
    int retries = 0;

    R[2*IOTSAFE_ECC_KSIZE] = '\0';
    S[2*IOTSAFE_ECC_KSIZE] = '\0';

    WOLFSSL_MSG("Enter iotsafe_sign_hash");

    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_SIGN_INIT, 0, 1);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PRIVKEY_ID, id_size, privkey_idx);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_MODE_OF_OPERATION, 1,
            &mode_of_operation);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_HASH_ALGO, 2,
            (byte *)&hash_algo_be);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_SIGN_ALGO, 1, &sign_algo);
    iotsafe_cmd_complete(csim_cmd);

    if (sign_algo == IOTSAFE_SIGN_ECDSA) {
        if (*sigLen < 2 * IOTSAFE_ECC_KSIZE) {
            return -1;
        }
        if (expect_ok(csim_cmd, (word32)XSTRLEN(csim_cmd)) < 0) {
            WOLFSSL_MSG("Unexpected reply from IoTsafe EC sign");
            return WC_HW_E;
        }
        do {
            ret = expect_ok("AT\r\n", 4);
            if (ret == 0) {
                if (++retries > IOTSAFE_MAX_RETRIES)
                    return WC_HW_E;
            }
        } while (ret == 0);
        if (ret < 0)
            return WC_HW_E;
        retries = 0;

        /* Compose sign_update message with hash to sign */
        iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS,
                IOTSAFE_INS_SIGN_UPDATE,
                IOTSAFE_DATA_LAST,
                0x01);
        iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_HASH_FIELD, hashLen, hash);
        iotsafe_cmd_complete(csim_cmd);

        ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp);
        if (ret >= 0) {
            byte sig_hdr[3];
            if (hex_to_bytes(resp, sig_hdr, 3) < 0) {
               ret = BAD_FUNC_ARG;
            }
#ifdef IOTSAFE_SIG_8BIT_LENGTH
            else if ((sig_hdr[0] == IOTSAFE_TAG_SIGNATURE_FIELD) &&
                       (sig_hdr[1] == 2 * IOTSAFE_ECC_KSIZE)) {
                XSTRNCPY(R, resp + 4, IOTSAFE_ECC_KSIZE * 2);
                XSTRNCPY(S, resp + 4 + IOTSAFE_ECC_KSIZE * 2,
                        IOTSAFE_ECC_KSIZE * 2);
                ret = wc_ecc_rs_to_sig(R, S, signature, sigLen);
            }
#endif
            else if ((sig_hdr[0] == IOTSAFE_TAG_SIGNATURE_FIELD) &&
                       (sig_hdr[1] == 0) &&
                       (sig_hdr[2] == 2 * IOTSAFE_ECC_KSIZE)) {
                XSTRNCPY(R, resp + 6, IOTSAFE_ECC_KSIZE * 2);
                XSTRNCPY(S, resp + 6 + IOTSAFE_ECC_KSIZE * 2,
                        IOTSAFE_ECC_KSIZE * 2);
                ret = wc_ecc_rs_to_sig(R, S, signature, sigLen);
            } else {
                ret = WC_HW_E;
                WOLFSSL_MSG("Invalid response from EC sign update");
            }
        } else {
            WOLFSSL_MSG("Invalid/no response from EC sign update");
        }

        /* Terminate sign/sign session. */
        iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_SIGN_INIT, 1, 1);
        iotsafe_cmd_complete(csim_cmd);
        if (expect_ok(csim_cmd, (word32)XSTRLEN(csim_cmd)) < 0) {
            ret = WC_HW_E;
        }
        do {
            ret = expect_ok("AT\r\n", 4);
            if (ret == 0) {
                if (++retries > IOTSAFE_MAX_RETRIES)
                    ret = WC_HW_E;
            }
        } while (ret == 0);
        if (ret < 0)
            return WC_HW_E;
        else
            ret = 0;
    } else {
        ret = NOT_COMPILED_IN; /* RSA not yet supported */
    }
    WOLFSSL_MSG("iotsafe_sign_hash return point");
    return ret;
}

static int iotsafe_verify_hash(byte *pubkey_idx, uint16_t id_size,
        uint16_t hash_algo, uint8_t sign_algo,
        const byte *hash, word32 hashLen,
        const byte *sig, word32 sigLen,
        int *result)
{
    byte mode_of_operation = IOTSAFE_MOO_SIGN_ONLY;
    uint16_t hash_algo_be = XHTONS(hash_algo);
    int ret = 1;
    char *resp;
    int retries = 0;
    *result = 0;

    iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_VERIFY_INIT, 0, 1);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PUBKEY_ID, id_size, pubkey_idx);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_MODE_OF_OPERATION, 1,
            &mode_of_operation);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_HASH_ALGO, 2,
            (byte *)&hash_algo_be);
    iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_SIGN_ALGO, 1, &sign_algo);
    iotsafe_cmd_complete(csim_cmd);

    if (sign_algo == IOTSAFE_SIGN_ECDSA) {
        if (expect_ok(csim_cmd, (word32)XSTRLEN(csim_cmd)) < 0) {
            WOLFSSL_MSG("Unexpected reply from IoTsafe EC verify");
            return WC_HW_E;
        }
        do {
            ret = expect_ok("AT\r\n", 4);
            if (ret == 0) {
                if (++retries > IOTSAFE_MAX_RETRIES)
                    return WC_HW_E;
            }
        } while (ret == 0);
        if (ret < 0)
            return WC_HW_E;
        retries = 0;

        /* Compose verify_update message with hash + signature */
        iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS,
                IOTSAFE_INS_VERIFY_UPDATE,
                IOTSAFE_DATA_LAST,
                0x01);
        iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_HASH_FIELD, hashLen, hash);
        iotsafe_cmd_add_tlv16(csim_cmd, IOTSAFE_TAG_SIGNATURE_FIELD,
                sigLen,sig);
        iotsafe_cmd_complete(csim_cmd);

        ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd), &resp);
        if (ret < 0) {
            WOLFSSL_MSG("Invalid response from EC verify update");
        } else if (ret > 0) {
            if (XSTRCMP("6D01", resp) == 0) {
                WOLFSSL_MSG("Invalid signature!");
                /* 6D01 : Invalid signature. Return 0 and set *result to 0 */
                *result = 0;
                ret = 0;
            } else {
                ret = WC_HW_E; /* IoT-safe bus error */
            }
        }
        else if (ret == 0) {
            *result = 1;
            ret = 0;
        }

        /* Terminate sign/verify session. */
        iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_VERIFY_INIT,1,1);
        iotsafe_cmd_complete(csim_cmd);
        if (expect_ok(csim_cmd, (word32)XSTRLEN(csim_cmd)) < 0) {
            *result = 0;
            ret = WC_HW_E;
        }

        do {
            ret = expect_ok("AT\r\n", 4);
            if (ret == 0) {
                if (++retries > IOTSAFE_MAX_RETRIES)
                    return WC_HW_E;
            }
        } while (ret == 0);
        if (ret < 0) {
            return WC_HW_E;
        } else {
            ret = 0;
        }
        retries = 0;
    } else {
        /* TODO: RSA */
        ret = NOT_COMPILED_IN;
    }
    return ret;
}


/*
 * Callbacks for IoT-Safe ECC/ECDH functions
 *
 *
 */

#ifndef IOTSAFE_ID_SIZE
#   define IOTSAFE_ID_SIZE (1)
#endif

#ifdef HAVE_PK_CALLBACKS
static int wolfIoT_ecc_keygen(WOLFSSL* ssl, struct ecc_key* key,
    unsigned int keySz, int ecc_curve, void* ctx)
{
    int ret;
    IOTSAFE *iotsafe = wolfSSL_get_iotsafe_ctx(ssl);

    if (iotsafe == NULL) {
        return BAD_FUNC_ARG;
    }
    WOLFSSL_MSG("IOTSAFE: Called wolfIoT_ecc_keygen");

#ifdef DEBUG_IOTSAFE
    printf("IOTSAFE PK ECC KeyGen: keySz %d, Curve ID %d, Slot %d\n",
        keySz, ecc_curve, iotsafe->ecdh_keypair_slot);
#endif

    if (iotsafe->enabled) {
        ret = iotsafe_gen_keypair((byte *)&iotsafe->ecdh_keypair_slot,
                IOTSAFE_ID_SIZE, key);
        if (ret == 0) {
            ret = iotsafe_get_public_key((byte *)&iotsafe->ecdh_keypair_slot,
                    IOTSAFE_ID_SIZE, key);
        } else if (ret > 0) {
            /* Key has been stored during generation */
            ret = 0;
        }
    } else {
        WC_RNG *rng = wolfSSL_GetRNG(ssl);
        ret = wc_ecc_init(key);
        if (ret == 0) {
            /* create new key */
            ret = wc_ecc_make_key_ex(rng, keySz, key, ecc_curve);
        }
    }
    (void)ctx;
    return ret;
}

#ifdef HAVE_HKDF
static int wolfIoT_hkdf_extract(byte* prk, const byte* salt, word32 saltLen,
       byte* ikm, word32 ikmLen, int digest, void* ctx)
{
    int ret;
    const  byte* localSalt;  /* either points to user input or tmp */
    byte tmp[WC_MAX_DIGEST_SIZE]; /* localSalt helper */

    WOLFSSL_MSG("IOTSAFE: Called wolfIoT_hkdf_extract\n");

    localSalt = salt;

    if (saltLen == 0) {
        ret = wc_HmacSizeByType(digest);
            if (ret < 0)
                return ret;
            saltLen = ret;
            if (localSalt == NULL) {
                XMEMSET(tmp, 0, saltLen);
                localSalt = tmp;
            }
    }
    
    ret = iotsafe_hkdf_extract(prk, localSalt, saltLen, ikm, ikmLen, digest);    
    (void)ctx;
    return ret;
}       
#endif

static int wolfIoT_ecc_sign(WOLFSSL* ssl,
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, word32* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx)
{
    int ret;
    IOTSAFE *iotsafe = wolfSSL_get_iotsafe_ctx(ssl);
    WC_RNG    *rng;
    word32    idx = 0;
    ecc_key   *myKey;
    byte*     keyBuf = (byte*)keyDer;
#ifndef WOLFSSL_SMALL_STACK
    ecc_key _myKey;
#endif

    if (iotsafe == NULL) {
        return BAD_FUNC_ARG;
    }
    WOLFSSL_MSG("IOTSAFE: Called wolfIoT_ecc_sign");

#ifdef DEBUG_IOTSAFE
    printf("IOTSAFE PK ECC Sign: InSz %d, KeySz %d\n", inSz, keySz);
#endif

    if (iotsafe->enabled) {
        ret = iotsafe_sign_hash((byte *)&iotsafe->privkey_id, IOTSAFE_ID_SIZE,
                IOTSAFE_HASH_SHA256,
                IOTSAFE_SIGN_ECDSA,
                in, inSz, out, outSz);
        return ret;
    }

#ifndef WOLFSSL_SMALL_STACK
    myKey = &_myKey;
#else
    myKey = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
    if (myKey == NULL)
        return MEMORY_E;
#endif

    rng = wolfSSL_GetRNG(ssl);
    ret = wc_ecc_init(myKey);
    if (ret == 0)
        ret = wc_EccPrivateKeyDecode(keyBuf, &idx, myKey, keySz);
    if (ret == 0)
        ret = wc_ecc_sign_hash(in, inSz, out, outSz, rng, myKey);
    wc_ecc_free(myKey);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(myKey, NULL, DYNAMIC_TYPE_ECC);
#endif

    (void)ctx;
    return ret;
}

static int wolfIoT_ecc_verify(WOLFSSL *ssl,
       const unsigned char* sig, unsigned int sigSz,
       const unsigned char* hash, unsigned int hashSz,
       const unsigned char* keyDer, unsigned int keySz,
       int* result, void* ctx)
{

    int ret;
    ecc_key *key;
    word32 r_size = IOTSAFE_ECC_KSIZE, s_size = IOTSAFE_ECC_KSIZE;
    uint16_t id_size = IOTSAFE_ID_SIZE;
    word32 inOutIdx = 0;
    IOTSAFE *iotsafe = wolfSSL_get_iotsafe_ctx(ssl);
    byte *pubkey_slot;
    byte *sig_raw;
#ifndef WOLFSSL_SMALL_STACK
    byte _sig_raw[IOTSAFE_ECC_KSIZE* 2];
    ecc_key _key;

    sig_raw = _sig_raw;
    key = &_key;
#endif

    if (iotsafe == NULL) {
        return BAD_FUNC_ARG;
    }

    pubkey_slot = (byte *)&iotsafe->peer_cert_slot;

    WOLFSSL_MSG("IOTSAFE: Called wolfIoT_ecc_verify");

#ifdef DEBUG_IOTSAFE
    printf("IOTSAFE PK ECC Verify: SigSz %d, HashSz %d, KeySz %d, Slot %d\n",
        sigSz, hashSz, keySz, *pubkey_slot);
#endif

    /* Invalidate verification, by default. */
    *result = 0;

#ifdef WOLFSSL_SMALL_STACK
    sig_raw = (byte*)XMALLOC(IOTSAFE_ECC_KSIZE * 2, NULL, DYNAMIC_TYPE_SIGNATURE);
    if (sig_raw == NULL)
        return MEMORY_E;
    key = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
    if (key == NULL) {
        XFREE(sig_raw, NULL, DYNAMIC_TYPE_SIGNATURE);
        return MEMORY_E;
    }
#endif
    ret = wc_ecc_init(key);
    if (ret != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(key, NULL, DYNAMIC_TYPE_ECC);
        XFREE(sig_raw, NULL, DYNAMIC_TYPE_SIGNATURE);
    #endif
        return ret;
    }

    if (iotsafe->enabled) {
        /* Store the server's public key in IoT-safe vault
         * Create an ecc object to handle the key */

        /* Convert ECC signature into R,S */
        ret = wc_ecc_sig_to_rs(sig, sigSz,
            sig_raw, &r_size,
            sig_raw + IOTSAFE_ECC_KSIZE, &s_size);
        if (ret == 0) {
            /* Import from keyDer argument */
            ret = wc_EccPublicKeyDecode(keyDer, &inOutIdx, key, keySz);
        }
        if (ret == 0) {
            /* Store public key in IoT-safe slot */
            ret = iotsafe_put_public_key(pubkey_slot, id_size, key);
            if (ret < 0) {
            #ifdef DEBUG_IOTSAFE
                printf("IOTSAFE: put public key failed\n");
            #endif
            }
        }
        if (ret == 0) {
            /* Call iotsafe_verify_hash with ECC256 + SHA256 */
            ret = iotsafe_verify_hash(pubkey_slot, id_size,
                    IOTSAFE_HASH_SHA256, IOTSAFE_SIGN_ECDSA,
                    hash, hashSz, sig_raw, 2 * IOTSAFE_ECC_KSIZE,
                    result);
        }
    }
    else {
        ret = wc_EccPublicKeyDecode(keyDer, &inOutIdx, key, keySz);
        if (ret == 0) {
            ret = wc_ecc_verify_hash(sig, sigSz, hash, hashSz, result, key);
        }
    }
    wc_ecc_free(key);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(key, NULL, DYNAMIC_TYPE_ECC);
    XFREE(sig_raw, NULL, DYNAMIC_TYPE_SIGNATURE);
#endif
    (void)ctx;
    return ret;
}

static int wolfIoT_ecc_shared_secret(WOLFSSL* ssl, struct ecc_key* otherKey,
        unsigned char* pubKeyDer, word32* pubKeySz,
        unsigned char* out, word32* outlen,
        int side, void* ctx)
{
    int ret;
    char *resp;
    ecc_key *tmpKey;
    IOTSAFE *iotsafe = wolfSSL_get_iotsafe_ctx(ssl);
    byte *keypair_slot;
    byte *pubkey_idx;
    int retries = 0;
#ifndef WOLFSSL_SMALL_STACK
    ecc_key _tmpKey;
    tmpKey = &_tmpKey;
#endif

    if (iotsafe == NULL) {
        return BAD_FUNC_ARG;
    }

    WOLFSSL_MSG("IOTSAFE: Called wolfIoT_ecc_shared_secret");

#ifdef DEBUG_IOTSAFE
    printf("IOTSAFE PK ECC PMS: Side %s, Peer Curve %d\n",
        side == WOLFSSL_CLIENT_END ? "client" : "server", otherKey->dp->id);
#endif

#ifdef WOLFSSL_SMALL_STACK
    tmpKey = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
    if (tmpKey == NULL)
        return MEMORY_E;
#endif
    ret = wc_ecc_init(tmpKey);
    if (ret != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(tmpKey, NULL, DYNAMIC_TYPE_ECC);
    #endif
        return ret;
    }

    if (iotsafe->enabled) {
        uint16_t id_size = IOTSAFE_ID_SIZE;
        keypair_slot = (byte *)(&iotsafe->ecdh_keypair_slot);
        pubkey_idx = (byte *)(&iotsafe->peer_pubkey_slot);

        /* TLS v1.3 calls key gen already, so don't do it here */
        if (wolfSSL_GetVersion(ssl) < WOLFSSL_TLSV1_3) {
            WOLFSSL_MSG("Generating ECDH key pair");
            ret = iotsafe_gen_keypair(keypair_slot, id_size, tmpKey);
            if (ret < 0) {
                WOLFSSL_MSG("Error generating IoT-safe key pair");
            }
            if (ret == 0) {
                WOLFSSL_MSG("Public key not yet retrieved, using GetPublic");
                /* Importing generated public key */
                ret = iotsafe_get_public_key(keypair_slot, id_size, tmpKey);
                if (ret < 0) {
                    WOLFSSL_MSG("Error retrieving public key via GetPublic");
                    ret = WC_HW_E;
                }
            } else if (ret == 1) {
               ret = 0;
            }
            if (ret == 0) {
                /* Exporting generated public key into DER buffer */
                ret = wc_ecc_export_x963(tmpKey, pubKeyDer, pubKeySz);
                if (ret == 0) {
                    WOLFSSL_MSG(
                            "IoT-SAFE: Key pair generated, public key exported");
                } else {
                    WOLFSSL_MSG("IoT-SAFE: Error storing Public key.");
                }
            }
        }

        if (ret == 0) {
            /* Store received public key from other endpoint in applet */
            ret = iotsafe_put_public_key(pubkey_idx, id_size, otherKey);
            if (ret < 0) {
                WOLFSSL_MSG("IoT-SAFE: Error in PutPublic");
            }
        }
        if (ret == 0) {
            do {
                ret = expect_ok("AT\r\n", 4);
                if (ret == 0) {
                    if (++retries > IOTSAFE_MAX_RETRIES)
                        ret = WC_HW_E;
                }
            } while (ret == 0);
            if (ret > 0) {
                /* Generating shared secret
                */
                iotsafe_cmd_start(csim_cmd, IOTSAFE_CLASS, IOTSAFE_INS_COMPUTE_DH,
                        0, 0);
                iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PRIVKEY_ID,
                        IOTSAFE_ID_SIZE, keypair_slot);
                iotsafe_cmd_add_tlv(csim_cmd, IOTSAFE_TAG_PUBKEY_ID,
                        IOTSAFE_ID_SIZE, pubkey_idx);
                iotsafe_cmd_complete(csim_cmd);
                ret = expect_csim_response(csim_cmd, (word32)XSTRLEN(csim_cmd),
                        &resp);
            }
        }
        if (ret <= 0) {
            WOLFSSL_MSG("Unexpected reply in ECDH command");
            ret = WC_HW_E;
        } else {
            int out_len = hex_to_bytes(resp, out, ret / 2);
            if (out_len < 0) {
                ret = BAD_FUNC_ARG;
            } else {
                *outlen = (word32)out_len;
                ret = 0;
            }
        }
    } else {
        ecc_key*  privKey = NULL;
        ecc_key*  pubKey = NULL;
        /* for client: create and export public key */
        if (side == WOLFSSL_CLIENT_END) {
            privKey = tmpKey;
            pubKey = otherKey;

            /* TLS v1.3 calls key gen already, so don't do it here */
            if (wolfSSL_GetVersion(ssl) < WOLFSSL_TLSV1_3) {
                WC_RNG *rng = wolfSSL_GetRNG(ssl);
                ret = wc_ecc_make_key_ex(rng, 0, privKey, otherKey->dp->id);
                if (ret == 0) {
                    ret = wc_ecc_export_x963(privKey, pubKeyDer, pubKeySz);
                }
            }
        }
        /* for server: import public key */
        else if (side == WOLFSSL_SERVER_END) {
            privKey = otherKey;
            pubKey = tmpKey;

            ret = wc_ecc_import_x963_ex(pubKeyDer, *pubKeySz, pubKey,
                    otherKey->dp->id);
        }
        else {
            ret = BAD_FUNC_ARG;
        }
        /* generate shared secret and return it */
        if (ret == 0) {
            ret = wc_ecc_shared_secret(privKey, pubKey, out, outlen);
        }
    }
    wc_ecc_free(tmpKey);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpKey, NULL, DYNAMIC_TYPE_ECC);
#endif
    (void)ctx;
    return ret;
}

#ifndef NO_DH
/* Public DH Key Callback support */
static int wolfIoT_dh_agree(WOLFSSL* ssl, struct DhKey* key,
        const unsigned char* priv, unsigned int privSz,
        const unsigned char* otherPubKeyDer, unsigned int otherPubKeySz,
        unsigned char* out, word32* outlen,
        void* ctx)
{

    WOLFSSL_MSG("IOTSAFE: Called wolfIoT_ecc_dh_agree");
    (void)ctx;
    (void)ssl;
    (void)key;
    (void)priv;
    (void)privSz;
    (void)otherPubKeyDer;
    (void)otherPubKeySz;
    (void)out;
    (void)outlen;

    /* return an error to notify user this function
     * is not yet implemented
     */
    return WC_HW_E;
}
#endif /* ! NO_DH */

#endif /* HAVE_PK_CALLBACKS */

/* Public API for ecc */

int wc_iotsafe_ecc_import_public_ex(ecc_key *key, byte *key_id, uint16_t id_size)
{
    return iotsafe_get_public_key(key_id, id_size, key);
}

int wc_iotsafe_ecc_export_public_ex(ecc_key *key, byte *key_id, uint16_t id_size)
{
    return iotsafe_put_public_key(key_id, id_size, key);
}

int wc_iotsafe_ecc_sign_hash_ex(byte *in, word32 inlen, byte *out,
        word32 *outlen, byte *key_id, uint16_t id_size)
{
    return iotsafe_sign_hash(key_id, id_size, IOTSAFE_HASH_SHA256,
            IOTSAFE_SIGN_ECDSA, in, inlen, out, outlen);
}

int wc_iotsafe_ecc_verify_hash_ex(byte *sig, word32 siglen, byte *hash,
        word32 hashlen, int *res, byte *key_id, uint16_t id_size)
{
    int ret;
    word32 r_size = IOTSAFE_ECC_KSIZE;
    word32 s_size = IOTSAFE_ECC_KSIZE;
    byte sig_raw[IOTSAFE_ECC_KSIZE * 2];

    /* Invalidate result, by default. */
    *res = 0;

    /* Convert ECC signature into R,S */
    ret = wc_ecc_sig_to_rs(sig, siglen, sig_raw, &r_size, sig_raw
            + IOTSAFE_ECC_KSIZE, &s_size);
    if (ret == 0)
        ret = iotsafe_verify_hash(key_id, id_size, IOTSAFE_HASH_SHA256,
                IOTSAFE_SIGN_ECDSA, hash, hashlen, sig_raw, 2 * IOTSAFE_ECC_KSIZE,
                res);
    return ret;
}

int wc_iotsafe_ecc_gen_k_ex(byte *key_id, uint16_t id_size)
{
    return iotsafe_gen_keypair(key_id, id_size, NULL);
}

int wc_iotsafe_ecc_import_public(ecc_key *key, byte key_id)
{
    return iotsafe_get_public_key(&key_id, 1, key);
}

int wc_iotsafe_ecc_export_public(ecc_key *key, byte key_id)
{
    return iotsafe_put_public_key(&key_id, 1, key);
}

int wc_iotsafe_ecc_sign_hash(byte *in, word32 inlen, byte *out,
        word32 *outlen, byte key_id)
{
    return iotsafe_sign_hash(&key_id, 1, IOTSAFE_HASH_SHA256, IOTSAFE_SIGN_ECDSA,
            in, inlen, out, outlen);
}


int wc_iotsafe_ecc_verify_hash(byte *sig, word32 siglen, byte *hash,
        word32 hashlen, int *res, byte key_id)
{
    return wc_iotsafe_ecc_verify_hash_ex(sig, siglen, hash, hashlen, res,
            &key_id, 1);
}
int wc_iotsafe_ecc_gen_k(byte key_id)
{
    return iotsafe_gen_keypair(&key_id, 1, NULL);
}

#endif /* HAVE_ECC */

/* API for GetRandom, used as custom Rand seed or block.
 * Returns 0 on success. */
int wolfIoTSafe_GetRandom(unsigned char *out, word32 sz)
{
    return iotsafe_getrandom(out, sz);
}

/* API for GetCert (proxy for Read File / Close File)
 */

int wolfIoTSafe_GetCert_ex(uint8_t *id, uint16_t id_sz, unsigned char *output,
        unsigned long sz)
{
    XMEMSET(output, 0, sz);
    return iotsafe_readfile(id, id_sz, output, (word32)sz);
}

int wolfIoTSafe_GetCert(uint8_t id, unsigned char *output, unsigned long sz)
{
    return wolfIoTSafe_GetCert_ex(&id, 1, output, sz);
}

/* API to set target specific I/O callbacks */
void wolfIoTSafe_SetCSIM_read_cb(wolfSSL_IOTSafe_CSIM_read_cb rf)
{
    csim_read_cb = rf;
}

void wolfIoTSafe_SetCSIM_write_cb(wolfSSL_IOTSafe_CSIM_write_cb wf)
{
    csim_write_cb = wf;
}

/* API to equip target wolfSSL CTX to the IoT-Safe subsystem. */
int wolfSSL_CTX_iotsafe_enable(WOLFSSL_CTX *ctx)
{
    if ( !wolfIoT_initialized) {
        if (iotsafe_init() < 0)
            return WC_HW_E;
    }

#if defined(HAVE_PK_CALLBACKS)
    #ifdef HAVE_ECC
    wolfSSL_CTX_SetEccSignCb(ctx, wolfIoT_ecc_sign);
    wolfSSL_CTX_SetEccVerifyCb(ctx, wolfIoT_ecc_verify);
    wolfSSL_CTX_SetEccKeyGenCb(ctx, wolfIoT_ecc_keygen);
    wolfSSL_CTX_SetEccSharedSecretCb(ctx, wolfIoT_ecc_shared_secret);
    #ifdef HAVE_HKDF
    wolfSSL_CTX_SetHKDFExtractCb(ctx, wolfIoT_hkdf_extract);
    #endif
    #ifndef NO_DH
    wolfSSL_CTX_SetDhAgreeCb(ctx, wolfIoT_dh_agree);
    #endif /* NO_DH */
    WOLFSSL_MSG("ECC callbacks set to IoT_safe interface");
    #endif
    #ifndef NO_RSA
    /* wolfSSL_CTX_SetRsaSignCb(wolfIoT_rsa_sign);  // TODO: RSA callbacks */
    #endif
#else
    (void)ctx;
#endif
    return 0;
}

int wolfSSL_iotsafe_on_ex(WOLFSSL *ssl, byte *privkey_id, byte *ecdh_keypair_slot,
        byte *peer_pubkey_slot, byte *peer_cert_slot, word16 id_size)
{
#if defined(HAVE_PK_CALLBACKS)
    int ret;
    IOTSAFE iotsafe;
    XMEMCPY(&iotsafe.privkey_id, privkey_id, id_size);
    XMEMCPY(&iotsafe.ecdh_keypair_slot, ecdh_keypair_slot, id_size);
    XMEMCPY(&iotsafe.peer_pubkey_slot, peer_pubkey_slot, id_size);
    XMEMCPY(&iotsafe.peer_cert_slot, peer_cert_slot, id_size);
    iotsafe.enabled = 1;
    ret = wolfSSL_set_iotsafe_ctx(ssl, &iotsafe);
    if (ret == 0) {
        WOLFSSL_MSG("IOTSAFE is ON");
    } else {
        WOLFSSL_MSG("Error setting IOT-Safe Context on SSL Session");
    }
    return ret;
#else
    (void)ssl;
    (void)privkey_id;
    (void)ecdh_keypair_slot;
    (void)peer_cert_slot;
    (void)peer_pubkey_slot;
    (void)id_size;
    return NOT_COMPILED_IN;
#endif
}

int wolfSSL_iotsafe_on(WOLFSSL *ssl, byte privkey_id,
       byte ecdh_keypair_slot, byte peer_pubkey_slot, byte peer_cert_slot)
{
    return wolfSSL_iotsafe_on_ex(ssl, &privkey_id, &ecdh_keypair_slot,
            &peer_pubkey_slot, &peer_cert_slot, 1);
}
#endif /* WOLFSSL_IOTSAFE */
