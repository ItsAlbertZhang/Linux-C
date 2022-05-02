#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdio.h>
#include <string.h>

#define PUBLICKEY "public.pem"
#define PRIVATEKEY "private.pem"

int generate_key() {
    int ret = 1;
    RSA *r = NULL;
    BIGNUM *bne = NULL;
    BIO *bp_public = NULL, *bp_private = NULL;

    int bits = 2048;
    unsigned long e = RSA_F4;

    // 1. generate rsa key
    if (1 == ret) {
        bne = BN_new();
        ret = BN_set_word(bne, e);
    }
    if (1 == ret) {
        r = RSA_new();
        ret = RSA_generate_key_ex(r, bits, bne, NULL);
    }

    // 2. save public key
    if (1 == ret) {
        bp_public = BIO_new_file("public.pem", "w+");
        ret = PEM_write_bio_RSAPublicKey(bp_public, r);
    }

    // 3. save private key
    if (1 == ret) {
        bp_private = BIO_new_file("private.pem", "w+");
        ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
    }

    // 4. free
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);

    return !(ret == 1);
}

int main(int argc, char *argv[]) {
    int ret = generate_key();
    return 0;
}