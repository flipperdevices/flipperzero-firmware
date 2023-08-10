/*!
    \ingroup Memory

    \brief This is not actually a function, but rather a preprocessor macro,
    which allows the user to substitute in their own malloc, realloc, and free
    functions in place of the standard C memory functions.
    To use external memory functions, define XMALLOC_USER. This will cause the
    memory functions to be replaced by external functions of the form:
    extern void *XMALLOC(size_t n, void* heap, int type);
    extern void *XREALLOC(void *p, size_t n, void* heap, int type);
    extern void XFREE(void *p, void* heap, int type);
    To use the basic C memory functions in place of wolfSSL_Malloc,
    wolfSSL_Realloc, wolfSSL_Free, define NO_WOLFSSL_MEMORY. This
    will replace the memory functions with:
    #define XMALLOC(s, h, t) 	((void)h, (void)t, malloc((s)))
    #define XFREE(p, h, t)   	{void* xp = (p); if((xp)) free((xp));}
    #define XREALLOC(p, n, h, t) realloc((p), (n))
    If none of these options are selected, the system will default to use
    the wolfSSL memory functions. A user can set custom memory functions
    through callback hooks, (see wolfSSL_Malloc,
    wolfSSL_Realloc, wolfSSL_Free). This option will replace the
    memory functions with:
    #define XMALLOC(s, h, t) 	((void)h, (void)t, wolfSSL_Malloc((s)))
    #define XFREE(p, h, t)   	{void* xp = (p); if((xp)) wolfSSL_Free((xp));}
    #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n))
    	
    \return pointer Return a pointer to allocated memory on success
	\return NULL on failure
	
	\param s size of memory to allocate
	\param h (used by custom XMALLOC function) pointer to the heap to use	
	\param t memory allocation types for user hints. See enum in types.h
	
	_Example_
	\code
	int* tenInts = XMALLOC(sizeof(int)*10, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (tenInts == NULL) {
	    // error allocating space
	    return MEMORY_E;
    }
	\endcode
	
	\sa wolfSSL_Malloc
	\sa wolfSSL_Realloc
	\sa wolfSSL_Free
	\sa wolfSSL_SetAllocators
*/
void* XMALLOC(size_t n, void* heap, int type);

/*!
    \ingroup Memory

    \brief This is not actually a function, but rather a preprocessor macro,
    which allows the user to substitute in their own malloc, realloc, and
    free functions in place of the standard C memory functions.
    To use external memory functions, define XMALLOC_USER. This will cause the
    memory functions to be replaced by external functions of the form:
    extern void *XMALLOC(size_t n, void* heap, int type);
    extern void *XREALLOC(void *p, size_t n, void* heap, int type);
    extern void XFREE(void *p, void* heap, int type);
    To use the basic C memory functions in place of wolfSSL_Malloc,
    wolfSSL_Realloc, wolfSSL_Free, define NO_WOLFSSL_MEMORY. This will
    replace the memory functions with:
    #define XMALLOC(s, h, t) 	((void)h, (void)t, malloc((s)))
   	#define XFREE(p, h, t)   	{void* xp = (p); if((xp)) free((xp));}
   	#define XREALLOC(p, n, h, t) realloc((p), (n))
    If none of these options are selected, the system will default to
    use the wolfSSL memory functions. A user can set custom memory
    functions through callback hooks, (see wolfSSL_Malloc,
    wolfSSL_Realloc, wolfSSL_Free). This option will replace
    the memory functions with:
    #define XMALLOC(s, h, t) 	((void)h, (void)t, wolfSSL_Malloc((s)))
    #define XFREE(p, h, t)   	{void* xp = (p); if((xp)) wolfSSL_Free((xp));}
    #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n))

    \return Return a pointer to allocated memory on success
	\return NULL on failure
	
	\param p pointer to the address to reallocate
	\param n size of memory to allocate
	\param h (used by custom XREALLOC function) pointer to the heap to use	
	\param t memory allocation types for user hints. See enum in types.h
	
	_Example_
	\code
	int* tenInts = (int*)XMALLOC(sizeof(int)*10, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    int* twentyInts = (int*)XREALLOC(tenInts, sizeof(int)*20, NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
	\endcode
	
	\sa wolfSSL_Malloc
	\sa wolfSSL_Realloc
	\sa wolfSSL_Free
	\sa wolfSSL_SetAllocators
*/
void* XREALLOC(void *p, size_t n, void* heap, int type);

/*!
    \ingroup Memory

    \brief This is not actually a function, but rather a preprocessor macro,
    which allows the user to substitute in their own malloc, realloc, and
    free functions in place of the standard C memory functions.
    To use external memory functions, define XMALLOC_USER. This will cause
    the memory functions to be replaced by external functions of the form:
    extern void *XMALLOC(size_t n, void* heap, int type);
    extern void *XREALLOC(void *p, size_t n, void* heap, int type);
    extern void XFREE(void *p, void* heap, int type);
    To use the basic C memory functions in place of wolfSSL_Malloc,
    wolfSSL_Realloc, wolfSSL_Free, define NO_WOLFSSL_MEMORY. This
    will replace the memory functions with:
    #define XMALLOC(s, h, t) 	((void)h, (void)t, malloc((s)))
    #define XFREE(p, h, t)   	{void* xp = (p); if((xp)) free((xp));}
    #define XREALLOC(p, n, h, t) realloc((p), (n))
    If none of these options are selected, the system will default to use
    the wolfSSL memory functions. A user can set custom memory functions
    through callback hooks, (see wolfSSL_Malloc, wolfSSL_Realloc,
    wolfSSL_Free). This option will replace the memory functions with:
    #define XMALLOC(s, h, t) 	((void)h, (void)t, wolfSSL_Malloc((s)))
    #define XFREE(p, h, t)   	{void* xp = (p); if((xp)) wolfSSL_Free((xp));}
    #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n))

    \return none No returns.

    \param p pointer to the address to free
	\param h (used by custom XFREE function) pointer to the heap to use	
	\param t memory allocation types for user hints. See enum in types.h
	
	_Example_
	\code
	int* tenInts = XMALLOC(sizeof(int) * 10, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (tenInts == NULL) {
	    // error allocating space
	    return MEMORY_E;
    }
	\endcode
	
	\sa wolfSSL_Malloc
	\sa wolfSSL_Realloc
	\sa wolfSSL_Free
	\sa wolfSSL_SetAllocators
*/
void XFREE(void *p, void* heap, int type);

/*!
    \ingroup Math

    \brief This function checks the compile time class settings. It is
    important when a user is using a wolfCrypt library independently, as
    the settings must match between libraries for math to work correctly.
    This check is defined as CheckCtcSettings(), which simply compares
    CheckRunTimeSettings and CTC_SETTINGS, returning 0 if there is a
    mismatch, or 1 if they match.

    \return settings Returns the runtime CTC_SETTINGS (Compile Time Settings)

    \param none No Parameters.

    _Example_
    \code
    if (CheckCtcSettings() != 1) {
	    return err_sys("Build vs. runtime math mismatch\n");
    }
    // This is converted by the preprocessor to:
    // if ( (CheckCtcSettings() == CTC_SETTINGS) != 1) {
    // and will compare whether the compile time class settings
    // match the current settings
    \endcode

    \sa CheckRunTimeFastMath
*/
word32 CheckRunTimeSettings(void);
