/*!
     \ingroup QUIC

     \brief ハンドシェイク中にシークレットが生成されたときに呼び出されるコールバック。
     QUIC プロトコル ハンドラはパケットの暗号化/復号化を実行するため、
     レベル Early_data/handshake/application のネゴシエートされたシークレットが必要です。

     コールバックは、ハンドシェイク中に数回呼び出されます。 両方のどちらか
     または、読み取りシークレットまたは書き込みシークレットのみが提供される場合があります。 これは、
     指定された暗号化レベルはすでに有効になっています。

     \return 成功すると 1 を返し、失敗すると 0 を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param level - シークレットの暗号化レベル
     \param read_secret - 特定のレベルで復号化に使用されるシークレット。NULL の場合があります。
     \param write_secret - 特定のレベルで暗号化に使用されるシークレット。NULL の場合があります。
     \param secret_len - シークレットの長さ

     \sa wolfSSL_set_quic_method
*/
int (*set_encryption_secrets)(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL レベル,
                               const uint8_t *read_secret,
                               const uint8_t *write_secret, size_t secret_len);

/*!
     \ingroup QUIC

     \brief ハンドシェイク CRYPTO データをピアに転送するために呼び出されるコールバック。
     この方法で転送されるデータは暗号化されません。 QUICの仕事です
     これを行うためのプロトコル実装。 どのシークレットを使用するか
     指定された暗号化レベルによって決まります。

     このコールバックは、ハンドシェイク中またはポスト ハンドシェイク中に数回呼び出される場合があります。
     処理。 データは完全な CRYPTO レコードをカバーする場合がありますが、
     部分的であること。 ただし、コールバックは以前にすべてのレコード データを受信しています。
     別の暗号化レベルを使用しています。

     \return 成功すると 1 を返し、失敗すると 0 を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param level - データの暗号化に使用する暗号化レベル
     \param data - データ自体
     \param len - データの長さ

     \sa wolfSSL_set_quic_method
*/
int (*add_handshake_data)(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL レベル,
                           const uint8_t *data, size_t len);

/*!
     \ingroup QUIC

     \brief 送信するデータのアドバイザリ フラッシュのために呼び出されるコールバック。

     \return 成功すると 1 を返し、失敗すると 0 を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_set_quic_method
*/
int (*flush_flight)(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief 処理中に SSL アラートが発生したときに呼び出されるコールバック。

     \return 成功すると 1 を返し、失敗すると 0 を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param level - アラートが発生したときに有効だった暗号化レベル
     \param alert - エラー

     \sa wolfSSL_set_quic_method
*/
int (*send_alert)(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL レベル, uint8_t アラート);

/*!
     \ingroup QUIC

     \brief WOLFSSL_CTX および派生したすべての WOLFSSL インスタンスに対して QUIC プロトコルを有効にします
     必要な 4 つのコールバックを提供します。 CTX は TLSv1.3 である必要があります。

     渡された quic_method には、SSL インスタンスよりも長い寿命が必要です。
     コピーされません。 すべてのコールバックを提供する必要があります。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ctx - wolfSSL_CTX_new() を使用して作成された WOLFSSL_CTX 構造体へのポインター。
     \param quic_method - コールバック構造

     \sa wolfSSL_is_quic
     \sa wolfSSL_set_quic_method
*/
int wolfSSL_CTX_set_quic_method(WOLFSSL_CTX *ctx, const WOLFSSL_QUIC_METHOD *quic_method);

/*!
     \ingroup QUIC

     \brief を提供して、WOLFSSL インスタンスの QUIC プロトコルを有効にします。
     4 つのコールバックが必要です。 WOLFSSL は TLSv1.3 である必要があります。

     渡された quic_method には、SSL インスタンスよりも長い寿命が必要です。
     コピーされません。 すべてのコールバックを提供する必要があります。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param quic_method - コールバック構造

     \sa wolfSSL_is_quic
     \sa wolfSSL_CTX_set_quic_method
*/
int wolfSSL_set_quic_method(WOLFSSL *ssl, const WOLFSSL_QUIC_METHOD *quic_method);

/*!
     \ingroup QUIC

     \brief QUIC が WOLFSSL インスタンスでアクティブ化されているかどうかを確認します。

     \return WOLFSSL が QUIC を使用している場合は 1 を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_CTX_quic_method
     \sa wolfSSL_CTX_set_quic_method
*/
int wolfSSL_is_quic(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief 現在使用中の読み取りの暗号化レベルを決定します。 場合にのみ意味があります。
     WOLFSSL インスタンスは QUIC を使用しています。

     有効レベルは、データを返すときは常にパラメーターであることに注意してください。
     前方へ。 ピアからのデータは、これを介して報告される以外のレベルで到着する可能性があります
     関数。

     \return 暗号化レベル。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_quic_write_level
*/
WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_read_level(const WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief 現在使用中の書き込みの暗号化レベルを決定します。 場合にのみ意味があります。
     WOLFSSL インスタンスは QUIC を使用しています。

     有効レベルは、データを返すときは常にパラメーターであることに注意してください。
     前方へ。 ピアからのデータは、これを介して報告される以外のレベルで到着する可能性があります
     関数。

     \return 暗号化レベル。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_quic_read_level
*/
WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_write_level(const WOLFSSL *ssl);


/*!
     \ingroup QUIC

     \brief どの QUIC バージョンを使用するかを設定します。 これを呼ばずに、
     WOLFSSL は両方 (draft-27 と v1) をサーバーに提供します。 受け入れる
     クライアントからの両方と、最新のものをネゴシエートします。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param use_legacy - ドラフト 27 を使用する場合は true、QUICv1 のみを使用する場合は 0。

     \sa wolfSSL_set_quic_transport_version
*/
void wolfSSL_set_quic_use_legacy_codepoint(WOLFSSL *ssl, int use_legacy);

/*!
     \ingroup QUIC

     \brief どの QUIC バージョンを使用するかを設定します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param version - QUIC バージョン用に定義された TLS 拡張。

     \sa wolfSSL_set_quic_use_legacy_codepoint
*/
void wolfSSL_set_quic_transport_version(WOLFSSL *ssl, int バージョン);

/*!
     \ingroup QUIC

     \brief 構成された QUIC バージョンを取得します。

     \return 構成されたバージョンの TLS 拡張。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_set_quic_use_legacy_codepoint
     \sa wolfSSL_set_quic_transport_version
*/
int wolfSSL_get_quic_transport_version(const WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief 使用する QUIC トランスポート パラメータを設定します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param params - 使用するパラメータ バイト
     ·param params_len - パラメータの長さ

     \sa wolfSSL_set_quic_use_legacy_codepoint
     \sa wolfSSL_set_quic_transport_version
*/
int wolfSSL_set_quic_transport_params(WOLFSSL *ssl, const uint8_t *params, size_t params_len);

/*!
     \ingroup QUIC

     \brief ネゴシエートされた QUIC トランスポート バージョンを取得します。 これは与えるだけです
     それぞれの TLS 拡張機能が有効になった後に呼び出されると、意味のある結果が得られます。
     ピアから見られました。

     \return ネゴシエートされたバージョンまたは -1 を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_set_quic_use_legacy_codepoint
     \sa wolfSSL_set_quic_transport_version
*/
int wolfSSL_get_peer_quic_transport_version(const WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief ネゴシエートされた QUIC トランスポート パラメータを取得します。 これは与えるだけです
     それぞれの TLS 拡張機能が有効になった後に呼び出されると、意味のある結果が得られます。
     ピアから見られました。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param out_params - ピアに送信されるパラメーター。利用できない場合は NULL に設定されます。
     \param out_params_len - ピアに送信されるパラメータの長さ。利用できない場合は 0 に設定

     \sa wolfSSL_get_peer_quic_transport_version
*/
void wolfSSL_get_peer_quic_transport_params(const WOLFSSL *ssl, const uint8_t **out_params, size_t *out_params_len);


/*!
     \ingroup QUIC

     \brief 初期データを有効にするかどうかを構成します。 サーバーがシグナルを送ることを目的としています
     これをクライアントに。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param enabled - != 初期データが有効な場合は 0

*/
void wolfSSL_set_quic_early_data_enabled(WOLFSSL *ssl, int enabled);

/*!
     \ingroup QUIC

     \brief 「飛行中」のデータ量についてアドバイスを得る。 未承認
     指定された暗号化レベルで。 これはWOLFSSLインスタンスのデータ量です
     バッファする準備ができています。

     \return 飛行中の推奨最大データを返す

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param level - 問い合わせる暗号化レベル

*/
size_t wolfSSL_quic_max_handshake_flight_len(const WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL レベル);


/*!
     \ingroup QUIC

     \brief 復号化された CRYPTO データを、さらに処理するために WOLFSSL インスタンスに渡します。
     通話間の暗号化レベルは、すべて増加することが許可されており、
     また、暗号化の変更前にデータレコードが完全であることを確認しました
     レベルが受け入れられます。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。
     \param level - データが暗号化されたレベル
     \param data - データ自体
     \param len - データの長さ

     \sa wolfSSL_process_quic_post_handshake
*/
int wolfSSL_provide_quic_data(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL レベル, const uint8_t *data, size_t len);

/*!
     \ingroup QUIC

     \brief ハンドシェイク後に提供されたすべての CRYPTO レコードを処理します
     完了しました。 それより前に呼び出すと失敗します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_provide_quic_data
     \sa wolfSSL_quic_read_write
     \sa wolfSSL_accept
     \sa wolfSSL_connect
*/
WOLFSSL_API int wolfSSL_process_quic_post_handshake(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief ハンドシェイク中またはハンドシェイク後に提供されたすべての CRYPTO レコードを処理します。
     ハンドシェイクがまだ完了していない場合は進行し、そうでない場合は次のように機能します
     wolfSSL_process_quic_post_handshake()。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_provide_quic_data
     \sa wolfSSL_quic_read_write
     \sa wolfSSL_accept
     \sa wolfSSL_connect
*/
int wolfSSL_quic_read_write(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief TLS ハンドシェイクでネゴシエートされた AEAD 暗号を取得します。

     \return ネゴシエートされた暗号、または決定されない場合は NULL を返します。

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_quic_aad_is_gcm
     \sa wolfSSL_quic_aad_is_ccm
     \sa wolfSSL_quic_aad_is_chacha20
     \sa wolfSSL_quic_get_aad_tag_len
     \sa wolfSSL_quic_get_md
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_crypt_new
     \sa wolfSSL_quic_aad_encrypt
     \sa wolfSSL_quic_aad_decrypt
*/
const WOLFSSL_EVP_CIPHER *wolfSSL_quic_get_aad(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief AEAD 暗号が GCM かどうかを確認します。

     \return != 0 (AEAD 暗号が GCM の場合)。

     \param cipher - 暗号

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_aad_is_ccm
     \sa wolfSSL_quic_aad_is_chacha20
     \sa wolfSSL_quic_get_aad_tag_len
     \sa wolfSSL_quic_get_md
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_crypt_new
     \sa wolfSSL_quic_aad_encrypt
     \sa wolfSSL_quic_aad_decrypt
*/
int wolfSSL_quic_aead_is_gcm(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
     \ingroup QUIC

     \brief AEAD 暗号が CCM かどうかを確認します。

     \return != 0 AEAD 暗号が CCM の場合。

     \param cipher - 暗号

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_aad_is_gcm
     \sa wolfSSL_quic_aad_is_chacha20
     \sa wolfSSL_quic_get_aad_tag_len
     \sa wolfSSL_quic_get_md
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_crypt_new
     \sa wolfSSL_quic_aad_encrypt
     \sa wolfSSL_quic_aad_decrypt
*/
int wolfSSL_quic_aead_is_ccm(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
     \ingroup QUIC

     \brief AEAD 暗号が CHACHA20 かどうかを確認します。

     \return != 0 は、AEAD 暗号が CHACHA20 の場合です。

     \param cipher - 暗号

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_aad_is_ccm
     \sa wolfSSL_quic_aad_is_gcm
     \sa wolfSSL_quic_get_aad_tag_len
     \sa wolfSSL_quic_get_md
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_crypt_new
     \sa wolfSSL_quic_aad_encrypt
     \sa wolfSSL_quic_aad_decrypt
*/
int wolfSSL_quic_aead_is_chacha20(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
     \ingroup QUIC

     \brief AEAD 暗号のタグの長さを決定します。

     \return AEAD 暗号のタグ長。

     \param cipher - 暗号

     \sa wolfSSL_quic_get_aad
*/
WOLFSSL_API size_t wolfSSL_quic_get_aead_tag_len(const WOLFSSL_EVP_CIPHER *aead_cipher);

/*!
     \ingroup QUIC

     \brief TLS ハンドシェイクでネゴシエートされたメッセージ ダイジェストを決定します。

     \return TLS ハンドシェイクでネゴシエートされたメッセージ ダイジェストを返す

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_get_hp
*/
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_quic_get_md(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief TLS ハンドシェイクでネゴシエートされたヘッダー保護暗号を決定します。

     \return TLS ハンドシェイクでネゴシエートされたヘッダー保護暗号を返します

     \param ssl - wolfSSL_new() を使用して作成された WOLFSSL 構造体へのポインタ。

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_get_md
*/
const WOLFSSL_EVP_CIPHER *wolfSSL_quic_get_hp(WOLFSSL *ssl);

/*!
     \ingroup QUIC

     \brief 暗号化/復号化のための暗号コンテキストを作成します。

     \return エラーの場合は、作成されたコンテキストまたは NULL を返します。

     \param cipher - コンテキストで使用する暗号。
     \param key - コンテキストで使用するキー。
     \param iv - コンテキストで使用する iv。
     \param encrypt - 暗号化の場合は != 0、それ以外の場合は復号化

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_aad_encrypt
     \sa wolfSSL_quic_aad_decrypt
*/
WOLFSSL_EVP_CIPHER_CTX *wolfSSL_quic_crypt_new(const WOLFSSL_EVP_CIPHER *cipher,
                                                const uint8_t *key, const uint8_t *iv, int encrypt);

/*!
     \ingroup QUIC

     \brief 指定されたコンテキストでプレーン テキストを暗号化します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param dest - 暗号化されたデータの書き込み先
     \param aead_ctx - 使用する暗号コンテキスト
     \param plain - 暗号化するプレーン データ
     \param plainlen - プレーン データの長さ
     \param iv - 使用する iv
     \param aad - 使用する追加
     \param aadlen - aad の長さ

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_crypt_new
     \sa wolfSSL_quic_aad_decrypt
*/
int wolfSSL_quic_aead_encrypt(uint8_t *dest, WOLFSSL_EVP_CIPHER_CTX *aead_ctx,
                               const uint8_t *plain, size_t plainlen,
                               const uint8_t *iv, const uint8_t *aad, size_t aadlen);

/*!
     \ingroup QUIC

     \brief 指定されたコンテキストで暗号文を復号化します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param dest - プレーンテキストの書き込み先
     \param ctx - 使用する暗号コンテキスト
     \param enc - 復号化する暗号化データ
     \param envlen - 暗号化されたデータの長さ
     \param iv - 使用する iv
     \param aad - 使用する追加
     \param aadlen - aad の長さ

     \sa wolfSSL_quic_get_aad
     \sa wolfSSL_quic_get_hp
     \sa wolfSSL_quic_crypt_new
     \sa wolfSSL_quic_aad_encrypt
*/
int wolfSSL_quic_aad_decrypt(uint8_t *dest, WOLFSSL_EVP_CIPHER_CTX *ctx,
                               const uint8_t *enc, size_t enclen,
                               const uint8_t *iv, const uint8_t *aad, size_t aadlen);

/*!
     \ingroup QUIC

     \brief 擬似乱数キーを抽出します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param dest - キーの書き込み先
     \param md - 使用するメッセージ ダイジェスト
     \param secret - 使用するシークレット
     \param secretlen - シークレットの長さ
     \param salt - 使用するソルト
     \param saltlen - ソルトの長さ

     \sa wolfSSL_quic_hkdf_expand
     \sa wolfSSL_quic_hkdf
*/
int wolfSSL_quic_hkdf_extract(uint8_t *dest, const WOLFSSL_EVP_MD *md,
                               const uint8_t *secret, size_t secretlen,
                               const uint8_t *salt, size_t saltlen);

/*!
     \ingroup QUIC

     \brief 疑似ランダム キーを新しいキーに展開します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param dest - キーの書き込み先
     \param destlen - 展開するキーの長さ
     \param md - 使用するメッセージ ダイジェスト
     \param secret - 使用するシークレット
     \param secretlen - シークレットの長さ
     \param info - 使用する情報
     \param infolen - 情報の長さ

     \sa wolfSSL_quic_hkdf_extract
     \sa wolfSSL_quic_hkdf
*/
int wolfSSL_quic_hkdf_expand(uint8_t *dest, size_t destlen,
                              const WOLFSSL_EVP_MD *md,
                              const uint8_t *secret, size_t secretlen,
                              const uint8_t *info, size_t infolen);

/*!
     \ingroup QUIC

     \brief 疑似乱数キーを展開して抽出します。

     \return WOLFSSL_SUCCESS 成功した場合。

     \param dest - キーの書き込み先
     \param destlen - キーの長さ
     \param md - 使用するメッセージ ダイジェスト
     \param secret - 使用するシークレット
     \param secretlen - シークレットの長さ
     \param salt - 使用するソルト
     \param saltlen - ソルトの長さ
     \param info - 使用する情報
     \param infolen - 情報の長さ

     \sa wolfSSL_quic_hkdf_extract
     \sa wolfSSL_quic_hkdf_expand
*/
int wolfSSL_quic_hkdf(uint8_t *dest, size_t destlen,
                       const WOLFSSL_EVP_MD *md,
                       const uint8_t *secret, size_t secretlen,
                       const uint8_t *salt, size_t saltlen,
                       const uint8_t *info, size_t infolen);
