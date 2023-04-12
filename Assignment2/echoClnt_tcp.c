#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

// 에러 핸들링을 위한 함수
void errProc(const char *);

int main(int argc, char **argv)
{
    int mySock, readLen, recvByte, maxBuff;
    char wBuff[BUFSIZ]; // 유저 입력을 위한 버퍼
    char rBuff[BUFSIZ]; // 서버 response 를 위한 버퍼
    struct sockaddr_in destAddr;

    // 사용자가 IP 주소와 포트 번호라는 두 개의 인수를 제공했는지 확인
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s [IP] [Port]\n", argv[0]);
        return 0;
    }

    // socket() 함수를 사용하여 서버와의 통신을 위한 TCP 소켓 생성
    mySock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mySock == -1)
        errProc("socket");

    // 사용자가 제공한 IP 주소 및 포트 번호를 포함하여 소켓의 대상 주소를 설정
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_addr.s_addr = inet_addr(argv[1]);
    destAddr.sin_port = htons(atoi(argv[2]));

    // 만들어진 소켓을 통해 서버와 connect
    if (connect(mySock, (struct sockaddr *)&destAddr, sizeof(destAddr)) == -1)
    {
        close(mySock);
        errProc("connect");
    }

    // 루프에 들어가 표준 입력에서 사용자 입력을 읽고(fgets() 사용) 서버로 보내고(write() 사용) 서버의 응답을 받는다.(read() 사용)
    // 사용자가 "END"를 입력할 때까지 계속되며 "END" 입력 시 루프가 종료
    while (1)
    {
        fgets(wBuff, BUFSIZ - 1, stdin); // 표준 입력 으로부터 user input 을 받는다.
        readLen = strlen(wBuff);
        if (readLen < 2)
            continue;                      // empty lines 무시
        write(mySock, wBuff, readLen - 1); // user input 을 서버로 보낸다.
        recvByte = 0;
        maxBuff = BUFSIZ - 1;
        // 서버 응답을 read 하는 루프
        do
        {
            recvByte += read(mySock, rBuff, maxBuff);
            maxBuff -= recvByte;
        } while (recvByte < (readLen - 1));
        rBuff[recvByte] = '\0';
        printf("Server: %s\n", rBuff); // 서버 응답 출력
        wBuff[readLen - 1] = '\0';
        if (!strcmp(wBuff, "END"))
            break;
    }
    printf("END ^^\n");
    close(mySock); // 소켓을 닫는다.
    return 0;
}

// 오류 발생 시 오류 메시지를 출력하고 프로그램을 종료
void errProc(const char *str)
{
    fprintf(stderr, "%s: %s \n", str, strerror(errno));
    exit(1);
}
