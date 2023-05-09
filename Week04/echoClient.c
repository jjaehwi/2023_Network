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
    int mySock, readLen, nSent, nRecv;
    char buff[BUFSIZ];
    char strAddr;
    struct sockaddr_in destAddr;
    socklen_t addrLen;

    mySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 서버의 주소 구조는 초기화되어 destAddr 구조체에 저장
    // 서버의 IP 주소는 명령줄 인수에서 가져오고 inet_addr을 사용하여 이진 형식으로 변환
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_addr.s_addr = inet_addr(argv[1]);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(atoi(argv[2]));
    addrLen = sizeof(destAddr);

    while (1)
    {
        // fgets를 사용하여 사용자로부터 입력을 읽고 sendto를 사용하여 서버로 보낸다.
        fgets(buff, BUFSIZ - 1, stdin);
        readLen = strlen(buff);
        // 보낸 바이트 수는 nSent에 저장
        nSent = sendto(mySock, buff, readLen, 0,
                       (struct sockaddr *)&destAddr, addrLen);
        // 전송 중 오류가 발생하면 해당 오류 메시지와 함께 errProc 함수가 호출
        if (nSent == -1)
            errProc("write");
        // 서버로부터의 응답은 recvfrom을 사용하여 수신되고 buff에 저장
        // 받은 바이트 수는 nRecv에 저장
        nRecv = recvfrom(mySock, buff, BUFSIZ - 1, 0,
                         (struct sockaddr *)&destAddr, &addrLen);
        // 수신 중 오류가 발생하면 해당 오류 메시지와 함께 errProc 함수가 호출
        if (nRecv == -1)
            errProc("read");
        buff[nRecv] = '\0';
        // 받은 메시지는 출력
        printf("Server: %s\n", buff);
        // 수신된 메시지가 "END"이면 루프가 종료, 루프는 사용자가 "END"를 입력할 때까지 계속된다.
        if (!strcmp(buff, "END"))
            break;
    }
    // close 시스템 호출을 사용하여 소켓을 닫고 프로그램을 종료
    close(mySock);
    return 0;
}

// errProc 함수는 strerror를 사용하여 얻은 오류 번호 및 설명과 함께 콘솔에 오류 메시지를 출력
// 오류가 발생하면 프로그램은 상태 코드 1로 종료
void errProc(const char *str)
{
    fprintf(stderr, "%s: %s \n", str, strerror(errno));
    exit(1);
}
