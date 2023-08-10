/*!
    \ingroup Random 
    \brief  Init Global WhiteWood Netrandomのコンテキスト
    \return 0  成功
    \return BAD_FUNC_ARG  configfileがnullまたはタイムアウトのどちらかが否定的です。
    \return RNG_FAILURE_E  RNGの初期化に失敗しました。
    \param configFile  設定ファイルへのパス
    \param hmac_cb  HMACコールバックを作成するにはオプションです。
    _Example_
    \code
    char* config = "path/to/config/example.conf";
    int time = // Some sufficient timeout value;

    if (wc_InitNetRandom(config, NULL, time) != 0)
    {
        // Some error occurred
    }
    \endcode
    \sa wc_FreeNetRandom
*/
int  wc_InitNetRandom(const char* configFile, wnr_hmac_key hmac_cb, int timeout);

/*!
    \ingroup Random 
    \brief  無料のGlobal WhiteWood Netrandomコンテキスト。
    \return 0  成功
    \return BAD_MUTEX_E  Wnr_Mutexでミューテックスをロックするエラー
    _Example_
    \code
    int ret = wc_FreeNetRandom();
    if(ret != 0)
    {
        // Handle the error
    }
    \endcode
    \sa wc_InitNetRandom
*/
int  wc_FreeNetRandom(void);

/*!
    \ingroup Random 
    \brief  RNGのシード（OSから）とキー暗号を取得します。割り当てられたRNG-> DRBG（決定論的ランダムビットジェネレータ）が割り当てられます（WC_FREERNGで割り当てられている必要があります）。これはブロッキング操作です。
    \return 0  成功しています。
    \return MEMORY_E  XMallocに失敗しました
    \return WINCRYPT_E  WC_GENERATSEED：コンテキストの取得に失敗しました
    \return CRYPTGEN_E  WC_GENERATSEED：ランダムになりました
    \return BAD_FUNC_ARG  WC_RNG_GenerateBlock入力はNULLまたはSZがMAX_REQUEST_LENを超えています
    \return DRBG_CONT_FIPS_E  wc_rng_generateblock：hash_genはdrbg_cont_failureを返しました
    \return RNG_FAILURE_E  wc_rng_generateBlock：デフォルトエラーです。RNGのステータスはもともとOKではなく、drbg_failedに設定されています
    _Example_
    \code
    RNG  rng;
    int ret;

    #ifdef HAVE_CAVIUM
    ret = wc_InitRngCavium(&rng, CAVIUM_DEV_ID);
    if (ret != 0){
        printf(“RNG Nitrox init for device: %d failed”, CAVIUM_DEV_ID);
        return -1;
    }
    #endif
    ret = wc_InitRng(&rng);
    if (ret != 0){
        printf(“RNG init failed”);
        return -1;
    }
    \endcode
    \sa wc_InitRngCavium
    \sa wc_RNG_GenerateBlock
    \sa wc_RNG_GenerateByte
    \sa wc_FreeRng
    \sa wc_RNG_HealthTest
*/
int  wc_InitRng(WC_RNG*);

/*!
    \ingroup Random 
    \brief  疑似ランダムデータのSZバイトを出力にコピーします。必要に応じてRNG（ブロッキング）します。
    \return 0  成功した
    \return BAD_FUNC_ARG  入力はNULLまたはSZがMAX_REQUEST_LENを超えています
    \return DRBG_CONT_FIPS_E  hash_genはdrbg_cont_failureを返しました
    \return RNG_FAILURE_E  デフォルトのエラーRNGのステータスはもともとOKではなく、drbg_failedに設定されています
    \param rng  乱数発生器はWC_INITRNGで初期化された
    \param output  ブロックがコピーされるバッファ
    _Example_
    \code
    RNG  rng;
    int  sz = 32;
    byte block[sz];

    int ret = wc_InitRng(&rng);
    if (ret != 0) {
        return -1; //init of rng failed!
    }

    ret = wc_RNG_GenerateBlock(&rng, block, sz);
    if (ret != 0) {
        return -1; //generating block failed!
    }
    \endcode
    \sa wc_InitRngCavium, wc_InitRng
    \sa wc_RNG_GenerateByte
    \sa wc_FreeRng
    \sa wc_RNG_HealthTest
*/
int  wc_RNG_GenerateBlock(WC_RNG* rng, byte* b, word32 sz);

/*!
    \ingroup Random 
    \brief  新しいWC_RNG構造を作成します。
    \return WC_RNG  成功の構造
    \return NULL  誤りに
    \param heap  ヒープ識別子へのポインタ
    \param nonce  nonceを含むバッファへのポインタ
    _Example_
    \code
    RNG  rng;
    byte nonce[] = { initialize nonce };
    word32 nonceSz = sizeof(nonce);

    wc_rng_new(&nonce, nonceSz, &heap);


    \endcode
    \sa wc_InitRng
    \sa wc_rng_free
    \sa wc_FreeRng
    \sa wc_RNG_HealthTest
*/
WC_RNG* wc_rng_new(byte* nonce, word32 nonceSz, void* heap)

/*!
    \ingroup Random 
    \brief  wc_rng_generateBlockを呼び出して、疑似ランダムデータのバイトをbにコピーします。必要に応じてRNGが再販されます。
    \return 0  成功した
    \return BAD_FUNC_ARG  入力はNULLまたはSZがMAX_REQUEST_LENを超えています
    \return DRBG_CONT_FIPS_E  hash_genはdrbg_cont_failureを返しました
    \return RNG_FAILURE_E  デフォルトのエラーRNGのステータスはもともとOKではなく、drbg_failedに設定されています
    \param rng:  乱数発生器はWC_INITRNGで初期化された
    _Example_
    \code
    RNG  rng;
    int  sz = 32;
    byte b[1];

    int ret = wc_InitRng(&rng);
    if (ret != 0) {
        return -1; //init of rng failed!
    }

    ret = wc_RNG_GenerateByte(&rng, b);
    if (ret != 0) {
        return -1; //generating block failed!
    }
    \endcode
    \sa wc_InitRngCavium
    \sa wc_InitRng
    \sa wc_RNG_GenerateBlock
    \sa wc_FreeRng
    \sa wc_RNG_HealthTest
*/
int  wc_RNG_GenerateByte(WC_RNG* rng, byte* b);

/*!
    \ingroup Random 
    \brief  RNGがDRGBを安全に解放するために必要なときに呼び出されるべきです。ゼロとXfrees RNG-DRBG。
    \return 0  成功した
    \return BAD_FUNC_ARG  RNGまたはRNG-> DRGB NULL
    \return RNG_FAILURE_E  DRBGの割り当て解除に失敗しました
    _Example_
    \code
    RNG  rng;
    int ret = wc_InitRng(&rng);
    if (ret != 0) {
        return -1; //init of rng failed!
    }

    int ret = wc_FreeRng(&rng);
    if (ret != 0) {
        return -1; //free of rng failed!
    }
    \endcode
    \sa wc_InitRngCavium
    \sa wc_InitRng
    \sa wc_RNG_GenerateBlock
    \sa wc_RNG_GenerateByte,
    \sa wc_RNG_HealthTest
*/
int  wc_FreeRng(WC_RNG*);

/*!
    \ingroup Random 
    \brief  RNGを安全に自由に解放するためにRNGが不要になったときに呼び出されるべきです。
    _Example_
    \code
    RNG  rng;
    byte nonce[] = { initialize nonce };
    word32 nonceSz = sizeof(nonce);

    rng = wc_rng_new(&nonce, nonceSz, &heap);

    // use rng

    wc_rng_free(&rng);

    \endcode
    \sa wc_InitRng
    \sa wc_rng_new
    \sa wc_FreeRng
    \sa wc_RNG_HealthTest
*/
WC_RNG* wc_rng_free(WC_RNG* rng);

/*!
    \ingroup Random 
    \brief  DRBGの機能を作成しテストします。
    \return 0  成功した
    \return BAD_FUNC_ARG  ELTOPYAと出力はNULLにしないでください。Reseed Set EntropybがNULLでなければならない場合
    \return -1  テスト失敗
    \param int  RESEED：設定されている場合は、Reseed機能をテストします
    \param entropyA:  DRGBをインスタンス化するエントロピー
    \param entropyASz:  バイト数のエントロピヤのサイズ
    \param entropyB:  Reseed Setを設定した場合、DRBGはEntropybでリサイードされます
    \param entropyBSz:  バイト単位のEntropybのサイズ
    \param output:  SEADRANDOMが設定されている場合は、Entropybに播種されたランダムなデータに初期化され、それ以外の場合はEntropya
    _Example_
    \code
    byte output[SHA256_DIGEST_SIZE * 4];
    const byte test1EntropyB[] = ....; // test input for reseed false
    const byte test1Output[] = ....;   // testvector: expected output of
                                   // reseed false
    ret = wc_RNG_HealthTest(0, test1Entropy, sizeof(test1Entropy), NULL, 0,
                        output, sizeof(output));
    if (ret != 0)
        return -1;//healthtest without reseed failed

    if (XMEMCMP(test1Output, output, sizeof(output)) != 0)
        return -1; //compare to testvector failed: unexpected output

    const byte test2EntropyB[] = ....; // test input for reseed
    const byte test2Output[] = ....;   // testvector expected output of reseed
    ret = wc_RNG_HealthTest(1, test2EntropyA, sizeof(test2EntropyA),
                        test2EntropyB, sizeof(test2EntropyB),
                        output, sizeof(output));

    if (XMEMCMP(test2Output, output, sizeof(output)) != 0)
        return -1; //compare to testvector failed
    \endcode
    \sa wc_InitRngCavium
    \sa wc_InitRng
    \sa wc_RNG_GenerateBlock
    \sa wc_RNG_GenerateByte
    \sa wc_FreeRng
*/
int wc_RNG_HealthTest(int reseed,
                                        const byte* entropyA, word32 entropyASz,
                                        const byte* entropyB, word32 entropyBSz,
                                        byte* output, word32 outputSz);
