/* sniffer_error.h
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



#ifndef WOLFSSL_SNIFFER_ERROR_H
#define WOLFSSL_SNIFFER_ERROR_H


/* General Sniffer Error */
#define WOLFSSL_SNIFFER_ERROR -1

/* Sniffer failed and is in Fatal state */
#define WOLFSSL_SNIFFER_FATAL_ERROR -2

/* need to have errors as #defines since .rc files can't handle enums */
/* need to start at 1 and go in order for same reason */

#define MEMORY_STR 1
#define NEW_SERVER_STR 2
#define IP_CHECK_STR 3
#define SERVER_NOT_REG_STR 4
#define TCP_CHECK_STR 5
#define SERVER_PORT_NOT_REG_STR 6
#define RSA_DECRYPT_STR 7
#define RSA_DECODE_STR 8
#define BAD_CIPHER_SPEC_STR 9
#define SERVER_HELLO_INPUT_STR 10

#define BAD_SESSION_RESUME_STR 11
#define SERVER_DID_RESUMPTION_STR 12
#define CLIENT_HELLO_INPUT_STR 13
#define CLIENT_RESUME_TRY_STR 14
#define HANDSHAKE_INPUT_STR 15
#define GOT_HELLO_VERIFY_STR 16
#define GOT_SERVER_HELLO_STR 17
#define GOT_CERT_REQ_STR 18
#define GOT_SERVER_KEY_EX_STR 19
#define GOT_CERT_STR 20

#define GOT_SERVER_HELLO_DONE_STR 21
#define GOT_FINISHED_STR 22
#define GOT_CLIENT_HELLO_STR 23
#define GOT_CLIENT_KEY_EX_STR 24
#define GOT_CERT_VER_STR 25
#define GOT_UNKNOWN_HANDSHAKE_STR 26
#define NEW_SESSION_STR 27
#define BAD_NEW_SSL_STR 28
#define GOT_PACKET_STR 29
#define NO_DATA_STR 30

#define BAD_SESSION_STR 31
#define GOT_OLD_CLIENT_HELLO_STR 32
#define OLD_CLIENT_INPUT_STR 33
#define OLD_CLIENT_OK_STR 34
#define BAD_OLD_CLIENT_STR 35
#define BAD_RECORD_HDR_STR 36
#define RECORD_INPUT_STR 37
#define GOT_HANDSHAKE_STR 38
#define BAD_HANDSHAKE_STR 39
#define GOT_CHANGE_CIPHER_STR 40

#define GOT_APP_DATA_STR 41
#define BAD_APP_DATA_STR 42
#define GOT_ALERT_STR 43
#define ANOTHER_MSG_STR 44
#define REMOVE_SESSION_STR 45
#define KEY_FILE_STR 46
#define BAD_IPVER_STR 47
#define BAD_PROTO_STR 48
#define PACKET_HDR_SHORT_STR 49
#define GOT_UNKNOWN_RECORD_STR 50

#define BAD_TRACE_FILE_STR 51
#define FATAL_ERROR_STR 52
#define PARTIAL_INPUT_STR 53
#define BUFFER_ERROR_STR 54
#define PARTIAL_ADD_STR 55
#define DUPLICATE_STR 56
#define OUT_OF_ORDER_STR 57
#define OVERLAP_DUPLICATE_STR 58
#define OVERLAP_REASSEMBLY_BEGIN_STR 59
#define OVERLAP_REASSEMBLY_END_STR 60

#define MISSED_CLIENT_HELLO_STR 61
#define GOT_HELLO_REQUEST_STR 62
#define GOT_SESSION_TICKET_STR 63
#define BAD_INPUT_STR 64
#define BAD_DECRYPT_TYPE 65
#define BAD_FINISHED_MSG 66
#define BAD_COMPRESSION_STR 67
#define BAD_DERIVE_STR 68
#define ACK_MISSED_STR 69
#define BAD_DECRYPT    70

#define DECRYPT_KEYS_NOT_SETUP 71
#define CLIENT_HELLO_LATE_KEY_STR 72
#define GOT_CERT_STATUS_STR 73
#define RSA_KEY_MISSING_STR 74
#define NO_SECURE_RENEGOTIATION 75
#define BAD_SESSION_STATS 76
#define REASSEMBLY_MAX_STR 77
#define DROPPING_LOST_FRAG_STR 78
#define DROPPING_PARTIAL_RECORD 79
#define CLEAR_ACK_FAULT 80

#define BAD_DECRYPT_SIZE 81
#define EXTENDED_MASTER_HASH_STR 82
#define SPLIT_HANDSHAKE_MSG_STR 83
#define ECC_DECODE_STR 84
#define ECC_PUB_DECODE_STR 85
#define WATCH_CB_MISSING_STR 86
#define WATCH_HASH_STR 87
#define WATCH_FAIL_STR 88
#define BAD_CERT_MSG_STR 89
#define STORE_DATA_CB_MISSING_STR 90

#define NO_DATA_DEST_STR 91
#define STORE_DATA_FAIL_STR 92
#define CHAIN_INPUT_STR 93
#define GOT_ENC_EXT_STR 94
#define GOT_HELLO_RETRY_REQ_STR 95

#define SNIFFER_KEY_SETUP_STR 96
#define UNSUPPORTED_TLS_VER_STR 97
#define KEY_MISMATCH_STR 98
/* !!!! also add to msgTable in sniffer.c and .rc file !!!! */


#endif /* wolfSSL_SNIFFER_ERROR_H */

