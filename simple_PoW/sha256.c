#include <stdio.h>
#include <openssl/sha.h> // sudo apt-get install libssl-dev
int main()
{
    // 입력 데이터
    unsigned char input[] = "Hello, world!";
    // 입력 데이터의 길이
    size_t input_len = sizeof(input) - 1;
    // SHA256 해시를 저장할 버퍼
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // SHA256 해시 계산 SHA256(input, input_len, hash);
    // 해시 출력
    printf("SHA256 Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
    return 0;
}