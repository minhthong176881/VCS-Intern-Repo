#include <stdio.h>
#include <openssl/ssl.h>

int verify_cert(int ok, X509_STORE_CTX *ctx)
{
    return 1;
}

int generate_cookie(SSL *ssl, unsigned char *cookie, unsigned int cookie_len)
{
    return 1;
}

int verify_cookie(SSL *ssl, unsigned char *cookie, unsigned int cookie_len)
{
    return 1;
}

int start_client(const char *ip, int port)
{
    if (OPENSSL_init_ssl(0, 0) == -1)
    {
        perror("OPENSSL_init_ssl()");
    }

    SSL_CTX *ctx = SSL_CTX_new(DTLSv1_2_client_method());
    if (ctx == NULL)
    {
        perror("SSL_CTX_new()");
    }

    
}