# Advanced 소켓

- 책의 내용은 구 버전이기 때문에 최신 버전의 추가된 내용에 대해서 공부한다.

- [소켓이란 무엇인가](#what-are-sockets)

- [Basic TCP/UDP program flow](#basic-tcpudp-program-flow)

    - [TCP flow](#tcp-program-flow)

    - [UDP flow](#udp-program-flow)

- [IPv6](#ipv6-의-구조)

    - [IPv4 vs IPv6](#ipv4-vs-ipv6)

- [addrinfo 구조체](#addrinfo-구조체)

    - [예제 1 : 서버](#예제-1-서버)

    - [예제 2 : 클라이언트](#예제-2-클라이언트)

    - [예제 3](#예제-3--ip-주소를-보여주는-예제)

- [Simple Time Server](#simple-time-server)

---

## What are sockets?

- There are a few different socket application programming interfaces (APIs)

- 지금 까지 썼던 소켓은 Berkeley socket (`POSIX 기반`으로 사용되는 API 형태) 이다.

    - 리눅스 기반으로 소켓 프로그래밍을 하므로 BSD 소켓을 쓴다.

- 윈도우에서 사용하는 윈도우 기반의 Winsock (Windows’ socket API)

---

## Basic TCP/UDP program flow

---

### TCP program flow

<img width="298" alt="스크린샷 2023-04-11 오후 3 47 59" src="https://user-images.githubusercontent.com/87372606/231078671-76489433-14cb-4828-a2ef-a16372e53afd.png">

- **TCP 클라이언트 프로그램**

    - address name 을 입력하면 tcp 클라이언트는 그 주소를 input 으로 받아서 ip 주소를 받아와야하기 때문에 `getaddrinfo()` 함수를 쓴다. 
    
    - 그리고 나서 `socket()`, `bind()`, `connect()`, `send()`, `recv()` 함수를 사용한다.

- **TCP 서버 프로그램**

    - 자신이 받을 포트넘버를 받아와야하고, 자신의 주소를 갖고 와야할 필요도 있기 때문에 `getaddrinfo()` 를 써서 세팅한다.

---

### UDP program flow

<img width="373" alt="스크린샷 2023-04-11 오후 3 50 20" src="https://user-images.githubusercontent.com/87372606/231079132-4d2784c3-cd4f-4863-ba40-9373d3cf5c95.png">

- **UDP 클라이언트 프로그램**

    - 첫 번째 패킷을 보내려면 원격 UDP peer 의 주소를 알아야 한다.

    - UDP 클라이언트는 getaddrinfo() 함수를 사용하여 주소를 구조체 addrinfo 구조체로 확인한다.

    - socket(), bind(), sendto() and recvfrom() 를 사용한다.

    - 원격 peer 가 클라이언트로부터 먼저 데이터를 수신하지 않으면 데이터를 보낼 위치를 알 수 없기 때문에 UDP 클라이언트가 먼저 데이터를 수신할 수 없다.

- **UDP 서버 프로그램**

    - 서버는 올바른 수신 IP 주소와 포트 번호를 사용하여 구조체 addrinfo 구조체를 초기화해야 한다.

    - getaddrinfo() 함수를 사용하여 프로토콜에 독립적인 방식으로 이 작업을 수행할 수 있다.

    - socket(), bind(), recvfrom(), and sendto()

---

## IPv6 의 구조

```
IPv6 only - see struct sockaddr_in and struct in_addr for IPv4

struct sockaddr_in6 {
    u_int16_t   sin6_family;    // address family, AF_INET6
    u_int16_t   sin6_port;      // port number, Network Byte Order
    u_int32_t   sin6_flowinfo;  // IPv6 flow information
    struct in6_addr sin6_addr;  // IPv6 address
    u_int32_t   sin6_scope_id   // Scope ID
};

struct in6_addr {
    unsigned char   s6_addr[16];    // IPv6 address
};
```

- IPv6 의 주소를 받아야하기 때문에 16byte 배열로 선언되어있다.

---

### IPv4 vs IPv6

```
struct sockaddr_in sa;   // IPv4
struct sockaddr_in6 sa6; // IPv6

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr));             // IPv4
inet_pton(AF_INET6, "2001:db8:63b3::1::3490", &(sa6.sin6_addr)); // IPv6
```

- INET6 로 바뀌고, . 기반에서 : 기반으로 바뀌었다.

- pton 을 사용하면 IPv4, IPv6 에 둘 다 사용가능하다.

```
// IPv4
char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
struct sockaddr_in sa;      // pretend this is loaded with something

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
printf("The IPv4 address is: %s\n",ip4);

//IPv6
char ip6[INET_ADDRSTRLEN];  // space to hold the IPv6 string
struct sockaddr_in6 sa6;      // pretend this is loaded with something

inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);
printf("The IPv6 address is: %s\n",ip6);
```

- 클라이언트가 어떤 주소를 입력했을 때, **서버를 통해 주소를 받아올 때 IPv4, IPv6 인지 모른다.** 그래서 어떤 주소가 오더라도 커버할 수 있도록 프로그램 되어야한다.

    - getaddrinfo() 를 통해 실제 동적으로 주소를 받아오고 IPv4, IPv6 둘 다 커버 가능해야하는 것.

- `INADDR_ANY`

    - 서버에 존재하는 다양한 네트워크 인터페이스 카드를 통해 데이터를 받을 수 있다. 

    - 일반적으로 서버에는 네트워크 인터페이스 카드가 많아서 다양한 인터페이스 카드의 IP 주소가 있는데 connection 이 올 때마다 그 네트워크 카드에서 쓸 수 있는걸 가져와서 binding 한다. (0으로 세팅하는 것)

<img width="799" alt="스크린샷 2023-04-11 오후 4 15 16" src="https://user-images.githubusercontent.com/87372606/231084457-ff778de8-bf19-4302-b87c-be2e39813360.png">

- inet_aton, inet_addr 대신 `inet_pton` 으로 IPv6 까지 커버

- inet_ntoa 대신 `inet_ntop` 로 IPv6 까지 커버

- gethostbyname 대신 `getaddrinfo` 사용

    - node : 호스트의 이름 혹은 IP 주소

    - service : 사용하고자하는 프로토콜 서비스 (http, https) 혹은 바로 포트넘버

    - addrinfo 의 hint

    - addrinfo 의 res

---

## addrinfo 구조체

```
struct addrinfo {
    int     ai_flags;           // AI_PASSIVE, AI_CANONNAME, etc
    int     ai_family;          // AF_INET, AF_INET6, AF_UNSPEC
    int     ai_socktype;        // SOCK_STREAM, SOCK_DGRAM
    int     ai_protocol;        // use 0 for 'any'
    size_t  ai_addrlen;         // size of ai_addr in bytes
    struct sockaddr *ai_addr;   // struct sockaddr_in or _in6
    char    *ai_canonname;      // full canonical hostname
    struct addrinfo *ai_next;   //linked list, next node
};
```

- addrinfo  구조체

    - ai_flags : address information flag  구조체를 쓰고있는 호스트의 주소를 받아온다.
    
    - ai_family : AF_INET, AF_INET6, AF_UNSPEC (뭐가 올지 모를 때)
    
    - ai_socktype : 소켓의 타입 설정
    
    - ai_protocol : 0 은 아무 프로토콜이나 받을 수 있다. (디폴트도 0)
    
    - ai_addrlen
    
    - ai_addr : 받아올 address IP 주소
    
    - ai_cannonname : 그 IP 주소의 별명
    
    - ai_next : 큰 서버들은 IP 가 여러개여서 그것들을 가리킬 수 있는.. 다음 ai structure 구조를 가리키는 애

---

### 예제 1 (서버)

- if you're a server who wants to listen on your host's IP address, port 3490

```
int status;
struct addrinfo hints;
struct addrinfo *servinfo;  // will point to the results

memset(&hints, 0, sizeof hints);    // make sure the struct is empty
hints.ai_family = AF_UNSPEC;        // don't care IPv4, IPv6
hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets
hints.ai_flags = AI_PASSIVE;        // fill in my IP for me

if(status = getaddrinfo(NULL, "3490", &hints, &servinfo) != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
}
```

- hint 변수, servinfo 포인터 변수

- hint 를 이용해 실제 가져올 result 를 포인터 변수에 저장할 것

- hint 를 초기화하고 그 hint 에 우리가 원하는 어떤 IP 주소를 얻고 싶은지 세팅한다. (앞서 배운 구조체에서 원하는 부분만 설정하면 되고 나머지는 디폴트로 들어가게 된다.)

- 서버 프로그램 이기 때문에 getaddrinfo 에 NULL 이 들어간다(자동으로 주소를 받아올 것이기 때문)

    - 포트는 3490으로 설정
    
    - 아까 설정한 힌트의 주소를 넣어주고

    - 힌트기반으로 서버의 주소를 가져올 것이기 때문에 servinfo 에 저장할 것. 그러면 servinfo 에 IPv4, IPv6 형태에 맞게 저장이 된다.

---

### 예제 2 (클라이언트)

- if you're a clinet who wants to connect to a particular server, say "www.example.net" port 3490.

```
int status
struct addrinfo hints;
struct addrinfo *servinfo;  // will point to the results

memset(&hints, 0, sizeof hints);    // make sure the struct is empty
hints.ai_family = AF_UNSPEC;        // don't care IPv4, IPv6
hints.ai_socktype = SOCK_STREAM;    // TCP stream sockets

// get ready to connect
status = getaddrinfo("www.example.net", "3490", &hints, &servinfo);
```

- 클라이언트의 경우 아까처럼 변수 설정을 하고 힌트를 설정한다.

- ai_flag 를 설정하지 않은 이유 : 어차피 내가 접속하고자하는 서버의 주소를 받아올 것이기 때문

- getaddrinfo 에 내가 원하는 얻고자하는 주소를 적고, 포트번호 설정하고, hint 와 servinfo 를 넣어준다.

- 과거엔 소켓을 만들고 소켓에 주소를 my_addr 에 static 하게 넣어서 bind 를 시켰다.

    - 그러나 getaddrinfo 를 사용하면 접속 원하는 사이트의 주소와 포트번호 힌트를 통해 정보가 반영된 servinfo에 주소를 받을 수 있다.

- `socket()` and `connect()`

```
struct addrinfo hints, *res;
int sockfd;

// first load up address structs with getaddrinfo():

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

getaddrinfo("www.example.net", "3490", &hints, &res);

// make a socket

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// connect!

connect(sockfd, res->ai_addr, res->ai_addrlen);
```

- res 에 우리가 원하는 주소들이 세팅이 되어있을 것이기 때문에 소켓을 만들 때, res 를 이용해서 세팅해준다.

- 소켓을 만들고 그 소켓의 파일디스크립터와 res 의 address 정보를 이용해서 connect 할 수 있다.

- 이런식으로 하면 static 하게 프로그래밍하는 부분이 많이 사라지고 IPv4, IPv6 를 전부 지원한다.

    - 이렇게 쓰는 습관을 들이는 것이 좋다.

---

### 예제 3 : IP 주소를 보여주는 예제

- [소스 코드 : showip.c](https://github.com/jjaehwi/2023_Network/blob/main/Week4/showip.c)

- IP 주소를 보여주는 실습

    - char ipstr 라는 변수에 저장할 것

    - 우리가 IP 주소를 확인하고 싶은 도메인 네임을 받아 출력한다.

- 힌트를 통해 res 를 받아오면 우리가 원하는 도메인의 IP 주소가 뭔지 print 를 한다.

    - for 문에서 p = res 가 NULL 이 아닐 때까지, p 의 ai_next 변수가 있을 때까지, (큰 서버들은 IP 주소가 여러개 이기 때문에) 받아온 result 에서 IPv4 인지 IPv6 인지 에 따라 맞게끔 저장을 한다

    - 그리고 ntop 로 addr 의 형태를 ipstr 에 string 형태로 presentation 으로 변환해준다.

---

## Simple Time Server

- Simple Time Console 코드

```
#include <stdio.h> 
#include <time.h>

int main() {

    time_t timer; 
    time(&timer);

    printf ("Local time is: %s", ctime(&timer));

    return 0;
}
```

- 이것을 콘솔에 네트워킹 기능을 넣어서 time 서버처럼 만들 수 있다. 접속하면 시간이 나오도록 세팅하는 것이다.

- [Time Server 예제](https://github.com/jjaehwi/2023_Network/blob/main/Week4/time_server.c)

```
매크로

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
```

```
printf("Configuring local address...\n");
struct addrinfo hints;
memset(&hints, 0, sizeof hints);    // make sure the struct is empty
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

struct addrinfo *bind_address;
getaddrinfo(0, "8080", &hints, &bind_address);
```

- 서버가 local address 로 바인딩 되어야한다.

    - 힌트를 이용하여 세팅하고, getaddrinfo 를 통해 서버의 주소를 받아오기때문에 0 을 넣고, 포트 설정하고 힌트를 넣고 받아온 주소를 bind_address 에 저장한다.

    - 8080 을 쓰는 이유는 기존 커널에서 관리하는 프로그램과 충돌할 수 있기때문에 8080 을 쓴다.

```
printf("Creating socket...\n");
SOCKET socket_listen;
socket_listen = socket(bind_address->ai_family, 
        bind_address->ai_socktype, bind_address->ai_protocol);
if(!ISVALIDSOCKET(socket_listen)){
    fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
}
```

- 이후 소켓을 만든다.

    - bind_address 를 이용하여 설정한다. (tcp 이고 tcp 프로토콜을 쓴다고 세팅했다)

    - ISVALIDSOCKET 은 매크로 (#define)

```
    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);
```

- 만든 socket_listen 이라는 소켓 디스크립터를 이용해 bind 한다.

```
    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
```

- listen 상태를 만든다.

- 백 로그는 10개 까지 지원하도록 한 것

```
    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
            (struct sockaddr*) &client_address, &client_len);
    if (!ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
```

- connection 을 기다린다.

- 오는 패킷들을 기다리고 패킷이 오면 accept 을 한다.

    -  accept 이 성공적으로 끝나면 클라이언트들의 address 들을 저장할 수 있다.

```
    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
            client_len, address_buffer, sizeof(address_buffer), 0, 0,
            NI_NUMERICHOST);
    printf("%s\n", address_buffer);
```

- optional 하지만 어떤 클라이언트로 부터 접속이 왔는지 로그를 남겨두는 습관을 가지면 좋다. 

- 방금 전 accept 하면서 client_address 를 저장한 구조체를 이용하여, getnameinfo 라는 주소 정보를 통해 실제 address 에 어떤 값들이 있었는지 저장할 수 있다.

    - address_buffer 에 담길 수 있는 내용들을 client_address 에서 뽑아서 저장하는 것

    - 어떻게 접속을 했는지, address buffer 에 어떤 클라이언트가 왔는지 IP 주소는 뭐고 포트번호를 뭘 요청했는지 기록이 남을 것이다.

    - 그리고 콘솔에 출력한다.

```
    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);
    //printf("%.*s", bytes_received, request);
```

- accept 을 통해 클라이언트의 요청을 받았으니 클라이언트가 보낸 request 를 recv 를 통해 읽어온다.

    - 클라이언트는 단지 HTTP request 를 보낸 것 뿐이다.

    - 클라이언트는 웹 브라우저에서 타임서버에 HTTP request 를 보냈으니 타임서버 또한 웹 브라우저에게 자신의 정보를 리턴해줘야한다.

```
    printf("Sending response...\n");
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));
```

- 서버는 그 request 에 응답을 해주면 된다.

- HTTP 요청을 받았기 때문에 response 형태를 HTTP response 형태로 맞춰서 보내준다.

    - connection 200 OK

    - connection 이 되었으면 닫는다.

    - 암호화되지 않은 context

    - 보내고자 하는 메세지

- response 메세지를 만들어서 타임서버가 소켓 클라이언트에게 response 를 보낸다. 아직 시간은 보내지 않았고 메세지만 보냈기 때문에 그 다음 시간에 대한 정보도 보내준다.

```
    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));
```

- 타임 서버가 다시한번 시간 값을 가져와서 시간 값을 만든 다음 소켓 클라이언트에게 보낸다. 그래서 실제로 send 가 두 번 호출된 것을 확인 할 수 있다.

- 이렇게 하는 이유는 요청이 와서 HTTP 메세지를 만들어야하는데 시간 값을 만들고 HTTP 메세지를 만들어서 보내면 약간의 시간 딜레이가 있기 때문에 메세지를 먼저 만들고 시간을 측정해서 보내는 것이다. 

    - 좀 더 정확한 시간을 보내기 위함!

```
    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);


    printf("Finished.\n");

    return 0;
```

- 마지막에 CLOSE 소켓하는 부분

<img width="1241" alt="스크린샷 2023-04-11 오후 5 02 25" src="https://user-images.githubusercontent.com/87372606/231095781-42488b99-f005-450f-9ac5-c448ec8a97c9.png">

- 실습 결과

    - 127.0.0.1:8080 포트로 접근을 해서 보면 서버에 HTTP request 를 보냈고 서버는 응답해서 메세지를 보낸 것을 확인할 수 있다. 
    
    - 뭔가 359 bytes 의 HTTP 요청을 받은 것이고 time 을 측정해서 79 bytes 와 25 byte 의 데이터를 보낸 것을 확인할 수 있다.

