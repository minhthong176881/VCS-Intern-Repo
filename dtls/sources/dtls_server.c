#define _POSIX_C_SOURCE 200112L

#include <openssl/ssl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>

// #define BUFFER_SIZE (1 << 16)
// #define COOKIE_SECRET_LENGTH 16

// unsigned char cookie_secret[COOKIE_SECRET_LENGTH];
// int cookie_initialized = 0;

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

int handle_connection(struct sockaddr_in server_addr, struct sockaddr_in client_addr, SSL *ssl)
{
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1)
    {
        perror("socket()");
        return -1;
    }

    if (bind(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind()");
        return -1;
    }

    if (connect(client_fd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("connect()");
        return -1;
    }

    BIO *cbio = SSL_get_rbio(ssl);
    if (cbio == NULL)
    {
        perror("SSL_get_rbio()");
    }
    BIO_set_fd(cbio, client_fd, BIO_NOCLOSE);
    BIO_ctrl(cbio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &client_addr);

    SSL_accept(ssl);

    return 0;
}

int start_server(const char *ip_address, int port)
{
    if (OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS, NULL) != 0)
    {
        perror("OPENSSL_init_ssl()");
    }

    SSL_CTX *ctx = SSL_CTX_new(DTLSv1_2_server_method);
    if (ctx == NULL)
    {
        perror("SSL_CTX_new()");
    }

    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);

    if (!SSL_CTX_use_certificate_file(ctx, "server/server-cert.pem", SSL_FILETYPE_PEM))
    {
        printf("Error: no certificate found!");
    }

    if (!SSL_CTX_use_PrivateKey_file(ctx, "server/server-key.pem", SSL_FILETYPE_PEM))
    {
        printf("Error: no private key found");
    }

    if (!SSL_CTX_check_private_key(ctx))
    {
        printf("Error: invalid private key");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 1);

    SSL_CTX_set_read_ahead(ctx, verify_cert);
    SSL_CTX_set_cookie_generate_cb(ctx, generate_cookie);
    SSL_CTX_set_cookie_verify_cb(ctx, &verify_cookie);

    struct sockaddr_in server_addr;
    memeset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    int fd = socket(server_addr.sin_family, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket()");
        return -1;
    }

    if (bind(fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind()");
        return -1;
    }

    while (1)
    {
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(struct sockaddr_in));

        BIO *bio = BIO_new_dgram(fd, BIO_NOCLOSE);
        if (bio == NULL)
        {
            perror("BIO_new_dgram()");
        }

        SSL *ssl = SSL_new(ctx);
        if (ssl == NULL)
        {
            perror("SSL_new()");
        }
        SSL_set_bio(ssl, bio, bio);
        
        SSL_set_options(ssl, SSL_OP_COOKIE_EXCHANGE);

        while (!DTLSv1_listen(ssl, (BIO_ADDR *)&client_addr));

        handle_connection(server_addr, client_addr, ssl);
    }

    return 0;
}

int main(int argc, char *argv[])
{
}