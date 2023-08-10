/*!
    \ingroup Math 
    \brief  この関数は、整数の最大サイズのランタイムFastMath設定をチェックします。FP_SIZEが正しく機能するために、FP_SIZEが各ライブラリーに一致しなければならないため、ユーザーがWolfCryptライブラリを独立して使用している場合に重要です。このチェックはCheckFastMathSettings（）として定義されています。これは、CheckRuntimeFastMathとFP_SIZEを比較するだけで、ミスマッチがある場合は0を返します。
    \return FP_SIZE  数学ライブラリで利用可能な最大サイズに対応するFP_SIZEを返します。
    _Example_
    \code
    if (CheckFastMathSettings() != 1) {
	return err_sys("Build vs. runtime fastmath FP_MAX_BITS mismatch\n");
    }
    // This is converted by the preprocessor to:
    // if ( (CheckRunTimeFastMath() == FP_SIZE) != 1) {
    // and confirms that the fast math settings match
    // the compile time settings
    \endcode
    \sa CheckRunTimeSettings
*/
word32 CheckRunTimeFastMath(void);
