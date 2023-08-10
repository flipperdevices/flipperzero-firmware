/* quic.h
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



/* wolfSSL QUIC API */

#ifndef WOLFSSL_QUIC_H
#define WOLFSSL_QUIC_H

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_QUIC

/* QUIC operates on three encryption levels which determine
 * which keys/algos are used for de-/encryption. These are
 * kept separately for incoming and outgoing data and.
 * Due to the nature of UDP, more than one might be in use
 * at the same time due to resends or out-of-order arrivals.
 */
typedef enum wolfssl_encryption_level_t {
    wolfssl_encryption_initial = 0,
    wolfssl_encryption_early_data,
    wolfssl_encryption_handshake,
    wolfssl_encryption_application
} WOLFSSL_ENCRYPTION_LEVEL;


/* All QUIC related callbacks to the application.
 */
typedef struct wolfssl_quic_method_t WOLFSSL_QUIC_METHOD;

struct wolfssl_quic_method_t {
    /**
     * Provide secrets to the QUIC stack when they become available in the SSL
     * instance during handshake processing. read/write secrets have the same
     * length. A call may only provide one, passing NULL as the other.
     */
    int (*set_encryption_secrets)(WOLFSSL* ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                  const uint8_t* read_secret,
                                  const uint8_t* write_secret,
                                  size_t secret_len);
    /**
     * Provide handshake packets to the QUIC stack to send to the peer. The
     * QUIC stack will wrap these and take care of re-transmissions.
     */
    int (*add_handshake_data)(WOLFSSL* ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                              const uint8_t* data, size_t len);
    /**
     * Flush any buffered packets during handshake.
     */
    int (*flush_flight)(WOLFSSL* ssl);
    /**
     * Send a TLS alert that happened during handshake. In QUIC, such alerts
     * lead to connection shutdown.
     */
    int (*send_alert)(WOLFSSL* ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                      uint8_t alert);
};


/**
 * Mark the given SSL context for QUIC protocol handling. Meaning all
 * SSL instances derived from it will inherit this. Provides all callbacks
 * to the QUIC application the SSL stack needs.
 */
WOLFSSL_API
int wolfSSL_CTX_set_quic_method(WOLFSSL_CTX* ctx,
                                const WOLFSSL_QUIC_METHOD* quic_method);
/**
 * Mark exactly this SSL instance for QUIC protocol handling.
 * Provides all callbacks to the QUIC application the SSL stack needs.
 */
WOLFSSL_API
int wolfSSL_set_quic_method(WOLFSSL* ssl,
                            const WOLFSSL_QUIC_METHOD* quic_method);

/**
 * Check if QUIC handling has been installed on the given SSL instance.
 */
WOLFSSL_API int wolfSSL_is_quic(WOLFSSL* ssl);

/**
 * Return the current encryption level of the SSL instance for READs.
 */
WOLFSSL_API
WOLFSSL_ENCRYPTION_LEVEL  wolfSSL_quic_read_level(const WOLFSSL* ssl);

/**
 * Return the current encryption level of the SSL instance for WRITEs.
 */
WOLFSSL_API
WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_write_level(const WOLFSSL* ssl);


/**
 * Configure the QUIC transport version to use. On `use_legacy` != 0,
 * selects TLSX_KEY_QUIC_TP_PARAMS_DRAFT, otherwise TLSX_KEY_QUIC_TP_PARAMS.
 * This method is part of the BoringSSL API and replicated here for app
 * portability (as in quictls/openssl).
 */
WOLFSSL_API
void wolfSSL_set_quic_use_legacy_codepoint(WOLFSSL* ssl, int use_legacy);

/**
 * Set the TLS extension for the transport parameter version to announce
 * to the peer. Known values are TLSX_KEY_QUIC_TP_PARAMS (V1) and
 * TLSX_KEY_QUIC_TP_PARAMS_DRAFT.
 * Setting it to 0 will announce both V1 and draft versions to a server.
 * Servers will, on 0, select the latest version seen from the client.
 * Default is 0.
 */
WOLFSSL_API
void wolfSSL_set_quic_transport_version(WOLFSSL* ssl, int version);

/**
 * Get the configured transport version.
 */
WOLFSSL_API int wolfSSL_get_quic_transport_version(const WOLFSSL* ssl);

/**
 * Set the raw QUIC transport parameter that will be sent in the TLS extension
 * to the peer, using the configured transport version(s).
 */
WOLFSSL_API int wolfSSL_set_quic_transport_params(WOLFSSL* ssl,
                                                  const uint8_t* params,
                                                  size_t params_len);
/**
 * Get the raw QUIC transport parameter as retrieved via TLS Extension
 * from the peer. If the peer announced several versions,
 * return the latest one.
 * If the extension has not arrived yet, initializes out parameter to
 * NULL, resp. 0.
 */
WOLFSSL_API
void wolfSSL_get_peer_quic_transport_params(const WOLFSSL* ssl,
                                            const uint8_t* *out_params,
                                            size_t* out_params_len);

/**
 * Get the QUIC version negotiated with the peer during the handshake.
 */
WOLFSSL_API int wolfSSL_get_peer_quic_transport_version(const WOLFSSL* ssl);

#ifdef WOLFSSL_EARLY_DATA
WOLFSSL_API void wolfSSL_set_quic_early_data_enabled(WOLFSSL* ssl, int enabled);
#endif

/**
 * Advisory amount of the maximum data a QUIC protocol handler should have
 * in flight. This varies during handshake processing, for example certificate
 * exchange will increase the limit.
 */
WOLFSSL_API
size_t wolfSSL_quic_max_handshake_flight_len(const WOLFSSL* ssl,
                                             WOLFSSL_ENCRYPTION_LEVEL level);


/**
 * The QUIC protocol handler provides peer TLS records to the SSL instance
 * during handshake to progress it. The SSL instance will use the registered
 * callbacks to send packets to the peer.
 * Encryption level is provided to indicate how to decrypt the data. Data may
 * be added for levels not yet reached by the SSL instance. However, data
 * may only be added in ever increasing levels and levels may only increase
 * at TLS record boundaries. Any violation will make this function fail.
 */
WOLFSSL_API
int wolfSSL_provide_quic_data(WOLFSSL* ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                              const uint8_t* data, size_t len);

WOLFSSL_API
int wolfSSL_quic_do_handshake(WOLFSSL* ssl);

/**
 * Process any CRYPTO data added post-handshake.
 */
WOLFSSL_API int wolfSSL_process_quic_post_handshake(WOLFSSL* ssl);

/**
 * Process any pending input and flush all output. Can be invoked
 * during and/or after handshake processing.
 */
WOLFSSL_API int wolfSSL_quic_read_write(WOLFSSL* ssl);

/**
 * Get the AEAD cipher that is currently selected in the SSL instance.
 * Will return NULL if none has been selected so far. This is used by the
 * QUIC stack to encrypt/decrypt packets after the handshake.
 */
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_quic_get_aead(WOLFSSL* ssl);

/**
 * Use to classify the AEAD cipher for key reuse limits.
 */
WOLFSSL_API int wolfSSL_quic_aead_is_gcm(const WOLFSSL_EVP_CIPHER* aead_cipher);
WOLFSSL_API int wolfSSL_quic_aead_is_ccm(const WOLFSSL_EVP_CIPHER* aead_cipher);
WOLFSSL_API
int wolfSSL_quic_aead_is_chacha20(const WOLFSSL_EVP_CIPHER* aead_cipher);

/**
 * Get the 'tag' length used by the AEAD cipher. Encryption buffer lengths
 * are plaintext length plus this tag length.
 */
WOLFSSL_API
size_t wolfSSL_quic_get_aead_tag_len(const WOLFSSL_EVP_CIPHER* aead_cipher);

/**
 * The message digest currently selected in the SSL instance.
 */
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_quic_get_md(WOLFSSL* ssl);

/**
 * The QUIC header protection cipher matching the AEAD cipher currently
 * selected in the SSL instance.
 */
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_quic_get_hp(WOLFSSL* ssl);

/**
 * Create and initialize a cipher context for use in en- or decryption.
 */
WOLFSSL_API WOLFSSL_EVP_CIPHER_CTX*
wolfSSL_quic_crypt_new(const WOLFSSL_EVP_CIPHER* cipher,
                       const uint8_t* key, const uint8_t* iv, int encrypt);

/**
 * Use a previously created cipher context to encrypt the given plain text.
 */
WOLFSSL_API
int wolfSSL_quic_aead_encrypt(uint8_t* dest, WOLFSSL_EVP_CIPHER_CTX* aead_ctx,
                              const uint8_t* plain, size_t plainlen,
                              const uint8_t* iv, const uint8_t* aad,
                              size_t aadlen);
/**
 * Use a previously created cipher context to decrypt the given encoded text.
 */
WOLFSSL_API
int wolfSSL_quic_aead_decrypt(uint8_t* dest, WOLFSSL_EVP_CIPHER_CTX* ctx,
                              const uint8_t* enc, size_t enclen,
                              const uint8_t* iv, const uint8_t* aad,
                              size_t aadlen);

/**
 * Extract a pseudo-random key, using the given message digest, a secret
 * and a salt. The key size is the size of the digest.
 */
WOLFSSL_API
int wolfSSL_quic_hkdf_extract(uint8_t* dest, const WOLFSSL_EVP_MD* md,
                              const uint8_t* secret, size_t secretlen,
                              const uint8_t* salt, size_t saltlen);
/**
 * Expand a pseudo-random key (secret) into a new key, using the mesasge
 * digest and the info bytes.
 */
WOLFSSL_API
int wolfSSL_quic_hkdf_expand(uint8_t* dest, size_t destlen,
                             const WOLFSSL_EVP_MD* md,
                             const uint8_t* secret, size_t secretlen,
                             const uint8_t* info, size_t infolen);

/**
 * Extract and extpand secret, salt and info into a new key.
 */
WOLFSSL_API
int wolfSSL_quic_hkdf(uint8_t* dest, size_t destlen,
                      const WOLFSSL_EVP_MD* md,
                      const uint8_t* secret, size_t secretlen,
                      const uint8_t* salt, size_t saltlen,
                      const uint8_t* info, size_t infolen);

#endif /* WOLFSSL_QUIC */

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_QUIC_H */
