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
    int mySock, readLen, nRecv, res;
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

    // 소켓 생성: socket() 함수를 사용하여 UDP 소켓을 생성
    mySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mySock == -1)
        errProc("socket");

    // 소켓 주소 초기화: 서버 주소 정보로 sockaddr_in 구조체를 초기화
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_port = htons(atoi(argv[1]));

    // 로컬 주소 및 포트에 소켓 바인딩: bind() 함수를 사용하여 sockaddr_in 구조에 지정된 로컬 주소 및 포트에 소켓을 바인딩
    res = bind(mySock, (struct sockaddr *)&srcAddr,
               sizeof(srcAddr));
    if (res == -1)
        errProc("bind");
    addrLen = sizeof(destAddr);

    // recvfrom 함수를 사용하여 메시지를 계속 수신하고 sendto 함수를 사용하여 보낸 사람에게 다시 보내는 무한 루프
    while (1)
    {
        // 클라이언트로부터 메세지를 receive
        nRecv = recvfrom(mySock, buff, BUFSIZ - 1, 0,
                         (struct sockaddr *)&destAddr,
                         &addrLen);
        if (nRecv == -1)
            errProc("recvfrom");
        if (nRecv > 0)
            buff[nRecv - 1] = '\0';
        else
            buff[nRecv] = '\0';

        // sender 의 IP 주소와 포트 번호를 얻는다.
        strAddr = inet_ntoa(destAddr.sin_addr);
        // 받은 메세지와 sender 의 IP 주소와 포트를 출력한다.
        printf("%s:%d>%s\n", strAddr, ntohs(destAddr.sin_port), buff);
        // Sender 에게 다시 메세지를 send 한다.
        nRecv = strlen(buff);
        sendto(mySock, buff, nRecv, 0,
               (struct sockaddr *)&destAddr, addrLen);
    }
    // 소켓을 닫는다.
    close(mySock);
    return 0;
}

void errProc(const char *str)
{
    fprintf(stderr, "%s: %s \n", str, strerror(errno));
    exit(1);
}
