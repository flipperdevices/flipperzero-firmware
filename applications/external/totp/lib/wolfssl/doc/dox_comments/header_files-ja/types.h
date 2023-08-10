/*!
    \ingroup Memory 
    \brief  これは実際には関数ではなく、むしろプリプロセッサマクロであり、ユーザーは自分のMalloc、Realloc、および標準のCメモリ関数の代わりに自由な関数に置き換えることができます。外部メモリ機能を使用するには、xmalloc_userを定義します。これにより、メモリ機能をフォームの外部関数に置き換えます.extern void * xmalloc（size_t n、void * heap、int型）; extern void * XrealLoc（void * p、size_t n、void *ヒープ、int型）。 extern void xfree（void * p、void * heap、int型）; wolfssl_malloc、wolfssl_realloc、wolfssl_freeの代わりに基本的なCメモリ機能を使用するには、NO_WOLFSSL_MEMORYを定義します。これにより、メモリ関数が次のものに置き換えられます。#define Xmalloc（s、h、t）（（void）h、（void）t、malloc（（s）））#define xfree（p、h、t）{void * xp =（p）; if（（xp））free（（xp））; #define xrealloc（p、n、h、t）Realloc（（p）、（n））これらのオプションのどれも選択されていない場合、システムはデフォルトで使用されます。 WolfSSLメモリ機能ユーザーはコールバックフックを介してカスタムメモリ機能を設定できます（Wolfssl_Malloc、WolfSSL_Realloc、wolfssl_freeを参照）。このオプションは、メモリ関数を次のものに置き換えます。#define xmalloc（s、h、t）（（void）H、（Void）T、wolfssl_malloc（（s）））#define xfree（p、h、t）{void * XP =（P）; if（（xp））wolfssl_free（（xp））; #define xrealloc（p、n、h、t）wolfssl_realloc（（p）、（n））
    \return pointer  成功したメモリへのポインタを返します
	\return NULL  失敗した
	\param s  割り当てるメモリのサイズ
	\param h  （カスタムXMalloc関数で使用されています）使用するヒープへのポインタ
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
    \brief  これは実際には関数ではなく、むしろプリプロセッサマクロであり、ユーザーは自分のMalloc、Realloc、および標準のCメモリ関数の代わりに自由な関数に置き換えることができます。外部メモリ機能を使用するには、xmalloc_userを定義します。これにより、メモリ機能をフォームの外部関数に置き換えます.extern void * xmalloc（size_t n、void * heap、int型）; extern void * XrealLoc（void * p、size_t n、void *ヒープ、int型）。 extern void xfree（void * p、void * heap、int型）; wolfssl_malloc、wolfssl_realloc、wolfssl_freeの代わりに基本的なCメモリ機能を使用するには、NO_WOLFSSL_MEMORYを定義します。これにより、メモリ関数が次のものに置き換えられます。#define Xmalloc（s、h、t）（（void）h、（void）t、malloc（（s）））#define xfree（p、h、t）{void * xp =（p）; if（（xp））free（（xp））; #define xrealloc（p、n、h、t）Realloc（（p）、（n））これらのオプションのどれも選択されていない場合、システムはデフォルトで使用されます。 WolfSSLメモリ機能ユーザーはコールバックフックを介してカスタムメモリ機能を設定できます（Wolfssl_Malloc、WolfSSL_Realloc、wolfssl_freeを参照）。このオプションは、メモリ関数を次のものに置き換えます。#define xmalloc（s、h、t）（（void）H、（Void）T、wolfssl_malloc（（s）））#define xfree（p、h、t）{void * XP =（P）; if（（xp））wolfssl_free（（xp））; #define xrealloc（p、n、h、t）wolfssl_realloc（（p）、（n））
    \return Return  成功したメモリを割り当てるポインタ
	\return NULL  失敗した
	\param p  Reallocateへのアドレスへのポインタ
	\param n  割り当てるメモリのサイズ
	\param h  （カスタムXrealloc関数で使用されています）使用するヒープへのポインタ
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
    \brief  これは実際には関数ではなく、むしろプリプロセッサマクロであり、ユーザーは自分のMalloc、Realloc、および標準のCメモリ関数の代わりに自由な関数に置き換えることができます。外部メモリ機能を使用するには、xmalloc_userを定義します。これにより、メモリ機能をフォームの外部関数に置き換えます.extern void * xmalloc（size_t n、void * heap、int型）; extern void * XrealLoc（void * p、size_t n、void *ヒープ、int型）。 extern void xfree（void * p、void * heap、int型）; wolfssl_malloc、wolfssl_realloc、wolfssl_freeの代わりに基本的なCメモリ機能を使用するには、NO_WOLFSSL_MEMORYを定義します。これにより、メモリ関数が次のものに置き換えられます。#define Xmalloc（s、h、t）（（void）h、（void）t、malloc（（s）））#define xfree（p、h、t）{void * xp =（p）; if（（xp））free（（xp））; #define xrealloc（p、n、h、t）Realloc（（p）、（n））これらのオプションのどれも選択されていない場合、システムはデフォルトで使用されます。 WolfSSLメモリ機能ユーザーはコールバックフックを介してカスタムメモリ機能を設定できます（Wolfssl_Malloc、WolfSSL_Realloc、wolfssl_freeを参照）。このオプションは、メモリ関数を次のものに置き換えます。#define xmalloc（s、h、t）（（void）H、（Void）T、wolfssl_malloc（（s）））#define xfree（p、h、t）{void * XP =（P）; if（（xp））wolfssl_free（（xp））; #define xrealloc（p、n、h、t）wolfssl_realloc（（p）、（n））
    \return none  いいえ返します。
    \param p  無料のアドレスへのポインタ
	\param h  使用するヒープへの（カスタムXFree関数で使用されています）。
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
    \brief  この関数はコンパイル時クラスの設定をチェックします。設定が正しく機能するためのライブラリ間のライブラリ間で一致する必要があるため、ユーザーがWolfCryptライブラリを独立して使用している場合は重要です。このチェックはCheckCtcSettings（）として定義されています。これは、CheckRuntimeSettingsとCTC_Settingsを比較するだけで、ミスマッチがある場合は0、または1が一致した場合は1を返します。
    \return settings  実行時CTC_SETTINGS（コンパイル時設定）を返します。
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
