#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"
#define PORT 9020
#define BUFSIZE 1024

int main()
{
    int sockfd, n;
    char buffer[BUFSIZE];
    struct sockaddr_in serv_addr;
    // 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf("socket error");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // IP 주소 변환
    if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0)
        printf("inet_pton error");

    // 서버와 connect
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        printf("connect error");

    // request 를 보낸다.
    sprintf(buffer, "GET /image.jpg HTTP/1.1\r\n"
                    "Host: %s:%d\r\n"
                    "\r\n",
            SERVER_ADDR, PORT);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
        printf("ERROR writing to socket");

    // response 를 받는다.
    memset(buffer, 0, BUFSIZE);
    n = read(sockfd, buffer, BUFSIZE);
    if (n < 0)
        printf("ERROR reading from socket");

    // response 를 출력한다.
    printf("%s", buffer);

    close(sockfd);

    return 0;
}
