/*!
    \ingroup QUIC

    \brief Callback invoked when secrets are generated during a handshake.
    Since QUIC protocol handlers perform the en-/decryption of packets, they
    need the negotiated secrets for the levels early_data/handshake/application.

    The callback will be invoked several times during a handshake. Either both
    or only the read or write secret might be provided. This does not mean the
    given encryption level is already in effect.

    \return 1 on success, 0 on failure.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param level - the encryption level the secrets are for
    \param read_secret - the secret used in decryption at the given level, may be NULL.
    \param write_secret - the secret used in encryption at the given level, may be NULL.
    \param secret_len - the length of the secret

    \sa wolfSSL_set_quic_method
*/
int (*set_encryption_secrets)(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                              const uint8_t *read_secret,
                              const uint8_t *write_secret, size_t secret_len);

/*!
    \ingroup QUIC

    \brief Callback invoked for forwarding handshake CRYPTO data to peer.
    The data forwarded this way is not encrypted. It is the job of the QUIC
    protocol implementation to do this. Which secrets are to be used
    is determined by the encryption level specified.

    This callback may be invoked several times during handshake or post handshake
    processing. The data may cover a complete CRYPTO record, but may also
    be partial. However, the callback will have received all records data before
    using another encryption level.

    \return 1 on success, 0 on failure.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param level - the encryption level to use for encrypting the data
    \param data - the data itself
    \param len - the length of the data

    \sa wolfSSL_set_quic_method
*/
int (*add_handshake_data)(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                          const uint8_t *data, size_t len);

/*!
    \ingroup QUIC

    \brief Callback invoked for advisory flushing of the data to send.

    \return 1 on success, 0 on failure.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_set_quic_method
*/
int (*flush_flight)(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Callback invoked when an SSL alert happened during processing.

    \return 1 on success, 0 on failure.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param level - the encryption level in effect when the alert happened
    \param alert - the error

    \sa wolfSSL_set_quic_method
*/
int (*send_alert)(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level, uint8_t alert);

/*!
    \ingroup QUIC

    \brief Activate QUIC protocol for a WOLFSSL_CTX and all derived WOLFSSL instances
    by providing the four callbacks required. The CTX needs to be a TLSv1.3 one.

    The passed quic_method needs to have a lifetime outlasting the SSL instances.
    It is not copied. All callbacks need to be provided.

    \return WOLFSSL_SUCCESS If successful.

    \param ctx - a pointer to a WOLFSSL_CTX structure, created using wolfSSL_CTX_new().
    \param quic_method - the callback structure

    \sa wolfSSL_is_quic
    \sa wolfSSL_set_quic_method
*/
int wolfSSL_CTX_set_quic_method(WOLFSSL_CTX *ctx, const WOLFSSL_QUIC_METHOD *quic_method);

/*!
    \ingroup QUIC

    \brief Activate QUIC protocol for a WOLFSSL instance  by providing the
    four callbacks required. The WOLFSSL needs to be a TLSv1.3 one.

    The passed quic_method needs to have a lifetime outlasting the SSL instance.
    It is not copied. All callbacks need to be provided.

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param quic_method - the callback structure

    \sa wolfSSL_is_quic
    \sa wolfSSL_CTX_set_quic_method
*/
int wolfSSL_set_quic_method(WOLFSSL *ssl, const WOLFSSL_QUIC_METHOD *quic_method);

/*!
    \ingroup QUIC

    \brief Check if QUIC has been activated in a WOLFSSL instance.

    \return 1 if WOLFSSL is using QUIC.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_CTX_quic_method
    \sa wolfSSL_CTX_set_quic_method
*/
int wolfSSL_is_quic(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Determine the encryption level for reads currently in use. Meaningful only when
    the WOLFSSL instance is using QUIC.

    Note that the effective level is always a parameter when passing data back and
    forth. Data from a peer might arrive at other levels than reported via this
    function.

    \return encryption level.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_quic_write_level
*/
WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_read_level(const WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Determine the encryption level for writes currently in use. Meaningful only when
    the WOLFSSL instance is using QUIC.

    Note that the effective level is always a parameter when passing data back and
    forth. Data from a peer might arrive at other levels than reported via this
    function.

    \return encryption level.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_quic_read_level
*/
WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_write_level(const WOLFSSL *ssl);


/*!
    \ingroup QUIC

    \brief Configure which QUIC version shall be used. Without calling this,
    the WOLFSSL will offer both (draft-27 and v1) to a server, resp. accept
    both from a client and negotiate the most recent one.

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param use_legacy - true if draft-27 shall be used, 0 if only QUICv1 is used.

    \sa wolfSSL_set_quic_transport_version
*/
void wolfSSL_set_quic_use_legacy_codepoint(WOLFSSL *ssl, int use_legacy);

/*!
    \ingroup QUIC

    \brief Configure which QUIC version shall be used.

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param version - the TLS Extension defined for the QUIC version.

    \sa wolfSSL_set_quic_use_legacy_codepoint
*/
void wolfSSL_set_quic_transport_version(WOLFSSL *ssl, int version);

/*!
    \ingroup QUIC

    \brief Get the configured QUIC version.

    \return TLS Extension of configured version.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_set_quic_use_legacy_codepoint
    \sa wolfSSL_set_quic_transport_version
*/
int wolfSSL_get_quic_transport_version(const WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Set the QUIC transport parameters to use.

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param params - the parameter bytes to use
    ·param params_len - the length of the parameters

    \sa wolfSSL_set_quic_use_legacy_codepoint
    \sa wolfSSL_set_quic_transport_version
*/
int wolfSSL_set_quic_transport_params(WOLFSSL *ssl, const uint8_t *params, size_t params_len);

/*!
    \ingroup QUIC

    \brief Get the negotiated QUIC transport version. This will only give
    meaningful results when called after the respective TLS extensions have
    been seen from the peer.

    \return the negotiated version or -1.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_set_quic_use_legacy_codepoint
    \sa wolfSSL_set_quic_transport_version
*/
int wolfSSL_get_peer_quic_transport_version(const WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Get the negotiated QUIC transport parameters. This will only give
    meaningful results when called after the respective TLS extensions have
    been seen from the peer.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param out_params - the parameters sent be the peer, set to NULL if not available.
    \param out_params_len - the length of the parameters sent be the peer, set to 0 if not available

    \sa wolfSSL_get_peer_quic_transport_version
*/
void wolfSSL_get_peer_quic_transport_params(const WOLFSSL *ssl, const uint8_t **out_params, size_t *out_params_len);


/*!
    \ingroup QUIC

    \brief Configure if Early Data is enabled. Intended for servers to signal
    this to clients.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param enabled - != 0 iff early data is enabled

*/
void wolfSSL_set_quic_early_data_enabled(WOLFSSL *ssl, int enabled);

/*!
    \ingroup QUIC

    \brief Get advice on the amount of data that shall be "in flight", e.g. unacknowledged
    at the given encryption level. This is the amount of data the WOLFSSL instance
    is prepared to buffer.

    \return the recommend max data in flight

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param level - the encryption level to inquire about

*/
size_t wolfSSL_quic_max_handshake_flight_len(const WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level);


/*!
    \ingroup QUIC

    \brief Pass decrypted CRYPTO data to the WOLFSSL instance for further processing.
    The encryption level between calls is only every allowed to increase and it is
    also checked that data records are complete before a change in encryption
    level is accepted.

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().
    \param level - the level the data was encrypted at
    \param data - the data itself
    \param len - the length of the data

    \sa wolfSSL_process_quic_post_handshake
    \sa wolfSSL_quic_read_write
    \sa wolfSSL_accept
    \sa wolfSSL_connect
*/
int wolfSSL_provide_quic_data(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level, const uint8_t *data, size_t len);

/*!
    \ingroup QUIC

    \brief Process any CRYPTO records that have been provided after the handshake
    has completed. Will fail if called before that.

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_provide_quic_data
    \sa wolfSSL_quic_read_write
    \sa wolfSSL_accept
    \sa wolfSSL_connect
*/
WOLFSSL_API int wolfSSL_process_quic_post_handshake(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Process any CRYPTO records that have been provided during or after the handshake.
    Will progress the handshake if not already complete and otherwise work like
    wolfSSL_process_quic_post_handshake().

    \return WOLFSSL_SUCCESS If successful.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_provide_quic_data
    \sa wolfSSL_quic_read_write
    \sa wolfSSL_accept
    \sa wolfSSL_connect
*/
int wolfSSL_quic_read_write(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Get the AEAD cipher negotiated in the TLS handshake.

    \return negotiated cipher or NULL if not determined.

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_quic_aead_is_gcm
    \sa wolfSSL_quic_aead_is_ccm
    \sa wolfSSL_quic_aead_is_chacha20
    \sa wolfSSL_quic_get_aead_tag_len
    \sa wolfSSL_quic_get_md
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_crypt_new
    \sa wolfSSL_quic_aead_encrypt
    \sa wolfSSL_quic_aead_decrypt
*/
const WOLFSSL_EVP_CIPHER *wolfSSL_quic_get_aead(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Check if the AEAD cipher is GCM.

    \return != 0 iff the AEAD cipher is GCM.

    \param cipher - the cipher

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_aead_is_ccm
    \sa wolfSSL_quic_aead_is_chacha20
    \sa wolfSSL_quic_get_aead_tag_len
    \sa wolfSSL_quic_get_md
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_crypt_new
    \sa wolfSSL_quic_aead_encrypt
    \sa wolfSSL_quic_aead_decrypt
*/
int wolfSSL_quic_aead_is_gcm(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
    \ingroup QUIC

    \brief Check if the AEAD cipher is CCM.

    \return != 0 iff the AEAD cipher is CCM.

    \param cipher - the cipher

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_aead_is_gcm
    \sa wolfSSL_quic_aead_is_chacha20
    \sa wolfSSL_quic_get_aead_tag_len
    \sa wolfSSL_quic_get_md
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_crypt_new
    \sa wolfSSL_quic_aead_encrypt
    \sa wolfSSL_quic_aead_decrypt
*/
int wolfSSL_quic_aead_is_ccm(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
    \ingroup QUIC

    \brief Check if the AEAD cipher is CHACHA20.

    \return != 0 iff the AEAD cipher is CHACHA20.

    \param cipher - the cipher

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_aead_is_ccm
    \sa wolfSSL_quic_aead_is_gcm
    \sa wolfSSL_quic_get_aead_tag_len
    \sa wolfSSL_quic_get_md
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_crypt_new
    \sa wolfSSL_quic_aead_encrypt
    \sa wolfSSL_quic_aead_decrypt
*/
int wolfSSL_quic_aead_is_chacha20(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
    \ingroup QUIC

    \brief Determine the tag length for the AEAD cipher.

    \return tag length of AEAD cipher.

    \param cipher - the cipher

    \sa wolfSSL_quic_get_aead
*/
WOLFSSL_API size_t wolfSSL_quic_get_aead_tag_len(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
    \ingroup QUIC

    \brief Determine the message digest negotiated in the TLS handshake.

    \return the message digest negotiated in the TLS handshake

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_get_hp
*/
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_quic_get_md(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Determine the header protection cipher negotiated in the TLS handshake.

    \return the header protection cipher negotiated in the TLS handshake

    \param ssl - a pointer to a WOLFSSL structure, created using wolfSSL_new().

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_get_md
*/
const WOLFSSL_EVP_CIPHER *wolfSSL_quic_get_hp(WOLFSSL *ssl);

/*!
    \ingroup QUIC

    \brief Create a cipher context for en-/decryption.

    \return the created context or NULL in case of errors.

    \param cipher - the cipher to use in the context.
    \param key - the key to use in the context.
    \param iv - the iv to use in the context.
    \param encrypt - != 0 if for encryption, otherwise decryption

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_aead_encrypt
    \sa wolfSSL_quic_aead_decrypt
*/
WOLFSSL_EVP_CIPHER_CTX *wolfSSL_quic_crypt_new(const WOLFSSL_EVP_CIPHER *cipher,
                                               const uint8_t *key, const uint8_t *iv, int encrypt);

/*!
    \ingroup QUIC

    \brief Encrypt the plain text in the given context.

    \return WOLFSSL_SUCCESS If successful.

    \param dest - destination where encrypted data is to be written
    \param aead_ctx - the cipher context to use
    \param plain - the plain data to encrypt
    \param plainlen - the length of the plain data
    \param iv - the iv to use
    \param aad - the add to use
    \param aadlen - the length of the aad

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_crypt_new
    \sa wolfSSL_quic_aead_decrypt
*/
int wolfSSL_quic_aead_encrypt(uint8_t *dest, WOLFSSL_EVP_CIPHER_CTX *aead_ctx,
                              const uint8_t *plain, size_t plainlen,
                              const uint8_t *iv, const uint8_t *aad, size_t aadlen);

/*!
    \ingroup QUIC

    \brief Decrypt the cipher text in the given context.

    \return WOLFSSL_SUCCESS If successful.

    \param dest - destination where plain text is to be written
    \param ctx - the cipher context to use
    \param enc - the encrypted data to decrypt
    \param envlen - the length of the encrypted data
    \param iv - the iv to use
    \param aad - the add to use
    \param aadlen - the length of the aad

    \sa wolfSSL_quic_get_aead
    \sa wolfSSL_quic_get_hp
    \sa wolfSSL_quic_crypt_new
    \sa wolfSSL_quic_aead_encrypt
*/
int wolfSSL_quic_aead_decrypt(uint8_t *dest, WOLFSSL_EVP_CIPHER_CTX *ctx,
                              const uint8_t *enc, size_t enclen,
                              const uint8_t *iv, const uint8_t *aad, size_t aadlen);

/*!
    \ingroup QUIC

    \brief Extract a pseudo random key.

    \return WOLFSSL_SUCCESS If successful.

    \param dest - destination where key is to be written
    \param md - message digest to use
    \param secret - the secret to use
    \param secretlen - the length of the secret
    \param salt - the salt to use
    \param saltlen - the length of the salt

    \sa wolfSSL_quic_hkdf_expand
    \sa wolfSSL_quic_hkdf
*/
int wolfSSL_quic_hkdf_extract(uint8_t *dest, const WOLFSSL_EVP_MD *md,
                              const uint8_t *secret, size_t secretlen,
                              const uint8_t *salt, size_t saltlen);

/*!
    \ingroup QUIC

    \brief Expand a pseudo random key into a new key.

    \return WOLFSSL_SUCCESS If successful.

    \param dest - destination where key is to be written
    \param destlen - length of the key to expand
    \param md - message digest to use
    \param secret - the secret to use
    \param secretlen - the length of the secret
    \param info - the info to use
    \param infolen - the length of the info

    \sa wolfSSL_quic_hkdf_extract
    \sa wolfSSL_quic_hkdf
*/
int wolfSSL_quic_hkdf_expand(uint8_t *dest, size_t destlen,
                             const WOLFSSL_EVP_MD *md,
                             const uint8_t *secret, size_t secretlen,
                             const uint8_t *info, size_t infolen);

/*!
    \ingroup QUIC

    \brief Expand and Extract a pseudo random key.

    \return WOLFSSL_SUCCESS If successful.

    \param dest - destination where key is to be written
    \param destlen - length of the key
    \param md - message digest to use
    \param secret - the secret to use
    \param secretlen - the length of the secret
    \param salt - the salt to use
    \param saltlen - the length of the salt
    \param info - the info to use
    \param infolen - the length of the info

    \sa wolfSSL_quic_hkdf_extract
    \sa wolfSSL_quic_hkdf_expand
*/
int wolfSSL_quic_hkdf(uint8_t *dest, size_t destlen,
                      const WOLFSSL_EVP_MD *md,
                      const uint8_t *secret, size_t secretlen,
                      const uint8_t *salt, size_t saltlen,
                      const uint8_t *info, size_t infolen);
