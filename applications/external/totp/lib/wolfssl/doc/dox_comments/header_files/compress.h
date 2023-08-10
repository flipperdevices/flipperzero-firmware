/*!
    \ingroup Compression

    \brief This function compresses the given input data using Huffman coding
    and stores the output in out. Note that the output buffer should still be
    larger than the input buffer because there exists a certain input for
    which there will be no compression possible, which will still require a
    lookup table. It is recommended that one allocate srcSz + 0.1% + 12 for
    the output buffer.

    \return On successfully compressing the input data, returns the number
    of bytes stored in the output buffer
    \return COMPRESS_INIT_E Returned if there is an error initializing the
    stream for compression
    \return COMPRESS_E Returned if an error occurs during compression

    \param out pointer to the output buffer in which to store the compressed
    data
    \param outSz size available in the output buffer for storage
    \param in pointer to the buffer containing the message to compress
    \param inSz size of the input message to compress
    \param flags flags to control how compression operates. Use 0 for normal
    decompression

    _Example_
    \code
    byte message[] = { // initialize text to compress };
    byte compressed[(sizeof(message) + sizeof(message) * .001 + 12 )];
    // Recommends at least srcSz + .1% + 12

    if( wc_Compress(compressed, sizeof(compressed), message, sizeof(message),
    0) != 0){
    	// error compressing data
    }
    \endcode

    \sa wc_DeCompress
*/
int wc_Compress(byte* out, word32 outSz, const byte* in, word32 inSz, word32 flags);

/*!
    \ingroup Compression

    \brief This function decompresses the given compressed data using Huffman
    coding and stores the output in out.

    \return Success On successfully decompressing the input data, returns the
    number of bytes stored in the output buffer
    \return COMPRESS_INIT_E: Returned if there is an error initializing the
    stream for compression
    \return COMPRESS_E: Returned if an error occurs during compression

    \param out pointer to the output buffer in which to store the decompressed
    data
    \param outSz size available in the output buffer for storage
    \param in pointer to the buffer containing the message to decompress
    \param inSz size of the input message to decompress

    _Example_
    \code
    byte compressed[] = { // initialize compressed message };
    byte decompressed[MAX_MESSAGE_SIZE];

    if( wc_DeCompress(decompressed, sizeof(decompressed),
    compressed, sizeof(compressed)) != 0 ) {
    	// error decompressing data
    }
    \endcode

    \sa wc_Compress
*/
int wc_DeCompress(byte* out, word32 outSz, const byte* in, word32 inSz);
