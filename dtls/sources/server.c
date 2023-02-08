#include <stdio.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include "inet_socket.h"

#define BUFSIZE 512
#define CERTIFICATE_PATH "server/certificate.crt"
#define PRIVATE_KEY_PATH "server/private.key"

void print_openssl_error(char *message)
{
    unsigned long error_code = ERR_get_error();
    printf("%s: %s\n", message, ERR_error_string(error_code, NULL));
}

int handle_connection(SSL *ssl)
{
    while (1)
    {
        char buf[BUFSIZE];
        int numRead = SSL_read(ssl, buf, BUFSIZE);
        if (numRead <= 0)
        {
            print_openssl_error("SSL_read()");
            return -1;
        }

        printf("%s\n", buf);

        if (SSL_write(ssl, buf, strlen(buf) + 1) <= 0)
        {
            print_openssl_error("SSL_write()");
            return -1;
        }
    }

    return 0;
}

int generate_key()
{
    EVP_PKEY *pkey = EVP_RSA_gen(2048);
    if (pkey == NULL)
    {
        print_openssl_error("EVP_RSA_gen()");
        return -1;
    }

    X509 *cert = X509_new();
    if (cert == NULL)
    {
        print_openssl_error("X509_new()");
        return -1;
    }

    X509_set_version(cert, X509_VERSION_3);
    X509_set_pubkey(cert, pkey);

    BIO *cert_file = BIO_new_file(CERTIFICATE_PATH, "w");
    if (cert_file == NULL)
    {
        printf("BIO_new_file()\n");
        return -1;
    }
    PEM_write_bio_X509(cert_file, cert);
    BIO_free(cert_file);

    BIO *pkey_file = BIO_new_file(PRIVATE_KEY_PATH, "w");
    if (pkey_file == NULL)
    {
        printf("BIO_new_file()\n");
        return -1;
    }
    PEM_write_bio_PrivateKey(pkey_file, pkey, NULL, NULL, 0, NULL, NULL);
    BIO_free(pkey_file);

    X509_free(cert);
    EVP_PKEY_free(pkey);
}

int start_server(int port)
{
    // generate_key();

    SSL_CTX *ctx = SSL_CTX_new(DTLS_server_method());
    if (ctx == NULL)
    {
        printf("SSL_CTX_new()\n");
        return -1;
    }

    SSL_CTX_set_cipher_list(ctx, "RSA");
    SSL_CTX_use_certificate_file(ctx, CERTIFICATE_PATH, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, PRIVATE_KEY_PATH, SSL_FILETYPE_PEM);

    SSL *ssl = SSL_new(ctx);

    int sfd = inet_bind(port, SOCK_DGRAM);
    if (sfd == -1)
    {
        printf("inet_bind()\n");
        return sfd;
    }

    SSL_set_fd(ssl, sfd);

    if (SSL_accept(ssl) <= 0)
    {
        print_openssl_error("SSL_accept()");
        return -1;
    }

    int ret = handle_connection(ssl);

    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return ret;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid command!\n");
        return 1;
    }

    int port = atoi(argv[1]);
    if (port == 0)
    {
        printf("Invalid port number!\n");
        return 1;
    }

    OPENSSL_init_ssl(0, NULL);

    return start_server(port);
}