/*!
    \defgroup 3DES Algorithms - 3DES
    \defgroup AES Algorithms - AES
    \defgroup ARC4 Algorithms - ARC4
    \defgroup BLAKE2 Algorithms - BLAKE2
    \defgroup Camellia Algorithms - Camellia
    \defgroup ChaCha Algorithms - ChaCha
    \defgroup ChaCha20Poly1305 Algorithms - ChaCha20_Poly1305
    \defgroup CMAC Algorithm - CMAC
    \defgroup Crypto Callbacks - CryptoCb
    \defgroup Curve25519 Algorithms - Curve25519
    \defgroup Curve448 Algorithms - Curve448
    \defgroup DSA Algorithms - DSA
    \defgroup Diffie-Hellman Algorithms - Diffie-Hellman
    \defgroup ECC Algorithms - ECC
    \defgroup ED25519 Algorithms - ED25519
    \defgroup ED448 Algorithms - ED448
    \defgroup ECCSI_Overview Overview of ECCSI
    ECCSI (Elliptic Curve-Based Certificateless Signatures for Identity-Based Encryption) is specified in RFC 6507 (https://tools.ietf.org/html/rfc6507).

    In Identity-Based cryptography, there is a Key Management Service that generates keys based on an identity for a client.
    The private key (SSK) and public key (PVT) are delivered to the signer and the public key (PVT) only delivered to the verifier on request.\n\n
    wolfCrypt offers the ability to:
      -# Create KMS keys,
      -# Generate signing key pairs,
      -# Validate signing key pairs,
      -# Sign messages and
      -# Verify messages.

    KMS:
      -# Initialize ECCSI Key: wc_InitEccsiKey()
      -# Make and save or load ECCSI Key:
        -# wc_MakeEccsiKey(), wc_ExportEccsiKey(), wc_ExportEccsiPublicKey() or
        -# wc_ImportEccsiKey()
      -# Wait for request:
        -# Receive signing ID from client.
        -# Generate signing key pair from ID: wc_MakeEccsiPair()
        -# Encode result:
          -# For signer, signing key pair: wc_EncodeEccsiPair()
        -# Send KPAK and result
      -# Free ECCSI Key: wc_FreeEccsiKey()

    Client, signer:
      -# Initialize ECCSI Key: wc_InitEccsiKey()
      -# (When signing pair not cached) Request KPAK and signing pair from KMS
        -# Send signing ID to KMS.
        -# Receive signing key pair from KMS.
        -# Load KMS Public Key: wc_ImportEccsiPublicKey()
        -# Decode signing key pair: wc_DecodeEccsiPair()
        -# Validate the key pair: wc_ValidateEccsiPair()
      -# (If not done above) Load KMS Public Key: wc_ImportEccsiPublicKey()
      -# (If not cached) Calculate hash of the ID and PVT: wc_HashEccsiId()
      -# For each message:
        -# Set Hash of Identity: wc_SetEccsiHash()
        -# Sign message: wc_SignEccsiHash()
        -# Send hash ID, message and signature to peer.
      -# Free ECCSI Key: wc_FreeEccsiKey()

    Client, verifier:
      -# Receive hash ID, message and signature from signer.
      -# Request KPAK (if not cached) and PVT (if not cached) for hash ID from KMS.
      -# Receive KPAK (if not cached) and PVT (if not cached) for hash ID from KMS.
      -# Initialize ECCSI Key: wc_InitEccsiKey()
      -# Load KMS Public Key: wc_ImportEccsiPublicKey()
      -# Decode PVT: wc_DecodeEccsiPvtFromSig()
      -# Calculate hash of the ID and PVT: wc_HashEccsiId()
      -# Set ECCSI key pair: wc_SetEccsiPair()
      -# Verify signature of message: wc_VerifyEccsiHash()
      -# Free ECCSI Key: wc_FreeEccsiKey()

    \defgroup ECCSI_Setup Setup ECCSI Key
    Operations for establinshing an ECCSI key.

    Initialize ECCSI Key before use (wc_InitEccsiKey()).\n
    Initialize ECCSI Key before use (wc_InitEccsiKey_ex()) for use with a curve other than P256.\n
    Either make a new key (wc_MakeEccsiKey()), import an existing key (wc_ImportEccsiKey()) or import existing private key (wc_ImportEccsiPrivateKey()) and public key (wc_ImportEccsiPublicKey()).\n
    Export the key (wc_ExportEccsiKey()) after making a new key for future use.\n
    Export the private key (wc_ExportEccsiPrivateKey()) after making a new key for future use.\n
    Export the public key (wc_ExportEccsiPublicKey()) from KMS to pass to client.\n
    Import the public key (wc_ImportEccsiPublicKey()) into client.\n
    Free the ECCSI Key (wc_FreeEccsiKey()) when finished.

    \defgroup ECCSI_Operations Operations for Signing and Verifying with ECCSI Key
    These operations are for signing and verifying with ECCSI keys.

    Make an ECCSI key pair (wc_MakeEccsiPair()) with the signer's ID for use when signing.\n
    Validate the ECCSI key pair (wc_ValidateEccsiPair()) with the signer's ID.\n
    Validate the ECCSI Public Validation Token (PVT) (wc_ValidateEccsiPvt()).\n
    Encode the ECCSI key pair (wc_EncodeEccsiPair()) for transfer to client.\n
    Encode the ECCSI SSK (wc_EncodeEccsiSsk()) for transfer to client.\n
    Encode the ECCSI PVT (wc_EncodeEccsiPvt()) for transfer to verifier.\n
    Decode the ECCSI key pair (wc_DecodeEccsiPair()) on client for signing.\n
    Decode the ECCSI SSK (wc_DecodeEccsiSsk()) on client for signing.\n
    Decode the ECCSI PVT (wc_DecodeEccsiPvt()) on client for signing.\n
    Decode the ECCSI PVT from the signature (wc_DecodeEccsiPvtFromSig()) on client for verifying.\n
    Calculate hash of the ID (wc_HashEccsiId()) for signing/verifying using ID and Public Validation Token (PVT).\n
    Sign (wc_SignEccsiHash()) a message with the hash of the ID and the Secret Signing Key (SSK) and Public Validation Token (PVT).\n
    Verify (wc_VerifyEccsiHash()) a message with the hash of the signer's ID.

    \defgroup SAKKE_Overview Overview of SAKKE Key
    SAKKE (Sakai-Kasahara Key Encryption) is specified in RFC 6508 (https://tools.ietf.org/html/rfc6508).

    SAKKE is used to transfer a secret to a peer using Identity Based cryptography.\n
    The Key Management Service (KMS) is responsible for issuing Receiver Secret %Keys (RSKs).
    Data up to (2^hashlen)^hashlen bytes of data can be transferred.\n
    The sender must know the identity of the receiver and the KMS Public Key.\n
    The receiver must have obtained a Receiver Secret Key (RSK) for the identity from a KMS in order to derive the secret.

    KMS:
      -# Initialize SAKKE Key: wc_InitSakkeKey()
      -# Make and save or load SAKKE Key:
        -# wc_MakeSakkeKey(), wc_ExportSakkeKey(), wc_ExportSakkePublicKey() or
        -# wc_ImportSakkeKey()
      -# Wait for request:
        -# Make an RSK base on ID for the client: wc_MakeSakkeRsk()
        -# Encode RSK for transfer to client: wc_EncodeSakkeRsk()
      -# Free SAKKE Key: wc_FreeSakkeKey()

    Key Exchange, Peer A:
      -# Initialize SAKKE Key: wc_InitSakkeKey()
      -# Load KMS Public Key: wc_ImportSakkePublicKey()
      -# Generate a random SSV: wc_GenerateSakkeSSV()
      -# Set the identity of Peer B: wc_SetSakkeIdentity()
      -# Make an encapsulated SSV and auth data: wc_MakeSakkeEncapsulatedSSV()
      -# Send encapsulated data to Peer B
      -# Free SAKKE Key: wc_FreeSakkeKey()

    Key Exchange, Peer B:
      -# Receive encapsulated data.
      -# Initialize SAKKE Key: wc_InitSakkeKey()
      -# Load KMS Public Key: wc_ImportSakkePublicKey()
      -# Decode RSK transferred from KMS or stored locally: wc_DecodeSakkeRsk()
      -# [Optional] Validate RSK before first use: wc_ValidateSakkeRsk()
      -# Set the identity: wc_SetSakkeIdentity()
      -# Set the RSK and, optionally precomputation table: wc_SetSakkeRsk()
      -# Derive SSV with auth data: wc_DeriveSakkeSSV()
      -# Free SAKKE Key: wc_FreeSakkeKey()

    Transfer secret, Peer A:
      -# Initialize SAKKE Key: wc_InitSakkeKey()
      -# Load KMS Public Key: wc_ImportSakkePublicKey()
      -# Set the identity of Peer B: wc_SetSakkeIdentity()
      -# Make an encapsulation of the SSV and auth data: wc_MakeSakkeEncapsulatedSSV()
      -# Send encapsulated data to Peer B
      -# Free SAKKE Key: wc_FreeSakkeKey()

    Transfer secret, Peer B:
      -# Initialize SAKKE Key: wc_InitSakkeKey()
      -# Load KMS Public Key: wc_ImportSakkePublicKey()
      -# Decode RSK transferred from KMS or stored locally: wc_DecodeSakkeRsk()
      -# [Optional] Validate RSK before first use: wc_ValidateSakkeRsk()
      -# Receive encapsulated data.
      -# Set the identity: wc_SetSakkeIdentity()
      -# Set the RSK and, optionally precomputation table: wc_SetSakkeRsk()
      -# Derive SSV and auth data: wc_DeriveSakkeSSV()
      -# Free SAKKE Key: wc_FreeSakkeKey()
    
    \defgroup SAKKE_Setup Setup SAKKE Key
    Operations for establishing a SAKKE key.

    Initialization SAKKE Key before use (wc_InitSakkeKey() or wc_InitSakkeKey_ex()).\n
    Either make a new key (wc_MakeSakkeKey()) or import an existing key (wc_ImportSakkeKey()).\n
    Export the key (wc_ExportSakkeKey()) after making a new key for future use.\n
    If only the private part of the KMS SAKKE Key is available, make the public key (wc_MakeSakkePublicKey()).\n
    Export the private key (wc_ExportSakkePrivateKey()) from KMS from storage.\n
    Import the private key (wc_ImportSakkePrivateKey()) into KMS from storage.\n
    Export the public key (wc_ExportSakkePublicKey()) from KMS to pass to client.\n
    Import the public key (wc_ImportSakkePublicKey()) into client.\n
    Set the identity to use (wc_SetSakkeIdentity()) into client.\n
    Free the SAKKE Key (wc_FreeSakkeKey()) when finished.

    \defgroup SAKKE_RSK Operations on/with SAKKE RSK
    These operations make, validate, encode and decode a Receiver Secret Key (RSK).

    An RSK is required to derive an SSV (see wc_DeriveSakkeSSV()).\n
    On the KMS, make an RSK (wc_MakeSakkeRsk()) from the client's ID.\n
    On the client, validate the RSK (wc_ValidateSakkeRsk()) with the ID.\n
    Encode the RSK (wc_EncodeSakkeRsk()) to pass to client or for storage.\n
    Decode the RSK (wc_DecodeSakkeRsk()) on the client when needed.\n
    Import the RSK (wc_ImportSakkeRsk()) on the client when needed.\n
    Set the RSK and, optionally, a pre-computation table (wc_SetSakkeRsk()) on the client when needed.

    \defgroup SAKKE_Operations Operations using SAKKE Key
    These operations transfer a Shared Secret Value (SSV) from one client to another. The SSV may be randomly generated.

    Calculate the size of the authentication data (wc_GetSakkeAuthSize()) to determine where the SSV starts in a buffer.\n
    Make the intermediate point I (wc_MakeSakkePointI()) to speed making an encapsulated and deriving SSV.\n
    Get intermediate point I (wc_GetSakkePointI()) for storage.\n
    Set intermediate point I (wc_SetSakkePointI()) from storage.\n
    Generate a pre-computation table for intermediate point I (wc_GenerateSakkePointITable()) to further enhance performance. Store as necessary.\n
    Set the pre-computation table for intermediate point I (wc_SetSakkePointITable()) to further enhance performance.\n
    Clear the pre-computation table for intermediate point I (wc_ClearSakkePointITable()) to remove reference to external table pointer.\n
    Make an encapsulated SSV (wc_MakeSakkeEncapsulatedSSV()) to share with another client. Data in SSV is modified.\n
    Generate a random SSV (wc_GenerateSakkeSSV()) for key exchange.\n
    Derive the SSV, (wc_DeriveSakkeSSV()) on the recipient from the encapsulated SSV.

    \defgroup HMAC Algorithms - HMAC
    \defgroup MD2 Algorithms - MD2
    \defgroup MD4 Algorithms - MD4
    \defgroup MD5 Algorithms - MD5
    \defgroup PKCS7 Algorithms - PKCS7
    \defgroup PKCS11 Algorithms - PKCS11
    \defgroup Password Algorithms - Password Based
    \defgroup Poly1305 Algorithms - Poly1305
    \defgroup RIPEMD Algorithms - RIPEMD
    \defgroup RSA Algorithms - RSA
    \defgroup SHA Algorithms - SHA 128/224/256/384/512
    \defgroup SipHash Algorithm - SipHash
    \defgroup SRP Algorithms - SRP

    \defgroup ASN ASN.1
    \defgroup Base_Encoding Base Encoding
    \defgroup CertManager CertManager API
    \defgroup Compression Compression
    \defgroup Error Error Reporting
    \defgroup IoTSafe IoT-Safe Module
    IoT-Safe (IoT-SIM Applet For Secure End-2-End Communication) is a technology that leverage the SIM as robust,
    scalable and standardized hardware Root of Trust to protect data communication.

    IoT-Safe SSL sessions use the SIM as Hardware Security Module, offloading all the crypto public
    key operations and reducing the attack surface by restricting access to certificate and keys
    to the SIM.

    IoT-Safe support can be enabled on an existing WOLFSSL_CTX context, using wolfSSL_CTX_iotsafe_enable().\n
    Session created within the context can set the parameters for IoT-Safe key and files usage, and enable
    the public keys callback, with wolfSSL_iotsafe_on().

    If compiled in, the module supports IoT-Safe random number generator as source of entropy for wolfCrypt.

    \defgroup PSA Platform Security Architecture (PSA) API
    \defgroup Keys Key and Cert Conversion
    \defgroup Logging Logging
    \defgroup Math Math API
    \defgroup Memory Memory Handling
    \defgroup Random Random Number Generation
    \defgroup Signature Signature API
    \defgroup openSSL OpenSSL API
    \defgroup wolfCrypt wolfCrypt Init and Cleanup
    \defgroup TLS wolfSSL Initialization/Shutdown
    \defgroup CertsKeys wolfSSL Certificates and Keys
    \defgroup Setup wolfSSL Context and Session Set Up
    \defgroup IO wolfSSL Connection, Session, and I/O
    \defgroup Debug wolfSSL Error Handling and Reporting
*/
