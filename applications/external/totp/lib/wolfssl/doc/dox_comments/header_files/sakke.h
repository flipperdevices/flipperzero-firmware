
/*!
    \ingroup SAKKE_Setup
*/
int wc_InitSakkeKey(SakkeKey* key, void* heap, int devId);
/*!
    \ingroup SAKKE_Setup
*/
int wc_InitSakkeKey_ex(SakkeKey* key, int keySize, int curveId,
        void* heap, int devId);
/*!
    \ingroup SAKKE_Setup
*/
void wc_FreeSakkeKey(SakkeKey* key);

/*!
    \ingroup SAKKE_Setup
*/
int wc_MakeSakkeKey(SakkeKey* key, WC_RNG* rng);
/*!
    \ingroup SAKKE_Setup
*/
int wc_MakeSakkePublicKey(SakkeKey* key, ecc_point* pub);

/*!
    \ingroup SAKKE_RSK
*/
int wc_MakeSakkeRsk(SakkeKey* key, const byte* id, word16 idSz,
        ecc_point* rsk);
/*!
    \ingroup SAKKE_RSK
*/
int wc_ValidateSakkeRsk(SakkeKey* key, const byte* id, word16 idSz,
        ecc_point* rsk, int* valid);
/*!
    \ingroup SAKKE_RSK
*/
int wc_GenerateSakkeRskTable(const SakkeKey* key,
        const ecc_point* rsk, byte* table, word32* len);


/*!
    \ingroup SAKKE_Setup
*/
int wc_ExportSakkeKey(SakkeKey* key, byte* data, word32* sz);
/*!
    \ingroup SAKKE_Setup
*/
int wc_ImportSakkeKey(SakkeKey* key, const byte* data, word32 sz);
/*!
    \ingroup SAKKE_Setup
*/
int wc_ExportSakkePrivateKey(SakkeKey* key, byte* data, word32* sz);
/*!
    \ingroup SAKKE_Setup
*/
int wc_ImportSakkePrivateKey(SakkeKey* key, const byte* data,
        word32 sz);

/*!
    \ingroup SAKKE_RSK
*/
int wc_EncodeSakkeRsk(const SakkeKey* key, ecc_point* rsk,
        byte* out, word32* sz, int raw);
/*!
    \ingroup SAKKE_RSK
*/
int wc_DecodeSakkeRsk(const SakkeKey* key, const byte* data,
        word32 sz, ecc_point* rsk);

/*!
    \ingroup SAKKE_RSK
*/
int wc_ImportSakkeRsk(SakkeKey* key, const byte* data, word32 sz);

/*!
    \ingroup SAKKE_Setup
*/
int wc_ExportSakkePublicKey(SakkeKey* key, byte* data,
        word32* sz, int raw);
/*!
    \ingroup SAKKE_Setup
*/
int wc_ImportSakkePublicKey(SakkeKey* key, const byte* data,
        word32 sz, int trusted);

/*!
    \ingroup SAKKE_Operations
*/
int wc_GetSakkeAuthSize(SakkeKey* key, word16* authSz);
/*!
    \ingroup SAKKE_Setup
*/
int wc_SetSakkeIdentity(SakkeKey* key, const byte* id, word16 idSz);
/*!
    \ingroup SAKKE_Operations
*/
int wc_MakeSakkePointI(SakkeKey* key, const byte* id, word16 idSz);
/*!
    \ingroup SAKKE_Operations
*/
int wc_GetSakkePointI(SakkeKey* key, byte* data, word32* sz);
/*!
    \ingroup SAKKE_Operations
*/
int wc_SetSakkePointI(SakkeKey* key, const byte* id, word16 idSz,
        const byte* data, word32 sz);
/*!
    \ingroup SAKKE_Operations
*/
int wc_GenerateSakkePointITable(SakkeKey* key, byte* table,
         word32* len);
/*!
    \ingroup SAKKE_Operations
*/
int wc_SetSakkePointITable(SakkeKey* key, byte* table, word32 len);
/*!
    \ingroup SAKKE_Operations
*/
int wc_ClearSakkePointITable(SakkeKey* key);
/*!
    \ingroup SAKKE_Operations
*/
int wc_MakeSakkeEncapsulatedSSV(SakkeKey* key,
        enum wc_HashType hashType, byte* ssv, word16 ssvSz, byte* auth,
        word16* authSz);
/*!
    \ingroup SAKKE_Operations
*/
int wc_GenerateSakkeSSV(SakkeKey* key, WC_RNG* rng, byte* ssv,
        word16* ssvSz);
/*!
    \ingroup SAKKE_RSK
*/
int wc_SetSakkeRsk(SakkeKey* key, const ecc_point* rsk, byte* table,
        word32 len);
/*!
    \ingroup SAKKE_Operations
*/
int wc_DeriveSakkeSSV(SakkeKey* key, enum wc_HashType hashType,
        byte* ssv, word16 ssvSz, const byte* auth,
        word16 authSz);

