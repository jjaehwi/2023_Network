// ssu20180598 이재휘
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int printAddr(struct sockaddr_in *);
int main(char argc, char **argv)
{
	char *sampleIP = "127.0.0.1";
	int port = 9002;

	struct sockaddr_in sockAddr1, sockAddr2, sockAddr3;

	sockAddr1.sin_family = AF_INET;
	sockAddr1.sin_addr.s_addr = inet_addr(sampleIP);
	sockAddr1.sin_port = htons(port);

	sockAddr2.sin_family = AF_INET;
	inet_aton(sampleIP, &(sockAddr2.sin_addr));
	sockAddr2.sin_port = htons(port);

	sockAddr3.sin_family = AF_INET;
	inet_pton(AF_INET, sampleIP, &(sockAddr3.sin_addr));
	sockAddr3.sin_port = htons(port);

	printAddr(&sockAddr1);
	printAddr(&sockAddr2);
	printAddr(&sockAddr3);

	printf("============ntoa==============\n");
	printf("IP:%s \n", inet_ntoa(sockAddr1.sin_addr));
	printf("IP:%s \n", inet_ntoa(sockAddr2.sin_addr));
	printf("IP:%s \n", inet_ntoa(sockAddr3.sin_addr));

	return 0;
}
// A.	inet_ntop()함수를 사용하지 않아야 함
// B.	inet_ntop()함수를 사용하지 않지만, 변경된 printAddr2(struct sockaddr_in *myAddr)함수는
// 		변경전 printAddr(struct sockaddr_in *myAddr)함수와 동일한 기능 (주소 출력 기능)을 제공해야 함
// 답 : inet_ntoa 로 네트워크 바이트 순서인 구조체의 포인터를 인자로 받아 Dotted-Decimal Notation 의 주소값으로 변환하여 출력한다.
int printAddr(struct sockaddr_in *myAddr)
{
	int port;
	char txt[INET_ADDRSTRLEN];
	port = ntohs(myAddr->sin_port);
	// inet_ntop(AF_INET, &(((struct sockaddr_in *)myAddr)->sin_addr),
	// 		  txt, sizeof(struct sockaddr_in));
	// printf("IP:%s, Port:%d \n", txt, myAddr->sin_port);
	printf("IP:%s, Port:%d \n", inet_ntoa(myAddr->sin_addr), myAddr->sin_port);

	return 0;
}