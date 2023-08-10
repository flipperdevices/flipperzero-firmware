/*!
    \ingroup Math

    \brief This function checks the runtime fastmath settings for the maximum
    size of an integer. It is important when a user is using a wolfCrypt
    library independently, as the FP_SIZE must match for each library in order
    for math to work correctly. This check is defined as
    CheckFastMathSettings(), which simply compares CheckRunTimeFastMath
    and FP_SIZE, returning 0 if there is a mismatch, or 1 if they match.

    \return FP_SIZE Returns FP_SIZE, corresponding to the max size
    available for the math library.

    \param none No parameters.

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
