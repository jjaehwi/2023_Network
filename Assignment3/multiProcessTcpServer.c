#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/wait.h>

void errProc();
void errPrint();
void sigchldHandler();
int main(int argc, char **argv)
{
    int srvSd, clntSd;
    struct sockaddr_in srvAddr, clntAddr;
    int clntAddrLen, readLen, strLen;
    char rBuff[BUFSIZ];
    pid_t pid;

    if (argc != 2)
    {
        printf("Usage: %s [port] \n", argv[0]);
        exit(1);
    }
    printf("Server start...\n");

    srvSd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srvSd == -1)
        errProc("socket error");

    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(atoi(argv[1]));

    if (bind(srvSd, (struct sockaddr *)&srvAddr, sizeof(srvAddr)) == -1)
        errProc("bind error");

    if (listen(srvSd, 5) < 0)
        errProc("listen error");

    clntAddrLen = sizeof(clntAddr);

    // SIGCHLD 시그널 핸들러 등록
    signal(SIGCHLD, sigchldHandler);

    while (1)
    {
        clntSd = accept(srvSd, (struct sockaddr *)&clntAddr, &clntAddrLen);
        if (clntSd == -1)
        {
            errPrint("accept error");
            continue;
        }

        printf("client %s:%d is connected...\n",
               inet_ntoa(clntAddr.sin_addr),
               ntohs(clntAddr.sin_port));

        pid = fork();
        if (pid == 0)
        { /* child process */
            close(srvSd);

            while (1)
            {
                readLen = read(clntSd, rBuff, sizeof(rBuff) - 1);
                if (readLen == 0)
                    break;
                rBuff[readLen] = '\0';
                printf("Client(%d): %s\n",
                       ntohs(clntAddr.sin_port), rBuff);
                write(clntSd, rBuff, strlen(rBuff));
            }

            printf("Client(%d): is disconnected\n",
                   ntohs(clntAddr.sin_port));

            close(clntSd);
            return 0;
        }

        else if (pid == -1)
            errProc("fork error");

        else
        { /*Parent Process*/
            close(clntSd);
        }
    }
    close(srvSd);
    return 0;
}

void errProc(const char *str)
{
    fprintf(stderr, "%s: %s \n", str, strerror(errno));
    exit(1);
}

void errPrint(const char *str)
{
    fprintf(stderr, "%s: %s \n", str, strerror(errno));
}

void sigchldHandler(int sig)
{
    // 현재 errno 값을 저장
    int saved_errno = errno;
    pid_t pid;
    int status;

    // 자식 프로세스가 종료되었는지 확인
    // waitpid 함수의 첫 번째 인자 -1 은 자식 프로세스 중 아무거나 종료되었을 때를 대기하겠다는 뜻
    // 두 번째 인자인 status 변수에는 자식 프로세스의 종료 상태를 저장
    // 마지막 인자인 WNOHANG 은 자식 프로세스가 종료될 때까지 무한히 대기하지 않고, 바로 반환하여 부모 프로세스가 다른 작업을 처리할 수 있도록 함.
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // 자식 프로세스가 종료되면, waitpid 함수는 종료된 자식 프로세스의 PID를 반환하고, while 문으로 들어가서 출력
        printf("child process %d is terminated\n", pid);
    }
    // errno 값 복원
    errno = saved_errno;
}