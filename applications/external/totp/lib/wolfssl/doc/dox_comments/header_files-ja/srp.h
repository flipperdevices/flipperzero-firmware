/*!
    \ingroup SRP 
    \brief  使用方法のためにSRP構造体を初期化します。
    \return 0  成功しています。
    \return BAD_FUNC_ARG  SRPなどの引数がNULLまたはSRPSIDEの問題がある場合は、SRP_CLIENT_SIESまたはSRP_SERVER_SIEDでは問題がある場合に返します。
    \return NOT_COMPILED_IN  タイプが引数として渡されたが、WolfCryptビルドに設定されていない場合。
    \return <0  エラー時に。
    \param srp  初期化されるSRP構造。
    \param type  使用するハッシュ型。
    _Example_
    \code
    Srp srp;
    if (wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE) != 0)
    {
        // Initialization error
    }
    else
    {
        wc_SrpTerm(&srp);
    }
    \endcode
    \sa wc_SrpTerm
    \sa wc_SrpSetUsername
*/
int wc_SrpInit(Srp* srp, SrpType type, SrpSide side);

/*!
    \ingroup SRP 
    \brief  使用後にSRP構造リソースを解放します。
    \return none  いいえ返します。
    _Example_
    \code
    Srp srp;
    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE);
    // Use srp
    wc_SrpTerm(&srp)
    \endcode
    \sa wc_SrpInit
*/
void wc_SrpTerm(Srp* srp);

/*!
    \ingroup SRP 
    \brief  ユーザー名を設定します。この関数は、wc_srpinitの後に呼び出す必要があります。
    \return 0  ユーザー名は正常に設定されました。
    \return BAD_FUNC_ARG:  srpまたはusernameがnullの場合に返します。
    \return MEMORY_E:  SRP->ユーザーにメモリを割り当てる問題がある場合
    \return <  0：エラー。
    \param srp  SRP構造
    \param username  ユーザー名を含むバッファ。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;

    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE);
    if(wc_SrpSetUsername(&srp, username, usernameSize) != 0)
    {
        // Error occurred setting username.
    }
    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpInit
    \sa wc_SrpSetParams
    \sa wc_SrpTerm
*/
int wc_SrpSetUsername(Srp* srp, const byte* username, word32 size);

/*!
    \ingroup SRP 
    \brief  ユーザー名に基づいてSRPパラメータを設定します.. wc_srpsetuserNameの後に呼び出す必要があります。
    \return 0  成功
    \return BAD_FUNC_ARG  SRP、N、G、またはSALTがNULLの場合、またはNSZ <GSZの場合は返します。
    \return SRP_CALL_ORDER_E  wc_srpsetuserNameの前にwc_srpsetparamsが呼び出された場合、返します。
    \return <0  エラー
    \param srp  SRP構造
    \param N  弾性率n = 2q + 1、[q、n]はプリムです。
    \param nSz  nサイズをバイト単位で。
    \param g  ジェネレータモジュロN.
    \param gSz  バイト数のGサイズ
    \param salt  小さいランダムな塩。各ユーザー名に特有のものです。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;

    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt

    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE);
    wc_SrpSetUsername(&srp, username, usernameSize);

    if(wc_SrpSetParams(&srp, N, sizeof(N), g, sizeof(g), salt,
    sizeof(salt)) != 0)
    {
        // Error setting params
    }
    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpInit
    \sa wc_SrpSetUsername
    \sa wc_SrpTerm
*/
int wc_SrpSetParams(Srp* srp, const byte* N,    word32 nSz,
                                          const byte* g,    word32 gSz,
                                          const byte* salt, word32 saltSz);

/*!
    \ingroup SRP 
    \brief  パスワードを設定します。パスワードを設定しても、SRP構造内のパスワードデータが消去されません。クライアントは、x = h（salt + h（user：pswd））を計算し、それを認証フィールドに格納します。この関数は、wc_srpsetparamsの後に呼び出されなければならず、クライアント側のみです。
    \return 0  成功
    \return BAD_FUNC_ARG  srpまたはpasswordがnullの場合、またはsrp-> sideがsrp_client_sideに設定されていない場合。
    \return SRP_CALL_ORDER_E  WC_SRPSETPASSWORDが順不同で呼び出されたときに戻ります。
    \return <0  エラー
    \param srp  SRP構造
    \param password  パスワードを含むバッファ。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;
    byte password[] = "password";
    word32 passwordSize = 8;

    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt

    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE);
    wc_SrpSetUsername(&srp, username, usernameSize);
    wc_SrpSetParams(&srp, N, sizeof(N), g, sizeof(g), salt, sizeof(salt));

    if(wc_SrpSetPassword(&srp, password, passwordSize) != 0)
    {
        // Error setting password
    }

    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpInit
    \sa wc_SrpSetUsername
    \sa wc_SrpSetParams
*/
int wc_SrpSetPassword(Srp* srp, const byte* password, word32 size);

/*!
    \ingroup SRP 
    \brief  検証者を設定します。この関数は、wc_srpsetparamsの後に呼び出され、サーバー側のみです。
    \return 0  成功
    \return BAD_FUNC_ARG  SRPまたはVerifierがNULLまたはSRP-> ISの場合、SRP_SERVER_SIEDではなく返されます。
    \return <0  エラー
    \param srp  SRP構造
    \param verifier  検証者を含む構造体。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;

    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt
    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_SERVER_SIDE);
    wc_SrpSetUsername(&srp, username, usernameSize);
    wc_SrpSetParams(&srp, N, sizeof(N), g, sizeof(g), salt, sizeof(salt))
    byte verifier[] = { }; // Contents of some verifier

    if(wc_SrpSetVerifier(&srp, verifier, sizeof(verifier)) != 0)
    {
        // Error setting verifier
    }

    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpInit
    \sa wc_SrpSetParams
    \sa wc_SrpGetVerifier
*/
int wc_SrpSetVerifier(Srp* srp, const byte* verifier, word32 size);

/*!
    \ingroup SRP 
    \brief  検証者を取得します。クライアントはV = g ^ x％Nで検証者を計算します。この関数は、wc_srpsetpasswordの後に呼び出され、クライアント側のみです。
    \return 0  成功
    \return BAD_FUNC_ARG  SRP、Verifier、またはSizeがNULLの場合、またはSRP-> SIDEがSRP_CLIENT_SIEDではない場合に返されます。
    \return SRP_CALL_ORDER_E  WC_SRPGetverifierが順不同で呼び出された場合に返されます。
    \return <0  エラー
    \param srp  SRP構造
    \param verifier  検証者を書き込むためのバッファー。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;
    byte password[] = "password";
    word32 passwordSize = 8;

    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt
    byte v[64];
    word32 vSz = 0;
    vSz = sizeof(v);

    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE);
    wc_SrpSetUsername(&srp, username, usernameSize);
    wc_SrpSetParams(&srp, N, sizeof(N), g, sizeof(g), salt, sizeof(salt))
    wc_SrpSetPassword(&srp, password, passwordSize)

    if( wc_SrpGetVerifier(&srp, v, &vSz ) != 0)
    {
        // Error getting verifier
    }
    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpSetVerifier
    \sa wc_SrpSetPassword
*/
int wc_SrpGetVerifier(Srp* srp, byte* verifier, word32* size);

/*!
    \ingroup SRP 
    \brief  プライベートのエフェラル値を設定します。プライベートの一時的な値は、クライアント側のAとして知られています。サーバー側のand random（）b。b = random（）この関数は、ユニットテストケース、または開発者が外部ランダムソースを使用してエフェメラル値を設定したい場合は便利です。この関数は、WC_SRPGetPublicの前に呼び出されることがあります。
    \return 0  成功
    \return BAD_FUNC_ARG  SRP、Private、またはSizeがNULLの場合に返されます。
    \return SRP_CALL_ORDER_E  WC_SRPSetPrivateが順不同で呼び出された場合に返されます。
    \return <0  エラー
    \param srp  SRP構造
    \param priv  一時的な値。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;

    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt
    byte verifier = { }; // Contents of some verifier
    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_SERVER_SIDE);
    wc_SrpSetUsername(&srp, username, usernameSize);
    wc_SrpSetParams(&srp, N, sizeof(N), g, sizeof(g), salt, sizeof(salt))
    wc_SrpSetVerifier(&srp, verifier, sizeof(verifier))

    byte b[] = { }; // Some ephemeral value
    if( wc_SrpSetPrivate(&srp, b, sizeof(b)) != 0)
    {
        // Error setting private ephemeral
    }

    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpGetPublic
*/
int wc_SrpSetPrivate(Srp* srp, const byte* priv, word32 size);

/*!
    \ingroup SRP 
    \brief  公共の一時的な値を取得します。公共の一時的な値は、クライアント側のAとして知られています。サーバ側のA = g ^ A％n b。B =（k * v +（g b％n））％n wc_srpsetpasswordまたはwc_srpsetverifierの後に呼び出す必要があります。関数WC_SRPSetPrivateは、WC_SRPGetPublicの前に呼び出されることがあります。
    \return 0  成功
    \return BAD_FUNC_ARG  srp、pub、またはsizeがnullの場合に返されます。
    \return SRP_CALL_ORDER_E  WC_SRPGetPublicが順不同で呼び出された場合に返されます。
    \return BUFFER_E  サイズ<srp.nの場合は返しました。
    \return <0  エラー
    \param srp  SRP構造
    \param pub  パブリックエフェラル値を書き込むためのバッファ。
    _Example_
    \code
    Srp srp;
    byte username[] = "user";
    word32 usernameSize = 4;
    byte password[] = "password";
    word32 passwordSize = 8;

    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt
    wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE);
    wc_SrpSetUsername(&srp, username, usernameSize);
    wc_SrpSetParams(&srp, N, sizeof(N), g, sizeof(g), salt, sizeof(salt));
    wc_SrpSetPassword(&srp, password, passwordSize)

    byte public[64];
    word32 publicSz = 0;

    if( wc_SrpGetPublic(&srp, public, &publicSz) != 0)
    {
        // Error getting public ephemeral
    }

    wc_SrpTerm(&srp);
    \endcode
    \sa wc_SrpSetPrivate
    \sa wc_SrpSetPassword
    \sa wc_SrpSetVerifier
*/
int wc_SrpGetPublic(Srp* srp, byte* pub, word32* size);

/*!
    \ingroup SRP 
    \brief  セッションキーを計算します。成功後にSRP->キーでキーをアクセスできます。
    \return 0  成功
    \return BAD_FUNC_ARG  SRP、ClientPubKey、またはServerPubKeyの場合、またはClientPubkeyszまたはServerPubKeyszが0の場合に返されます。
    \return SRP_CALL_ORDER_E  WC_SRPComputeKeyが順不同で呼び出された場合に返されます。
    \return <0  エラー
    \param srp  SRP構造
    \param clientPubKey  クライアントの公共の一時的な価値。
    \param clientPubKeySz  クライアントの公共の一時的な値のサイズ。
    \param serverPubKey  サーバーの一般の一時的な値。
    _Example_
    \code
    Srp server;

    byte username[] = "user";
        word32 usernameSize = 4;
    byte password[] = "password";
    word32 passwordSize = 8;
    byte N[] = { }; // Contents of byte array N
    byte g[] = { }; // Contents of byte array g
    byte salt[] = { }; // Contents of byte array salt
    byte verifier[] = { }; // Contents of some verifier
    byte serverPubKey[] = { }; // Contents of server pub key
    word32 serverPubKeySize = sizeof(serverPubKey);
    byte clientPubKey[64];
    word32 clientPubKeySize = 64;

    wc_SrpInit(&server, SRP_TYPE_SHA, SRP_SERVER_SIDE);
    wc_SrpSetUsername(&server, username, usernameSize);
    wc_SrpSetParams(&server, N, sizeof(N), g, sizeof(g), salt, sizeof(salt));
    wc_SrpSetVerifier(&server, verifier, sizeof(verifier));
    wc_SrpGetPublic(&server, serverPubKey, &serverPubKeySize);

    wc_SrpComputeKey(&server, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySize)
    wc_SrpTerm(&server);
    \endcode
    \sa wc_SrpGetPublic
*/
int wc_SrpComputeKey(Srp* srp,
                                 byte* clientPubKey, word32 clientPubKeySz,
                                 byte* serverPubKey, word32 serverPubKeySz);

/*!
    \ingroup SRP 
    \brief  証明を取得します。この関数は、wc_srpcomputekeyの後に呼び出す必要があります。
    \return 0  成功
    \return BAD_FUNC_ARG  SRP、PROV、またはSIZEがNULLの場合に返します。
    \return BUFFER_E  サイズがSRP-> Typeのハッシュサイズより小さい場合に返します。
    \return <0  エラー
    \param srp  SRP構造
    \param proof  ピアプルーフ。
    _Example_
    \code
    Srp cli;
    byte clientProof[SRP_MAX_DIGEST_SIZE];
    word32 clientProofSz = SRP_MAX_DIGEST_SIZE;

    // Initialize Srp following steps from previous examples

    if (wc_SrpGetProof(&cli, clientProof, &clientProofSz) != 0)
    {
        // Error getting proof
    }
    \endcode
    \sa wc_SrpComputeKey
*/
int wc_SrpGetProof(Srp* srp, byte* proof, word32* size);

/*!
    \ingroup SRP 
    \brief  ピアプルーフを確認します。この関数は、WC_SRPGetSessionKeyの前に呼び出す必要があります。
    \return 0  成功
    \return <0  エラー
    \param srp  SRP構造
    \param proof  ピアプルーフ。
    _Example_
    \code
    Srp cli;
    Srp srv;
    byte clientProof[SRP_MAX_DIGEST_SIZE];
    word32 clientProofSz = SRP_MAX_DIGEST_SIZE;

    // Initialize Srp following steps from previous examples
    // First get the proof
    wc_SrpGetProof(&cli, clientProof, &clientProofSz)

    if (wc_SrpVerifyPeersProof(&srv, clientProof, clientProofSz) != 0)
    {
        // Error verifying proof
    }
    \endcode
    \sa wc_SrpGetSessionKey
    \sa wc_SrpGetProof
    \sa wc_SrpTerm
*/
int wc_SrpVerifyPeersProof(Srp* srp, byte* proof, word32 size);
