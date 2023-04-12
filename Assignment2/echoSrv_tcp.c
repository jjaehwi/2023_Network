#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>

// 에러 핸들링을 위한 함수
void errProc(const char *);

int main(int argc, char **argv)
{
    // 변수: 소켓, 클라이언트 연결 및 데이터 전송을 위한 변수를 초기화
    int mySock, newSock, readLen, res;
    char buff[BUFSIZ];
    char *strAddr;
    struct sockaddr_in srcAddr, destAddr;
    socklen_t addrLen;

    // 사용자가 올바른 인수를 입력했는지 확인 (수신할 포트 번호)
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s Port", argv[0]);
        return 0;
    }

    printf("Server start...\n");

    // 소켓 생성: socket() 함수를 사용하여 TCP 소켓을 생성
    mySock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mySock == -1)
        errProc("socket");

    // 소켓 주소 초기화: 서버 주소 정보로 sockaddr_in 구조체를 초기화
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_port = htons(atoi(argv[1]));

    // 로컬 주소 및 포트에 소켓 바인딩: bind() 함수를 사용하여 sockaddr_in 구조에 지정된 로컬 주소 및 포트에 소켓을 바인딩
    res = bind(mySock, (struct sockaddr *)&srcAddr, sizeof(srcAddr));
    if (res == -1)
        errProc("bind");

    // 수신 연결에 대해 listen 상태로: listen() 함수를 사용하여 listening 상태가 된다.
    res = listen(mySock, 2);
    if (res == -1)
        errProc("listen");

    // 들어오는 연결 accept 및 처리: accept() 함수를 사용하여 들어오는 연결을 수락하고, 프로그램이 종료될 때까지 실행되는 while 루프에서 이 연결을 처리한다.
    while (1)
    {
        addrLen = sizeof(destAddr);
        newSock = accept(mySock, (struct sockaddr *)&destAddr, &addrLen);
        if (newSock == -1)
            errProc("accept");

        // Print client connection information
        printf("client %s:%d is connected...\n",
               inet_ntoa(destAddr.sin_addr),
               ntohs(destAddr.sin_port));

        // 클라이언트로부터 데이터를 수신하고 돌려주기: recv() 함수를 사용하여 클라이언트로부터 데이터를 수신하고, send() 함수를 사용하여 다시 돌려준다.
        while (1)
        {
            readLen = recv(newSock, buff, BUFSIZ - 1, 0);
            if (readLen == -1)
                errProc("recv");
            if (readLen == 0)
                break;
            buff[readLen] = '\0';

            // 클라이언트 정보와 받은 데이터를 출력한다.
            strAddr = inet_ntoa(destAddr.sin_addr);
            printf("Client(%d): %s\n", ntohs(destAddr.sin_port), buff);
            printf("%s:%d>%s\n", strAddr, ntohs(destAddr.sin_port), buff);

            res = send(newSock, buff, readLen, 0);
            if (res == -1)
                errProc("send");
        }

        // 연결 닫기: close() 함수를 사용하여 연결을 닫는다.
        close(newSock);
    }

    // listen 소켓 닫기: close() 함수를 사용하여 listen 소켓을 닫습니다.
    close(mySock);
    return 0;
}

// 에러 핸들링: 오류를 일으킨 함수의 이름과 시스템에서 반환된 오류 메시지를 포함하는 오류 메시지를 출력해서 오류를 처리한다.
void errProc(const char *str)
{
    fprintf(stderr, "%s: %s \n", str, strerror(errno));
    exit(1);
}