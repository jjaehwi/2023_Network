#  소켓 API

- [네트워크 주소](#네트워크-주소)

- [소켓 주소](#소켓-주소)

- [바이트 순서 (Byte order)](#바이트-순서)

- [주소 변환](#주소-변환)

- [소켓 프로그램](#소켓-프로그램)

## 네트워크 주소

- 다른 호스트상에서 실행중인 두 프로세스가 정보를 주고 받으려면 두 개의 주소가 필요하다.

    - **호스트를 식별**하기 위한 `IP 주소`

    - 호스트에 도착한 데이터를 **해당 프로세스에 전달**하기 위한 `포트 번호`

- IPv4 : 32비트 – 8비트 단위로 . 으로 구분하여 10진수 4개 표기 (ex. 147.46.114.70)

- IPv6 : 128비트 – 16비트 단위로 : 으로 구분하여 16진수 8개 표기 (ex. 2001:0230:abcd:ffab:0023:eb00:ffff:1111)

- 32 비트의 IPv4 주소의 앞부분은 네트워크 주소를 의미하고, 뒷부분은 호스트 주소를 의미한다. 범위에 따라 A, B, C, D, E 5개의 클래스로 구분한다.

<img width="661" alt="스크린샷 2023-03-23 오후 2 13 49" src="https://user-images.githubusercontent.com/87372606/227109650-93c38337-4877-4344-8d8f-42bf404ab206.png">

---

### 네트워크 주소 : CIDR 방식

- 네트워크 주소를 바이트 별로 구분할 때, `낭비가 발생하는 현상을 막아주는 방법`이다.

- (ex) 10.10.1.32/27 

    - 뒤에 `/숫자` 는 CIDR 표현법이다. 
    
    - 상위 /숫자 만큼의 비트가 `네트워크 주소`로 사용되고 있다는 것이고, **나머지 비트로 호스트를 표현**한다.

    - 원하는 주소 대역에서 상위 몇 비트만 네트워크 주소 체계로 사용할 수 있는 방법이다.

---

### 네트워크 주소 : IPv4 사설 주소

<img width="645" alt="스크린샷 2023-03-23 오후 2 17 10" src="https://user-images.githubusercontent.com/87372606/227110127-ac4c492a-2494-493f-b341-53cccb9486b1.png">

- 사설 주소를 RFC1918에서 정의해놨다.

---

### 네트워크 주소 : Loopback 인터페이스

- Lookback 주소는 `호스트를 지칭하는 주소`이다.

- 네트워크 주소에는 Lookback 인터페이스가 존재한다.

- IPv4 의 경우 `127.0.0.1` 로 사용한다.

    - 해당 주소는 같은 호스트 내에서 실행 중인 프로세스가 소켓 인터페이스로 통신할 때 사용한다.

    - 호스트 명은 `localhost` 이다.

---

### 네트워크 주소 : 포트 번호

<img width="628" alt="스크린샷 2023-03-23 오후 2 19 33" src="https://user-images.githubusercontent.com/87372606/227110607-9b8095b1-6baa-48c9-903a-5998c7a0e173.png">

- 호스트에서 `프로세스를 찾아가기 위해 포트 번호`를 알아야한다.

- 일반적으로 상용화된 어플리케이션들은 잘 알려진 포트 번호를 사용한다.

## 소켓 주소

- 네트워크 통신을 위해서는 아래 사항들이 결정되어야 한다.

    - 전송 계층 프로토콜 : TCP, UDP

    - 네트워크 계층 프로토콜 : IP

    - Source 호스트의 IP 주소

    - Destination 호스트의 IP 주소

    - Source 프로세스의 포트 번호

    - Destination 프로세스의 포트 번호

### 소켓 주소 관련 구조체

<https://github.com/torvalds/linux/blob/master/include/linux/socket.h>

```
struct sockaddr {
    sa_family_t sa_family /* address family, AF_xxx */
    char sa_data[14] /* 14 bytes of protocol address */
}
```

- <sys/socket.h> 헤더는 적어도 다음과 같은 멤버들을 포함하고 있는 `sockaddr 구조체`를 정의한다. 

    - sa_family_t sa_family (Address family)

    - char sa_data[] (Socket address (variable-length data))

- `sockaddr` 은 **다른 프로토콜과의 확장성을 위해 정의된 구조체** 이다.

- struct sockaddr 를 통해 소켓을 생성하고 거기에 정보를 저장할 수 있다.

---

- `sockaddr_in`, `in_addr` 구조체

```
struct sockaddr_in {
    sa_family_t sin_family;     /* address family: AF_INET */ 
    in_port_t sin_port;         /* port in network byte order */ 
    struct in_addr sin_addr;    /* internet address */
};

/* Internet address. */ 
struct in_addr {
    uint32_t s_addr;            /* address in network byte order */
};
```

- 소켓 `address family 에 대한 정보`가 있고, `port 넘버`가 있고, `실제 IP 주소가 저장되는 address` 가 구조체로 정의되어 있다. 

- IP 주소가 저장되는 address 를 담는 in_addr 구조체 는 unsigned int 32비트의 address를 할당할 수 있는 공간이 확보되어 있다.

- 실제 사용할 때 IPv4, IPv6 사용할지 정하고, 포트 넘버를 할당하고, IP주소를 넣어줘서 사용하면 된다.

---

- `소켓에 주소를 설정하는 bind 함수`

```
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- 소켓에 주소를 설정하는 함수 bind 의 argument

    - bind 에서도 어떤 소켓에 주소를 할당할 것인지 명시해줘야한다. 소켓도 파일 디스크립터가 있기 때문에 소켓의 파일 디스크립터가 들어간다.

    - 지정할 address 가 들어가고

    - 총 길이를 알려준다.

성공적으로 return 시 0, 실패시 -1

## 바이트 순서

- 메모리에 데이터를 저장할 때 `바이트 순서`가 중요하다.

    - 빅 엔디안 (big - endian) : 상위 바이트 값이 메모리의 작은 주소에 저장

    - 리틀 엔디안 (little - endian) : 하위 바이트 값이 메모리의 작은 주소에 저장 (intel 아키텍처)

<img width="567" alt="스크린샷 2023-03-23 오후 4 35 37" src="https://user-images.githubusercontent.com/87372606/227134665-48f47433-d2da-4b8a-b9c0-6cc289c5eef4.png">

- 네트워크를 통해 데이터를 전달한다.
    
    - 여러개의 라우터를 거쳐 데이터가 전송된다.

    - IP 주소를 할당하고 포트넘버를 할당하고 패킷을 다른 호스트로 보낼 때, 첫번째 호스트를 떠난 데이터가 라우터를 지날 때 라우터에서 보낸 데이터는 제일 앞에 영역, 그리고 호스트에 도착해서 포트번호로 프로세스를 찾아갈 때는 중간 영역, 그리고 데이터는 제일 뒤 영역에 있다.

    - **호스트가 데이터를 읽는 방법**이랑 **네트워크를 통해 전달되는 데이터가 보내지는 방법**이랑 다르다면 (`호스트와 라우터가 데이터를 읽는 방법에 차이가 존재하면`) 그 때, 보내고자 했던 데이터가 전달이 잘 안될 수 있다.

- 프로토콜 구현을 위해 필요한 정보

    - (a) IP 주소 -> 빅 엔디안

    - (b) 포트 번호 -> 빅 엔디안

- 응용 프로그램이 주고 받는 데이터

    - (c) 빅 엔디안 또는 리틀 엔디안으로 통일

<img width="405" alt="스크린샷 2023-03-23 오후 4 39 19" src="https://user-images.githubusercontent.com/87372606/227135372-8715bec1-7f4c-4b08-92dd-fd5717432500.png">

---

### 바이트 순서 변경 함수

- Intel 계열 컴퓨터는 리틀 엔디언 방식을 사용하므로 네트워크로 데이터를 전송하기 전에 빅 엔디언 방식으로 변경해야한다.

```
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);      // host-to-network-long 
uint16_t htons(uint16_t hostshort);     // host-to-network-short 
uint32_t ntohl(uint32_t netlong);       // network-to-host-long
uint16_t ntohs(uint16_t netshort);      // network-to-host-short
```

<img width="506" alt="스크린샷 2023-03-23 오후 4 43 35" src="https://user-images.githubusercontent.com/87372606/227136214-4dbeddb6-50de-4f1b-856a-752bdba05695.png">

- (ex)

```
struct sockaddr_in srvAddr;
Int port = 9001;
...
srvAddr.sin_port = htons(port);
```

- 포트넘버를 할당했을 때, intel 아키텍처 일 때 컴파일이 되면 리틀 엔디언 방식으로 데이터가 저장되어있다. 근데 네트워크로 보내면 `네트워크는 빅 엔디언으로 전달되어야한다고 정의`를 한다.

- 그렇기 때문에 sockaddr 구조체의 포트번호 변수를 호스트 저장방식으로 저장하는 것이 아닌 `호스트에서 네트워크로 가야하는 데이터`니까 `htons 함수`를 호출해서 포트 번호가 저장된 형태의 바이트 순서를 바꿔줘야한다.

- `응용 프로그램에서 소켓 함수로 데이터를 넘겨줄 때는 hton 함수를 써야한다.`

    - 소켓을 통해 저장된 데이터는 항상 네트워크를 통해 전달이 되기 때문이다.

    - `데이터를 읽을 땐` ntoh 함수를 통해 **네트워크 바이트 순서에서 호스트 바이트 순서로 바꿔줘야한다.**

## 주소 변환

- 네트워크 주소를 변환하는 함수에 대해 알아본다.

- hton 이나 ntoh 는 함수의 input 이 unsigned int 형이었다. 하지만 **IPv4 는 일반적으로 . 으로 8비트씩 나눠서 이뤄져있다.** (`IPv4 numbers – and – dots notation`)

- 이 자체를 호스트 바이트 순서에서 네트워크 바이트 순서로 바꿔줄 일이 발생한다. 그렇기 때문에 IP 주소 변환함수가 추가적으로 제안된다.

- IP 주소를 다룸에 있어서 이것을 지원하는 구조체(소켓에 주소를 할당하는 소켓 구조체)가 있기 때문에 이걸 지원하는 특별한 함수가 필요했고, 이것은 `소켓 API` 로 제공된다.


```
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

in_addr_t inet_addr(const char *cp);
in_addr_t inet_network(const char *cp);
int inet_aton(const char *cp, struct in_addr *inp); 
char *inet_ntoa(struct in_addr in);
```

---

### inet_addr

- char 형의 배열을 받는다. (string 형태로 들어올 것) 

- **host 주소**를 (IPv4 numbers-and-dots notation 형식) **network 바이트 순서**인 **binary 데이터**로 바꿔준다. 

- `host 주소 --> network 바이트 순서`

---

### inet_network

- **host 주소**를 (IPv4 numbers-and-dots notation 형식) input 으로 받고 **host 바이트 순서**로 바꿔준다. 

- `host 주소 --> host 바이트 순서`

---

### inet_aton

- **host 주소**를 (IPv4 numbers-and-dots notation 형식) 를 **binary form (네트워크 바이트 순서)** 으로 바꿀 때, 두 번째 인자인 `address 구조체 (struct in_addr) 로 input 값으로 넣은 값이 변환된 바이트 순서가 들어간다.`

- `host 주소 --> network 바이트 순서 (두 번째 인자에 저장)`

---

### *inet_ntoa

- **네트워크 바이트 순서**인 internet host address 를 입력으로 받아서 **IPv4 dotted-decimal notation** 으로 바꿔준다.

- `network 바이트 순서 --> host 주소`

---

### inet_pton

```
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>

int inet_pton(int af, const char *src, void *dst);
```

- `IPv6` 가 나오면서 지원하기 위한 함수이다.

- `문자열 (리틀엔디언)` 을 바이너리 형태의 IPv6 또는 IPv4 주소 `네트워크정보 (빅엔디안)` 로 변환해 주는 함수이다.

- `pointer --> network 바이트 순서`

- int af : IPv4, IPv6 를 선택하는 int 형

- string 형 : 네트워크와 관련된 string (dotted number)

- destination point : 2 번째의 string 형태를 변환하여 이 곳에 저장한다.

---

### inet_ntop

```
#include <arpa/inet.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

- `IPv6` 가 나오면서 지원하기 위한 함수이다.

- 바이너리 형태의 IPv6 또는 IPv4 주소 `네트워크정보(빅엔디안)`를, `문자열(리틀엔디언)`로 변환해주는 함수이다.

- `network 바이트 순서 --> pointer`

- int af : IPv4, IPv6 를 선택하는 int 형

- const void *src : 네트워크 주소 구조체(network address structure)

- char *dst : 결과 문자열이 복사되어질 버퍼를 가리키는 주소

- socklen_t size : 버퍼의 사용 가능한 사이즈 (Caller는 이 버퍼의 가용한 사이즈를 명시해야 한다.)

---

### 바이트 정렬 함수와 IP 주소 변환 함수 사용 예시

1. ***응용 프로그램이 소켓 주소 구조체를 초기화하고, 소켓 함수에 넘겨주는 경우***
```
// 소켓 주소 구조체를 초기화한다.
SOCKADDR_IN addr;
ZeroMemory(&addr, sizeof(addr)); /* 0으로 채운다. */ 
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr("147.46.114.70"); 
addr.sin_port = htons(9000);

// 소켓 함수를 호출한다.
SocketFunc(..., (SOCKADDR *)&addr, sizeof(addr), ...);
```

-	소켓 주소 구조체를 만든다.

-	memset 함수 등으로 0으로 초기화 한다.

-	AF_INET (IPv4 를 쓸 것) 을 통해 address 구조체 변수에 어떤 IP 를 쓸 것인지 (IPv4, IPv6) 정해준다.

-	address 변수에는 우리가 사용하는 IP 주소를 할당 (dotted notation 일 것)

    - 이것을 네트워크로 전달 하려면 dot 이 없어진 네트워크 바이트 순서로 바꿔줘야한다. 그래서 inet_addr 를 사용하여 변환한다.

-	포트번호도 호스트에서 네트워크 바이트 순서로 바꿔줘야하기 대문에 htons 함수를 사용한다.

-	그리고 주소가 할당되면 소켓 함수를 통해 데이터를 보내고 받을 수 있다.

2. ***소켓 함수가 소켓 주소 구조체를 입력으로 받아 내용을 채우면, 응용 프로그램이 이를 출력 등의 목적으로 사용하는 경우***

```
// 소켓 함수를 호출한다.
SOCKADDR_IN addr;
int addrlen = sizeof(addr);
SocketFunc(..., (SOCKADDR *)&addr, &addrlen, ...);

// 소켓 주소 구조체를 사용한다.
printf("IP 주소=%s, 포트 번호=%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
```

- 받는 입장에서는 소켓 구조체를 선언한 다음 특정 소켓 함수를 통해 주소와 그 주소에 저장된 데이터를 받아올 수 있다.

- 그 주소에 저장된 값들은 네트워크 바이트 순서여서 호스트에서 확인을 하려면 호스트 바이트 순서로 바꿔줘야 한다. 
    
    - 그래서 inet_ntoa 함수를 호출하고 포트 넘버도 ntoh 를 통해 호스트가 읽을 수 있는 값으로 변환할 수 있다.


---

### [실습 01 : ip-uint](https://github.com/jjaehwi/2023_Network/blob/main/Week2/ip-uint.c)

- LoooBack 주소

- inet_network 함수 : string 을 받아 . 을 없애준다. dotted notation 을 호스트 바이트 순서로 바꾸는 것.

- ipInInt 에는 127.0.0.1 이 호스트 바이트 순서로 변환된 값이 저장되어 있을 것이다.

- 호스트 바이트 순서로 변환된 값을 htonl 함수로 네트워크 바이트 순서로 바꾼다.

- `inet_network() + htonl() --> inet_addr()`

---

### [실습 02 : address-resolution](https://github.com/jjaehwi/2023_Network/blob/main/Week2/address-resolution.c)

다양한 변환 주소 변환함수를 활용해보는 코드 (sample IP = 127.0.0.1)

- 소켓 주소 구조체를 3개 선언

- 각 구조체 변수에 주소 변환 함수를 다양하게 사용해본다. (전부 IPv4)

- inet_aton 은 변환할 IP 를 넣고, 그 변환된 값을 저장할 주소 구조체의 포인터를 두번째 인자로 준다.

- inet_pton 은 IPv4 로 변환할 것이라고 하고, 변환할 IP 를 주고, 변환된 값을 저장할 장소를 인자로 준다.

- printAddr 함수를 보면 소켓 주소 구조체에 대한 포인터를 받아와서 출력을 하는데 내부 변수로 포트가 선언되어 있고 txt 라는 char 형 배열이 선언되어있다.

- 넘어오는 소켓 주소 구조체 에 저장된 포트 번호를 받아오는데 (네트워크 바이트 순서로 저장되어있는..) 얘를 ntohs 함수로 호스트 바이트 순서로 바꿔서 port 변수에 저장하고,
txt 배열에 네트워크 바이트 순서로 저장된 주소 를 inet_ntop 함수를 통해 dotted 된 주소로 바꿔서 저장한다.

---

### 호스트 이름과 IP 주소

- `호스트 이름을 이용하여 IP 주소를 조회`할 수 있다.

```
#include <netdb.h>

struct hostent *gethostbyname(const char *name);
```

- 네임서버에 존재하는 `hostent 의 구조체`를 활용한다.

```
The hostent structure is defined in <netdb.h> as follows:

struct hostent {
char *h_name;       /* official name of host */
char **h_aliases;   /* alias list */
int h_addrtype;     /* host address type */
int h_length;       /* length of address */
char **h_addr_list; /* list of addresses */
}
#define h_addr h_addr_list[0] /* for backward compatibility */
```

<img width="597" alt="스크린샷 2023-03-24 오전 12 48 21" src="https://user-images.githubusercontent.com/87372606/227258848-67f5753c-53d3-4106-ae5d-c07da0f68ad8.png">

---

### [실습 03 : getbyhostname.c](https://github.com/jjaehwi/2023_Network/blob/main/Week3/gethostbyname.c)

- gethostbyname 인자로 argv[1] 을 전해주고 이 리턴 값은 ent 에 저장, ent 가 null 이 아니면 입력된 호스트 네임으로 IP 주소를 조회한다.

---

## 소켓 프로그램

<img width="578" alt="스크린샷 2023-03-24 오전 12 58 23" src="https://user-images.githubusercontent.com/87372606/227262188-19e03df4-6c1a-4565-8328-9052d29de956.png">

- 서버는 `listen` 이라는 함수를 통해 클라이언트의 접속을 받을 수 있는 **열린 상태**가 되어야한다.

- 클라이언트가 `connet` 라는 함수를 통해 **소켓에 접근을 요청**할 때, 서버는 `accept` 을 통해 **수용**한다.

- 클라이언트가 `send`, `write` 라는 함수를 통해 네트워크를 통해 메세지를 보내면 서버는 `recv` 함수를 통해 데이터를 받고 자기도 `send` 를 통해 클라이언트에게 보낼 수 있다.

- 통신이 끝나면 `closesocket` 으로 닫는다.

- tcp 서버는 `클라이언트가 접속할 때마다 서비스를 제공`해준다.

- 궁극적으로 다수의 클라이언트들이 붙었을 때, 서버가 응답을 해줘야한다. (`multiple access`)

---

### listen()

```
#include <sys/types.h> 
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

- 인자
    
    - 소켓 파일 디스크립터

    - 얼마나 많은 요청을 받을 수 있는지 maximum length (pending 하고 있는 connections 을 얼마나 수용할 수 있는지에 대한 사이즈)

- 성공 시 0 을 리턴, 에러 발생시 -1 리턴

---

### accept()

```
#include <sys/types.h> 
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr * restrict addr, socklen_t * restrict addrlen);
```

- 보류 중인 연결 대기열에서 첫 번째 연결 요청을 추출하고, 새 소켓을 만들고, 원래 소켓의 상태를 상속하는 소켓에 `새 파일 설명자를 할당`한다.

- 인자

    -	소켓에서 만든 소켓에 대한 파일 디스크립터

    -	서버에 접속하는 클라이언트의 정보를 담을 소켓 어드레스의 구조체 포인터

    -	포인터가 담고있는 소켓에 대한 정보에 대한 사이즈

- 성공적으로 실행되면 accepted 소켓에 대한 파일 디스크립터를 가리키는 non-negative 한 정수가 반환된다.

    - 클라이언트로 부터 요청이 들어오면 그것을 처리하기 위한 `새로운 파일 디스크립터가 생기는 것`이다.

    - 에러 발생시 -1 리턴

---

### connect()

```
#include <sys/types.h> 
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- 클라이언트에 의해 요청되는 함수로 파일 디스크럽터 sockfd 에서 참조하는 소켓을 `addr 에서 지정한 주소로 연결`한다.

- 인자

    - sockfd : connet 를 수행한 소켓에 대한 파일 디스크립터

    - destination 을 가리키는 소켓 어드레스의 구조체

    - 그 destination 에 대한 소켓 어드레스의 사이즈

- 성공 시 0 을 리턴, 에러 발생시 -1 리턴

---

### send()

```
#include <sys/types.h> 
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

- `정해진 파일 디스크립터에 버퍼만큼 써주는 함수`이다.

- send() 호출은 `소켓이 연결된 상태에 있을 때만 사용`할 수 있다.(**의도된 수신자**를 알 수 있도록) 

- send()와 write()의 유일한 차이점은 `플래그의 존재`이다. 플래그가 0인 경우 send()는 쓰기와 같다.

- 인자

    - 소켓 디스크립터

    - 보낼 내용이 버퍼에 들어간다.

    - 그 버퍼의 크기

    - flag

- 성공적으로 수행되면 보낸 byte 만큼의 숫자가 리턴, 에러 발생시 -1 리턴

---

### recv()

```
#include <sys/types.h> 
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

- 일반적으로 `연결된 소켓에서만 사용`된다.

- 인자

    - 소켓 디스크럽터

    - 읽을 버퍼의 메세지

    - 얼마나 읽을지에 대한 길이

    - flag

- 성공적으로 수행되면 받은 byte 만큼의 숫자가 리턴, 에러 발생시 -1 리턴

---

### 실습 04 : [Basictcpsrv.c](https://github.com/jjaehwi/2023_Network/blob/main/Week3/Basictcpsrv.c) , [Basictcpclnt.c](https://github.com/jjaehwi/2023_Network/blob/main/Week3/Basictcpclnt.c)

***Basictcpsrv.c***

- 서버 / 클라이언트 소켓 디스크립터를 가리킬 수 있는 변수 선언

- 서버 / 클라이언트 어드레스 구조체를 다룰 수 있는 소켓 어드레스 선언

- 구조체의 길이를 저장할 수 있는 변수 선언, 클라이언트로부터 읽을 값의 사이즈를 저장할 변수 선언

- 실제 receive 버퍼의 크기를 정해진 사이즈만큼 할당

- 다 읽고 나서 서버가 클라이언트에 보내는 값은 미리 fixed 된 메세지를 보내도록 선언

- 처음에 서버는 소켓을 만들고 IPv4 를 쓰고 tcp 를 쓸것 이라고 선언한다.

    - 반환되는 파일 디스크립터를 처음에 선언한 서버 소켓 디스크립터 변수에 저장한다. (-1 이라면 에러 출력)

- memset 함수로 전부 0 으로 세팅

- IPv4 를 쓸 것이고, 어드레스는 어느 어드레스나 허용하겠다고 선언하는 그 값을 hton 로 설정

- 그리고 hton 으로 포트번호도 설정

- 이제 이 값들을 만들어진 서버 소켓에 binding 한다.

- 그 다음 listen 을 한다. 

    - 서버 소켓이 listen을 하는데 5개 까지 connection을 허용한다. (5개 까지 클라이언트 요청을 기다릴 수 있다.)

- 요청이 들어왔을 때 클라이언트로 부터 정보를 받아와야하니까 클라이언트의 어드레스에 대한 구조체의 사이즈를 변수에 저장하고, 클라이언트 소켓에 accept 함수를 통해 파일 디스크립터를 할당한다.

- 그 다음 클라이언트 소켓으로부터 아까 선언한 버퍼의 크기만큼 읽어온다. 

    - -1 이면 read error 발생한 것이다. 그게 아니라면 읽은 만큼 readLen 에 리턴 값으로 저장될 것.

- 마지막에 널 문자를 넣어준다.

- 읽어온 값을 print 한다.

- 마지막으로 write 를 한다. 

    - 클라이언트 소켓으로 아까 할당한 I’m 20 years old 라는 문장을 보낸다.

- 이미 소켓이 열린 상태에서 파일을 읽고 쓰기 때문에 1:1 mapping 이다. 
    
    - 그래서 일반 파일을 읽고 쓰는 것처럼 read, write 가 가능하고, recv(), send() 함수로 바꿔도 무방하다.


***Basictcpclnt.c***

- 클라이언트 소켓을 나타낼 수 있는 구조체

- 길이 정보

- 클라이언트가 보내는 메세지

- 클라이언트가 받을 버퍼

- 클라이언트도 소켓 API 를 통해 소켓을 만든다. (AP, PF 무방) 

    - 성공적으로 만들어지면 파일 디스크립터 반환하여 clntSd 에 저장된다.

- 어드레스 구조체에 어떤 것을 쓸건지, 주소, 포트넘버를 할당한다.

- 클라이언트 소켓을 이용하여 connet 함수를 사용
    
    - 클라이언트 소켓 파일 디스크립터, 아까 만든 주소에 대한 주소와 크기

- connection 이 성공하면 클라이언트 소켓을 통해 write 가 가능하다.

- 클라이언트가 보낸 메세지를 출력한다.

- 서버가 응답을 보내면 read 함수를 통해서 응답을 읽는다.

- 응답을 읽고 서버가 어떤 응답을 보냈는지 출력하고 close

---

***References***

- 네트워크 프로그래밍/최지훈/프리렉

- TCP/IP 소켓 프로그래밍/윤성우/오렌지미디어

- TCP/IP 윈도우프로그래밍/김선우/한빛네트워크

- http://manpages.ubuntu.com/manpages/bionic/en/man2