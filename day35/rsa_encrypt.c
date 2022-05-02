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

int rsa_encrypt(const unsigned char *plaintext, unsigned char *ciphertext, const char *rsa_filename, int rsa_type) {
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

    // 解密
    int rsa_len = RSA_size(rsa);
    bzero(ciphertext, rsa_len);
    if (rsa_type) {
        ret = RSA_public_encrypt(rsa_len - 11, plaintext, ciphertext, rsa, RSA_PKCS1_PADDING);
    } else {
        ret = RSA_private_encrypt(rsa_len - 11, plaintext, ciphertext, rsa, RSA_PKCS1_PADDING);
    }
    // PKCS#1 建议的 padding 占用 11 字节
    if (ret < 0) {
        printf("%s:%d:error\n", __FILE__, __LINE__);
        return -1;
    } else {
        rsa_len = RSA_size(rsa);
    }

    RSA_free(rsa);
    return rsa_len; // 返回密文长度 (2048 位密钥情况下为 256 字节)
}

int main(int argc, char *argv[]) {
    unsigned char *plaintext = (unsigned char *)"风带来故事的种子, 时间使之发芽.";
    unsigned char ciphertext[1024] = {0};
    int rsa_len = rsa_encrypt(plaintext, ciphertext, "private.pem", 0);
    int fd = open("ciphertext", O_RDWR | O_CREAT | O_TRUNC, 0666);
    ftruncate(fd, rsa_len);
    write(fd, ciphertext, rsa_len);
}