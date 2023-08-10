
/*!
*/
int wc_InitSakkeKey(SakkeKey* key, void* heap, int devId);
/*!
*/
int wc_InitSakkeKey_ex(SakkeKey* key, int keySize, int curveId,
        void* heap, int devId);
/*!
*/
void wc_FreeSakkeKey(SakkeKey* key);

/*!
*/
int wc_MakeSakkeKey(SakkeKey* key, WC_RNG* rng);
/*!
*/
int wc_MakeSakkePublicKey(SakkeKey* key, ecc_point* pub);

/*!
*/
int wc_MakeSakkeRsk(SakkeKey* key, const byte* id, word16 idSz,
        ecc_point* rsk);
/*!
*/
int wc_ValidateSakkeRsk(SakkeKey* key, const byte* id, word16 idSz,
        ecc_point* rsk, int* valid);
/*!
*/
int wc_GenerateSakkeRskTable(const SakkeKey* key,
        const ecc_point* rsk, byte* table, word32* len);


/*!
*/
int wc_ExportSakkeKey(SakkeKey* key, byte* data, word32* sz);
/*!
*/
int wc_ImportSakkeKey(SakkeKey* key, const byte* data, word32 sz);
/*!
*/
int wc_ExportSakkePrivateKey(SakkeKey* key, byte* data, word32* sz);
/*!
*/
int wc_ImportSakkePrivateKey(SakkeKey* key, const byte* data,
        word32 sz);

/*!
*/
int wc_EncodeSakkeRsk(const SakkeKey* key, ecc_point* rsk,
        byte* out, word32* sz, int raw);
/*!
*/
int wc_DecodeSakkeRsk(const SakkeKey* key, const byte* data,
        word32 sz, ecc_point* rsk);

/*!
*/
int wc_ImportSakkeRsk(SakkeKey* key, const byte* data, word32 sz);

/*!
*/
int wc_ExportSakkePublicKey(SakkeKey* key, byte* data,
        word32* sz, int raw);
/*!
*/
int wc_ImportSakkePublicKey(SakkeKey* key, const byte* data,
        word32 sz, int trusted);

/*!
*/
int wc_GetSakkeAuthSize(SakkeKey* key, word16* authSz);
/*!
*/
int wc_SetSakkeIdentity(SakkeKey* key, const byte* id, word16 idSz);
/*!
*/
int wc_MakeSakkePointI(SakkeKey* key, const byte* id, word16 idSz);
/*!
*/
int wc_GetSakkePointI(SakkeKey* key, byte* data, word32* sz);
/*!
*/
int wc_SetSakkePointI(SakkeKey* key, const byte* id, word16 idSz,
        const byte* data, word32 sz);
/*!
*/
int wc_GenerateSakkePointITable(SakkeKey* key, byte* table,
         word32* len);
/*!
*/
int wc_SetSakkePointITable(SakkeKey* key, byte* table, word32 len);
/*!
*/
int wc_ClearSakkePointITable(SakkeKey* key);
/*!
*/
int wc_MakeSakkeEncapsulatedSSV(SakkeKey* key,
        enum wc_HashType hashType, byte* ssv, word16 ssvSz, byte* auth,
        word16* authSz);
/*!
*/
int wc_GenerateSakkeSSV(SakkeKey* key, WC_RNG* rng, byte* ssv,
        word16* ssvSz);
/*!
*/
int wc_SetSakkeRsk(SakkeKey* key, const ecc_point* rsk, byte* table,
        word32 len);
/*!
*/
int wc_DeriveSakkeSSV(SakkeKey* key, enum wc_HashType hashType,
        byte* ssv, word16 ssvSz, const byte* auth,
        word16 authSz);

