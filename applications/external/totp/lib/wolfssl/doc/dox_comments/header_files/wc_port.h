/*!
    \ingroup wolfCrypt

    \brief Used to initialize resources used by wolfCrypt.

    \return 0 upon success.
    \return <0 upon failure of init resources.

    \param none No parameters.

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

    \brief Used to clean up resources used by wolfCrypt.

    \return 0 upon success.
    \return <0 upon failure of cleaning up resources.

    \param none No parameters.

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
