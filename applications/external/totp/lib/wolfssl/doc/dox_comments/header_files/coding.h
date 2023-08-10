/*!
    \ingroup Base_Encoding

    \brief This function decodes the given Base64 encoded input, in, and
    stores the result in the output buffer out. It also sets the size
    written to the output buffer in the variable outLen.

    \return 0 Returned upon successfully decoding the Base64 encoded input
    \return BAD_FUNC_ARG Returned if the output buffer is too small to
    store the decoded input
    \return ASN_INPUT_E Returned if a character in the input buffer falls
    outside of the Base64 range ([A-Za-z0-9+/=]) or if there is an invalid
    line ending in the Base64 encoded input

    \param in pointer to the input buffer to decode
    \param inLen length of the input buffer to decode
    \param out pointer to the output buffer in which to store the decoded
    message
    \param outLen pointer to the length of the output buffer. Updated with
    the bytes written at the end of the function call

    _Example_
    \code
    byte encoded[] = { // initialize text to decode };
    byte decoded[sizeof(encoded)];
    // requires at least (sizeof(encoded) * 3 + 3) / 4 room

    int outLen = sizeof(decoded);

    if( Base64_Decode(encoded,sizeof(encoded), decoded, &outLen) != 0 ) {
    	// error decoding input buffer
    }
    \endcode

    \sa Base64_Encode
    \sa Base16_Decode
*/
int Base64_Decode(const byte* in, word32 inLen, byte* out,
                               word32* outLen);

/*!
    \ingroup Base_Encoding

    \brief This function encodes the given input, in, and stores the Base64
    encoded result in the output buffer out. It writes the data with the
    traditional ‘\n’ line endings, instead of escaped %0A line endings. Upon
    successfully completing, this function also sets outLen to the number
    of bytes written to the output buffer.

    \return 0 Returned upon successfully decoding the Base64 encoded input
    \return BAD_FUNC_ARG Returned if the output buffer is too small to
    store the encoded input
    \return BUFFER_E Returned if the output buffer runs out of room
    while encoding

    \param in pointer to the input buffer to encode
    \param inLen length of the input buffer to encode
    \param out pointer to the output buffer in which to store the
    encoded message
    \param outLen pointer to the length of the output buffer in
    which to store the encoded message

    _Example_
    \code
    byte plain[] = { // initialize text to encode };
    byte encoded[MAX_BUFFER_SIZE];

    int outLen = sizeof(encoded);

    if( Base64_Encode(plain, sizeof(plain), encoded, &outLen) != 0 ) {
    	// error encoding input buffer
    }
    \endcode

    \sa Base64_EncodeEsc
    \sa Base64_Decode
*/

int Base64_Encode(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);

/*!
    \ingroup Base_Encoding

    \brief This function encodes the given input, in, and stores the
    Base64 encoded result in the output buffer out. It writes the data
    with %0A escaped line endings instead of ‘\n’ line endings.
    Upon successfully completing, this function also sets outLen
    to the number of bytes written to the output buffer.

    \return 0 Returned upon successfully decoding the Base64 encoded input
    \return BAD_FUNC_ARG Returned if the output buffer is too small
    to store the encoded input
    \return BUFFER_E Returned if the output buffer runs out of
    room while encoding
    \return ASN_INPUT_E Returned if there is an error processing
    the decode on the input message

    \param in pointer to the input buffer to encode
    \param inLen length of the input buffer to encode
    \param out pointer to the output buffer in which to store
    the encoded message
    \param outLen pointer to the length of the output buffer in
    which to store the encoded message

    _Example_
    \code
    byte plain[] = { // initialize text to encode };
    byte encoded[MAX_BUFFER_SIZE];

    int outLen = sizeof(encoded);

    if( Base64_EncodeEsc(plain, sizeof(plain), encoded, &outLen) != 0 ) {
    	// error encoding input buffer
    }
    \endcode

    \sa Base64_Encode
    \sa Base64_Decode
*/
int Base64_EncodeEsc(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);

/*!
    \ingroup Base_Encoding

    \brief This function encodes the given input, in, and stores the
    Base64 encoded result in the output buffer out. It writes the data
    with no new lines. Upon successfully completing, this function
    also sets outLen to the number of bytes written to the output buffer

    \return 0 Returned upon successfully decoding the Base64 encoded input
    \return BAD_FUNC_ARG Returned if the output buffer is too small
    to store the encoded input
    \return BUFFER_E Returned if the output buffer runs out of room
    while encoding
    \return ASN_INPUT_E Returned if there is an error processing the
    decode on the input message

    \param in pointer to the input buffer to encode
    \param inLen length of the input buffer to encode
    \param out pointer to the output buffer in which to store the
    encoded message
    \param outLen pointer to the length of the output buffer in which to
    store the encoded message

    _Example_
    \code
    byte plain[] = { // initialize text to encode };
    byte encoded[MAX_BUFFER_SIZE];
    int outLen = sizeof(encoded);
    if( Base64_Encode_NoNl(plain, sizeof(plain), encoded, &outLen) != 0 ) {
    	// error encoding input buffer
    }
    \endcode

    \sa Base64_Encode
    \sa Base64_Decode
*/

int Base64_Encode_NoNl(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);

/*!
    \ingroup Base_Encoding

    \brief This function decodes the given Base16 encoded input, in, and
    stores the result in the output buffer out. It also sets the size written
    to the output buffer in the variable outLen.

    \return 0 Returned upon successfully decoding the Base16 encoded input
    \return BAD_FUNC_ARG Returned if the output buffer is too small to store
    the decoded input or if the input length is not a multiple of two
    \return ASN_INPUT_E Returned if a character in the input buffer falls
    outside of the Base16 range ([0-9A-F])

    \param in pointer to the input buffer to decode
    \param inLen length of the input buffer to decode
    \param out pointer to the output buffer in which to store the decoded
    message
    \param outLen pointer to the length of the output buffer. Updated with the
    bytes written at the end of the function call

    _Example_
    \code
    byte encoded[] = { // initialize text to decode };
    byte decoded[sizeof(encoded)];
    int outLen = sizeof(decoded);

    if( Base16_Decode(encoded,sizeof(encoded), decoded, &outLen) != 0 ) {
    	// error decoding input buffer
    }
    \endcode

    \sa Base64_Encode
    \sa Base64_Decode
    \sa Base16_Encode
*/

int Base16_Decode(const byte* in, word32 inLen, byte* out, word32* outLen);

/*!
    \ingroup Base_Encoding

    \brief Encode input to base16 output.

    \return 0 Success
    \return BAD_FUNC_ARG Returns if in, out, or outLen is null or if outLen is
    less than 2 times inLen plus 1.

    \param in Pointer to input buffer to be encoded.
    \param inLen Length of input buffer.
    \param out Pointer to output buffer.
    \param outLen Length of output buffer.  Is set to len of encoded output.

    _Example_
    \code
    byte in[] = { // Contents of something to be encoded };
    byte out[NECESSARY_OUTPUT_SIZE];
    word32 outSz = sizeof(out);

    if(Base16_Encode(in, sizeof(in), out, &outSz) != 0)
    {
        // Handle encode error
    }
    \endcode

    \sa Base64_Encode
    \sa Base64_Decode
    \sa Base16_Decode
*/

int Base16_Encode(const byte* in, word32 inLen, byte* out, word32* outLen);
