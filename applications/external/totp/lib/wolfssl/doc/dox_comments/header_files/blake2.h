/*!
    \ingroup BLAKE2

    \brief This function initializes a Blake2b structure for use with the
    Blake2 hash function.

    \return 0 Returned upon successfully initializing the Blake2b structure and
    setting the digest size.

    \param b2b pointer to the Blake2b structure to initialize
    \param digestSz length of the blake 2 digest to implement

    _Example_
    \code
    Blake2b b2b;
    // initialize Blake2b structure with 64 byte digest
    wc_InitBlake2b(&b2b, 64);
    \endcode

    \sa wc_Blake2bUpdate
*/
int wc_InitBlake2b(Blake2b* b2b, word32 digestSz);

/*!
    \ingroup BLAKE2

    \brief This function updates the Blake2b hash with the given input data.
    This function should be called after wc_InitBlake2b, and repeated until
    one is ready for the final hash: wc_Blake2bFinal.

    \return 0 Returned upon successfully update the Blake2b structure with
    the given data
    \return -1 Returned if there is a failure while compressing the input data

    \param b2b pointer to the Blake2b structure to update
    \param data pointer to a buffer containing the data to append
    \param sz length of the input data to append

    _Example_
    \code
    int ret;
    Blake2b b2b;
    // initialize Blake2b structure with 64 byte digest
    wc_InitBlake2b(&b2b, 64);

    byte plain[] = { // initialize input };

    ret = wc_Blake2bUpdate(&b2b, plain, sizeof(plain));
    if( ret != 0) {
    	// error updating blake2b
    }
    \endcode

    \sa wc_InitBlake2b
    \sa wc_Blake2bFinal
*/
int wc_Blake2bUpdate(Blake2b* b2b, const byte* data, word32 sz);

/*!
    \ingroup BLAKE2

    \brief This function computes the Blake2b hash of the previously supplied
    input data. The output hash will be of length requestSz, or, if
    requestSz==0, the digestSz of the b2b structure. This function should be
    called after wc_InitBlake2b and wc_Blake2bUpdate has been processed for
    each piece of input data desired.

    \return 0 Returned upon successfully computing the Blake2b hash
    \return -1 Returned if there is a failure while parsing the Blake2b hash

    \param b2b pointer to the Blake2b structure to update
    \param final pointer to a buffer in which to store the blake2b hash.
    Should be of length requestSz
    \param requestSz length of the digest to compute. When this is zero,
    b2b->digestSz will be used instead

    _Example_
    \code
    int ret;
    Blake2b b2b;
    byte hash[64];
    // initialize Blake2b structure with 64 byte digest
    wc_InitBlake2b(&b2b, 64);
    ... // call wc_Blake2bUpdate to add data to hash

    ret = wc_Blake2bFinal(&b2b, hash, 64);
    if( ret != 0) {
    	// error generating blake2b hash
    }
    \endcode

    \sa wc_InitBlake2b
    \sa wc_Blake2bUpdate
*/
int wc_Blake2bFinal(Blake2b* b2b, byte* final, word32 requestSz);
