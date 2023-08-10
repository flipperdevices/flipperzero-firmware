/*!
    \ingroup Memory

    \brief This function is similar to malloc(), but calls the memory
    allocation function which wolfSSL has been configured to use. By default,
    wolfSSL uses malloc().  This can be changed using the wolfSSL memory
    abstraction layer - see wolfSSL_SetAllocators(). Note wolfSSL_Malloc is not 
    called directly by wolfSSL, but instead called by macro XMALLOC.
    For the default build only the size argument exists. If using 
    WOLFSSL_STATIC_MEMORY build then heap and type arguments are included.

    \return pointer If successful, this function returns a pointer to
    allocated memory.
    \return error If there is an error, NULL will be returned.

    \param size size, in bytes, of the memory to allocate
    \param heap heap hint to use for memory. Can be NULL
    \param type dynamic type (see DYNAMIC_TYPE_ list in types.h)

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

    \brief This function is similar to free(), but calls the memory free
    function which wolfSSL has been configured to use. By default, wolfSSL
    uses free(). This can be changed using the wolfSSL memory abstraction
    layer - see wolfSSL_SetAllocators(). Note wolfSSL_Free is not 
    called directly by wolfSSL, but instead called by macro XFREE.
    For the default build only the ptr argument exists. If using 
    WOLFSSL_STATIC_MEMORY build then heap and type arguments are included.

    \return none No returns.

    \param ptr pointer to the memory to be freed.
    \param heap heap hint to use for memory. Can be NULL
    \param type dynamic type (see DYNAMIC_TYPE_ list in types.h)

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

    \brief This function is similar to realloc(), but calls the memory
    re-allocation function which wolfSSL has been configured to use.
    By default, wolfSSL uses realloc().  This can be changed using the
    wolfSSL memory abstraction layer - see wolfSSL_SetAllocators(). 
    Note wolfSSL_Realloc is not called directly by wolfSSL, but instead called 
    by macro XREALLOC. For the default build only the size argument exists.
    If using WOLFSSL_STATIC_MEMORY build then heap and type arguments are included.

    \return pointer If successful, this function returns a pointer to
    re-allocated memory. This may be the same pointer as ptr, or a
    new pointer location.
    \return Null If there is an error, NULL will be returned.

    \param ptr pointer to the previously-allocated memory, to be reallocated.
    \param size number of bytes to allocate.
    \param heap heap hint to use for memory. Can be NULL
    \param type dynamic type (see DYNAMIC_TYPE_ list in types.h)

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

    \brief This function registers the allocation functions used by wolfSSL.
    By default, if the system supports it, malloc/free and realloc are used.
    Using this function allows the user at runtime to install their own
    memory handlers.

    \return Success If successful this function will return 0.
    \return BAD_FUNC_ARG is the error that will be returned if a
    function pointer is not provided.

    \param malloc_function memory allocation function for wolfSSL to use.
    Function signature must match wolfSSL_Malloc_cb prototype, above.
    \param free_function memory free function for wolfSSL to use.  Function
    signature must match wolfSSL_Free_cb prototype, above.
    \param realloc_function memory re-allocation function for wolfSSL to use.
    Function signature must match wolfSSL_Realloc_cb prototype, above.

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

    \brief This function is available when static memory feature is used
    (--enable-staticmemory). It gives the optimum buffer size for memory
    “buckets”. This allows for a way to compute buffer size so that no
    extra unused memory is left at the end after it has been partitioned.
    The returned value, if positive, is the computed buffer size to use.

    \return Success On successfully completing buffer size calculations a
    positive value is returned. This returned value is for optimum buffer size.
    \return Failure All negative values are considered to be error cases.

    \param buffer pointer to buffer
    \param size size of buffer
    \param type desired type of memory ie WOLFMEM_GENERAL or WOLFMEM_IO_POOL

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

    \brief This function is available when static memory feature is used
    (--enable-staticmemory). It gives the size of padding needed for each
    partition of memory. This padding size will be the size needed to
    contain a memory management structure along with any extra for
    memory alignment.

    \return On successfully memory padding calculation the return value will
    be a positive value
    \return All negative values are considered error cases.

    \param none No parameters.

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
