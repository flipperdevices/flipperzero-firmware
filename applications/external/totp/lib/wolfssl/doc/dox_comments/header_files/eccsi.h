
/*!
    \ingroup ECCSI_Setup
*/
int wc_InitEccsiKey(EccsiKey* key, void* heap, int devId);
/*!
    \ingroup ECCSI_Setup
*/
int wc_InitEccsiKey_ex(EccsiKey* key, int keySz, int curveId,
        void* heap, int devId);
/*!
    \ingroup ECCSI_Setup
*/
void wc_FreeEccsiKey(EccsiKey* key);

/*!
    \ingroup ECCSI_Setup
*/
int wc_MakeEccsiKey(EccsiKey* key, WC_RNG* rng);

/*!
    \ingroup ECCSI_Operations
*/
int wc_MakeEccsiPair(EccsiKey* key, WC_RNG* rng,
        enum wc_HashType hashType, const byte* id, word32 idSz, mp_int* ssk,
        ecc_point* pvt);
/*!
    \ingroup ECCSI_Operations
*/
int wc_ValidateEccsiPair(EccsiKey* key, enum wc_HashType hashType,
        const byte* id, word32 idSz, const mp_int* ssk, ecc_point* pvt,
        int* valid);
/*!
    \ingroup ECCSI_Operations
*/
int wc_ValidateEccsiPvt(EccsiKey* key, const ecc_point* pvt,
        int* valid);
/*!
    \ingroup ECCSI_Operations
*/
int wc_EncodeEccsiPair(const EccsiKey* key, mp_int* ssk,
        ecc_point* pvt, byte* data, word32* sz);
/*!
    \ingroup ECCSI_Operations
*/
int wc_EncodeEccsiSsk(const EccsiKey* key, mp_int* ssk, byte* data,
        word32* sz);
/*!
    \ingroup ECCSI_Operations
*/
int wc_EncodeEccsiPvt(const EccsiKey* key, ecc_point* pvt,
        byte* data, word32* sz, int raw);
/*!
    \ingroup ECCSI_Operations
*/
int wc_DecodeEccsiPair(const EccsiKey* key, const byte* data,
        word32 sz, mp_int* ssk, ecc_point* pvt);
/*!
    \ingroup ECCSI_Operations
*/
int wc_DecodeEccsiSsk(const EccsiKey* key, const byte* data,
        word32 sz, mp_int* ssk);
/*!
    \ingroup ECCSI_Operations
*/
int wc_DecodeEccsiPvt(const EccsiKey* key, const byte* data,
        word32 sz, ecc_point* pvt);
/*!
    \ingroup ECCSI_Operations
*/
int wc_DecodeEccsiPvtFromSig(const EccsiKey* key, const byte* sig,
        word32 sz, ecc_point* pvt);

/*!
    \ingroup ECCSI_Setup
*/
int wc_ExportEccsiKey(EccsiKey* key, byte* data, word32* sz);
/*!
    \ingroup ECCSI_Setup
*/
int wc_ImportEccsiKey(EccsiKey* key, const byte* data, word32 sz);

/*!
    \ingroup ECCSI_Setup
*/
int wc_ExportEccsiPrivateKey(EccsiKey* key, byte* data, word32* sz);
/*!
    \ingroup ECCSI_Setup
*/
int wc_ImportEccsiPrivateKey(EccsiKey* key, const byte* data,
        word32 sz);

/*!
    \ingroup ECCSI_Setup
*/
int wc_ExportEccsiPublicKey(EccsiKey* key, byte* data, word32* sz,
        int raw);
/*!
    \ingroup ECCSI_Setup
*/
int wc_ImportEccsiPublicKey(EccsiKey* key, const byte* data,
        word32 sz, int trusted);

/*!
    \ingroup ECCSI_Operations
*/
int wc_HashEccsiId(EccsiKey* key, enum wc_HashType hashType,
        const byte* id, word32 idSz, ecc_point* pvt, byte* hash, byte* hashSz);
/*!
    \ingroup ECCSI_Setup
*/
int wc_SetEccsiHash(EccsiKey* key, const byte* hash, byte hashSz);
/*!
    \ingroup ECCSI_Setup
*/
int wc_SetEccsiPair(EccsiKey* key, const mp_int* ssk,
        const ecc_point* pvt);

/*!
    \ingroup ECCSI_Operations
*/
int wc_SignEccsiHash(EccsiKey* key, WC_RNG* rng,
        enum wc_HashType hashType, const byte* msg, word32 msgSz, byte* sig,
        word32* sigSz);
/*!
    \ingroup ECCSI_Operations
*/
int wc_VerifyEccsiHash(EccsiKey* key, enum wc_HashType hashType,
        const byte* msg, word32 msgSz, const byte* sig, word32 sigSz,
        int* verified);

