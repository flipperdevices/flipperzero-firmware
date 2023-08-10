/*!
    \ingroup Memory 
    \brief  この関数はmalloc（）と似ていますが、WolfSSLが使用するように構成されているメモリ割り当て関数を呼び出します。デフォルトでは、WolfSSLはmalloc（）を使用します。これは、WolfSSLメモリ抽象化レイヤを使用して変更できます -  wolfssl_setAllocator（）を参照してください。注WOLFSSL_MALLOCは、WOLFSSLによって直接呼び出されませんが、代わりにMacro XMallocによって呼び出されます。デフォルトのビルドの場合、size引数のみが存在します。wolfssl_static_memoryビルドを使用する場合は、ヒープとタイプ引数が含まれます。
    \return pointer  成功した場合、この関数は割り当てられたメモリへのポインタを返します。
    \return error  エラーがある場合は、NULLが返されます。
    \param size  割り当てるメモリのサイズ（バイト）
    \param heap  メモリに使用するヒントヒント。nullになることができます
    _Example_
    \code
    int* tenInts = (int*)wolfSSL_Malloc(sizeof(int)*10);
    \endcode
    \sa wolfSSL_Free
    \sa wolfSSL_Realloc
    \sa wolfSSL_SetAllocators
    \sa XMALLOC
    \sa XFREE
    \sa XREALLOC
*/
void* wolfSSL_Malloc(size_t size, void* heap, int type);

/*!
    \ingroup Memory 
    \brief  この関数はfree（）と似ていますが、WolfSSLが使用するように構成されているメモリフリー機能を呼び出します。デフォルトでは、WolfSSLはfree（）を使用します。これは、WolfSSLメモリ抽象化レイヤを使用して変更できます -  wolfssl_setAllocator（）を参照してください。注WOLFSSL_FREEはWOLFSSLによって直接呼び出されませんが、代わりにマクロXFreeによって呼び出されます。デフォルトのビルドの場合、PTR引数のみが存在します。wolfssl_static_memoryビルドを使用する場合は、ヒープとタイプ引数が含まれます。
    \return none  いいえ返します。
    \param ptr  解放されるメモリへのポインタ。
    \param heap  メモリに使用するヒントヒント。nullになることができます
    _Example_
    \code
    int* tenInts = (int*)wolfSSL_Malloc(sizeof(int)*10);
    // process data as desired
    ...
    if(tenInts) {
    	wolfSSL_Free(tenInts);
    }
    \endcode
    \sa wolfSSL_Alloc
    \sa wolfSSL_Realloc
    \sa wolfSSL_SetAllocators
    \sa XMALLOC
    \sa XFREE
    \sa XREALLOC
*/
void  wolfSSL_Free(void *ptr, void* heap, int type);

/*!
    \ingroup Memory 
    \brief  この関数はREALLOC（）と似ていますが、WolfSSLが使用するように構成されているメモリ再割り当て機能を呼び出します。デフォルトでは、WolfSSLはRealLoc（）を使用します。これは、WolfSSLメモリ抽象化レイヤを使用して変更できます -  wolfssl_setAllocator（）を参照してください。注WOLFSSL_REALLOCはWOLFSSLによって直接呼び出されませんが、代わりにマクロXreallocによって呼び出されます。デフォルトのビルドの場合、size引数のみが存在します。wolfssl_static_memoryビルドを使用する場合は、ヒープとタイプ引数が含まれます。
    \return pointer  成功した場合、この関数はマイポイントを再割り当てするためのポインタを返します。これはPTRと同じポインタ、または新しいポインタの場所であり得る。
    \return Null  エラーがある場合は、NULLが返されます。
    \param ptr  再割り当てされているメモリへのポインタ。
    \param size  割り当てるバイト数。
    \param heap  メモリに使用するヒントヒント。nullになることができます
    _Example_
    \code
    int* tenInts = (int*)wolfSSL_Malloc(sizeof(int)*10);
    int* twentyInts = (int*)wolfSSL_Realloc(tenInts, sizeof(int)*20);
    \endcode
    \sa wolfSSL_Free
    \sa wolfSSL_Malloc
    \sa wolfSSL_SetAllocators
    \sa XMALLOC
    \sa XFREE
    \sa XREALLOC
*/
void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type);

/*!
    \ingroup Memory 
    \brief  この機能は、WolfSSLが使用する割り当て関数を登録します。デフォルトでは、システムがそれをサポートしている場合、Malloc / FreeとRealLocが使用されます。この機能を使用すると、実行時にユーザーは独自のメモリハンドラをインストールできます。
    \return Success  成功した場合、この関数は0を返します。
    \return BAD_FUNC_ARG  関数ポインタが提供されていない場合に返されるエラーです。
    \param malloc_function  使用するWolfSSLのメモリ割り当て機能関数署名は、上記のwolfssl_malloc_cbプロトタイプと一致する必要があります。
    \param free_function  使用するWolfSSLのメモリフリー機能関数シグネチャは、上記のwolfssl_free_cbプロトタイプと一致する必要があります。
    _Example_
    \code
    static void* MyMalloc(size_t size)
    {
    	// custom malloc function
    }

    static void MyFree(void* ptr)
    {
    	// custom free function
    }

    static void* MyRealloc(void* ptr, size_t size)
    {
    	// custom realloc function
    }

    // Register custom memory functions with wolfSSL
    int ret = wolfSSL_SetAllocators(MyMalloc, MyFree, MyRealloc);
    if (ret != 0) {
    	// failed to set memory functions
    }
    \endcode
    \sa none
*/
int wolfSSL_SetAllocators(wolfSSL_Malloc_cb,
                                      wolfSSL_Free_cb,
                                      wolfSSL_Realloc_cb);

/*!
    \ingroup Memory 
    \brief  この機能は、静的メモリ機能が使用されている場合（--enable-staticMemory）の場合に使用できます。メモリの「バケット」に最適なバッファサイズを示します。これにより、パーティション化された後に追加の未使用のメモリが終了しないように、バッファサイズを計算する方法が可能になります。返された値は、正の場合、使用するコンピュータのバッファサイズです。
    \return Success  バッファサイズ計算を正常に完了すると、正の値が返されます。この返された値は最適なバッファサイズです。
    \return Failure  すべての負の値はエラーの場合と見なされます。
    \param buffer  バッファへのポインタ
    \param size  バッファのサイズ
    _Example_
    \code
    byte buffer[1000];
    word32 size = sizeof(buffer);
    int optimum;
    optimum = wolfSSL_StaticBufferSz(buffer, size, WOLFMEM_GENERAL);
    if (optimum < 0) { //handle error case }
    printf(“The optimum buffer size to make use of all memory is %d\n”,
    optimum);
    ...
    \endcode
    \sa wolfSSL_Malloc
    \sa wolfSSL_Free
*/
int wolfSSL_StaticBufferSz(byte* buffer, word32 sz, int flag);

/*!
    \ingroup Memory 
    \brief  この機能は、静的メモリ機能が使用されている場合（--enable-staticMemory）の場合に使用できます。メモリの各パーティションに必要なパディングのサイズを示します。このパディングサイズは、メモリアライメントのために追加のメモリ管理構造を含む必要があるサイズになります。
    \return On  正常なメモリパディング計算戻り値は正の値になります
    \return All  負の値はエラーケースと見なされます。
    _Example_
    \code
    int padding;
    padding = wolfSSL_MemoryPaddingSz();
    if (padding < 0) { //handle error case }
    printf(“The padding size needed for each \”bucket\” of memory is %d\n”,
    padding);
    // calculation of buffer for IO POOL size is number of buckets
    // times (padding + WOLFMEM_IO_SZ)
    ...
    \endcode
    \sa wolfSSL_Malloc
    \sa wolfSSL_Free
*/
int wolfSSL_MemoryPaddingSz(void);
