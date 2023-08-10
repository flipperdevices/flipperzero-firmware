/*!
*/
int wc_Pkcs11_Initialize(Pkcs11Dev* dev, const char* library,
                                     void* heap);

/*!
*/
void wc_Pkcs11_Finalize(Pkcs11Dev* dev);

/*!
*/
int wc_Pkcs11Token_Init(Pkcs11Token* token, Pkcs11Dev* dev,
    int slotId, const char* tokenName, const unsigned char *userPin,
    int userPinSz);

/*!
*/
void wc_Pkcs11Token_Final(Pkcs11Token* token);

/*!
 */
int wc_Pkcs11Token_Open(Pkcs11Token* token, int readWrite);

/*!
 */
void wc_Pkcs11Token_Close(Pkcs11Token* token);

/*!
 */
int wc_Pkcs11StoreKey(Pkcs11Token* token, int type, int clear,

/*!
 */
int wc_Pkcs11_CryptoDevCb(int devId, wc_CryptoInfo* info,
    void* ctx);
