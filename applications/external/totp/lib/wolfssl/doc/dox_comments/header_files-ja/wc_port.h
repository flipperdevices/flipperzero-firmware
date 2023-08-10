/*!
    \ingroup wolfCrypt 
    \brief  WolfCryptによって使用されるリソースを初期化するために使用されます。
    \return 0  成功すると。
    \return <0  initリソースが失敗すると。
    _Example_
    \code
    ...
    if (wolfCrypt_Init() != 0) {
        WOLFSSL_MSG("Error with wolfCrypt_Init call");
    }
    \endcode
    \sa wolfCrypt_Cleanup
*/
int wolfCrypt_Init(void);

/*!
    \ingroup wolfCrypt 
    \brief  WolfCryptによって使用されるリソースをクリーンアップするために使用されます。
    \return 0  成功すると。
    \return <0  リソースのクリーンアップが失敗したとき。
    _Example_
    \code
    ...
    if (wolfCrypt_Cleanup() != 0) {
        WOLFSSL_MSG("Error with wolfCrypt_Cleanup call");
    }
    \endcode
    \sa wolfCrypt_Init
*/
int wolfCrypt_Cleanup(void);
