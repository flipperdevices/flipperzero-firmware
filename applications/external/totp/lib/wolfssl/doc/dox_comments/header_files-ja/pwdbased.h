/*!
    \ingroup Password 
    \brief  この機能はパスワードベースの鍵導出機能1（PBKDF1）を実装し、入力パスワードを連結塩と共により安全な鍵に変換し、出力に記憶する。これにより、HASH関数としてSHAとMD5を選択できます。
    \return 0  入力パスワードからキーの派生に正常に戻された
    \return BAD_FUNC_ARG  与えられた無効なハッシュタイプがある場合（有効なタイプは：MD5とSHA）、反復は1未満、または要求されたキーの長さ（Klen）は提供されたハッシュのハッシュ長よりも大きいです。
    \return MEMORY_E  SHAまたはMD5オブジェクトにメモリを割り当てるエラーがある場合は返されます。
    \param output  生成されたキーを保存するバッファへのポインタ。少なくともklen longになるべきです
    \param passwd  キーの派生に使用するパスワードを含むバッファへのポインタ
    \param pLen  キーの派生に使用するパスワードの長さ
    \param salt  鍵由来に使用する塩を含む緩衝液へのポインター
    \param sLen  塩の長さ
    \param iterations  ハッシュを処理するための回数
    \param kLen  派生キーの希望の長さ。選択したハッシュのダイジェストサイズより長くしてはいけません
    _Example_
    \code
    int ret;
    byte key[MD5_DIGEST_SIZE];
    byte pass[] = { }; // initialize with password
    byte salt[] = { }; // initialize with salt

    ret = wc_PBKDF1(key, pass, sizeof(pass), salt, sizeof(salt), 1000,
    sizeof(key), MD5);
    if ( ret != 0 ) {
    	// error deriving key from password
    }
    \endcode
    \sa wc_PBKDF2
    \sa wc_PKCS12_PBKDF
*/
int wc_PBKDF1(byte* output, const byte* passwd, int pLen,
                      const byte* salt, int sLen, int iterations, int kLen,
                      int typeH);

/*!
    \ingroup Password 
    \brief  この機能はパスワードベースのキー導出機能2（PBKDF2）を実装し、入力パスワードを連結された塩とともにより安全なキーに変換し、出力に記憶されています。これにより、MD5、SHA、SHA256、SHA384、SHA512、およびBLAKE2Bなど、サポートされているHMACハッシュ関数のいずれかを選択できます。
    \return 0  入力パスワードからキーの派生に正常に戻された
    \return BAD_FUNC_ARG  無効なハッシュタイプがある場合、または反復が1未満の場合は返されます。
    \return MEMORY_E  HMACオブジェクトに割り振りメモリがある場合
    \param output  生成されたキーを保存するバッファへのポインタ。klen longにするべきです
    \param passwd  キーの派生に使用するパスワードを含むバッファへのポインタ
    \param pLen  キーの派生に使用するパスワードの長さ
    \param salt  鍵由来に使用する塩を含む緩衝液へのポインター
    \param sLen  塩の長さ
    \param iterations  ハッシュを処理するための回数
    \param kLen  派生鍵の望ましい長さ
    _Example_
    \code
    int ret;
    byte key[64];
    byte pass[] = { }; // initialize with password
    byte salt[] = { }; // initialize with salt

    ret = wc_PBKDF2(key, pass, sizeof(pass), salt, sizeof(salt), 2048, sizeof(key),
    SHA512);
    if ( ret != 0 ) {
    	// error deriving key from password
    }
    \endcode
    \sa wc_PBKDF1
    \sa wc_PKCS12_PBKDF
*/
int wc_PBKDF2(byte* output, const byte* passwd, int pLen,
                      const byte* salt, int sLen, int iterations, int kLen,
                      int typeH);

/*!
    \ingroup Password 
    \brief  この関数は、RFC 7292付録Bに記載されているパスワードベースのキー導出機能（PBKDF）を実装しています。この関数は、入力パスワードを連結塩でより安全なキーに変換します。それは、MD5、SHA、SHA256、SHA384、SHA512、およびBLAKE2Bを含む、ユーザーはサポートされているHMACハッシュ関数のいずれかを選択できます。
    \return 0  入力パスワードからキーの派生に正常に戻された
    \return BAD_FUNC_ARG  返された無効なハッシュタイプが与えられた場合、繰り返しは1未満、または要求されたキーの長さ（klen）が提供されたハッシュのハッシュ長よりも大きいです。
    \return MEMORY_E  割り当てメモリがある場合は返されます
    \return MP_INIT_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_READ_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_CMP_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_INVMOD_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_EXPTMOD_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_MOD_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_MUL_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_ADD_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_MULMOD_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_TO_E  キー生成中にエラーがある場合は返却される可能性があります
    \return MP_MEM  キー生成中にエラーがある場合は返却される可能性があります
    \param output  生成されたキーを保存するバッファへのポインタ。klen longにするべきです
    \param passwd  キーの派生に使用するパスワードを含むバッファへのポインタ
    \param pLen  キーの派生に使用するパスワードの長さ
    \param salt  鍵由来に使用する塩を含む緩衝液へのポインター
    \param sLen  塩の長さ
    \param iterations  ハッシュを処理するための回数
    \param kLen  派生鍵の望ましい長さ
    \param hashType  使用するハッシュアルゴリズム有効な選択肢は次のとおりです.MD5、SHA、SHA256、SHA384、SHA512、およびBLAKE2B
    _Example_
    \code
    int ret;
    byte key[64];
    byte pass[] = { }; // initialize with password
    byte salt[] = { }; // initialize with salt

    ret = wc_PKCS512_PBKDF(key, pass, sizeof(pass), salt, sizeof(salt), 2048,
    sizeof(key), SHA512, 1);
    if ( ret != 0 ) {
    	// error deriving key from password
    }
    \endcode
    \sa wc_PBKDF1
    \sa wc_PBKDF2
*/
int wc_PKCS12_PBKDF(byte* output, const byte* passwd, int pLen,
                            const byte* salt, int sLen, int iterations,
                            int kLen, int typeH, int purpose);
