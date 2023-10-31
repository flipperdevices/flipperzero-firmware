/* srp.h
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

/*!
    \file wolfssl/wolfcrypt/srp.h
*/

#ifdef WOLFCRYPT_HAVE_SRP

#ifndef WOLFCRYPT_SRP_H
#define WOLFCRYPT_SRP_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/wolfmath.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Select the largest available hash for the buffer size. */
#if defined(WOLFSSL_SHA512)
    #define SRP_MAX_DIGEST_SIZE WC_SHA512_DIGEST_SIZE
#elif defined(WOLFSSL_SHA384)
    #define SRP_MAX_DIGEST_SIZE WC_SHA384_DIGEST_SIZE
#elif !defined(NO_SHA256)
    #define SRP_MAX_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
#elif !defined(NO_SHA)
    #define SRP_MAX_DIGEST_SIZE WC_SHA_DIGEST_SIZE
#else
    #error "You have to have some kind of SHA hash if you want to use SRP."
#endif

/* Set the minimum number of bits acceptable in an SRP modulus */
#define SRP_MODULUS_MIN_BITS 512

/* Set the minimum number of bits acceptable for private keys (RFC 5054) */
#define SRP_PRIVATE_KEY_MIN_BITS 256

/* salt size for SRP password */
#define SRP_SALT_SIZE  16

/**
 * SRP side, client or server.
 */
typedef enum {
    SRP_CLIENT_SIDE  = 0,
    SRP_SERVER_SIDE  = 1
} SrpSide;

/**
 * SRP hash type, SHA[1|256|384|512].
 */
typedef enum {
        SRP_TYPE_SHA    = 1,
        SRP_TYPE_SHA256 = 2,
        SRP_TYPE_SHA384 = 3,
        SRP_TYPE_SHA512 = 4
} SrpType;


/**
 * SRP hash struct.
 */
typedef struct {
    byte type;
    union {
        #ifndef NO_SHA
            wc_Sha sha;
        #endif
        #ifndef NO_SHA256
            wc_Sha256 sha256;
        #endif
        #ifdef WOLFSSL_SHA384
            wc_Sha384 sha384;
        #endif
        #ifdef WOLFSSL_SHA512
            wc_Sha512 sha512;
        #endif
    } data;
} SrpHash;

typedef struct Srp {
    SrpSide side;                   /**< Client or Server, @see SrpSide.      */
    SrpType type;                   /**< Hash type, @see SrpType.             */
    byte*   user;                   /**< Username, login.                     */
    word32  userSz;                 /**< Username length.                     */
    byte*   salt;                   /**< Small salt.                          */
    word32  saltSz;                 /**< Salt length.                         */
    mp_int  N;                      /**< Modulus. N = 2q+1, [q, N] are primes.*/
    mp_int  g;                      /**< Generator. A generator modulo N.     */
    byte    k[SRP_MAX_DIGEST_SIZE]; /**< Multiplier parameter. k = H(N, g)   */
    mp_int  auth;                   /**< Client: x = H(salt + H(user:pswd))   */
                                    /**< Server: v = g ^ x % N                */
    mp_int  priv;                   /**< Private ephemeral value.             */
    SrpHash client_proof;           /**< Client proof. Sent to the Server.    */
    SrpHash server_proof;           /**< Server proof. Sent to the Client.    */
    byte*   key;                    /**< Session key.                         */
    word32  keySz;                  /**< Session key length.                  */
    int (*keyGenFunc_cb) (struct Srp* srp, byte* secret, word32 size);
        /**< Function responsible for generating the session key.             */
        /**< It MUST use XMALLOC with type DYNAMIC_TYPE_SRP to allocate the   */
        /**< key buffer for this structure and set keySz to the buffer size.  */
        /**< The default function used by this implementation is a modified   */
        /**< version of t_mgf1 that uses the proper hash function according   */
        /**< to srp->type.                                                    */
    void*   heap;                   /**< heap hint pointer                    */
} Srp;

/**
 * Initializes the Srp struct for usage.
 *
 * @param[out] srp   the Srp structure to be initialized.
 * @param[in]  type  the hash type to be used.
 * @param[in]  side  the side of the communication.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpInit(Srp* srp, SrpType type, SrpSide side);
WOLFSSL_API int wc_SrpInit_ex(Srp* srp, SrpType type, SrpSide side,
    void* heap, int devId);

/**
 * Releases the Srp struct resources after usage.
 *
 * @param[in,out] srp    the Srp structure to be terminated.
 */
WOLFSSL_API void wc_SrpTerm(Srp* srp);

/**
 * Sets the username.
 *
 * This function MUST be called after wc_SrpInit.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[in]     username  the buffer containing the username.
 * @param[in]     size      the username size in bytes
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpSetUsername(Srp* srp, const byte* username, word32 size);


/**
 * Sets the srp parameters based on the username.
 *
 * This function MUST be called after wc_SrpSetUsername.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[in]     N         the Modulus. N = 2q+1, [q, N] are primes.
 * @param[in]     nSz       the N size in bytes.
 * @param[in]     g         the Generator modulo N.
 * @param[in]     gSz       the g size in bytes
 * @param[in]     salt      a small random salt. Specific for each username.
 * @param[in]     saltSz    the salt size in bytes
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpSetParams(Srp* srp, const byte* N,    word32 nSz,
                                          const byte* g,    word32 gSz,
                                          const byte* salt, word32 saltSz);

/**
 * Sets the password.
 *
 * Setting the password does not persists the clear password data in the
 * srp structure. The client calculates x = H(salt + H(user:pswd)) and stores
 * it in the auth field.
 *
 * This function MUST be called after wc_SrpSetParams and is CLIENT SIDE ONLY.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[in]     password  the buffer containing the password.
 * @param[in]     size      the password size in bytes.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpSetPassword(Srp* srp, const byte* password, word32 size);

/**
 * Sets the verifier.
 *
 * This function MUST be called after wc_SrpSetParams and is SERVER SIDE ONLY.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[in]     verifier  the buffer containing the verifier.
 * @param[in]     size      the verifier size in bytes.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpSetVerifier(Srp* srp, const byte* verifier, word32 size);

/**
 * Gets the verifier.
 *
 * The client calculates the verifier with v = g ^ x % N.
 * This function MAY be called after wc_SrpSetPassword and is CLIENT SIDE ONLY.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[out]    verifier  the buffer to write the verifier.
 * @param[in,out] size      the buffer size in bytes. Will be updated with the
 *                          verifier size.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpGetVerifier(Srp* srp, byte* verifier, word32* size);

/**
 * Sets the private ephemeral value.
 *
 * The private ephemeral value is known as:
 *   a at the client side. a = random()
 *   b at the server side. b = random()
 * This function is handy for unit test cases or if the developer wants to use
 * an external random source to set the ephemeral value.
 * This function MAY be called before wc_SrpGetPublic.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[in]     priv      the ephemeral value.
 * @param[in]     size      the private size in bytes.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpSetPrivate(Srp* srp, const byte* priv, word32 size);

/**
 * Gets the public ephemeral value.
 *
 * The public ephemeral value is known as:
 *   A at the client side. A = g ^ a % N
 *   B at the server side. B = (k * v + (g ^ b % N)) % N
 * This function MUST be called after wc_SrpSetPassword or wc_SrpSetVerifier.
 *
 * @param[in,out] srp       the Srp structure.
 * @param[out]    pub       the buffer to write the public ephemeral value.
 * @param[in,out] size      the the buffer size in bytes. Will be updated with
 *                          the ephemeral value size.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpGetPublic(Srp* srp, byte* pub, word32* size);


/**
 * Computes the session key.
 *
 * The key can be accessed at srp->key after success.
 *
 * @param[in,out] srp               the Srp structure.
 * @param[in]     clientPubKey      the client's public ephemeral value.
 * @param[in]     clientPubKeySz    the client's public ephemeral value size.
 * @param[in]     serverPubKey      the server's public ephemeral value.
 * @param[in]     serverPubKeySz    the server's public ephemeral value size.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpComputeKey(Srp* srp,
                                 byte* clientPubKey, word32 clientPubKeySz,
                                 byte* serverPubKey, word32 serverPubKeySz);

/**
 * Gets the proof.
 *
 * This function MUST be called after wc_SrpComputeKey.
 *
 * @param[in,out] srp   the Srp structure.
 * @param[out]    proof the buffer to write the proof.
 * @param[in,out] size  the buffer size in bytes. Will be updated with the
 *                          proof size.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpGetProof(Srp* srp, byte* proof, word32* size);

/**
 * Verifies the peers proof.
 *
 * This function MUST be called before wc_SrpGetSessionKey.
 *
 * @param[in,out] srp   the Srp structure.
 * @param[in]     proof the peers proof.
 * @param[in]     size  the proof size in bytes.
 *
 * @return 0 on success, {@literal <} 0 on error. @see error-crypt.h
 */
WOLFSSL_API int wc_SrpVerifyPeersProof(Srp* srp, byte* proof, word32 size);

#ifdef __cplusplus
   } /* extern "C" */
#endif

#endif /* WOLFCRYPT_SRP_H */
#endif /* WOLFCRYPT_HAVE_SRP */
