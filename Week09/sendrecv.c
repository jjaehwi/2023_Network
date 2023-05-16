#include <sys/socket.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>

void errProc(const char*);
int main(int argc, char** argv) 
{
	int pSocket;
	char buff[BUFSIZ];
	int hdrLen;
	struct sockaddr_ll sockAddr;
	int res,i;
	int addrLen;
	unsigned char test;

	memset(&sockAddr, 0, sizeof(struct sockaddr_ll));
	sockAddr.sll_family = AF_PACKET;
	sockAddr.sll_protocol =htons(ETH_P_ARP); 
	sockAddr.sll_ifindex = if_nametoindex("enp0s3");
	sockAddr.sll_halen = ETH_ALEN;
	
	pSocket = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	strcpy(buff,"TestPacket");
	hdrLen = 10;
	for(i=0;i<10;i++)	{
		res = sendto(pSocket, buff, hdrLen, 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)); 
		if(res < 0) 
		{
			fprintf(stderr,"sendto: %s \n",strerror(errno));
		}	
		printf("%d sent \n",res);	
	}

	for(i=0;i<10;i++)	{
		res = recvfrom(pSocket, buff, BUFSIZ-1 , 0, (struct sockaddr *)&sockAddr,&addrLen);
		if(res < 0)
		{
			fprintf(stderr,"sendto: %s \n",strerror(errno));
		}	
		printf("%d recv \n",res);
	}
	close(pSocket);
	return 1;
}

void errProc(const char* str) {
	fprintf(stderr,"%s: %s\n", str, strerror(errno));
	exit(1);

}
