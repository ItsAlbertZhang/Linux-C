#include <fcntl.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PRIKEY 0
#define PUBKEY 1

int rsa_decrypt(unsigned char *plaintext, const unsigned char *ciphertext, const char *rsa_filename, int rsa_type) {
    int ret = 0;

    // 打开 RSA 密钥文件
    FILE *fp = fopen(rsa_filename, "rb");
    if (NULL == fp) {
        printf("%s:%d:error\n", __FILE__, __LINE__);
        return -1;
    }

    // 从文件中获取 RSA 信息
    RSA *rsa = NULL;
    if (rsa_type) {
        rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
    } else {
        rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    }
    if (NULL == rsa) {
        printf("%s:%d:error\n", __FILE__, __LINE__);
        return -1;
    }
    fclose(fp);

    // 加密
    int rsa_len = RSA_size(rsa);
    bzero(plaintext, rsa_len);
    if (rsa_type) {
        ret = RSA_public_decrypt(rsa_len, ciphertext, plaintext, rsa, RSA_PKCS1_PADDING);
    } else {
        ret = RSA_private_decrypt(rsa_len, ciphertext, plaintext, rsa, RSA_PKCS1_PADDING);
    }

    RSA_free(rsa);
    return ret; // 返回读取到的明文长度
}

int main(int argc, char *argv[]) {
    unsigned char ciphertext[1024] = {0};
    unsigned char plaintext[1024] = {0};
    int fd = open("ciphertext", O_RDWR, 0666);
    int ret = read(fd, ciphertext, sizeof(ciphertext));
    int len = rsa_decrypt(plaintext, ciphertext, "public.pem", 1);
    printf("%s\n", plaintext);
    return 0;
}