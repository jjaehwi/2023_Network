# Error, IPv6 and Packet Socket

- [Error 처리](#error-처리)

  - [Error 처리 예시](#error-처리-예시)

- [IPv6](#ipv6)

  - [IPv6 주소 체계 복습](#ipv6-주소-체계-복습)

- [Packet Socket](#packet-socket)

  - [Packet Socket 설정](#packet-socket-설정)

  - [예제 : Packet Socket 기반 패킷 송수신 프로그램](#packet-socket-기반-패킷-송수신-프로그램)

- [Packet Socket 응용](#packet-socket-응용)

  - [MAC address](#mac-addresses)

  - [ARP](#arp-protocol)

    - [ARP 헤더 정의](#arp-헤더-정의)

    - [ARP 패킷을 만들어내는 프로그램 예제](#arp-패킷을-만들어내는-프로그램-예제)

- [Appendix](#appendix)

---

## Error 처리

- 리눅스에서는 **함수 호출 후, 발생한 에러를 저장**하는 `errno` 라는 전역변수를 제공한다.

  - errno.h 를 include 하여 사용

  - (ex) socket 함수의 인자 중 프로토콜 패밀리가 잘못되면 socket은 -1 을 리턴하면서 종료

    - errno 변수에 저장되는 값은 `EINVAL 로 정의`되어있음

    - #define EINVAL 22

- [https://man7.org/linux/man-pages/man3/errno.3.html](https://man7.org/linux/man-pages/man3/errno.3.html)

  - 어떤 매크로들로 에러가 정의되어있는지 확인할 수 있는 사이트

- [https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html](https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html)

  - 에러와 관련된 넘버링과 어떤 에러가 있는지 확인할 수 있는 사이트

---

### Error 처리 예시

```
clntSd = socket(PF_INET, SOCK_STREAM, 0);
    if(clntSd == -1)
    {
        if (errno == EINVAL)
        {
            printf("Protocl Family Error");
            return -1;
        }
        printf("Socket Creation Error");
        return -1;
    }
```

- PF_INET 을 넣어 소켓을 만들 때, 생성에 에러가 생기면 -1 이 return 되는데 errno 를 체크해서 EINVAL 이 나오면 Protocl Family Error 를 출력하도록 한 것

---

## IPv6

<img width="592" alt="스크린샷 2023-05-04 오후 11 48 30" src="https://user-images.githubusercontent.com/87372606/236244446-33d4f859-e19b-4cdb-af6d-9399bf34dfb8.png">

- 서비스를 할 때, 서비스의 quality 를 보장하기 위해 패킷 우선순위 같은 것들이 존재한다.

  - 라우터에서 어떤 것을 먼저 처리할지 라우터 알고리즘과 관련된 traffic class

- Flow Label : 패킷 흐름 식별자 또는 필드

- 16 바이트의 주소 체계

- IP 주소에 0이 연속될 경우 생략 가능함

- IPv6의 특수 주소공간

  - `::` : 주소의 모든값을 0 으로 세팅 (IPv4의 0.0.0.0 과 같다)

  - `::1` : Loopback 주소 (IPv4의 127.0.0.1 과 같다)

```
(ex1)
fe80:0000:0000:1234:5678:9012:3456:7890
--> fe80:0:0:1234:5678:9012:3456:7890
--> fe80::1234:5678:9012:3456:7890

(ex2) IPv6 를 위한 ping 프로토콜을 통해 Loopback 으로 ping 을 보냄
ping6 ::1
```

---

### IPv6 주소 체계 복습

- Week4 - article2 에서 배운 내용 복습

- IPv4 와 달리 추가적인 field 들이 존재한다.

<img width="592" alt="스크린샷 2023-05-05 오전 12 07 30" src="https://user-images.githubusercontent.com/87372606/236249588-99539aeb-c8fe-433a-a027-381ae70d5cbb.png">

<img width="592" alt="스크린샷 2023-05-05 오전 12 07 40" src="https://user-images.githubusercontent.com/87372606/236249627-4176aeb0-a309-49e0-ad6e-192a8eed8788.png">

<img width="592" alt="스크린샷 2023-05-05 오전 12 07 50" src="https://user-images.githubusercontent.com/87372606/236249667-d806e4c8-84fe-4587-92c6-031a620253d2.png">

<img width="592" alt="스크린샷 2023-05-05 오전 12 08 01" src="https://user-images.githubusercontent.com/87372606/236249719-5fa104f4-c026-4864-884f-dd74d6b5432b.png">

---

## Packet Socket

<img width="605" alt="스크린샷 2023-05-05 오전 12 25 28" src="https://user-images.githubusercontent.com/87372606/236254627-4e27a5b1-4beb-43cf-bd76-49936e0a5813.png">

- `Packet Socket` 은 `네트워크 7계층의 데이터링크 계층에 대해 수정을 가능한 소켓`이다.

- Low 소켓은 IP Header 까지 수정을 할 수 있었다.

- Packet 소켓은 데이터링크 계층까지 수정이 가능하고, MAC 주소가 존재하는 헤더 (Link-Layer Header) 까지 수정이 가능하다.

- `socket(AF_INET, SOCK_STREAM, ...)`, `socket(AF_INET, SOCK_DGRAM, ...)` 을 통해 TCP, UDP 소켓을 만드는데 이때는 Application Data 만 조작이 가능했고,

- `socket(AF_INET, SOCK_RAW, IP_HDRINCL=FALSE)` 는 TCP/UDP 헤더를 Application Layer 에서 조작이 가능하다.

- `socket(AF_INET, SOCK_RAW, IP_HDRINCL=TRUE)` 는 IP 헤더 정보까지 수정이 가능했다.

- `socket(AF_PACKET, SOCK_DGRAM, ...)` 은 IP 헤더까지 제어가 가능하고, `socket(AF_PACKET, SOCK_RAW, ...)` 를 통해 Link-Layer 헤더까지 조작이 가능하다.

  - MAC 주소 기반의 프로토콜인 ARP 프로토콜을 어떻게 처리하는지 살펴본다.

---

### Packet Socket 설정

```
#include <sys/socket.h>
#include <netpacket/packet.h>
socket(AF_PACKET, int socket_type, int protocol);
```

- Packet Socket 을 사용하기 위해서는 `AF_PACKET` 을 설정해야 함

- `socket_type` 에는 **SOCK_RAW** 와 **SOCK_DGRAM** 이 사용될 수 있음

  - SOCK_RAW : 프로그래머가 네트워크의 2 계층 헤더까지 작업하는 것을 허용

    - MAC 주소 변경을 허용

  - SOCK_DGRAM : 네트워크의 2 계층 헤더는 자동으로 생성되며 2 계층의 데이터 부분을 프로그래머가 작성 할 수 있음

    - MAC 주소 변경 불가능

- `protocol` 에는 IEEE 802.3 프로토콜 (Ethernet 프로토콜 표준명) 에서 정의하는 프로토콜 코드를 입력함

<img width="563" alt="스크린샷 2023-05-05 오전 12 34 26" src="https://user-images.githubusercontent.com/87372606/236256904-1ef74340-27c4-4c4a-81e7-5a75005e23ac.png">

<img width="563" alt="스크린샷 2023-05-05 오전 12 34 53" src="https://user-images.githubusercontent.com/87372606/236257023-ea62170f-a417-4672-9b0e-0a203b42b7c1.png">

- `Packet 소켓은 Frame Header (Mac 헤더) 를 수정할 수 있다.`

- Application 계층에선 Data 를 만들고, Transparent 계층에선 TCP/UDP 헤더를 만든다.

- `SOCK_STREAM` 이나 `SOCK_DGRAM` 을 통해 `TCP/UDP 헤더` 를 OS 에서 자동적으로 만들어주는데

  - `SOCK_RAW` 를 통해 `TCP/UDP 헤더를 수정`할 수 있는 것이고,

  - `IP_HDRINCL=TRUE` 까지 해서 `IP 헤더까지 수정`이 가능하다.

  - 거기에 `AF_PACKET` 을 쓰면 `Frame 헤더까지 수정`이 가능한 것이다.

- 물리계층 헤더를 제외한 MAC 주소인 destination address(DA), source address(SA), ethernet type, data 등을 수정할 수 있다.

- **`Packet Socket 을 위한 구조체 `**

```
struct sockaddr_ll {
    unsigned short sll_family;      /* Always AF_PACKET */
    unsigned short sll_protocol;    /* Physical-layer protocol */
    int sll_ifindex;                /* Interface number */
    unsigned short sll_hatype;      /* ARP hardware type */
    unsigned char sll_pkttype;      /* Packet type */
    unsigned char sll_halen;        /* Length of address */
    unsigned char sll_addr[8];      /* Physical-layer address */
};
```

---

### Packet Socket 기반 패킷 송수신 프로그램

<img width="526" alt="스크린샷 2023-05-05 오전 1 03 46" src="https://user-images.githubusercontent.com/87372606/236262727-637607c6-1d9d-44ac-9834-4f5a04602a55.png">

<img width="573" alt="스크린샷 2023-05-05 오전 1 24 28" src="https://user-images.githubusercontent.com/87372606/236265400-aa2dab79-1105-4b5b-9a6f-bf3c6dcad670.png">

- Packet Socket 을 다루기 위한 구조체 변수 선언

  - sockAddr 들어가는 family 는 AF_PACKET

  - ARP 프로토콜을 설정 (host 정보이기 때문에 htons 로 byte order 를 network 로)

  - 이더넷 인터페이스를 사용하기 때문에 이더넷 인터페이스 이름을 가져와서 적음 (string 을 index 형태로 바꾸는 함수 : if_nametoindex)

  - length 를 미리 선언된 이더넷 length 로 선언

- 소켓을 만들 때, AF_PACKT, SOCK_DGRAM 을 선언 (MAC 주소를 수정하지 않기 때문)

- 선언된 버퍼에 TestPacket 을 넣은 후 해당 데이터를 설정한 sockAddr 로 보낸다. (sendto)

  - ARP 패킷을 보내지만 제대로 패킷 형태를 갖추지 않고 보냈기 때문에 이상한 데이터가 sendto 를 통해 가는 것을 확인할 수 있다.

- 해당 10 개의 패킷에 대한 response 도 recvfrom 을 통해 받는다.

<img width="524" alt="스크린샷 2023-05-05 오전 1 29 58" src="https://user-images.githubusercontent.com/87372606/236266925-ba9bbdd4-f50b-4cc3-9e9c-d83c19eb108a.png">

- 10 개의 데이터를 보내고 그에 해당하는 10 개의 데이터를 받는 것을 확인할 수 있다.

<img width="545" alt="스크린샷 2023-05-05 오전 1 30 33" src="https://user-images.githubusercontent.com/87372606/236267090-fc22f0b1-e5b4-4551-bf1d-f71695fe5504.png">

- 제대로 ARP 패킷 을 만들어 보내지 않았기 때문에 Malformed Packet 이라고 명시된다.

---

## Packet Socket 응용

---

### MAC addresses

- Packet Socket 을 이용해서 ARP query 를 보내본다.

- 32 bit IP address : Network - layer 에서 라우팅하기위해 사용되는 주소

  - 네트워크 망을 통해 먼 곳으로 가기 위해 필요한 정보

- `MAC (or LAN or physical or Ethernet) address` : used `locally` to get frame from one interface to another physically connected interface (same network, in IP-addressing sense)

  - local 하게 통신하기 위해 필요한 정보

  - 48 비트로 : 으로 구분되는 정보

  - LAN 카드의 MAC 주소는 고유함 (Although MAC addresses were designed to be `permanent`, **it is now possible to change an adapter’s MAC address via software**)

<img width="258" alt="스크린샷 2023-05-05 오전 1 42 58" src="https://user-images.githubusercontent.com/87372606/236270236-3ef91e91-59ea-4215-b07b-a02db5f607f4.png">

<img width="279" alt="스크린샷 2023-05-05 오전 1 44 59" src="https://user-images.githubusercontent.com/87372606/236270670-397776fb-f38f-4d87-bb5e-490fd085e366.png">

- 모든 local area network 인터페이스에는 MAC 주소가 할당되어있고 unique 해야한다.

- 네트워크 adapter 가 데이터를 보낼 땐 해당 MAC 주소를 통해 데이터를 보낼 수 있다.

  - 전체 내부의 전체에 데이터를 보내고 싶으면 `special MAC broadcast address` (FF-FF-FF-FF-FF-FF) 로 설정해서 보낸다.

---

- MAC 주소는 IEEE 에서 관리한다.

  - Manufacturer buys portion of MAC address space (to assure uniqueness)

- `Analogy`

  - MAC address : 주민등록번호처럼 고유

  - IP address : postal address

- **MAC flat address : `portability`**

  - LAN 카드를 다른 곳에 꽂아도 동작 가능

- `IP hierarchical address not portable`

  - IP 주소 subnet 에 기반해서 바뀌기 때문

---

### ARP (Address Resolution Protocol)

<img width="630" alt="스크린샷 2023-05-05 오전 1 54 33" src="https://user-images.githubusercontent.com/87372606/236272832-595f1fd4-e336-49f2-80ae-e01c4e63f10e.png">

- `ARP : MAC 주소를 알아오는 프로토콜`

- IP 를 모르기 때문에 domain 을 쳐서 결론적으로 IP 에 접근해서 통신하는 것처럼..

  - 통신을 하기 위해 해당 기기의 MAC 주소도 알아야하는데 MAC 주소에는 domain 이 없으므로 ARP protocol 을 통해 MAC 주소를 알아옴

- 모든 PC 들은 자신만의 ARP 테이블을 갖고 있다. 이 테이블에 자신이 접속했던 서버들의 MAC 주소를 저장하고 있다.

  - IP 주소와 IP 주소에 해당하는 MAC 주소, 저장하는 기간 (TTL) 을 관리하는 것

  - **새로운 IP 주소가 왔을 때, 이에 해당하는 MAC 주소가 없다면 자체적으로 ARP 를 구동해서 IP 를 가진 PC 에게 MAC 주소가 뭔지 물어보고 response 를 받아와서 저장할 수 있게 되는 것이다.**

<img width="630" alt="스크린샷 2023-05-05 오전 1 58 26" src="https://user-images.githubusercontent.com/87372606/236273727-79b8d018-c332-405d-af6c-c79a954a92b1.png">

1. 머신 A 가 머신 B 에게 데이터를 보내고 싶을 때 (둘은 같은 인트라)

   - A 는 B 의 IP 주소를 아는데, 통신을 처음하기 때문에 MAC 주소는 모르는 상황

   - B 의 MAC 주소가 A 의 ARP table 에 없는 것

   - 통신을 할 수 없는 상황 (주소를 세팅할 때 IP 를 세팅하고 MAC 주소도 세팅하기 때문)

   - 이전에는 OS 를 신경쓰지 않고 상위 계층에서 IP 를 세팅해서 통신하면 MAC 주소가 있는지 없는지 신경안쓰고 OS 에서 자체적으로 ARP 를 실행하고 관리했었다. 하지만 지금은 MAC 주소까지 신경을 써야하는 상황 (firmware / embedded)

2. A 는 ARP query 를 만들고, 이 query 에 B 의 IP 주소를 포함시킨다.

   - A 는 주변 인트라에게 전부 물어보는 것이다. 해당 IP 를 가진 애가 누군지 물어보고 해당한다면 MAC 주소를 보내달라는 ARP query 를 작성한 것

   - 궁극적으로 인트라 에서는 MAC 주소로 통신을 하기 때문에 destination MAC 주소를 broadcase 주소로 세팅한다. (FF-FF-FF-FF-FF-FF)

   - source : A 머신의 MAC 주소, destination : broadcase

   - LAN 에 있는 모든 노드들은 ARP query 를 받을 수 있다.

3. B 가 A 와 같은 인트라에 있다면 당연히 B 도 A 가 보낸 ARP 패킷을 받게 된다. 그리고 자신의 MAC 주소를 A 에게 reply 한다.

   - source 가 A 머신의 MAC 주소와 IP 주소였기 때문에 이를 다시 destination 으로 세팅해서 자신의 MAC 주소를 source 로 보내는 것 (unicast)

4. 응답을 받은 A 는 IP - to - MAC 쌍을 ARP 테이블에 만든다.

   - TTL 시간동안 IP 와 MAC 의 mapping 관계를 저장하고 있는 것.

- `ARP 는 plug - and - play 성질을 가진다.`

  - 따로 세팅할 필요없이 어디에서나 적용 가능

<img width="555" alt="스크린샷 2023-05-05 오전 2 10 48" src="https://user-images.githubusercontent.com/87372606/236276727-5f8998ac-1763-4c68-b6a4-77405c2e0a10.png">

- MAC 주소 길이 6 bytes, IP 주소 길이 4 bytes

- 목적지 MAC 주소는 현재 내가 모르기 때문에 broadcase 주소로 세팅이 되어야하고, IP 주소는 알고 있다는 가정하에 ARP 가 동작한다.

---

### ARP 헤더 정의

- 리눅스 머신에서는 <linux.if_arp.h> 에 정의되어 있다.

```
struct arp_ha {
    unsigned char ha[6]; // hardware 정보 (MAC 주소)
};

struct arp_pa {
    unsigned char pa[4]; // 프로토콜 address 인 IP 주소
};

struct arp_packet {
    uint16_t ar_hrd;    // hardware, protocol 타입, 이더넷을 의미하는 인자
    uint16_t ar_pro;    // IP protocol 을 의미하는 인자
    uint8_t ar_hln;     // hardware MAC 주소의 길이
    uint8_t ar_pln;     // IP 주소의 길이
    uint16_t ar_op;     // operation 의 값
    struct arp_ha ar_sha;   // source hardware 주소
    struct arp_pa ar_spa;   // source IP 주소
    struct arp_ha ar_tha;   // destination hardware 주소
    struct arp_pa ar_tpa;   // destination IP 주소
};
```

- IP 주소 및 MAC 주소 처리 함수 정의 (예제 참고)

  - `convertTextToArppa`

    - IP 주소 일반 텍스트로 받을 경우, 해당 데이터를 struct arp_pa 로 변경

    - (ex) 192.168.0.1 을 struct arp_pa 로 변경

  - `convertTextToArpha`

    - MAC 주소 일반 텍스트로 받을 경우, 해당 데이터를 struct arp_ha 로 변경

    - (ex) aa:aa:aa:aa:aa:aa 을 struct arp_ha 로 변경

---

### ARP 패킷을 만들어내는 프로그램 예제

- `ARP flooding attack 주의!` (자체적으로 구축한 네트워크 혹은 자신의 AP 내부망에서 수행할 것)

  - ARP 패킷을 의미없이 보내기 때문에 밴 당할 수도 있으므로 본인이 구축한 망에 대해서만 사용해야한다.

<img width="610" alt="스크린샷 2023-05-05 오전 2 20 05" src="https://user-images.githubusercontent.com/87372606/236280485-24907495-c0b9-4337-beb6-dbc4454064ec.png">

<img width="610" alt="스크린샷 2023-05-05 오전 2 20 19" src="https://user-images.githubusercontent.com/87372606/236280616-afcc47d7-3776-4f4b-8c16-ba630db0487c.png">

- `ARP 패킷에 대한 구조체` 포인터 : struct arp_packet \* buff

- `packet 소켓을 쓸 수 있는 구조체` : struct sockaddr_ll sockAddr

- 선언된 packet 소켓 구조체를 세팅한다.

  - ARP 선언 : sockAddr.sll_protocol =htons(ETH_P_ARP)

  - main 에 들어온 argument 를 전달 : sockAddr.sll_ifindex = if_nametoindex(argv[1])

  - 이더넷 length : sockAddr.sll_halen = ETH_ALEN

  - destination address 는 broadcase 인 0xff 로 세팅

- 소켓을 만들고 난 후 만들려고 하는 ARP 패킷의 정보를 넣을 수 있다. (buff 라는 ARP 패킷 세팅)

  - 테스트하는 머신 (host) 의 MAC 주소를 hard 하게 세팅 : convertTextToArpha 를 통해 buff 에 세팅

  - convertTextToArppa 를 통해 테스트하는 머신 (host) 의 IP 주소를 세팅

  - convertTextToArppa 를 통해 알고자 하는 MAC 주소와 연관된 IP 주소 세팅

- 이후 그 값을 기준으로 packet 을 보냄 (sendto)

<img width="610" alt="스크린샷 2023-05-05 오전 2 26 43" src="https://user-images.githubusercontent.com/87372606/236282062-3188d2ec-d59a-414f-b3f9-a57a6a7c77fd.png">

- 관리자 권한으로 수행.

- ifconfig 로 이더넷 인터페이스를 확인하여 넣고 host 머신의 IP 주소와 알고싶은 MAC 주소에 대한 IP 주소를 전달한다.

- Who has 192.186.56.1 : 누가 이 IP 주소를 가지고 있는지.. request 형태 인 것

---

## Appendix

- packet 소켓을 사용할 때, 소켓 타입을 SOCK_RAW 로 세팅해야 MAC 주소를 수정할 수 있다고 했는데, 반은 맞고 반은 틀리다.

  - SOCK_DGRAM 에서도 어느정도 수정이 가능하다.

  - 방금 예제에서도 SOCK_DGRAM 을 통해 소켓의 destination 을 0xff 로 수정한 것처럼..

- `SOCK_RAW` 는 자신의 source MAC 주소와 destination MAC 주소까지 전부 수정할 때 사용

  - 전부 세팅해야한다.

- `SOCK_DGRAM` 은 도움을 받아 어느정도까지 sockaddr_ll 구조체를 이용해서 MAC 주소의 수정이 허용되는 것

  - 예제의 경우 `destination MAC 주소만 수정을 하면 OS 에서 나머지 를 채워서 보낸다.`

---
