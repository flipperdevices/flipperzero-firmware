/*!
    \ingroup Random

    \brief Init global Whitewood netRandom context

    \return 0 Success
    \return BAD_FUNC_ARG Either configFile is null or timeout is negative.
    \return RNG_FAILURE_E There was a failure initializing the rng.

    \param configFile Path to configuration file
    \param hmac_cb Optional to create HMAC callback.
    \param timeout A timeout duration.

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

    \brief Free global Whitewood netRandom context.

    \return 0 Success
    \return BAD_MUTEX_E Error locking mutex on wnr_mutex

    \param none No returns.

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

    \brief Gets the seed (from OS) and key cipher for rng.  rng->drbg
    (deterministic random bit generator) allocated (should be deallocated
    with wc_FreeRng).  This is a blocking operation.

    \return 0 on success.
    \return MEMORY_E XMALLOC failed
    \return WINCRYPT_E wc_GenerateSeed: failed to acquire context
    \return CRYPTGEN_E wc_GenerateSeed: failed to get random
    \return BAD_FUNC_ARG wc_RNG_GenerateBlock input is null or sz exceeds
    MAX_REQUEST_LEN
    \return DRBG_CONT_FIPS_E wc_RNG_GenerateBlock: Hash_gen returned
    DRBG_CONT_FAILURE
    \return RNG_FAILURE_E wc_RNG_GenerateBlock: Default error.  rng’s
    status originally not ok, or set to DRBG_FAILED

    \param rng random number generator to be initialized for use
    with a seed and key cipher

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

    \brief Copies a sz bytes of pseudorandom data to output. Will
    reseed rng if needed (blocking).

    \return 0 on success
    \return BAD_FUNC_ARG an input is null or sz exceeds MAX_REQUEST_LEN
    \return DRBG_CONT_FIPS_E Hash_gen returned DRBG_CONT_FAILURE
    \return RNG_FAILURE_E Default error. rng’s status originally not
    ok, or set to DRBG_FAILED

    \param rng random number generator initialized with wc_InitRng
    \param output buffer to which the block is copied
    \param sz size of output in bytes

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

    \brief Creates a new WC_RNG structure.


    \return WC_RNG structure on success
    \return NULL on error


    \param heap pointer to a heap identifier
    \param nonce pointer to the buffer containing the nonce
    \param nonceSz length of the nonce

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

    \brief Calls wc_RNG_GenerateBlock to copy a byte of pseudorandom
    data to b. Will reseed rng if needed.

    \return 0 on success
    \return BAD_FUNC_ARG an input is null or sz exceeds MAX_REQUEST_LEN
    \return DRBG_CONT_FIPS_E Hash_gen returned DRBG_CONT_FAILURE
    \return RNG_FAILURE_E Default error.  rng’s status originally not
    ok, or set to DRBG_FAILED

    \param rng: random number generator initialized with wc_InitRng
    \param b one byte buffer to which the block is copied

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

    \brief Should be called when RNG no longer needed in order to securely
    free drgb.  Zeros and XFREEs rng-drbg.

    \return 0 on success
    \return BAD_FUNC_ARG rng or rng->drgb null
    \return RNG_FAILURE_E Failed to deallocated drbg

    \param rng random number generator initialized with wc_InitRng

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

    \brief Should be called when RNG no longer needed in order to securely
    free rng.


    \param rng random number generator initialized with wc_InitRng

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

    \brief Creates and tests functionality of drbg.

    \return 0 on success
    \return BAD_FUNC_ARG entropyA and output must not be null.  If reseed
    set entropyB must not be null
    \return -1 test failed

    \param int reseed: if set, will test reseed functionality
    \param entropyA: entropy to instantiate drgb with
    \param entropyASz: size of entropyA in bytes
    \param entropyB: If reseed set, drbg will be reseeded with entropyB
    \param entropyBSz: size of entropyB in bytes
    \param output: initialized to random data seeded with entropyB if
    seedrandom is set, and entropyA otherwise
    \param outputSz: length of output in bytes

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
