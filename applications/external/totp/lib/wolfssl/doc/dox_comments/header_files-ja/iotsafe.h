/*!
    \ingroup IoTSafe 
    \brief  この関数は与えられたコンテキストでのIoTセーフサポートを有効にします。
    \param ctx  IOTセーフサポートを有効にする必要があるWOLFSSL_CTXオブジェクトへのポインタ
    \return 0  成功した
    _Example_
    \code
    WOLFSSL_CTX *ctx;
    ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (!ctx)
        return NULL;
    wolfSSL_CTX_iotsafe_enable(ctx);
    \endcode
    \sa wolfSSL_iotsafe_on
    \sa wolfIoTSafe_SetCSIM_read_cb
    \sa wolfIoTSafe_SetCSIM_write_cb
*/
int wolfSSL_CTX_iotsafe_enable(WOLFSSL_CTX *ctx);


/*!
    \ingroup IoTSafe 
    \brief  この関数は、IOT-SAFE TLSコールバックを特定のSSLセッションに接続します。
    \brief  スロットのIDが1バイトの長さの場合、SSLセッションをIoT-Safeアプレットに接続するように呼び出す必要があります。IOTセーフスロットのIDが2バイト以上の場合、\ REF WOLFSSL_IOTSAFE_ON_EX「WOLFSSL_IOTSAFE_ON_EX（）」を使用する必要があります。
    \param ssl  コールバックが有効になるWolfSSLオブジェクトへのポインタ
    \param privkey_id  ホストの秘密鍵を含むIOTセーフなアプレットスロットのID
    \param ecdh_keypair_slot  ECDH鍵ペアを保存するためのIoT安全アプレットスロットのID
    \param peer_pubkey_slot  ECDH用の他のエンドポイントの公開鍵を保存するためのIOT-SAFEアプレットスロットのID
    \param peer_cert_slot  検証のための他のエンドポイントの公開鍵を保存するためのIOTセーフなアプレットスロットのID
    \return 0  成功すると
    \return NOT_COMPILED_IN  habe_pk_callbacksが無効になっている場合
    _Example_
    \code
    // Define key ids for IoT-Safe
    #define PRIVKEY_ID 0x02
    #define ECDH_KEYPAIR_ID 0x03
    #define PEER_PUBKEY_ID 0x04
    #define PEER_CERT_ID 0x05
    // Create new ssl session
    WOLFSSL *ssl;
    ssl = wolfSSL_new(ctx);
    if (!ssl)
        return NULL;
    // Enable IoT-Safe and associate key slots
    ret = wolfSSL_CTX_iotsafe_enable(ctx);
    if (ret == 0) {
        ret = wolfSSL_iotsafe_on(ssl, PRIVKEY_ID, ECDH_KEYPAIR_ID, PEER_PUBKEY_ID, PEER_CERT_ID);
    }
    \endcode
    \sa wolfSSL_iotsafe_on_ex
    \sa wolfSSL_CTX_iotsafe_enable
*/
int wolfSSL_iotsafe_on(WOLFSSL *ssl, byte privkey_id,
       byte ecdh_keypair_slot, byte peer_pubkey_slot, byte peer_cert_slot);


/*!
    \ingroup IoTSafe 
    \brief  この関数は、IOT-SAFE TLSコールバックを特定のSSLセッションに接続します。これは、IOTセーフスロットのIDを参照で渡すことができ、IDフィールドの長さをパラメータ "id_size"で指定できます。
    \param ssl  コールバックが有効になるWolfSSLオブジェクトへのポインタ
    \param privkey_id  ホストの秘密鍵を含むIoTセーフアプレットスロットのIDへのポインタ
    \param ecdh_keypair_slot  ECDH鍵ペアを保存するIOT-SafeアプレットスロットのIDへのポインタ
    \param peer_pubkey_slot  ECDH用の他のエンドポイントの公開鍵を保存するIOTセーフアプレットスロットのIDへのポインタ
    \param peer_cert_slot  検証のために他のエンドポイントの公開鍵を保存するためのIOT-SAFEアプレットスロットのIDへのポインタ
    \param id_size  各スロットIDのサイズ
    \return 0  成功すると
    \return NOT_COMPILED_IN  habe_pk_callbacksが無効になっている場合
    _Example_
    \code
    // Define key ids for IoT-Safe (16 bit, little endian)
    #define PRIVKEY_ID 0x0201
    #define ECDH_KEYPAIR_ID 0x0301
    #define PEER_PUBKEY_ID 0x0401
    #define PEER_CERT_ID 0x0501
    #define ID_SIZE (sizeof(word16))

    word16 privkey = PRIVKEY_ID,
             ecdh_keypair = ECDH_KEYPAIR_ID,
             peer_pubkey = PEER_PUBKEY_ID,
             peer_cert = PEER_CERT_ID;



    // Create new ssl session
    WOLFSSL *ssl;
    ssl = wolfSSL_new(ctx);
    if (!ssl)
        return NULL;
    // Enable IoT-Safe and associate key slots
    ret = wolfSSL_CTX_iotsafe_enable(ctx);
    if (ret == 0) {
        ret = wolfSSL_CTX_iotsafe_on_ex(ssl, &privkey, &ecdh_keypair, &peer_pubkey, &peer_cert, ID_SIZE);
    }
    \endcode
    \sa wolfSSL_iotsafe_on
    \sa wolfSSL_CTX_iotsafe_enable
*/
int wolfSSL_iotsafe_on_ex(WOLFSSL *ssl, byte *privkey_id,
       byte *ecdh_keypair_slot, byte *peer_pubkey_slot, byte *peer_cert_slot, word16 id_size);


/*!
    \ingroup IoTSafe 
    \brief  AT + CSIMコマンドのリードコールバックを関連付けます。この入力関数は通常、モデムと通信するUARTチャネルの読み取りイベントに関連付けられています。読み取りコールバックが関連付けられているのは、同時にIoT-Safeサポートを使用するすべてのコンテキストのグローバルと変更です。
    _Example_
    \code

    // USART read function, defined elsewhere
    int usart_read(char *buf, int len);

    wolfIoTSafe_SetCSIM_read_cb(usart_read);

    \endcode
    \sa wolfIoTSafe_SetCSIM_write_cb
*/
void wolfIoTSafe_SetCSIM_read_cb(wolfSSL_IOTSafe_CSIM_read_cb rf);

/*!
    \ingroup IoTSafe 
    \brief  AT + CSIMコマンドの書き込みコールバックを関連付けます。この出力関数は通常、モデムと通信するUARTチャネル上のライトイベントに関連付けられています。Write Callbackが関連付けられているのは、同時にIoT-Safeサポートを使用するすべてのコンテキストのグローバルと変更です。
    _Example_
    \code
    // USART write function, defined elsewhere
    int usart_write(const char *buf, int len);
    wolfIoTSafe_SetCSIM_write_cb(usart_write);
    \endcode
    \sa wolfIoTSafe_SetCSIM_read_cb
*/
void wolfIoTSafe_SetCSIM_write_cb(wolfSSL_IOTSafe_CSIM_write_cb wf);



/*!
    \ingroup IoTSafe 
    \brief  IOTセーフ機能getrandomを使用して、指定されたサイズのランダムなバッファを生成します。この関数は、WolfCrypt RNGオブジェクトによって自動的に使用されます。
    \param out  ランダムなバイトシーケンスが格納されているバッファ。
    \param sz  生成するランダムシーケンスのサイズ（バイト単位）
*/
int wolfIoTSafe_GetRandom(unsigned char* out, word32 sz);


/*!
    \ingroup IoTSafe 
    \brief  IOT-Safeアプレット上のファイルに保存されている証明書をインポートし、ローカルにメモリに保存します。1バイトのファイルIDフィールドで動作します。
    \param id  証明書が保存されているIOTセーフ・アプレットのファイルID
    \param output  証明書がインポートされるバッファー
    \param sz  バッファ出力で使用可能な最大サイズ
    \return the  輸入された証明書の長さ
    _Example_
    \code
    #define CRT_CLIENT_FILE_ID 0x03
    unsigned char cert_buffer[2048];
    // Get the certificate into the buffer
    cert_buffer_size = wolfIoTSafe_GetCert(CRT_CLIENT_FILE_ID, cert_buffer, 2048);
    if (cert_buffer_size < 1) {
        printf("Bad cli cert\n");
        return -1;
    }
    printf("Loaded Client certificate from IoT-Safe, size = %lu\n", cert_buffer_size);

    // Use the certificate buffer as identity for the TLS client context
    if (wolfSSL_CTX_use_certificate_buffer(cli_ctx, cert_buffer,
                cert_buffer_size, SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("Cannot load client cert\n");
        return -1;
    }
    printf("Client certificate successfully imported.\n");
    \endcode
*/
int wolfIoTSafe_GetCert(uint8_t id, unsigned char *output, unsigned long sz);


/*!
    \ingroup IoTSafe 
    \brief  IOT-Safeアプレット上のファイルに保存されている証明書をインポートし、ローカルにメモリに保存します。\ ref wolfiotsafe_getcert "wolfiotsafe_getcert"と同等です。ただし、2バイト以上のファイルIDで呼び出すことができます。
    \param id  証明書が保存されているIOT-SAFEアプレットのファイルIDへのポインタ
    \param id_sz  ファイルIDのサイズ：バイト数
    \param output  証明書がインポートされるバッファー
    \param sz  バッファ出力で使用可能な最大サイズ
    \return the  輸入された証明書の長さ
    _Example_
    \code
    #define CRT_CLIENT_FILE_ID 0x0302
    #define ID_SIZE (sizeof(word16))
    unsigned char cert_buffer[2048];
    word16 client_file_id = CRT_CLIENT_FILE_ID;



    // Get the certificate into the buffer
    cert_buffer_size = wolfIoTSafe_GetCert_ex(&client_file_id, ID_SIZE, cert_buffer, 2048);
    if (cert_buffer_size < 1) {
        printf("Bad cli cert\n");
        return -1;
    }
    printf("Loaded Client certificate from IoT-Safe, size = %lu\n", cert_buffer_size);

    // Use the certificate buffer as identity for the TLS client context
    if (wolfSSL_CTX_use_certificate_buffer(cli_ctx, cert_buffer,
                cert_buffer_size, SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("Cannot load client cert\n");
        return -1;
    }
    printf("Client certificate successfully imported.\n");
    \endcode
*/
int wolfIoTSafe_GetCert_ex(uint8_t *id, uint16_t id_sz, unsigned char *output, unsigned long sz);

/*!
    \ingroup IoTSafe 
    \brief  IOTセーフアプレットに格納されているECC 256ビットの公開鍵をECC_Keyオブジェクトにインポートします。
    \param key  IOT-SAFEアプレットからインポートされたキーを含むECC_KEYオブジェクト
    \param id  公開鍵が保存されているIOTセーフアプレットのキーID
    \return 0  成功すると
    \sa wc_iotsafe_ecc_export_public
    \sa wc_iotsafe_ecc_export_private
*/
int wc_iotsafe_ecc_import_public(ecc_key *key, byte key_id);

/*!
    \ingroup IoTSafe 
    \brief  ECC_KEYオブジェクトからIOT-SAFEアプレットへの書き込み可能なパブリックキースロットにECC 256ビット公開鍵をエクスポートします。
    \param key  エクスポートする鍵を含むecc_keyオブジェクト
    \param id  公開鍵が保存されているIOTセーフアプレットのキーID
    \return 0  成功すると
    \sa wc_iotsafe_ecc_import_public_ex
    \sa wc_iotsafe_ecc_export_private
*/
int wc_iotsafe_ecc_export_public(ecc_key *key, byte key_id);


/*!
    \ingroup IoTSafe 
    \brief  ECC_KEYオブジェクトからIOT-SAFEアプレットへの書き込み可能なパブリックキースロットにECC 256ビット公開鍵をエクスポートします。\ ref WC_IOTSAFE_ECC_IMPORT_PUBLIC「WC_IOTSAFE_ECC_IMPORT_PUBLIC」と同等のものは、2バイト以上のキーIDで呼び出すことができる点を除きます。
    \param key  エクスポートする鍵を含むecc_keyオブジェクト
    \param id  公開鍵が保存されるIOTセーフアプレットのキーIDへのポインタ
    \param id_size  キーIDサイズ
    \return 0  成功すると
    \sa wc_iotsafe_ecc_import_public
    \sa wc_iotsafe_ecc_export_private
*/
int wc_iotsafe_ecc_import_public_ex(ecc_key *key, byte *key_id, word16 id_size);

/*!
    \ingroup IoTSafe 
    \brief  ECC 256ビットキーをECC_KEYオブジェクトからIOTセーフアプレットに書き込み可能なプライベートキースロットにエクスポートします。
    \param key  エクスポートする鍵を含むecc_keyオブジェクト
    \param id  秘密鍵が保存されるIOTセーフアプレットのキーID
    \return 0  成功すると
    \sa wc_iotsafe_ecc_export_private_ex
    \sa wc_iotsafe_ecc_import_public
    \sa wc_iotsafe_ecc_export_public
*/
int wc_iotsafe_ecc_export_private(ecc_key *key, byte key_id);

/*!
    \ingroup IoTSafe 
    \brief  ECC 256ビットキーをECC_KEYオブジェクトからIOTセーフアプレットに書き込み可能なプライベートキースロットにエクスポートします。\ ref WC_IOTSAFE_ECC_EXPORT_PRIVATE「WC_IOTSAFE_ECC_EXPORT_PRIVATE」を除き、2バイト以上のキーIDを呼び出すことができる点を除き、
    \param key  エクスポートする鍵を含むecc_keyオブジェクト
    \param id  秘密鍵が保存されるIOTセーフアプレットのキーIDへのポインタ
    \param id_size  キーIDサイズ
    \return 0  成功すると
    \sa wc_iotsafe_ecc_export_private
    \sa wc_iotsafe_ecc_import_public
    \sa wc_iotsafe_ecc_export_public
*/
int wc_iotsafe_ecc_export_private_ex(ecc_key *key, byte *key_id, word16 id_size);

/*!
    \ingroup IoTSafe 
    \brief  事前計算された256ビットハッシュに署名して、IOT-SAFEアプレットに、以前に保存されたプライベートキー、またはプリプロビジョニングされています。
    \param in  サインするメッセージハッシュを含むバッファへのポインタ
    \param inlen  署名するメッセージの長さ
    \param out  生成された署名を保存するためのバッファ
    \param outlen  出力バッファの最大長。バイトを保存します
    \param id  メッセージ署名の生成に成功したときに書き込まれたペイロードに署名するための秘密鍵を含むスロットのIOTセーフアプレットのキーID
    \return 0  成功すると
    \sa wc_iotsafe_ecc_sign_hash_ex
    \sa wc_iotsafe_ecc_verify_hash
    \sa wc_iotsafe_ecc_gen_k
*/
int wc_iotsafe_ecc_sign_hash(byte *in, word32 inlen, byte *out, word32 *outlen, byte key_id);

/*!
    \ingroup IoTSafe 
    \brief  事前計算された256ビットハッシュに署名して、IOT-SAFEアプレットに、以前に保存されたプライベートキー、またはプリプロビジョニングされています。\ ref wc_iotsafe_ecc_sign_hash "wc_iotsafe_ecc_sign_hash"と同等です。ただし、2バイト以上のキーIDで呼び出すことができます。
    \param in  サインするメッセージハッシュを含むバッファへのポインタ
    \param inlen  署名するメッセージの長さ
    \param out  生成された署名を保存するためのバッファ
    \param outlen  出力バッファの最大長。バイトを保存します
    \param id  秘密鍵を含むスロットのIOT-SAFEアプレットのキーIDへのポインタメッセージ署名の生成に成功したときに書き込まれるペイロードに署名する
    \param id_size  キーIDサイズ
    \return 0  成功すると
    \sa wc_iotsafe_ecc_sign_hash
    \sa wc_iotsafe_ecc_verify_hash
    \sa wc_iotsafe_ecc_gen_k
*/
int wc_iotsafe_ecc_sign_hash_ex(byte *in, word32 inlen, byte *out, word32 *outlen, byte *key_id, word16 id_size);

/*!
    \ingroup IoTSafe 
    \brief  予め計算された256ビットハッシュに対するECCシグネチャを、IOT-SAFEアプレット内のプリプロビジョニング、またはプロビジョニングされたプリプロビジョニングを使用します。結果はRESに書き込まれます。1が有効で、0が無効です。注：有効なテストに戻り値を使用しないでください。Resのみを使用してください。
    \return 0  成功すると（署名が無効であっても）
    \return <  故障の場合は0
    \param sig  検証する署名を含むバッファ
    \param hash  署名されたハッシュ（メッセージダイジェスト）
    \param hashlen  ハッシュの長さ（オクテット）
    \param res  署名の結果、1 ==有効、0 ==無効
    \sa wc_iotsafe_ecc_verify_hash_ex
    \sa wc_iotsafe_ecc_sign_hash
    \sa wc_iotsafe_ecc_gen_k
*/
int wc_iotsafe_ecc_verify_hash(byte *sig, word32 siglen, byte *hash, word32 hashlen, int *res, byte key_id);

/*!
    \ingroup IoTSafe 
    \brief  予め計算された256ビットハッシュに対するECCシグネチャを、IOT-SAFEアプレット内のプリプロビジョニング、またはプロビジョニングされたプリプロビジョニングを使用します。結果はRESに書き込まれます。1が有効で、0が無効です。注：有効なテストに戻り値を使用しないでください。Resのみを使用してください。\ ref WC_IOTSAFE_ECC_VERIFY_HASH "WC_IOTSAFE_ECC_VERIFY_HASH"を除き、2バイト以上のキーIDで呼び出すことができる点を除きます。
    \return 0  成功すると（署名が無効であっても）
    \return <  故障の場合は0
    \param sig  検証する署名を含むバッファ
    \param hash  署名されたハッシュ（メッセージダイジェスト）
    \param hashlen  ハッシュの長さ（オクテット）
    \param res  署名の結果、1 ==有効、0 ==無効
    \param key_id  パブリックECCキーがIOTセーフアプレットに保存されているスロットのID
    \sa wc_iotsafe_ecc_verify_hash
    \sa wc_iotsafe_ecc_sign_hash
    \sa wc_iotsafe_ecc_gen_k
*/
int wc_iotsafe_ecc_verify_hash_ex(byte *sig, word32 siglen, byte *hash, word32 hashlen, int *res, byte *key_id, word16 id_size);

/*!
    \ingroup IoTSafe 
    \brief  ECC 256ビットのキーペアを生成し、それを（書き込み可能な）スロットにIOTセーフなアプレットに保存します。
    \param key_id  ECCキーペアがIOTセーフアプレットに格納されているスロットのID。
    \return 0  成功すると
    \sa wc_iotsafe_ecc_gen_k_ex
    \sa wc_iotsafe_ecc_sign_hash
    \sa wc_iotsafe_ecc_verify_hash
*/
int wc_iotsafe_ecc_gen_k(byte key_id);

/*!
    \ingroup IoTSafe 
    \brief  ECC 256ビットのキーペアを生成し、それを（書き込み可能な）スロットにIOTセーフなアプレットに保存します。\ ref wc_iotsafe_ecc_gen_k "wc_iotsafe_ecc_gen_k"と同等です。ただし、2バイト以上のキーIDで呼び出すことができます。
    \param key_id  ECCキーペアがIOTセーフアプレットに格納されているスロットのID。
    \param id_size  キーIDサイズ
    \return 0  成功すると
    \sa wc_iotsafe_ecc_gen_k
    \sa wc_iotsafe_ecc_sign_hash_ex
    \sa wc_iotsafe_ecc_verify_hash_ex
*/
int wc_iotsafe_ecc_gen_k(byte key_id);
