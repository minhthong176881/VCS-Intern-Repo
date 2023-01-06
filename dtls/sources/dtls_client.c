#include <stdio.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

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

int start_client(const char *ip_address, int port)
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

    SSL_CTX_use_certificate_chain_file(ctx, "client/cert.pem");
    SSL_CTX_use_PrivateKey_file(ctx, "client/key.pem", SSL_FILETYPE_PEM);

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_cert);
    SSL_CTX_set_cookie_generate_cb(ctx, generate_cookie);
    SSL_CTX_set_cookie_verify_cb(ctx, verify_cookie);

    struct sockaddr_in server_addr;
    memeset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("socket()");
    }

    BIO *bio = BIO_new_dgram(fd, BIO_NOCLOSE);
    BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &server_addr);

    SSL *ssl = SSL_new(ctx);
    SSL_set_bio(ssl, bio, bio);

    SSL_connect(ssl);
}

int main(int argc, char *argv[])
{
    if (argv != 3)
    {
        printf("Invalid command!");
        return -1;
    }

    if (inet_addr(argv[1]) == (in_addr_t)(-1))
    {
        printf("Invalid IP address!");
        return -1;
    }

    int port = atoi(argv[2]);
    if (port <= 0 || port >= (1 << 16))
    {
        printf("Invalid port!");
        return -1;
    }

    start_client(argv[1], port);
}