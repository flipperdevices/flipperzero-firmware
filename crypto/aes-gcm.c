//
//  aes-gcm.c
//  Pods
//
//  Created by Markus Kosmal on 20/11/14.
//
//

#include "aes-gcm.h"

int aes_gcm_encrypt(unsigned char* output, const unsigned char* input, int input_length, const unsigned char* key, const size_t key_len, const unsigned char * iv, const size_t iv_len){
    
    int ret = 0;                // our return value
    gcm_context ctx;            // includes the AES context structure
    
    size_t tag_len = 0;
    unsigned char * tag_buf = NULL;
    
    gcm_setkey( &ctx, key, (const uint)key_len );
    
    ret = gcm_crypt_and_tag( &ctx, ENCRYPT, iv, iv_len, NULL, 0,
                            input, output, input_length, tag_buf, tag_len);
    
    gcm_zero_ctx( &ctx );
    
    return( ret );
}

int aes_gcm_decrypt(unsigned char* output, const unsigned char* input, int input_length, const unsigned char* key, const size_t key_len, const unsigned char * iv, const size_t iv_len){
    
    int ret = 0;                // our return value
    gcm_context ctx;            // includes the AES context structure
    
    size_t tag_len = 0;
    unsigned char * tag_buf = NULL;
    
    gcm_setkey( &ctx, key, (const uint)key_len );
    
    ret = gcm_crypt_and_tag( &ctx, DECRYPT, iv, iv_len, NULL, 0,
                            input, output, input_length, tag_buf, tag_len);
    
    gcm_zero_ctx( &ctx );
    
    return( ret );

}