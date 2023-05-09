# UDP 소켓

- [UDP 기본](#udp-기본)

    - [TCP vs UDP](#tcp-vs-udp)

    - [UDP 헤더](#udp-헤더)

    - [UDP 특징](#udp-의-특징)

- [UDP 서버 / 클라이언트 분석](#udp-서버--클라이언트-분석)

    - [UDP 소켓 생성](#udp-소켓-생성)

    - [UDP 데이터 송 / 수신](#udp-데이터-송--수신)

    - [예제](#예제-sendtoc--recvfromc)

- [UDP 소켓의 특징](#udp-소켓의-특징)

    - [connect() 함수의 역할](#connect-함수의-역할)

    - [예제](#connect-함수-예제-echoserverc--connectedsendc)

- [UDP 패킷 모니터링](#udp-패킷-모니터링)

- [정리](#정리)

---

## UDP 기본

<img width="648" alt="스크린샷 2023-04-04 오전 1 35 27" src="https://user-images.githubusercontent.com/87372606/229572432-2c0519d1-4256-45de-a0f0-643980dc04f1.png">

- `UDP (User Datagram Protocol)` 은 **호스트안에서의 프로세스 식별을 통한 데이터 전송을 수행하는 프로토콜**

    - TCP 와 달리 신뢰성 (reliable) 한 데이터 전송을 보장하지 않음

    - 즉, 순차적인 데이터 전달, 혼잡제어, 흐름제어 등을 수행하지 않음

---

### TCP vs UDP

<img width="925" alt="스크린샷 2023-04-04 오전 1 36 23" src="https://user-images.githubusercontent.com/87372606/229572590-17b6fb26-9508-41f2-903c-d7276a181622.png">

---

### UDP 헤더

<img width="829" alt="스크린샷 2023-04-04 오전 1 37 37" src="https://user-images.githubusercontent.com/87372606/229572836-5f1dec0f-3919-4196-81fd-b272bcf9808c.png">

- TCP 헤더에 비해 간단

    - 소스 포트, 목적지 포트 : UDP 데이터그램을 주고받는 송수신 포트번호

    - Length : UDP 데이터그램의 크기

    - Checksum : UDP 데이터그램의 무결성 검증

--- 

### UDP 의 특징

- `연결 설정을 하지 않으므로` **connect() 함수 불필요**

- 프로토콜 수준에서 `신뢰성 있는 데이터 전송을 보장하지 않으므로`, 필요하다면 응용 프로그램 수준에서 신뢰성 있는 데이터 전송 기능을 구현해야 함 

- 간단한 소켓 함수 호출 절차만 따르면 `다자 간 통신을 쉽게 구현`할 수 있음 

    - connect 를 쓰지않는 경우 포트 넘버 하나로 다자 간 통신을 지원할 수 있기 때문

---

## UDP 서버 / 클라이언트 분석

<img width="611" alt="스크린샷 2023-04-04 오전 1 49 49" src="https://user-images.githubusercontent.com/87372606/229575535-c61b7dd7-5851-4f0a-b2d2-c5ed5bdaab2e.png">

<img width="608" alt="스크린샷 2023-04-04 오전 1 50 15" src="https://user-images.githubusercontent.com/87372606/229575625-9e937b80-f2c6-486c-898e-39db822d5790.png">

- UDP 서버가 대기 중이었다가 통신을 하면 하나의 포트로 클라이언트가 붙는데 이 포트로 ***여러 클라이언트가 붙을 수 있다.*** (`포트넘버 하나로 다수의 클라이언트가 통신할 수 있다.`)

<img width="690" alt="스크린샷 2023-04-04 오전 1 51 32" src="https://user-images.githubusercontent.com/87372606/229575885-578fa8f3-7a89-4079-97d7-adce819bb78b.png">

---

### UDP 소켓 생성

```
socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) 

(ex)socket(AF_INET, SOCK_DGRAM, 0) 
```
- Socket 함수의 2번째 인자로 소켓의 종류 선택 

- 3번째 인자로 0을 선택하면 해당 소켓의 종류를 대표하는 프로토콜이 선택됨 – SOCK_DGRAM의 경우 UDP 프로토콜임 

---

### UDP 데이터 송 / 수신

<img width="860" alt="스크린샷 2023-04-04 오전 1 53 44" src="https://user-images.githubusercontent.com/87372606/229576371-9190c5f5-5e8e-4cca-a0bb-24e139ef96b4.png">

- `sendto` 함수

```
#include <sys/types.h> 
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
```

- send 함수와 비슷

    - sending 하는 소켓의 파일 디스크립터

    - 보내고자하는 데이터가 버퍼에 들어가있다.

    - 보내고자하는 버퍼의 길이

    - 데이터 타입을 정의 (일반적으로 0)

    - dest_addr 가 들어간다.

    - 그 어드레스에 대한 구조체의 크기

- 성공적으로 보내면 보낸 바이트를 리턴하고 에러 발생시 -1 을 리턴

<img width="860" alt="스크린샷 2023-04-04 오전 2 02 32" src="https://user-images.githubusercontent.com/87372606/229578236-da528864-e33f-456f-8c62-77178f7560df.png">

- `recvfrom` 함수

```
#include <sys/types.h> 
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
```

- 데이터 받는 부분을 구현하기 위해 recvfrom 함수를 사용

    - listening 하는 소켓의 파일디스크립터

    - recvfrom 함수를 통해 송신자가 보낸 데이터를 저장하는 버퍼의 주소를 가리킴

    - 그 버퍼의 길이

    - 플래그

    - 작업을 한 후 다시 송신자에게 작업 결과를 보내는 경우도 존재하기 때문에 데이터를 보낸 송신자의 주소를 저장할 수 있는 포인터 (송신자의 주소가 저장되는 포인터)

    - 그 포인터 주소의 길이

---

### 예제 sendto.c / recvfrom.c

***sendto***

- sock_dgram 을 통해 소켓을 생성

- destination address 를 세팅

    - 입력받은 argv 로 부터 ip 주소를 입력받아 네트워크 address 를 반환, 포트넘버도 입력받아 변환한다.

    - 그 전에 char 형태의 포트넘버를 int 형태로 바꾸고 (atoi) host -> network 로 바꾸는 것(htons)

- 세팅된 destination 주소를 sendto 함수를 할 때 사용한다. 
    
    - 만든 소켓에 buff 의 데이터를 보내는데 readLen 길이만큼 보낸다. 

    - destination 주소에 쓰고 보내진 바이트 수 만큼 리턴되면 nSent 에 저장

- 키보드로 입력 받기 때문에 fgets(buff,BUFSIZ-1, stdin)

- while 은 무한루프, 프로그램이 끝나면 close

---

***recvfrom***

- argv 를 체크하는데 포트넘버를 할당받는다.

- 서버와 같은 역할을 할 때, 소켓을 만들어주는데, UDP 소켓으로 만들어지고, 어디서든 ip 주소를 받을 수 있다고 세팅 (htonl(INADDR_ANY))

- 유저에게 입력을 받는 포트번호로 서버가 수신한 포트넘버를 할당 

    - bind 를 통해 만든 포트번호를 넣는다.

- recv 를 해서 자기 소켓으로 데이터가 오면 자기가 설정한 버퍼에 데이터를 저장하고 데이터를 보낸 사람의 주소를 저장한다.

    - 총 읽은 값이 nRecv 에 저장하고 에러가 발생하지 않으면 몇 byte를 받았다고 출력

---

<img width="479" alt="스크린샷 2023-04-04 오전 2 07 40" src="https://user-images.githubusercontent.com/87372606/229579313-ca36600d-97fc-4168-af22-e9734a3f47ed.png">

- UDP 도 connect 를 활용할 수 있는데 이 둘의 차이를 알아보자..

---

## UDP 소켓의 특징

1. `UDP 클라이언트 포트 번호 지정`

    - bind() 함수를 호출해서 포트 번호 지정 가능하다.

    - bind() 함수를 사용하지 않으면 sendto() 에 의해 임의로 포트 번호가 지정됨

    - bind 를 하면 서버 프로그램에서 소켓 디스크립터에 srcAddr 가 바인딩이 되고 포트 번호가 할당이 되고 어느 IP 에서 받겠다라고 선언이 되는 것이다.

```
mySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
memset(&srcAddr, 0, sizeof(srcAddr)); 
srcAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
srcAddr.sin_port = htons(atoi(argv[3])); 
srcAddr.sin_family = AF_INET;
res = bind(mySock, (struct sockaddr *) &srcAddr, sizeof(srcAddr));
if(res == -1) errProc("bind");
```

2. `다수의 클라이언트 처리`

    - recvfrom 함수를 통해 내가 만든 소켓에서 데이터가 왔을 때 buff 에 저장한다. destAddr 는 데이터를 보낸 송신자의 주소 (패킷의 src 주소일 것)

        - ***recvfrom 함수를 통해 송신자의 주소를 가져올 수 있는 것이다.***

        - **sendto 함수는 목적지로 설정된 곳에 데이터를 전달하므로, 여러곳의 목적지로 데이터 전송이 가능하다.**

    - destAddr 를 ntoa 로 바꾼다음 그거를 출력하고 포트번호도 ntohs 로 바꿔서 출력하고, 데이터가 뭔지도 출력해보는 코드

```
while(1) {
    nRecv = recvfrom(mySock, buff, BUFSIZ-1 , 0,
        (struct sockaddr *) &destAddr,
        &addrLen);
if(nRecv == -1) errProc("recvfrom");
if(nRecv > 0) buff[nRecv-1]='\0';
else buff[nRecv] = '\0';
strAddr = inet_ntoa(destAddr.sin_addr);
buff[nRecv]='\0'; printf("%s:%d>%s\n",strAddr,ntohs(destAddr.sin_port),buff);
}
```

---

### connect() 함수의 역할

- `UDP 소켓은 connect 가 없어도 sendto() 와 recvfrom() 으로 충분히 데이터를 주고받을 수 있다.`

- 하지만 이 함수가 **자주 호출이 되면 커널에 `overhead` 를 줄 수 있다.** 

- 그래서 다 vs 다 통신, 즉, ***여러 클라이언트가 붙을 필요가 없을 경우 UDP 소켓에 connect 를 구현***할 수도 있다.

    - connect() 함수를 사용할 경우 하나의 UDP 소켓으로 여러 프로세스들과 통신가능하다는 장점은 사라진다.

    - read(), write() 함수를 사용할 수 있다.

    - CMP Port Unreachable 메시지 확인 가능하다.

---

### connect() 함수 예제 (echoServer.c / connectedSend.c)

***echoServer***

- echoServer 는 우리가 데이터를 보내면 동일한 데이터를 보내주는 메아리 서버

- connect 를 이용해 데이터를 보내면 그 데이터를 받은 애가 recvfrom 을 통해 destAddr 를 저장했다가 버퍼에 저장된 데이터와 destAddr 를 이용해 데이터를 보낸 애한테 데이터를 다시 보내주는 코드

---

***connectedSend***

- 소켓의 파일 디스크립터를 만든 후 주소를 2 개 만든다

    1. 유저 입력 값으로 부터 포트번호를 받고 주소를 할당 받는다.

    2. 기존에 입력받은 IP 주소를 동일하게 addr 주소로 설정하고 미리 정해진 포트번호를 사용한다.

    - IP 주소는 같지만 포트번호는 다른 상황인 것이다.

- connect 함수를 통해 destAddr 에 커넥션을 하는데 주소 1 에 커넥션을 했다.

- 무한 루프를 도는데 fgets 로 유저의 input 을 stdin 로 받고 connect 를 사용했기 때문에 write 를 쓸 수 있다. 

    - write 로 파일 디스크립터에 stdin 으로 입력받은 것을 쓴다. (서버로 데이터를 보냄)

    - 그리고 connect 를 써서 read 도 가능하고 서버로부터 데이터를 받아와서 서버가 뭘 보냈는지 출력한다.

- 내가 보낸 것을 서버가 메아리 쳐서 다시 돌려주는 코드이기 때문에 버퍼에 저장된 것도 내가 보낸 값 그대로 될 것이다.

- 그리고 다시 또 유저의 입력을 기다린다. 유저가 입력을 하면 sendto 를 통해 아까 설정한 주소 2 에 데이터를 보낸다.

    - sendto 와 connection 기반 read 의 차이를 보기 위함

- 그리고 recvfrom 을 통해 메아리 친 데이터를 받는다.

- 결과 : 서버에서는 소켓에 9020 포트로 커넥션 되어있어서 에서 오는 것만 받을 수 있다.

    - connect(mySock, (struct sockaddr*) &destAddr, sizeof(destAddr)); 으로 주소 1 의 소켓과 connect 했기 때문

    - 그래서 마지막 포트번호 9050 에서 55492 클라이언트 포트로 보냈을 때 도달하지 못하는 것이다. (unreachable)

    - `connect() 를 쓰면 특정 포트가 connect 가 되는 것이기 때문에 그 포트 외 다른 포트로부터 데이터가 return 됐을 때 받을 수가 없다.` (**하나의 UDP 소켓으로 여러 프로세스들과 통신 가능하다는 장점이 사라진 것**)

<img width="688" alt="스크린샷 2023-04-04 오전 2 23 10" src="https://user-images.githubusercontent.com/87372606/229582452-b530d14c-6e78-4405-a166-3bb858c13faa.png">

---

## UDP 패킷 모니터링

- 예제 echoServer.c 와 echoClient.c 를 실행해서 와이어샤크를 통해 확인해본다.

    - 클라이언트가 데이터를 보내면 서버가 응답을 똑같이 해주고, 계속 메세지를 주고받을 수 있는 구조가 된다. 

    - 클라이언트가 END 라는 메세지를 보낼 때까지 서버가 응답을 메아리 쳐서 하는 프로그램이다. (코드 비교해보기)


<img width="688" alt="스크린샷 2023-04-04 오전 2 23 25" src="https://user-images.githubusercontent.com/87372606/229582506-8595df42-f9f1-4053-a6eb-fbeeecd47471.png">

- 헤더가 심플한 것을 확인해볼 수 있다.

---

## 정리

- UDP 기본

- UDP 서버/클라이언트 분석 

    - (UDP 관련 함수 : sendto, recvfrom 에 대해 배우고 connectionless 한 communication 이기 때문에 하나의 포트번호로 여러 클라이언트를 지원할 수 있다는 것을 배웠다.)

- UDP 소켓의 특징으로 unreachable packet (데이터를 보내더라도 못 받을 수도 있음) 이고 connect 를 쓸 수 도 있다는 것을 배웠다.

- 패킷 헤더에 대한 것을 와이어샤크 모니터링을 통해 알아봤다.

***References***

- TCP/IP 윈도우 소켓 프로그래밍, 한빛아카데미