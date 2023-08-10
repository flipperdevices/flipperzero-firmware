

typedef struct func_args
{
    int argc;
    char **argv;
    int return_code;
} func_args;

void wolfSSL_init(void) ;
void wolfSSL_main(void) ;
WOLFSSL_CTX *wolfSSL_TLS_server_init(void);
WOLFSSL_CTX *wolfSSL_TLS_client_init(void);

void benchmark_test(void *args);
void wolfcrypt_test(void *args);
void wolfSSL_TLS_client(void *ctx, func_args *args);
void wolfSSL_TLS_server(void *ctx, func_args *args);
