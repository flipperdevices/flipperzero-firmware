/*!
    \ingroup ASN

    \brief This function initializes a default cert, with the default options:
    version = 3 (0x2), serial = 0, sigType = SHA_WITH_RSA, issuer = blank,
    daysValid = 500, selfSigned = 1 (true) use subject as issuer,
    subject = blank

    \return none No returns.

    \param cert pointer to an uninitialized cert structure to initialize

    _Example_
    \code
    Cert myCert;
    wc_InitCert(&myCert);
    \endcode

    \sa wc_MakeCert
    \sa wc_MakeCertReq
*/
int wc_InitCert(Cert*);

/*!
     \ingroup ASN

     \brief This function allocates a new Cert structure for use during
     cert operations without the application having to allocate the structure
     itself. The Cert structure is also initialized by this function thus
     removing the need to call wc_InitCert(). When the application is finished
     using the allocated Cert structure wc_CertFree() must be called.

     \return pointer If successful the call will return a pointer to the
     newly allocated and initialized Cert.
     \return NULL On a memory allocation failure.

     \param A pointer to the heap used for dynamic allocation. Can be NULL.

     _Example_
     \code
     Cert*   myCert;

     myCert = wc_CertNew(NULL);
     if (myCert == NULL) {
         // Cert creation failure
     }
     \endcode

     \sa wc_InitCert
     \sa wc_MakeCert
     \sa wc_CertFree

*/
Cert* wc_CertNew(void* heap);

/*!
     \ingroup ASN

     \brief This function frees the memory allocated for a cert structure
     by a previous call to wc_CertNew().

     \return None.

     \param A pointer to the cert structure to free.

     _Example_
     \code
     Cert*   myCert;

     myCert = wc_CertNew(NULL);

     // Perform cert operations.

     wc_CertFree(myCert);
     \endcode

     \sa wc_InitCert
     \sa wc_MakeCert
     \sa wc_CertNew

*/
void  wc_CertFree(Cert* cert);

/*!
    \ingroup ASN

    \brief Used to make CA signed certs. Called after the subject information
    has been entered. This function makes an x509 Certificate v3 RSA or ECC
    from a cert input. It then writes this cert to derBuffer. It takes in
    either an rsaKey or an eccKey to generate the certificate.  The certificate
    must be initialized with wc_InitCert before this method is called.

    \return Success On successfully making an x509 certificate from the
    specified input cert, returns the size of the cert generated.
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return BUFFER_E Returned if the provided derBuffer is too small to
    store the generated certificate
    \return Others Additional error messages may be returned if the cert
    generation is not successful.

    \param cert pointer to an initialized cert structure
    \param derBuffer pointer to the buffer in which to hold the generated cert
    \param derSz size of the buffer in which to store the cert
    \param rsaKey pointer to an RsaKey structure containing the rsa key used
    to generate the certificate
    \param eccKey pointer to an EccKey structure containing the ecc key used
    to generate the certificate
    \param rng pointer to the random number generator used to make the cert

    _Example_
    \code
    Cert myCert;
    wc_InitCert(&myCert);
    WC_RNG rng;
    //initialize rng;
    RsaKey key;
    //initialize key;
    byte * derCert = malloc(FOURK_BUF);
    word32 certSz;
    certSz = wc_MakeCert(&myCert, derCert, FOURK_BUF, &key, NULL, &rng);
    \endcode

    \sa wc_InitCert
    \sa wc_MakeCertReq
*/
int  wc_MakeCert(Cert* cert, byte* derBuffer, word32 derSz, RsaKey* rsaKey,
                             ecc_key* eccKey, WC_RNG* rng);

/*!
    \ingroup ASN

    \brief This function makes a certificate signing request using the input
    certificate and writes the output to derBuffer. It takes in either an
    rsaKey or an eccKey to generate the certificate request. wc_SignCert()
    will need to be called after this function to sign the certificate request.
    Please see the wolfCrypt test application (./wolfcrypt/test/test.c) for an
    example usage of this function.

    \return Success On successfully making an X.509 certificate request from
    the specified input cert, returns the size of the certificate
    request generated.
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return BUFFER_E Returned if the provided derBuffer is too small to store
    the generated certificate
    \return Other Additional error messages may be returned if the certificate
    request generation is not successful.

    \param cert pointer to an initialized cert structure
    \param derBuffer pointer to the buffer in which to hold the generated
    certificate request
    \param derSz size of the buffer in which to store the certificate request
    \param rsaKey pointer to an RsaKey structure containing the rsa key used
    to generate the certificate request
    \param eccKey pointer to an EccKey structure containing the ecc key used
    to generate the certificate request

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    EccKey key;
    //initialize key;
    byte* derCert = (byte*)malloc(FOURK_BUF);

    word32 certSz;
    certSz = wc_MakeCertReq(&myCert, derCert, FOURK_BUF, NULL, &key);
    \endcode

    \sa wc_InitCert
    \sa wc_MakeCert
*/
int  wc_MakeCertReq(Cert* cert, byte* derBuffer, word32 derSz,
                                    RsaKey* rsaKey, ecc_key* eccKey);

/*!
    \ingroup ASN

    \brief This function signs buffer and adds the signature to the end of
    buffer. It takes in a signature type. Must be called after wc_MakeCert()
    or wc_MakeCertReq() if creating a CA signed cert.

    \return Success On successfully signing the certificate, returns the new
    size of the cert (including signature).
    \return MEMORY_E Returned if there is an error allocating
    memory with XMALLOC
    \return BUFFER_E Returned if the provided buffer is too small to store
    the generated certificate
    \return Other Additional error messages may be returned if the cert
    generation is not successful.

    \param requestSz the size of the certificate body we’re requesting
    to have signed
    \param sType Type of signature to create. Valid options are: CTC_MD5wRSA,
    CTC_SHAwRSA, CTC_SHAwECDSA, CTC_SHA256wECDSA, and CTC_SHA256wRSA
    \param buffer pointer to the buffer containing the certificate to be
    signed. On success: will hold the newly signed certificate
    \param buffSz the (total) size of the buffer in which to store the newly
    signed certificate
    \param rsaKey pointer to an RsaKey structure containing the rsa key
    to used to sign the certificate
    \param eccKey pointer to an EccKey structure containing the ecc key
    to used to sign the certificate
    \param rng pointer to the random number generator used to sign
    the certificate

    _Example_
    \code
    Cert myCert;
    byte* derCert = (byte*)malloc(FOURK_BUF);
    // initialize myCert, derCert
    RsaKey key;
    // initialize key;
    WC_RNG rng;
    // initialize rng

    word32 certSz;
    certSz = wc_SignCert(myCert.bodySz, myCert.sigType,derCert,FOURK_BUF,
    &key, NULL,
    &rng);
    \endcode

    \sa wc_InitCert
    \sa wc_MakeCert
*/
int  wc_SignCert(int requestSz, int sigType, byte* derBuffer,
                 word32 derSz, RsaKey* rsaKey, ecc_key* eccKey, WC_RNG* rng);

/*!
    \ingroup ASN

    \brief This function is a combination of the previous two functions,
    wc_MakeCert and wc_SignCert for self signing (the previous functions may
    be used for CA requests). It makes a certificate, and then signs it,
    generating a self-signed certificate.

    \return Success On successfully signing the certificate, returns the
    new size of the cert.
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return BUFFER_E Returned if the provided buffer is too small to store
    the generated certificate
    \return Other Additional error messages may be returned if the cert
    generation is not successful.

    \param cert pointer to the cert to make and sign
    \param buffer pointer to the buffer in which to hold the signed certificate
    \param buffSz size of the buffer in which to store the signed certificate
    \param key pointer to an RsaKey structure containing the rsa key to
    used to sign the certificate
    \param rng pointer to the random number generator used to generate
    and sign the certificate

    _Example_
    \code
    Cert myCert;
    byte* derCert = (byte*)malloc(FOURK_BUF);
    // initialize myCert, derCert
    RsaKey key;
    // initialize key;
    WC_RNG rng;
    // initialize rng

    word32 certSz;
    certSz = wc_MakeSelfCert(&myCert, derCert, FOURK_BUF, &key, NULL, &rng);
    \endcode

    \sa wc_InitCert
    \sa wc_MakeCert
    \sa wc_SignCert
*/
int  wc_MakeSelfCert(Cert* cert, byte* derBuffer, word32 derSz, RsaKey* key,
                             WC_RNG* rng);

/*!
    \ingroup ASN

    \brief This function sets the issuer for a certificate to the issuer
    in the provided pem issuerFile. It also changes the certificate’s
    self-signed attribute to false.  The issuer specified in issuerFile is
    verified prior to setting the cert issuer.  This method is used to set
    fields prior to signing.

    \return 0 Returned on successfully setting the issuer for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the
    cert header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in
    the encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify
    the certificate’s authenticity

    \param cert pointer to the cert for which to set the issuer
    \param issuerFile path of the file containing the pem formatted certificate

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    if(wc_SetIssuer(&myCert, ”./path/to/ca-cert.pem”) != 0) {
    	// error setting issuer
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetSubject
    \sa wc_SetIssuerBuffer
*/
int  wc_SetIssuer(Cert* cert, const char* issuerFile);

/*!
    \ingroup ASN

    \brief This function sets the subject for a certificate to the subject
    in the provided pem subjectFile.  This method is used to set fields prior
    to signing.

    \return 0 Returned on successfully setting the issuer for the certificate
    \return MEMORY_E Returned if there is an error allocating memory with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the issuer
    \param subjectFile path of the file containing the pem formatted certificate

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    if(wc_SetSubject(&myCert, ”./path/to/ca-cert.pem”) != 0) {
    	// error setting subject
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetIssuer
*/
int  wc_SetSubject(Cert* cert, const char* subjectFile);


/*!
    \ingroup ASN

    \brief This function sets the raw subject for a certificate from the
    subject in the provided der buffer. This method is used to set the raw
    subject field prior to signing.

    \return 0 Returned on successfully setting the subject for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the raw subject
    \param der pointer to the buffer containing the der formatted certificate
    from which to grab the subject
    \param derSz size of the buffer containing the der formatted certificate
    from which to grab the subject

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    byte* der;
    der = (byte*)malloc(FOURK_BUF);
    // initialize der
    if(wc_SetSubjectRaw(&myCert, der, FOURK_BUF) != 0) {
        // error setting subject
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetSubject
*/
int  wc_SetSubjectRaw(Cert* cert, const byte* der, int derSz);

/*!
    \ingroup ASN

    \brief This function gets the raw subject from the certificate structure.

    \return 0 Returned on successfully getting the subject from the certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension

    \param subjectRaw pointer-pointer to the raw subject upon successful return
    \param cert pointer to the cert from which to get the raw subject

    _Example_
    \code
    Cert myCert;
    byte *subjRaw;
    // initialize myCert

    if(wc_GetSubjectRaw(&subjRaw, &myCert) != 0) {
        // error setting subject
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetSubjectRaw
*/
int  wc_GetSubjectRaw(byte **subjectRaw, Cert *cert);

/*!
    \ingroup ASN

    \brief This function sets the alternate names for a certificate to the
    alternate names in the provided pem file. This is useful in the case that
    one wishes to secure multiple domains with the same certificate. This
    method is used to set fields prior to signing.

    \return 0 Returned on successfully setting the alt names for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the alt names
    \param file path of the file containing the pem formatted certificate

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    if(wc_SetSubject(&myCert, ”./path/to/ca-cert.pem”) != 0) {
    	// error setting alt names
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetIssuer
*/
int  wc_SetAltNames(Cert* cert, const char* file);

/*!
    \ingroup ASN

    \brief This function sets the issuer for a certificate from the issuer in
    the provided der buffer. It also changes the certificate’s self-signed
    attribute to false.  This method is used to set fields prior to signing.

    \return 0 Returned on successfully setting the issuer for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC
    key from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify
    the certificate’s authenticity

    \param cert pointer to the cert for which to set the issuer
    \param der pointer to the buffer containing the der formatted certificate
    from which to grab the issuer
    \param derSz size of the buffer containing the der formatted certificate
    from which to grab the issuer

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    byte* der;
    der = (byte*)malloc(FOURK_BUF);
    // initialize der
    if(wc_SetIssuerBuffer(&myCert, der, FOURK_BUF) != 0) {
	    // error setting issuer
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetIssuer
*/
int  wc_SetIssuerBuffer(Cert* cert, const byte* der, int derSz);

/*!
    \ingroup ASN

    \brief This function sets the raw issuer for a certificate from the
    issuer in the provided der buffer. This method is used to set the raw
    issuer field prior to signing.

    \return 0 Returned on successfully setting the issuer for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the raw issuer
    \param der pointer to the buffer containing the der formatted certificate
    from which to grab the subject
    \param derSz size of the buffer containing the der formatted certificate
    from which to grab the subject

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    byte* der;
    der = (byte*)malloc(FOURK_BUF);
    // initialize der
    if(wc_SetIssuerRaw(&myCert, der, FOURK_BUF) != 0) {
        // error setting subject
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetIssuer
*/
int  wc_SetIssuerRaw(Cert* cert, const byte* der, int derSz);

/*!
    \ingroup ASN

    \brief This function sets the subject for a certificate from the subject in
    the provided der buffer. This method is used to set fields prior to signing.

    \return 0 Returned on successfully setting the subject for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the subject
    \param der pointer to the buffer containing the der formatted certificate
    from which to grab the subject
    \param derSz size of the buffer containing the der formatted certificate
    from which to grab the subject

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    byte* der;
    der = (byte*)malloc(FOURK_BUF);
    // initialize der
    if(wc_SetSubjectBuffer(&myCert, der, FOURK_BUF) != 0) {
    	// error setting subject
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetSubject
*/
int  wc_SetSubjectBuffer(Cert* cert, const byte* der, int derSz);

/*!
    \ingroup ASN

    \brief This function sets the alternate names for a certificate from the
    alternate names in the provided der buffer. This is useful in the case that
    one wishes to secure multiple domains with the same certificate. This
    method is used to set fields prior to signing.

    \return 0 Returned on successfully setting the alternate names for the
    certificate
    \return MEMORY_E Returned if there is an error allocating memory with
    XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the
    certificate start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not the
    same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the alternate names
    \param der pointer to the buffer containing the der formatted certificate
    from which to grab the alternate names
    \param derSz size of the buffer containing the der formatted certificate
    from which to grab the alternate names

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    byte* der;
    der = (byte*)malloc(FOURK_BUF);
    // initialize der
    if(wc_SetAltNamesBuffer(&myCert, der, FOURK_BUF) != 0) {
    	// error setting subject
    }
    \endcode

    \sa wc_InitCert
    \sa wc_SetAltNames
*/
int  wc_SetAltNamesBuffer(Cert* cert, const byte* der, int derSz);

/*!
    \ingroup ASN

    \brief This function sets the dates for a certificate from the date range
    in the provided der buffer. This method is used to set fields prior
    to signing.

    \return 0 Returned on successfully setting the dates for the certificate
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_PARSE_E Returned if there is an error parsing the cert
    header file
    \return ASN_OBJECT_ID_E Returned if there is an error parsing the
    encryption type from the cert
    \return ASN_EXPECT_0_E Returned if there is a formatting error in the
    encryption specification of the cert file
    \return ASN_BEFORE_DATE_E Returned if the date is before the certificate
    start date
    \return ASN_AFTER_DATE_E Returned if the date is after the certificate
    expiration date
    \return ASN_BITSTR_E Returned if there is an error parsing a bit string
    from the certificate
    \return ECC_CURVE_OID_E Returned if there is an error parsing the ECC key
    from the certificate
    \return ASN_UNKNOWN_OID_E Returned if the certificate is using an unknown
    key object id
    \return ASN_VERSION_E Returned if the ALLOW_V1_EXTENSIONS option is not
    defined and the certificate is a V1 or V2 certificate
    \return BAD_FUNC_ARG Returned if there is an error processing the
    certificate extension
    \return ASN_CRIT_EXT_E Returned if an unfamiliar critical extension is
    encountered in processing the certificate
    \return ASN_SIG_OID_E Returned if the signature encryption type is not
    the same as the encryption type of the certificate in the provided file
    \return ASN_SIG_CONFIRM_E Returned if confirming the certification
    signature fails
    \return ASN_NAME_INVALID_E Returned if the certificate’s name is not
    permitted by the CA name constraints
    \return ASN_NO_SIGNER_E Returned if there is no CA signer to verify the
    certificate’s authenticity

    \param cert pointer to the cert for which to set the dates
    \param der pointer to the buffer containing the der formatted certificate
    from which to grab the date range
    \param derSz size of the buffer containing the der formatted certificate
    from which to grab the date range

    _Example_
    \code
    Cert myCert;
    // initialize myCert
    byte* der;
    der = (byte*)malloc(FOURK_BUF);
    // initialize der
    if(wc_SetDatesBuffer(&myCert, der, FOURK_BUF) != 0) {
    	// error setting subject
    }
    \endcode

    \sa wc_InitCert
*/
int  wc_SetDatesBuffer(Cert* cert, const byte* der, int derSz);

/*!
    \ingroup ASN

    \brief Set AKID from either an RSA or ECC public key. note: Only set one of
    rsakey or eckey, not both.

    \return 0 Success
    \return BAD_FUNC_ARG Either cert is null or both rsakey and eckey are null.
    \return MEMORY_E Error allocating memory.
    \return PUBLIC_KEY_E Error writing to the key.

    \param cert Pointer to the certificate to set the SKID.
    \param rsakey Pointer to the RsaKey struct to read from.
    \param eckey Pointer to the ecc_key to read from.

    _Example_
    \code
    Cert myCert;
    RsaKey keypub;

    wc_InitRsaKey(&keypub, 0);

    if (wc_SetAuthKeyIdFromPublicKey(&myCert, &keypub, NULL) != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_SetSubjectKeyId
    \sa wc_SetAuthKeyId
    \sa wc_SetAuthKeyIdFromCert
*/
int wc_SetAuthKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey,
                                             ecc_key *eckey);

/*!
    \ingroup ASN

    \brief Set AKID from from DER encoded certificate.

    \return 0 Success
    \return BAD_FUNC_ARG Error if any argument is null or derSz is less than 0.
    \return MEMORY_E Error if problem allocating memory.
    \return ASN_NO_SKID No subject key ID found.

    \param cert The Cert struct to write to.
    \param der The DER encoded certificate buffer.
    \param derSz Size of der in bytes.

    _Example_
    \code
    Cert some_cert;
    byte some_der[] = { // Initialize a DER buffer };
    wc_InitCert(&some_cert);
    if(wc_SetAuthKeyIdFromCert(&some_cert, some_der, sizeof(some_der) != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_SetAuthKeyIdFromPublicKey
    \sa wc_SetAuthKeyId
*/
int wc_SetAuthKeyIdFromCert(Cert *cert, const byte *der, int derSz);

/*!
    \ingroup ASN

    \brief Set AKID from certificate file in PEM format.

    \return 0 Success
    \return BAD_FUNC_ARG Error if cert or file is null.
    \return MEMORY_E Error if problem allocating memory.

    \param cert Cert struct you want to set the AKID of.
    \param file Buffer containing PEM cert file.

    _Example_
    \code
    char* file_name = "/path/to/file";
    cert some_cert;
    wc_InitCert(&some_cert);

    if(wc_SetAuthKeyId(&some_cert, file_name) != 0)
    {
        // Handle Error
    }
    \endcode

    \sa wc_SetAuthKeyIdFromPublicKey
    \sa wc_SetAuthKeyIdFromCert
*/
int wc_SetAuthKeyId(Cert *cert, const char* file);

/*!
    \ingroup ASN

    \brief Set SKID from RSA or ECC public key.

    \return 0 Success
    \return BAD_FUNC_ARG Returned if cert or rsakey and eckey are null.
    \return MEMORY_E Returned if there is an error allocating memory.
    \return PUBLIC_KEY_E Returned if there is an error getting the public key.

    \param cert Pointer to a Cert structure to be used.
    \param rsakey Pointer to an RsaKey structure
    \param eckey Pointer to an ecc_key structure

    _Example_
    \code
    Cert some_cert;
    RsaKey some_key;
    wc_InitCert(&some_cert);
    wc_InitRsaKey(&some_key);

    if(wc_SetSubjectKeyIdFromPublicKey(&some_cert,&some_key, NULL) != 0)
    {
        // Handle Error
    }
    \endcode

    \sa wc_SetSubjectKeyId
*/
int wc_SetSubjectKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey,
                                                ecc_key *eckey);

/*!
    \ingroup ASN

    \brief Set SKID from public key file in PEM format.  Both arguments
    are required.

    \return 0 Success
    \return BAD_FUNC_ARG Returns if cert or file is null.
    \return MEMORY_E Returns if there is a problem allocating memory for key.
    \return PUBLIC_KEY_E Returns if there is an error decoding the public key.

    \param cert Cert structure to set the SKID of.
    \param file Contains the PEM encoded file.

    _Example_
    \code
    const char* file_name = "path/to/file";
    Cert some_cert;
    wc_InitCert(&some_cert);

    if(wc_SetSubjectKeyId(&some_cert, file_name) != 0)
    {
        // Handle Error
    }
    \endcode

    \sa wc_SetSubjectKeyIdFromPublicKey
*/
int wc_SetSubjectKeyId(Cert *cert, const char* file);

/*!
    \ingroup RSA

    \brief This function allows you to set the key usage using a comma
    delimited string of tokens. Accepted tokens are: digitalSignature,
    nonRepudiation, contentCommitment, keyCertSign, cRLSign, dataEncipherment,
    keyAgreement, keyEncipherment, encipherOnly, decipherOnly. Example:
    "digitalSignature,nonRepudiation" nonRepudiation and contentCommitment
    are for the same usage.

    \return 0 Success
    \return BAD_FUNC_ARG Returned when either arg is null.
    \return MEMORY_E Returned when there is an error allocating memory.
    \return KEYUSAGE_E Returned if an unrecognized token is entered.

    \param cert Pointer to initialized Cert structure.
    \param value Comma delimited string of tokens to set usage.

    _Example_
    \code
    Cert cert;
    wc_InitCert(&cert);

    if(wc_SetKeyUsage(&cert, "cRLSign,keyCertSign") != 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_InitCert
    \sa wc_MakeRsaKey
*/
int wc_SetKeyUsage(Cert *cert, const char *value);

/*!
    \ingroup ASN

    \brief Loads a PEM key from a file and converts to a DER encoded buffer.

    \return 0 Success
    \return <0 Error
    \return SSL_BAD_FILE There is a problem with opening the file.
    \return MEMORY_E There is an error allocating memory for the file buffer.
    \return BUFFER_E derBuf is not large enough to hold the converted key.

    \param fileName Name of the file to load.
    \param derBuf Buffer for DER encoded key.
    \param derSz Size of DER buffer.

    _Example_
    \code
    char* some_file = "filename";
    unsigned char der[];

    if(wc_PemPubKeyToDer(some_file, der, sizeof(der)) != 0)
    {
        //Handle Error
    }
    \endcode

    \sa wc_PubKeyPemToDer
*/
int wc_PemPubKeyToDer(const char* fileName,
                                       unsigned char* derBuf, int derSz);

/*!
    \ingroup ASN

    \brief Convert a PEM encoded public key to DER.  Returns the number of
    bytes written to the buffer or a negative value for an error.

    \return >0 Success, number of bytes written.
    \return BAD_FUNC_ARG Returns if pem, buff, or buffSz are null
    \return <0 An error occurred in the function.

    \param pem PEM encoded key
    \param pemSz Size of pem
    \param buff Pointer to buffer for output.
    \param buffSz Size of buffer.

    _Example_
    \code
    byte some_pem[] = { Initialize with PEM key }
    unsigned char out_buffer[1024]; // Ensure buffer is large enough to fit DER

    if(wc_PubKeyPemToDer(some_pem, sizeof(some_pem), out_buffer,
    sizeof(out_buffer)) < 0)
    {
        // Handle error
    }
    \endcode

    \sa wc_PemPubKeyToDer
*/
int wc_PubKeyPemToDer(const unsigned char* pem, int pemSz,
                                      unsigned char* buff, int buffSz);

/*!
    \ingroup ASN

    \brief This function converts a pem certificate to a der certificate,
    and places the resulting certificate in the derBuf buffer provided.

    \return Success On success returns the size of the derBuf generated
    \return BUFFER_E Returned if the size of derBuf is too small to hold
    the certificate generated
    \return MEMORY_E Returned if the call to XMALLOC fails

    \param fileName path to the file containing a pem certificate to
    convert to a der certificate
    \param derBuf pointer to a char buffer in which to store the
    converted certificate
    \param derSz size of the char buffer in which to store the
    converted certificate

    _Example_
    \code
    char * file = “./certs/client-cert.pem”;
    int derSz;
    byte* der = (byte*)XMALLOC((8*1024), NULL, DYNAMIC_TYPE_CERT);

    derSz = wc_PemCertToDer(file, der, (8*1024));
    if (derSz <= 0) {
        //PemCertToDer error
    }
    \endcode

    \sa none
*/

int wc_PemCertToDer(const char* fileName, unsigned char* derBuf, int derSz);

/*!
    \ingroup ASN

    \brief This function converts a der formatted input certificate, contained
    in the der buffer, into a pem formatted output certificate, contained in
    the output buffer. It should be noted that this is not an in place
    conversion, and a separate buffer must be utilized to store the pem
    formatted output.

    \return Success On successfully making a pem certificate from the input
    der cert, returns the size of the pem cert generated.
    \return BAD_FUNC_ARG Returned if there is an error parsing the der file
    and storing it as a pem file
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_INPUT_E Returned in the case of a base64 encoding error
    \return BUFFER_E May be returned if the output buffer is too small to
    store the pem formatted certificate

    \param der pointer to the buffer of the certificate to convert
    \param derSz size of the the certificate to convert
    \param output pointer to the buffer in which to store the pem
    formatted certificate
    \param outSz size of the buffer in which to store the pem formatted
    certificate
    \param type the type of certificate to generate. Valid types are:
    CERT_TYPE, PRIVATEKEY_TYPE, ECC_PRIVATEKEY_TYPE, and CERTREQ_TYPE.

    _Example_
    \code
    byte* der;
    // initialize der with certificate
    byte* pemFormatted[FOURK_BUF];

    word32 pemSz;
    pemSz = wc_DerToPem(der, derSz,pemFormatted,FOURK_BUF, CERT_TYPE);
    \endcode

    \sa wc_PemCertToDer
*/
int wc_DerToPem(const byte* der, word32 derSz, byte* output,
                                word32 outputSz, int type);

/*!
    \ingroup ASN

    \brief This function converts a der formatted input certificate,
    contained in the der buffer, into a pem formatted output certificate,
    contained in the output buffer. It should be noted that this is not an
    in place conversion, and a separate buffer must be utilized to store the
    pem formatted output.  Allows setting cipher info.

    \return Success On successfully making a pem certificate from the input
    der cert, returns the size of the pem cert generated.
    \return BAD_FUNC_ARG Returned if there is an error parsing the der file
    and storing it as a pem file
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return ASN_INPUT_E Returned in the case of a base64 encoding error
    \return BUFFER_E May be returned if the output buffer is too small to
    store the pem formatted certificate

    \param der pointer to the buffer of the certificate to convert
    \param derSz size of the the certificate to convert
    \param output pointer to the buffer in which to store the pem
    formatted certificate
    \param outSz size of the buffer in which to store the pem formatted
    certificate
    \param cipher_inf Additional cipher information.
    \param type the type of certificate to generate. Valid types are:
    CERT_TYPE, PRIVATEKEY_TYPE, ECC_PRIVATEKEY_TYPE, and CERTREQ_TYPE.

    _Example_
    \code
    byte* der;
    // initialize der with certificate
    byte* pemFormatted[FOURK_BUF];

    word32 pemSz;
    byte* cipher_info[] { Additional cipher info. }
    pemSz = wc_DerToPemEx(der, derSz,pemFormatted,FOURK_BUF, ,CERT_TYPE);
    \endcode

    \sa wc_PemCertToDer
*/
int wc_DerToPemEx(const byte* der, word32 derSz, byte* output,
                                word32 outputSz, byte *cipherIno, int type);

/*!
    \ingroup CertsKeys

    \brief Converts a key in PEM format to DER format.

    \return int the function returns the number of bytes written to
    the buffer on successful execution.
    \return int negative int returned indicating an error.

    \param pem a pointer to the PEM encoded certificate.
    \param pemSz the size of the PEM buffer (pem)
    \param buff a pointer to the copy of the buffer member of the
    DerBuffer struct.
    \param buffSz size of the buffer space allocated in the DerBuffer struct.
    \param pass password passed into the function.

    _Example_
    \code
    byte* loadBuf;
    long fileSz = 0;
    byte* bufSz;
    static int LoadKeyFile(byte** keyBuf, word32* keyBufSz,
    const char* keyFile,
                    int typeKey, const char* password);
    …
    bufSz = wc_KeyPemToDer(loadBuf, (int)fileSz, saveBuf,
    (int)fileSz, password);

    if(saveBufSz > 0){
        // Bytes were written to the buffer.
    }
    \endcode

    \sa wc_PemToDer
*/
int wc_KeyPemToDer(const unsigned char* pem, int pemSz,
                                    unsigned char* buff, int buffSz, const char* pass);

/*!
    \ingroup CertsKeys

    \brief This function converts a PEM formatted certificate to DER
    format. Calls OpenSSL function PemToDer.

    \return buffer returns the bytes written to the buffer.

    \param pem pointer PEM formatted certificate.
    \param pemSz size of the certificate.
    \param buff buffer to be copied to DER format.
    \param buffSz size of the buffer.
    \param type Certificate file type found in asn_public.h enum CertType.

    _Example_
    \code
    const unsigned char* pem;
    int pemSz;
    unsigned char buff[BUFSIZE];
    int buffSz = sizeof(buff)/sizeof(char);
    int type;
    ...
    if(wc_CertPemToDer(pem, pemSz, buff, buffSz, type) <= 0) {
        // There were bytes written to buffer
    }
    \endcode

    \sa wc_PemToDer
*/
int wc_CertPemToDer(const unsigned char* pem, int pemSz,
                    unsigned char* buff, int buffSz, int type);

/*!
    \ingroup CertsKeys

    \brief This function gets the public key in DER format from a populated
    DecodedCert struct. Users must call wc_InitDecodedCert() and wc_ParseCert()
    before calling this API. wc_InitDecodedCert() accepts a DER/ASN.1 encoded
    certificate. To convert a PEM cert to DER, first use wc_CertPemToDer()
    before calling wc_InitDecodedCert().

    \return 0 on success, negative on error. LENGTH_ONLY_E if derKey is NULL
    and returning length only.

    \param cert populated DecodedCert struct holding X.509 certificate
    \param derKey output buffer to place DER encoded public key
    \param derKeySz [IN/OUT] size of derKey buffer on input, size of public key
    on return. If derKey is passed in as NULL, derKeySz will be set to required
    buffer size for public key and LENGTH_ONLY_E will be returned from function.

    \sa wc_GetPubKeyDerFromCert
*/
int wc_GetPubKeyDerFromCert(struct DecodedCert* cert,
                                        byte* derKey, word32* derKeySz);

/*!
    \ingroup ASN

    \brief This function reads in an ECC private key from the input buffer,
    input, parses the private key, and uses it to generate an ecc_key object,
    which it stores in key.

    \return 0 On successfully decoding the private key and storing the result
    in the ecc_key struct
    \return ASN_PARSE_E: Returned if there is an error parsing the der file
    and storing it as a pem file
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return BUFFER_E Returned if the certificate to convert is large than
    the specified max certificate size
    \return ASN_OBJECT_ID_E Returned if the certificate encoding has an
    invalid object id
    \return ECC_CURVE_OID_E Returned if the ECC curve of the provided key is
    not supported
    \return ECC_BAD_ARG_E Returned if there is an error in the ECC key format
    \return NOT_COMPILED_IN Returned if the private key is compressed, and no
    compression key is provided
    \return MP_MEM Returned if there is an error in the math library used
    while parsing the private key
    \return MP_VAL Returned if there is an error in the math library used
    while parsing the private key
    \return MP_RANGE Returned if there is an error in the math library used
    while parsing the private key

    \param input pointer to the buffer containing the input private key
    \param inOutIdx pointer to a word32 object containing the index in
    the buffer at which to start
    \param key pointer to an initialized ecc object, on which to store
    the decoded private key
    \param inSz size of the input buffer containing the private key

    _Example_
    \code
    int ret, idx=0;
    ecc_key key; // to store key in

    byte* tmp; // tmp buffer to read key from
    tmp = (byte*) malloc(FOURK_BUF);

    int inSz;
    inSz = fread(tmp, 1, FOURK_BUF, privateKeyFile);
    // read key into tmp buffer

    wc_ecc_init(&key); // initialize key
    ret = wc_EccPrivateKeyDecode(tmp, &idx, &key, (word32)inSz);
    if(ret < 0) {
        // error decoding ecc key
    }
    \endcode

    \sa wc_RSA_PrivateKeyDecode
*/
int wc_EccPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                           ecc_key* key, word32 inSz);

/*!
    \ingroup ASN

    \brief This function writes a private ECC key to der format.

    \return Success On successfully writing the ECC key to der format,
    returns the length written to the buffer
    \return BAD_FUNC_ARG Returned if key or output is null, or inLen equals zero
    \return MEMORY_E Returned if there is an error allocating memory
    with XMALLOC
    \return BUFFER_E Returned if the converted certificate is too large
    to store in the output buffer
    \return ASN_UNKNOWN_OID_E Returned if the ECC key used is of an
    unknown type
    \return MP_MEM Returned if there is an error in the math library used
    while parsing the private key
    \return MP_VAL Returned if there is an error in the math library used
    while parsing the private key
    \return MP_RANGE Returned if there is an error in the math library used
    while parsing the private key

    \param key pointer to the buffer containing the input ecc key
    \param output pointer to a buffer in which to store the der formatted key
    \param inLen the length of the buffer in which to store the
    der formatted key

    _Example_
    \code
    int derSz;
    ecc_key key;
    // initialize and make key
    byte der[FOURK_BUF];
    // store der formatted key here

    derSz = wc_EccKeyToDer(&key, der, FOURK_BUF);
    if(derSz < 0) {
        // error converting ecc key to der buffer
    }
    \endcode

    \sa wc_RsaKeyToDer
*/
int wc_EccKeyToDer(ecc_key* key, byte* output, word32 inLen);

/*!
    \ingroup ASN

    \brief Decodes an ECC public key from an input buffer.  It will parse an
    ASN sequence to retrieve the ECC key.

    \return 0 Success
    \return BAD_FUNC_ARG Returns if any arguments are null.
    \return ASN_PARSE_E Returns if there is an error parsing
    \return ASN_ECC_KEY_E Returns if there is an error importing the key.
    See wc_ecc_import_x963 for possible reasons.

    \param input Buffer containing DER encoded key to decode.
    \param inOutIdx Index to start reading input buffer from.  On output,
    index is set to last position parsed of input buffer.
    \param key Pointer to ecc_key struct to store the public key.
    \param inSz Size of the input buffer.

    _Example_
    \code
    int ret;
    word32 idx = 0;
    byte buff[] = { // initialize with key };
    ecc_key pubKey;
    wc_ecc_init(&pubKey);
    if ( wc_EccPublicKeyDecode(buff, &idx, &pubKey, sizeof(buff)) != 0) {
            // error decoding key
    }
    \endcode

    \sa wc_ecc_import_x963
*/
int wc_EccPublicKeyDecode(const byte* input, word32* inOutIdx,
                          ecc_key* key, word32 inSz);

/*!
    \ingroup ASN

    \brief This function converts the ECC public key to DER format. It
    returns the size of buffer used. The public ECC key in DER format is stored
    in output buffer. The with_AlgCurve flag will include a header that
    has the Algorithm and Curve information

    \return >0 Success, size of buffer used
    \return BAD_FUNC_ARG Returned if output or key is null.
    \return LENGTH_ONLY_E Error in getting ECC public key size.
    \return BUFFER_E Returned when output buffer is too small.

    \param key Pointer to ECC key
    \param output Pointer to output buffer to write to.
    \param inLen Size of buffer.
    \param with_AlgCurve a flag for when to include a header that has the
    Algorithm and Curve information.

    _Example_
    \code
    ecc_key key;
    wc_ecc_init(&key);
    WC_RNG rng;
    wc_InitRng(&rng);
    wc_ecc_make_key(&rng, 32, &key);
    int derSz = // Some appropriate size for der;
    byte der[derSz];

    if(wc_EccPublicKeyToDer(&key, der, derSz, 1) < 0)
    {
        // Error converting ECC public key to der
    }
    \endcode

    \sa wc_EccKeyToDer
    \sa wc_EccPrivateKeyDecode
*/
int wc_EccPublicKeyToDer(ecc_key* key, byte* output,
                                         word32 inLen, int with_AlgCurve);

/*!
    \ingroup ASN

    \brief This function converts the ECC public key to DER format. It
    returns the size of buffer used. The public ECC key in DER format is stored
    in output buffer. The with_AlgCurve flag will include a header that
    has the Algorithm and Curve information. The comp parameter determines if
    the public key will be exported as compressed.

    \return >0 Success, size of buffer used
    \return BAD_FUNC_ARG Returned if output or key is null.
    \return LENGTH_ONLY_E Error in getting ECC public key size.
    \return BUFFER_E Returned when output buffer is too small.

    \param key Pointer to ECC key
    \param output Pointer to output buffer to write to.
    \param inLen Size of buffer.
    \param with_AlgCurve a flag for when to include a header that has the
    Algorithm and Curve information.
    \param comp If 1 (non-zero) the ECC public key will be written in
    compressed form. If 0 it will be written in an uncompressed format.

    _Example_
    \code
    ecc_key key;
    wc_ecc_init(&key);
    WC_RNG rng;
    wc_InitRng(&rng);
    wc_ecc_make_key(&rng, 32, &key);
    int derSz = // Some appropriate size for der;
    byte der[derSz];

    // Write out a compressed ECC key
    if(wc_EccPublicKeyToDer_ex(&key, der, derSz, 1, 1) < 0)
    {
        // Error converting ECC public key to der
    }
    \endcode

    \sa wc_EccKeyToDer
    \sa wc_EccPublicKeyDecode
*/
int wc_EccPublicKeyToDer_ex(ecc_key* key, byte* output,
                                     word32 inLen, int with_AlgCurve, int comp);

/*!
    \ingroup ASN

    \brief This function encodes a digital signature into the output buffer,
    and returns the size of the encoded signature created.

    \return Success On successfully writing the encoded signature to output,
    returns the length written to the buffer

    \param out pointer to the buffer where the encoded signature will be written
    \param digest pointer to the digest to use to encode the signature
    \param digSz the length of the buffer containing the digest
    \param hashOID OID identifying the hash type used to generate the
    signature. Valid options, depending on build configurations, are: SHAh,
    SHA256h, SHA384h, SHA512h, MD2h, MD5h, DESb, DES3b, CTC_MD5wRSA,
    CTC_SHAwRSA, CTC_SHA256wRSA, CTC_SHA384wRSA, CTC_SHA512wRSA, CTC_SHAwECDSA,
    CTC_SHA256wECDSA, CTC_SHA384wECDSA, and CTC_SHA512wECDSA.

    \endcode
    \code
    int signSz;
    byte encodedSig[MAX_ENCODED_SIG_SZ];
    Sha256 sha256;
    // initialize sha256 for hashing

    byte* dig = = (byte*)malloc(WC_SHA256_DIGEST_SIZE);
    // perform hashing and hash updating so dig stores SHA-256 hash
    // (see wc_InitSha256, wc_Sha256Update and wc_Sha256Final)
    signSz = wc_EncodeSignature(encodedSig, dig, WC_SHA256_DIGEST_SIZE, SHA256h);
    \endcode

    \sa none
*/
word32 wc_EncodeSignature(byte* out, const byte* digest,
                                      word32 digSz, int hashOID);

/*!
    \ingroup ASN

    \brief This function returns the hash OID that corresponds to a hashing
    type. For example, when given the type: WC_SHA512, this function returns the
    identifier corresponding to a SHA512 hash, SHA512h.

    \return Success On success, returns the OID corresponding to the
    appropriate hash to use with that encryption type.
    \return 0 Returned if an unrecognized hash type is passed in as argument.

    \param type the hash type for which to find the OID. Valid options,
    depending on build configuration, include: WC_MD5, WC_SHA, WC_SHA256,
    WC_SHA384, WC_SHA512, WC_SHA3_224, WC_SHA3_256, WC_SHA3_384 or WC_SHA3_512

    _Example_
    \code
    int hashOID;

    hashOID = wc_GetCTC_HashOID(WC_SHA512);
    if (hashOID == 0) {
	    // WOLFSSL_SHA512 not defined
    }
    \endcode

    \sa none
*/
int wc_GetCTC_HashOID(int type);

/*!
    \ingroup ASN

    \brief This function cleans up memory and resources used by the certificate
     structure's decoded cert cache. When WOLFSSL_CERT_GEN_CACHE is defined the
     decoded cert structure is cached in the certificate structure. This allows
     subsequent calls to certificate set functions to avoid parsing the decoded
     cert on each call.

    \return 0 on success.
    \return BAD_FUNC_ARG Returned if invalid pointer is passed in as argument.

    \param cert pointer to an uninitialized certificate information structure.

    _Example_
    \code
    Cert cert; // Initialized certificate structure

    wc_SetCert_Free(&cert);
    \endcode

    \sa wc_SetAuthKeyIdFromCert
    \sa wc_SetIssuerBuffer
    \sa wc_SetSubjectBuffer
    \sa wc_SetSubjectRaw
    \sa wc_SetIssuerRaw
    \sa wc_SetAltNamesBuffer
    \sa wc_SetDatesBuffer
*/
void wc_SetCert_Free(Cert* cert);

/*!
    \ingroup ASN

    \brief This function finds the beginning of the traditional private key
     inside a PKCS#8 unencrypted buffer.

    \return Length of traditional private key on success.
    \return Negative values on failure.

    \param input Buffer containing unencrypted PKCS#8 private key. 
    \param inOutIdx Index into the input buffer. On input, it should be a byte
    offset to the beginning of the the PKCS#8 buffer. On output, it will be the
    byte offset to the traditional private key within the input buffer.
    \param sz The number of bytes in the input buffer.

    _Example_
    \code
    byte* pkcs8Buf; // Buffer containing PKCS#8 key.
    word32 idx = 0;
    word32 sz; // Size of pkcs8Buf.
    ...
    ret = wc_GetPkcs8TraditionalOffset(pkcs8Buf, &idx, sz);
    // pkcs8Buf + idx is now the beginning of the traditional private key bytes.
    \endcode

    \sa wc_CreatePKCS8Key
    \sa wc_EncryptPKCS8Key
    \sa wc_DecryptPKCS8Key
    \sa wc_CreateEncryptedPKCS8Key
*/
int wc_GetPkcs8TraditionalOffset(byte* input,
                                             word32* inOutIdx, word32 sz);

/*!
    \ingroup ASN

    \brief This function takes in a DER private key and converts it to PKCS#8
    format. Also used in creating PKCS#12 shrouded key bags. See RFC 5208.
    
    \return The size of the PKCS#8 key placed into out on success.
    \return LENGTH_ONLY_E if out is NULL, with required output buffer size in
    outSz.
    \return Other negative values on failure.

    \param out Buffer to place result in. If NULL, required out buffer size
    returned in outSz.
    \param outSz Size of out buffer.
    \param key Buffer with traditional DER key.
    \param keySz Size of key buffer.
    \param algoID Algorithm ID (e.g. RSAk).
    \param curveOID ECC curve OID if used. Should be NULL for RSA keys.
    \param oidSz Size of curve OID. Is set to 0 if curveOID is NULL.

    _Example_
    \code
    ecc_key eccKey;              // wolfSSL ECC key object.
    byte* der;                   // DER-encoded ECC key.
    word32 derSize;              // Size of der.
    const byte* curveOid = NULL; // OID of curve used by eccKey.
    word32 curveOidSz = 0;       // Size of curve OID.
    byte* pkcs8;                 // Output buffer for PKCS#8 key.
    word32 pkcs8Sz;              // Size of output buffer.

    derSize = wc_EccKeyDerSize(&eccKey, 1);
    ...
    derSize = wc_EccKeyToDer(&eccKey, der, derSize);
    ...
    ret = wc_ecc_get_oid(eccKey.dp->oidSum, &curveOid, &curveOidSz);
    ...
    ret = wc_CreatePKCS8Key(NULL, &pkcs8Sz, der,
        derSize, ECDSAk, curveOid, curveOidSz); // Get size needed in pkcs8Sz.
    ...
    ret = wc_CreatePKCS8Key(pkcs8, &pkcs8Sz, der,
        derSize, ECDSAk, curveOid, curveOidSz);
    \endcode

    \sa wc_GetPkcs8TraditionalOffset
    \sa wc_EncryptPKCS8Key
    \sa wc_DecryptPKCS8Key
    \sa wc_CreateEncryptedPKCS8Key
*/
int wc_CreatePKCS8Key(byte* out, word32* outSz,
        byte* key, word32 keySz, int algoID, const byte* curveOID,
        word32 oidSz);

/*!
    \ingroup ASN

    \brief This function takes in an unencrypted PKCS#8 DER key (e.g. one
     created by wc_CreatePKCS8Key) and converts it to PKCS#8 encrypted format.
     The resulting encrypted key can be decrypted using wc_DecryptPKCS8Key. See
     RFC 5208.

    \return The size of the encrypted key placed in out on success.
    \return LENGTH_ONLY_E if out is NULL, with required output buffer size in
    outSz.
    \return Other negative values on failure.

    \param key Buffer with traditional DER key.
    \param keySz Size of key buffer.
    \param out Buffer to place result in. If NULL, required out buffer size
    returned in outSz.
    \param outSz Size of out buffer.
    \param password The password to use for the password-based encryption
    algorithm.
    \param passwordSz The length of the password (not including the NULL
    terminator).
    \param vPKCS The PKCS version to use. Can be 1 for PKCS12 or PKCS5.
    \param pbeOid The OID of the PBE scheme to use (e.g. PBES2 or one of the
    OIDs for PBES1 in RFC 2898 A.3).
    \param encAlgId The encryption algorithm ID to use (e.g. AES256CBCb).
    \param salt The salt buffer to use. If NULL, a random salt will be used.
    \param saltSz The length of the salt buffer. Can be 0 if passing NULL for
    salt.
    \param itt The number of iterations to use for the KDF.
    \param rng A pointer to an initialized WC_RNG object.
    \param heap A pointer to the heap used for dynamic allocation. Can be NULL.

    _Example_
    \code
    byte* pkcs8;          // Unencrypted PKCS#8 key.
    word32 pkcs8Sz;       // Size of pkcs8.
    byte* pkcs8Enc;       // Encrypted PKCS#8 key.
    word32 pkcs8EncSz;    // Size of pkcs8Enc.
    const char* password; // Password to use for encryption.
    int passwordSz;       // Length of password (not including NULL terminator).
    WC_RNG rng;

    // The following produces an encrypted version of pkcs8 in pkcs8Enc. The
    // encryption uses password-based encryption scheme 2 (PBE2) from PKCS#5 and
    // the AES cipher in CBC mode with a 256-bit key. See RFC 8018 for more on
    // PKCS#5.
    ret = wc_EncryptPKCS8Key(pkcs8, pkcs8Sz, pkcs8Enc, &pkcs8EncSz, password,
            passwordSz, PKCS5, PBES2, AES256CBCb, NULL, 0,
            WC_PKCS12_ITT_DEFAULT, &rng, NULL);
    \endcode

    \sa wc_GetPkcs8TraditionalOffset
    \sa wc_CreatePKCS8Key
    \sa wc_DecryptPKCS8Key
    \sa wc_CreateEncryptedPKCS8Key
*/
int wc_EncryptPKCS8Key(byte* key, word32 keySz, byte* out,
        word32* outSz, const char* password, int passwordSz, int vPKCS,
        int pbeOid, int encAlgId, byte* salt, word32 saltSz, int itt,
        WC_RNG* rng, void* heap);

/*!
    \ingroup ASN

    \brief This function takes an encrypted PKCS#8 DER key and decrypts it to
     PKCS#8 unencrypted DER. Undoes the encryption done by wc_EncryptPKCS8Key.
     See RFC5208. The input buffer is overwritten with the decrypted data.

    \return The length of the decrypted buffer on success.
    \return Negative values on failure.

    \param input On input, buffer containing encrypted PKCS#8 key. On successful
    output, contains the decrypted key.
    \param sz Size of the input buffer.
    \param password The password used to encrypt the key.
    \param passwordSz The length of the password (not including NULL
    terminator).

    _Example_
    \code
    byte* pkcs8Enc;       // Encrypted PKCS#8 key made with wc_EncryptPKCS8Key.
    word32 pkcs8EncSz;    // Size of pkcs8Enc.
    const char* password; // Password to use for decryption.
    int passwordSz;       // Length of password (not including NULL terminator).

    ret = wc_DecryptPKCS8Key(pkcs8Enc, pkcs8EncSz, password, passwordSz);
    \endcode

    \sa wc_GetPkcs8TraditionalOffset
    \sa wc_CreatePKCS8Key
    \sa wc_EncryptPKCS8Key
    \sa wc_CreateEncryptedPKCS8Key
*/
int wc_DecryptPKCS8Key(byte* input, word32 sz, const char* password,
        int passwordSz);

/*!
    \ingroup ASN

    \brief This function takes a traditional, DER key, converts it to PKCS#8
     format, and encrypts it. It uses wc_CreatePKCS8Key and wc_EncryptPKCS8Key
     to do this. 

    \return The size of the encrypted key placed in out on success.
    \return LENGTH_ONLY_E if out is NULL, with required output buffer size in
    outSz.
    \return Other negative values on failure.

    \param key Buffer with traditional DER key.
    \param keySz Size of key buffer.
    \param out Buffer to place result in. If NULL, required out buffer size
    returned in outSz.
    \param outSz Size of out buffer.
    \param password The password to use for the password-based encryption
    algorithm.
    \param passwordSz The length of the password (not including the NULL
    terminator).
    \param vPKCS The PKCS version to use. Can be 1 for PKCS12 or PKCS5.
    \param pbeOid The OID of the PBE scheme to use (e.g. PBES2 or one of the
    OIDs for PBES1 in RFC 2898 A.3).
    \param encAlgId The encryption algorithm ID to use (e.g. AES256CBCb).
    \param salt The salt buffer to use. If NULL, a random salt will be used.
    \param saltSz The length of the salt buffer. Can be 0 if passing NULL for
    salt.
    \param itt The number of iterations to use for the KDF.
    \param rng A pointer to an initialized WC_RNG object.
    \param heap A pointer to the heap used for dynamic allocation. Can be NULL.

    _Example_
    \code
    byte* key;            // Traditional private key (DER formatted).
    word32 keySz;         // Size of key.
    byte* pkcs8Enc;       // Encrypted PKCS#8 key.
    word32 pkcs8EncSz;    // Size of pkcs8Enc.
    const char* password; // Password to use for encryption.
    int passwordSz;       // Length of password (not including NULL terminator).
    WC_RNG rng;

    // The following produces an encrypted, PKCS#8 version of key in pkcs8Enc.
    // The encryption uses password-based encryption scheme 2 (PBE2) from PKCS#5
    // and the AES cipher in CBC mode with a 256-bit key. See RFC 8018 for more
    // on PKCS#5.
    ret = wc_CreateEncryptedPKCS8Key(key, keySz, pkcs8Enc, &pkcs8EncSz,
            password, passwordSz, PKCS5, PBES2, AES256CBCb, NULL, 0,
            WC_PKCS12_ITT_DEFAULT, &rng, NULL);
    \endcode

    \sa wc_GetPkcs8TraditionalOffset
    \sa wc_CreatePKCS8Key
    \sa wc_EncryptPKCS8Key
    \sa wc_DecryptPKCS8Key
*/
int wc_CreateEncryptedPKCS8Key(byte* key, word32 keySz, byte* out,
        word32* outSz, const char* password, int passwordSz, int vPKCS,
        int pbeOid, int encAlgId, byte* salt, word32 saltSz, int itt,
        WC_RNG* rng, void* heap);

/*!
    \ingroup ASN

    \brief This function initializes the DecodedCert pointed to by the "cert"
     parameter. It saves the "source" pointer to a DER-encoded certificate of
     length "inSz." This certificate can be parsed by a subsequent call to
     wc_ParseCert.

    \param cert Pointer to an allocated DecodedCert object.
    \param source Pointer to a DER-encoded certificate.
    \param inSz Length of the DER-encoded certificate in bytes.
    \param heap A pointer to the heap used for dynamic allocation. Can be NULL.

    _Example_
    \code
    DecodedCert decodedCert; // Decoded certificate object.
    byte* certBuf;           // DER-encoded certificate buffer.
    word32 certBufSz;        // Size of certBuf in bytes.

    wc_InitDecodedCert(&decodedCert, certBuf, certBufSz, NULL);
    \endcode

    \sa wc_ParseCert
    \sa wc_FreeDecodedCert
*/
void wc_InitDecodedCert(struct DecodedCert* cert,
    const byte* source, word32 inSz, void* heap);

/*!
    \ingroup ASN

    \brief This function parses the DER-encoded certificate saved in the
    DecodedCert object and populates the fields of that object. The DecodedCert
    must have been initialized with a prior call to wc_InitDecodedCert. This
    function takes an optional pointer to a CertificateManager object, which
    is used to populate the certificate authority information of the
    DecodedCert, if the CA is found in the CertificateManager.

    \return 0 on success.
    \return Other negative values on failure.

    \param cert Pointer to an initialized DecodedCert object.
    \param type Type of certificate. See the CertType enum in asn_public.h.
    \param verify Flag that, if set, indicates the user wants to verify the
    validity of the certificate.
    \param cm An optional pointer to a CertificateManager. Can be NULL.

    _Example_
    \code
    int ret;
    DecodedCert decodedCert; // Decoded certificate object.
    byte* certBuf;           // DER-encoded certificate buffer.
    word32 certBufSz;        // Size of certBuf in bytes.

    wc_InitDecodedCert(&decodedCert, certBuf, certBufSz, NULL);
    ret = wc_ParseCert(&decodedCert, CERT_TYPE, NO_VERIFY, NULL);
    if (ret != 0) {
        fprintf(stderr, "wc_ParseCert failed.\n");
    }
    \endcode

    \sa wc_InitDecodedCert
    \sa wc_FreeDecodedCert
*/
int wc_ParseCert(DecodedCert* cert, int type, int verify, void* cm);

/*!
    \ingroup ASN

    \brief This function frees a DecodedCert that was previously initialized
    with wc_InitDecodedCert.

    \param cert Pointer to an initialized DecodedCert object.

    _Example_
    \code
    int ret;
    DecodedCert decodedCert; // Decoded certificate object.
    byte* certBuf;           // DER-encoded certificate buffer.
    word32 certBufSz;        // Size of certBuf in bytes.

    wc_InitDecodedCert(&decodedCert, certBuf, certBufSz, NULL);
    ret = wc_ParseCert(&decodedCert, CERT_TYPE, NO_VERIFY, NULL);
    if (ret != 0) {
        fprintf(stderr, "wc_ParseCert failed.\n");
    }
    wc_FreeDecodedCert(&decodedCert);
    \endcode

    \sa wc_InitDecodedCert
    \sa wc_ParseCert
*/
void wc_FreeDecodedCert(struct DecodedCert* cert);

/*!
    \ingroup ASN

    \brief This function registers a time callback that will be used anytime
    wolfSSL needs to get the current time. The prototype of the callback should
    be the same as the "time" function from the C standard library.

    \return 0 Returned on success.

    \param f function to register as the time callback.

    _Example_
    \code
    int ret = 0;
    // Time callback prototype
    time_t my_time_cb(time_t* t);
    // Register it
    ret = wc_SetTimeCb(my_time_cb);
    if (ret != 0) {
        // failed to set time callback
    }
    time_t my_time_cb(time_t* t)
    {
        // custom time function
    }
    \endcode

    \sa wc_Time
*/
int wc_SetTimeCb(wc_time_cb f);

/*!
    \ingroup ASN

    \brief This function gets the current time. By default, it uses the XTIME
    macro, which varies between platforms. The user can use a function of their
    choosing instead via the wc_SetTimeCb function.

    \return Time Current time returned on success.

    \param t Optional time_t pointer to populate with current time.

    _Example_
    \code
    time_t currentTime = 0;
    currentTime = wc_Time(NULL);
    wc_Time(&currentTime);
    \endcode

    \sa wc_SetTimeCb
*/
time_t wc_Time(time_t* t);

/*!
    \ingroup ASN

    \brief This function injects a custom extension in to an X.509 certificate.
     note: The content at the address pointed to by any of the parameters that
           are pointers must not be modified until the certificate is generated
           and you have the der output. This function does NOT copy the
           contents to another buffer.

    \return 0 Returned on success.
    \return Other negative values on failure.

    \param cert Pointer to an initialized DecodedCert object.
    \param critical If 0, the extension will not be marked critical, otherwise
     it will be marked critical.
    \param oid Dot separated oid as a string. For example "1.2.840.10045.3.1.7"
    \param der The der encoding of the content of the extension.
    \param derSz The size in bytes of the der encoding.


    _Example_
    \code
    int ret = 0;
    Cert newCert;
    wc_InitCert(&newCert);

    // Code to setup subject, public key, issuer, and other things goes here.

    ret = wc_SetCustomExtension(&newCert, 1, "1.2.3.4.5",
              (const byte *)"This is a critical extension", 28);
    if (ret < 0) {
        // Failed to set the extension.
    }

    ret = wc_SetCustomExtension(&newCert, 0, "1.2.3.4.6",
              (const byte *)"This is NOT a critical extension", 32)
    if (ret < 0) {
        // Failed to set the extension.
    }

    // Code to sign the certificate and then write it out goes here.

    \endcode

    \sa wc_InitCert
    \sa wc_SetUnknownExtCallback
*/
int wc_SetCustomExtension(Cert *cert, int critical, const char *oid,
                                      const byte *der, word32 derSz);

/*!
    \ingroup ASN

    \brief This function registers a callback that will be used anytime
    wolfSSL encounters an unknown X.509 extension in a certificate while parsing
    a certificate. The prototype of the callback should be:

    \return 0 Returned on success.
    \return Other negative values on failure.

    \param cert the DecodedCert struct that is to be associated with this
    callback.
    \param cb function to register as the time callback.

    _Example_
    \code
    int ret = 0;
    // Unknown extension callback prototype
    int myUnknownExtCallback(const word16* oid, word32 oidSz, int crit,
                             const unsigned char* der, word32 derSz);

    // Register it
    ret = wc_SetUnknownExtCallback(cert, myUnknownExtCallback);
    if (ret != 0) {
        // failed to set the callback
    }

    // oid: Array of integers that are the dot separated values in an oid.
    // oidSz: Number of values in oid.
    // crit: Whether the extension was mark critical.
    // der: The der encoding of the content of the extension.
    // derSz: The size in bytes of the der encoding.
    int myCustomExtCallback(const word16* oid, word32 oidSz, int crit,
                            const unsigned char* der, word32 derSz) {

        // Logic to parse extension goes here.

        // NOTE: by returning zero, we are accepting this extension and
        // informing wolfSSL that it is acceptable. If you find an extension
        // that you do not find acceptable, you should return an error. The
        // standard behavior upon encountering an unknown extension with the
        // critical flag set is to return ASN_CRIT_EXT_E. For the sake of
        // brevity, this example is always accepting every extension; you
        // should use different logic.
        return 0;
    }
    \endcode

    \sa ParseCert
    \sa wc_SetCustomExtension
*/
int wc_SetUnknownExtCallback(DecodedCert* cert,
                                             wc_UnknownExtCallback cb);
/*!
    \ingroup ASN

    \brief This function verifies the signature in the der form of an X.509
    certificate against a public key. The public key is expected to be the full
    subject public key info in der form.

    \return 0 Returned on success.
    \return Other negative values on failure.

    \param cert The der encoding of the X.509 certificate.
    \param certSz The size in bytes of cert.
    \param heap A pointer to the heap used for dynamic allocation. Can be NULL.
    \param pubKey The der encoding of the public key.
    \param pubKeySz The size in bytes of pubKey.
    \param pubKeyOID OID identifying the algorithm of the public key.
    (ie: ECDSAk, DSAk or RSAk)
*/
int wc_CheckCertSigPubKey(const byte* cert, word32 certSz,
                                      void* heap, const byte* pubKey,
                                      word32 pubKeySz, int pubKeyOID);

/*!
    \ingroup ASN

    \brief This function initializes the ASN.1 print options.

    \return  0 on success.
    \return  BAD_FUNC_ARG when asn1 is NULL.

    \param opts  The ASN.1 options for printing.

    _Example_
    \code
    Asn1PrintOptions opt;

    // Initialize ASN.1 print options before use.
    wc_Asn1PrintOptions_Init(&opt);
    \endcode

    \sa wc_Asn1PrintOptions_Set
    \sa wc_Asn1_PrintAll
*/
int wc_Asn1PrintOptions_Init(Asn1PrintOptions* opts);

/*!
    \ingroup ASN

    \brief This function sets a print option into an ASN.1 print options object.

    \return  0 on success.
    \return  BAD_FUNC_ARG when asn1 is NULL.
    \return  BAD_FUNC_ARG when val is out of range for option.
 
    \param opts  The ASN.1 options for printing.
    \param opt   An option to set value for.
    \param val   The value to set.

    _Example_
    \code
    Asn1PrintOptions opt;

    // Initialize ASN.1 print options before use.
    wc_Asn1PrintOptions_Init(&opt);
    // Set the number of indents when printing tag name to be 1.
    wc_Asn1PrintOptions_Set(&opt, ASN1_PRINT_OPT_INDENT, 1);
    \endcode

    \sa wc_Asn1PrintOptions_Init
    \sa wc_Asn1_PrintAll
*/
int wc_Asn1PrintOptions_Set(Asn1PrintOptions* opts, enum Asn1PrintOpt opt,
    word32 val);

/*!
    \ingroup ASN

    \brief This function initializes an ASN.1 parsing object.

    \return  0 on success.
    \return  BAD_FUNC_ARG when asn1 is NULL.

    \param asn1  ASN.1 parse object.

    _Example_
    \code
    Asn1 asn1;

    // Initialize ASN.1 parse object before use.
    wc_Asn1_Init(&asn1);
    \endcode

    \sa wc_Asn1_SetFile
    \sa wc_Asn1_PrintAll
 */
int wc_Asn1_Init(Asn1* asn1);

/*!
    \ingroup ASN

    \brief This function sets the file to use when printing into an ASN.1
    parsing object.

    \return  0 on success.
    \return  BAD_FUNC_ARG when asn1 is NULL.
    \return  BAD_FUNC_ARG when file is XBADFILE.

    \param asn1  The ASN.1 parse object.
    \param file  File to print to.

    _Example_
    \code
    Asn1 asn1;

    // Initialize ASN.1 parse object before use.
    wc_Asn1_Init(&asn1);
    // Set standard out to be the file descriptor to write to.
    wc_Asn1_SetFile(&asn1, stdout);
    \endcode

    \sa wc_Asn1_Init
    \sa wc_Asn1_PrintAll
 */
int wc_Asn1_SetFile(Asn1* asn1, XFILE file);

/*!
    \ingroup ASN

    \brief Print all ASN.1 items.

    \return  0 on success.
    \return  BAD_FUNC_ARG when asn1 or opts is NULL.
    \return  ASN_LEN_E when ASN.1 item's length too long.
    \return  ASN_DEPTH_E when end offset invalid.
    \return  ASN_PARSE_E when not all of an ASN.1 item parsed.

    \param asn1  The ASN.1 parse object.
    \param opts  The ASN.1 print options.
    \param data  Buffer containing BER/DER data to print.
    \param len   Length of data to print in bytes.

    \code
    Asn1PrintOptions opts;
    Asn1 asn1;
    unsigned char data[] = { Initialize with DER/BER data };
    word32 len = sizeof(data);

    // Initialize ASN.1 print options before use.
    wc_Asn1PrintOptions_Init(&opt);
    // Set the number of indents when printing tag name to be 1.
    wc_Asn1PrintOptions_Set(&opt, ASN1_PRINT_OPT_INDENT, 1);

    // Initialize ASN.1 parse object before use.
    wc_Asn1_Init(&asn1);
    // Set standard out to be the file descriptor to write to.
    wc_Asn1_SetFile(&asn1, stdout);
    // Print all ASN.1 items in buffer with the specified print options.
    wc_Asn1_PrintAll(&asn1, &opts, data, len);
    \endcode

    \sa wc_Asn1_Init
    \sa wc_Asn1_SetFile
 */
int wc_Asn1_PrintAll(Asn1* asn1, Asn1PrintOptions* opts, unsigned char* data,
    word32 len);

