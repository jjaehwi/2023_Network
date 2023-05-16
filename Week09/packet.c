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

struct arp_ha {
	unsigned char ha[6];
};
struct arp_pa {
	unsigned char pa[4];
};

struct arp_packet {
	uint16_t ar_hrd;
	uint16_t ar_pro;
	uint8_t ar_hln;
	uint8_t ar_pln;
	uint16_t ar_op;
	struct arp_ha ar_sha;
	struct arp_pa ar_spa;
	struct arp_ha ar_tha;
	struct arp_pa ar_tpa;
};

int convertTextToArppa(char *, struct arp_pa *);
int convertTextToArpha(char *, struct arp_ha *);
void errProc(const char*);
int main(int argc, char** argv) 
{
	int pSocket;
	struct arp_packet * buff;
	int hdrLen;
	struct sockaddr_ll sockAddr;
	int res, i;
	int tempInt;
	unsigned char test;
	
	if(argc != 4) 
	{
		fprintf(stderr,"Usage: %s <Interface> <SenderIP> <TargetIP> \n",argv[0]);
		return -1;
	}
	memset(&sockAddr, 0, sizeof(struct sockaddr_ll));
	sockAddr.sll_family = AF_PACKET;
	sockAddr.sll_protocol =htons(ETH_P_ARP); 
	sockAddr.sll_ifindex = if_nametoindex(argv[1]);
	sockAddr.sll_halen = ETH_ALEN;
	sockAddr.sll_addr[0] = 0xff;
	sockAddr.sll_addr[1] = 0xff;
	sockAddr.sll_addr[2] = 0xff;
	sockAddr.sll_addr[3] = 0xff;
	sockAddr.sll_addr[4] = 0xff;
	sockAddr.sll_addr[5] = 0xff;


pSocket = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	buff = (struct arp_packet *) malloc(sizeof(struct arp_packet));	
	hdrLen = sizeof(struct arp_packet);
	buff->ar_hrd = htons(0x0001);
	buff->ar_pro = htons(0x0800);
	buff->ar_hln = 6;
	buff->ar_pln = 4;
	buff->ar_op = htons(0x0001);	
	convertTextToArpha("08:00:27:ea:85:20",&(buff->ar_sha));	
	convertTextToArppa(argv[2],&(buff->ar_spa));	
	convertTextToArppa(argv[3],&(buff->ar_tpa));
	for(i = 0; i< 10; i++) 	{
		res = sendto(pSocket, buff, hdrLen, 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)); 
		if(res < 0) 
		{
			fprintf(stderr,"sendto: %s \n",strerror(errno));
		}	
		printf("%d sent \n",res);	
	}
	close(pSocket);
	return 1;
}

int convertTextToArpha(char *str, struct arp_ha *address)
{
	char *ptrStr;
	int len,i,j,k;
	unsigned int temp;
	ptrStr = str;
	//strlen("xx:xx:xx:xx:xx:xx") == 17 
	if(strlen(str) != 17) errProc("convertTextToArpha address length");
	temp = k = j = 0;	
	for(i = 0;i < 17;i++)
	{
		j = i % 3;
		if(j == 2) 
		{
			ptrStr++;
			k++;
			continue;
		}

		switch(*ptrStr)
		{
			case 'a':
			case 'A':
				temp = 10;
			   	temp *= (j==0?16:1);	
				break;
			case 'b':
			case 'B':
				temp = 11;
				temp *= (j==0?16:1);
				break;

			case 'c':
			case 'C':
				temp = 12;
				temp *= (j==0?16:1);
				break;

			case 'd':
			case 'D':
				temp = 13;
				temp *= (j==0?16:1);
				break;

			case 'e':
			case 'E':
				temp = 14;
				temp *= (j==0?16:1);
				break;

			case 'f':
			case 'F':
				temp = 15;
				temp *= (j==0?16:1);
				break;
			default:
				temp = (unsigned char) *ptrStr;
				temp -= 48;
				if(j==0) temp *= 16;
		}
		if(j==0) address->ha[k] = temp;	
		if(j==1) address->ha[k] = address->ha[k] + temp;
		ptrStr = ptrStr + 1;
	}
	return 1;	
}

int convertTextToArppa(char *str, struct arp_pa *address) 
{
	char *ptrStr;
	int i;
	
	ptrStr = str;
	i = 0;
	if(address == NULL) return -1;	
	while(*ptrStr) 
	{
		if(isdigit((unsigned char)*ptrStr)) {
			address->pa[i] *= 10;
			address->pa[i] += *ptrStr - '0';
		}
		else{
			i++;
		}
		ptrStr++;
	}

	
	return 0;
}

void errProc(const char* str) {
	fprintf(stderr,"%s: %s\n", str, strerror(errno));
	exit(1);

}
