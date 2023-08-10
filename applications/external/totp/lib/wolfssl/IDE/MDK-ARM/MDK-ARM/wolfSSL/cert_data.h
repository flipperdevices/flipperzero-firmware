#ifndef WOLFSSL_CERT_DATA_H
#define WOLFSSL_CERT_DATA_H

#ifdef USE_CERT_BUFFERS_1024
extern const unsigned char client_key_der_1024[]  ;
extern int sizeof_client_key_der_1024 ;
/* ./certs/1024/client-cert.der, 1024-bit */
extern const unsigned char client_cert_der_1024[] ;
extern int sizeof_client_cert_der_1024 ;
/* ./certs/1024/dh1024.der, 1024-bit */
extern const unsigned char dh_key_der_1024[] ; 
extern int sizeof_dh_key_der_1024 ; 
/* ./certs/1024/dsa1024.der, 1024-bit */
extern const unsigned char dsa_key_der_1024[] ;
extern int sizeof_dsa_key_der_1024 ;
/* ./certs/1024/rsa1024.der, 1024-bit */
extern const unsigned char rsa_key_der_1024[] ;
extern int sizeof_rsa_key_der_1024 ;

#elif defined(USE_CERT_BUFFERS_2048)
/* ./certs/client-key.der, 2048-bit */
extern const unsigned char client_key_der_2048[] ;
extern int sizeof_client_key_der_2048 ;
/* ./certs/client-cert.der, 2048-bit */
extern const unsigned char client_cert_der_2048[] ;
extern int sizeof_client_cert_der_2048 ;
/* ./certs/dh2048.der, 2048-bit */
extern const unsigned char dh_key_der_2048[] ;
extern int sizeof_dh_key_der_2048 ;
/* ./certs/dsa2048.der, 2048-bit */
extern const unsigned char dsa_key_der_2048[] ;
extern int sizeof_dsa_key_der_2048;
/* ./certs/rsa2048.der, 2048-bit */
extern const unsigned char rsa_key_der_2048[] ;
extern int sizeof_rsa_key_der_2048 ;
#endif

#endif

