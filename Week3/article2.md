# TCP 소켓

- [TCP 기본](#tcp-기본)

    - [TCP 헤더](#tcp-헤더)

    - [TCP 상태](#tcp-상태)

    - [TCP 연결과 종료](#tcp-연결--종료)

    - [TCP 흐름 제어](#tcp-흐름-제어)

- [TCP 기반 서버 / 클라이언트 분석](#서버--클라이언트-분석)

    - [서버 설정](#서버-소켓-주소-초기화-및-설정)

    - [클라이언트 연결](#클라이언트-연결-요청)

    - [읽기 / 쓰기](#서버의-tcp-연결과-읽기--쓰기)

- [Iterative 서버 모델](#iterative-서버-모델)

---

## TCP 기본

- **`전송 계층 (Transport Layer)` 의 핵심 기능은 `호스트 안에서의 프로세스 식별을 통한 데이터의 배달`이다.**

<img width="494" alt="스크린샷 2023-04-11 오후 2 10 14" src="https://user-images.githubusercontent.com/87372606/231061968-81e64e05-ea57-4d34-b83b-fa1f5ad1518c.png">

<img width="494" alt="스크린샷 2023-04-11 오후 2 10 25" src="https://user-images.githubusercontent.com/87372606/231061999-c8157db7-75d2-4912-a30b-38a68ef57c5e.png">

- 다음 우체국과 배달부의 예시를 보자.

    - 직원 a 만으로는 혼자서 목적지까지 우편물이 순서대로 잘 갔는지 모르기 때문에 직원 b 의 도움이 필요하다.

    - `ACK` 를 통해 데이터가 잘 갔는지 확인한다.

    - 데이터에는 `sequence number` 가 있어서 데이터가 순차적으로 잘 전달됐는지 확인 가능하다.

    - 수신 측에 `receive buffer (데이터를 받는 버퍼)` 가 데이터를 다 못담는다면 미리 알려주는 `흐름 제어와 관련된 패킷 헤더` 존재

    - 전송하는 데이터들의 ACK 가 도달하지않거나 중복 ACK 가 오면 데이터가 수신자에게 전달이 잘 안되는 것이다. (다른 사람들도 사용하므로..) 그래서 **혼잡한 상황이라면 전송 양을 조절하는 기능을 TCP 가 제공**

---

### TCP 헤더

- `TCP 의 기능`

    - 호스트 안에서의 **프로세스 식별을 통한 데이터의 배달**
    
    - **`신뢰적`이고 `순차적`인 데이터 전달**

    - **흐름 제어, 혼잡 제어, 오류 제어**

<img width="512" alt="스크린샷 2023-04-11 오후 2 13 42" src="https://user-images.githubusercontent.com/87372606/231062438-e19f6261-02f1-4774-87a6-bb1f0abbb8d7.png">

- TCP 헤더는 UDP 헤더에 비해 복잡한 정보를 많이 가지고 있다.

1. `Source Port`

    - TCP 세그먼트를 보내는 프로세스가 사용하는 포트 번호

2. `Destination Port`

    - TCP 세그먼트를 수신하는 프로세스가 사용하는 포트 번호

3. `Sequence Number`

    - TCP 는 바이트스트림을 쪼개서 전송한다. 따라서 전체 바이트스트림에서 해당 TCP 세그먼트의 번호 중 가장 빠른 번호가 Sequence Number로 할당된다.

<img width="677" alt="스크린샷 2023-04-11 오후 2 17 36" src="https://user-images.githubusercontent.com/87372606/231062957-3de523c6-956a-4b45-bf52-0ed2b107f00c.png">

4. `Acknowledgement Number`

    - 데이터가 성공적으로 전달됐으면 그 여부를 상대에게 알리기 위해 패킷을 수신한 노드가 다음에 전송받을 데이터의 sequence number 를 할당한다.

5. `Header Length`

    - 기본적인 TCP 헤더의 길이는 20 바이트 (옵션 필드에 따라 헤더의 크기는 변경됨)

    - 값의 단위가 32비트 (4바이트) 이기 때문에 해당 필드에 최소값은 5 (5*4바이트=20바이트)임

6. Reserved Field

    - 예약 필드 (기능 확장에 사용)

7. `TCP Flags`

    - TCP 세그먼트 기능을 표현한다.

    - 해당 패킷의 상태를 알려주는 척도가 된다.

<img width="653" alt="스크린샷 2023-04-11 오후 2 21 02" src="https://user-images.githubusercontent.com/87372606/231063433-0223a0e7-44ca-4db0-bdf2-09ea375bca0c.png">

8. `Window Size`

    - 수신 버퍼의 크기를 알리는데 사용된다.

    - 데이터를 수신하는 애가 ACK 와 같은 응답을 보내는데 이 응답에 window size 를 적는다. (내 버퍼는 이 정도니까 이 사이즈 안 넘게 잘 보내라고 말해주는 것)

9. `TCP Checksum`

    - 세그먼트의 데이터 무결성을 검사하는 비트

10. Urgent Pointer

    - 긴급 데이터를 가리키는 포인터

---

### TCP 상태

<img width="374" alt="스크린샷 2023-04-11 오후 2 27 17" src="https://user-images.githubusercontent.com/87372606/231064416-0ee13765-05b8-4057-aa5a-4c6f2f6c8b99.png">

- **clinet 가 서버를 여는 initiator**, **서버는 listen 중인 상태**

    - 클라이언트가 connect 를 통해 syn 패킷을 보내면 SYN - SENT 상태가 되고, 이것을 서버가 받고 자신이 요청을 받으면서 그 요청에 대한 응답으로 ACK 를 보낸 뒤 SYN - RECEIVED 상태가 된다.

    - 클라이언트는 SYN - SENT 상태에서 서버가 보낸 ACK 를 받으면 받은 ACK 에 대한 ACK 를 또 보낸다. (`3 - way handshake`)

    - ACK 를 보낸 클라이언트는 ESTABLISHED 상태가 되고, SYN - RECEIVED 상태에 있던 서버도 클라이언트로부터 ACK 를 받으면 ESTABLISHED 상태가 된다. (이 상태는 서버와 클라이언트가 connection 을 맺은 상태이다.)

- TCP connection 을 종료하고 싶으면 서버, 클라이언트 누구나 먼저 종료할 수 있다. 이 때, close 를 initiate 하는 애가 있을 거고 response 하는 애가 있을 것이다.

    - 클라이언트가 종료한다고 했을 때, 서버가 요청을 받는다. 통신을 하다가 클라이언트가 종료하기 위해 소켓을 close 한다. 
    
    - 그러면 OS 커널에 있는 TCP 함수는 소켓 close 되면 FIN 플래그가 세팅된 TCP 패킷을 서버 (responder) 에게 보낸다. FIN 플래그가 세팅된 패킷을 보내면 FIN - WAIT - 1 상태가 된다.

    - 통신을 하다가 responder 는 FIN 패킷을 받고 종료를 하고싶다는 것을 인지한다. 패킷을 받음과 동시에 responder 는 OS 커널에서 close 를 initiate 한 애에게 ACK 를 보내고 CLOSE - WAIT 상태가 된다.

    - initiater 는 자신이 보낸 FIN 패킷에 대한 ACK 를 받으면 FIN - WAIT - 2 상태가 된다. 그리고 responder 는 자기가 하던 작업이 있는 상태에서 FIN 요청을 받았기 때문에, 작업을 종료하고 자기도 종료할 준비가 되면 FIN 을 보내고 LAST - ACK 상태가 된다.

    - initiater 가 responder 로 부터 FIN 을 받으면 이에 대한 응답으로 ACK 를 보내고 자기는 TIME - WAIT 상태가 된다. 그리고 responder 가 LAST - ACK 상태에서 initiater 로 부터 ACK 를 받으면 FIN 절차가 끝나고 CLOSED 상태로 다시 올라간다. initiater 도 TIME - WAIT 상태에서 일정 시간이 지나면 CLOSED 상태로 간다.

- CLOSED 로 가는 두 번째 경우

    - initiater 가 responder 에게 FIN 을 보내고 responder 는 ACK 만 보내는 것이 아닌 자기도 종료하기 위해 FIN 을 ACK 와 같이 보낸다.

    - initiater 는 FIN - WAIT - 1 상태에서 바로 FIN 을 받기 때문에 그 FIN 에 대한 ACK 를 보내고 CLOSING 상태가 된다. 그리고 이 상태에서 자신이 보낸 FIN 에 대한 ACK 도 받기 때문에 바로 TIME - WAIT 상태가 된다.

<img width="753" alt="스크린샷 2023-04-11 오후 2 47 14" src="https://user-images.githubusercontent.com/87372606/231067612-3aa54a17-c411-4f19-8c3c-b8605b404bfa.png">

---

### TCP 연결 / 종료

<img width="672" alt="스크린샷 2023-04-11 오후 2 50 10" src="https://user-images.githubusercontent.com/87372606/231068084-412823e4-267c-4f04-bcad-04a2c3f448fe.png">

- `클라이언트`는 자신의 프로세스를 통해 오픈 요청을 하기 때문에 `active`, `서버`는 listening 중 요청을 받아 오픈을 해주기 때문에 `passive`

- `패킷을 보낼 때 sequence 를 할당 : 전송계층에서 랜덤한 값을 생성하여 넣어준다.` (보안상의 이유 : 왜 랜덤한 값을 넣어주는지 생각해보기)

- ack 를 보내면 8001 을 보내야 함을 클라이언트가 알아차리고 8001 을 보낸다. 그리고 ack 가 담아온 패킷에 대한 ack 도 보낸다.

<img width="729" alt="스크린샷 2023-04-11 오후 2 50 21" src="https://user-images.githubusercontent.com/87372606/231068116-c0997a3d-07a2-4cd5-8796-65356ad837a3.png">

- FIN WAIT 1, 2 가 없는 기본적인 종료 과정

<img width="749" alt="스크린샷 2023-04-11 오후 2 52 34" src="https://user-images.githubusercontent.com/87372606/231068483-9456280f-d28f-4a75-95f6-a396c874dcb6.png">

- TIME WAIT 1, 2 가 있는 종료 과정 (FIN 과 ACK 가 떨어져있다는 점에서 차이가 있다.)

---

### TCP 흐름 제어

<img width="552" alt="스크린샷 2023-04-11 오후 2 54 12" src="https://user-images.githubusercontent.com/87372606/231068740-dfe883c4-3fb2-4455-8753-65c85b46204e.png">

- `송신 버퍼` : 송신 프로세스로부터 생산된 데이터를 받아 수신 측에 전송할 데이터를 보관하는 공간

- `수신 버퍼` : 송신 측으로부터 전송받은 데이터를 받아 보관하며, 수신 프로세스를 통해 데이터가 소비되는 공간

- 송/수신 버퍼는 `링 버퍼`로 구성 : 얼마나 받을 수 있는지를 `received window bit (rwnd)` 를 통해 수신자에게 알려준다.

    - 송/수신 하는 애들끼리 서로 버퍼가 얼마나 남았는지 알 수 있다.

- `송신 window` : 수신 측의 확인 (ACK) 없이, 한 번에 전달할 수 있는 바이트 사이즈

    - 데이터를 얼마나 보낼지를 결정한다. (tcp 는 요청하고 ACK 기다렸다 데이터 보내고 하기 너무 느리니까 한번에 bulk data 를 보내는 방식을 선택함) bulk 를 보내고 ACK 를 기다리는 방식..

- `수신 window` : 수신 측에서 수신할 수 있는 바이트의 사이즈

    - 얼마나 수신할 수 있는지에 대한 정보

---

## 서버 / 클라이언트 분석

- `서버 소켓 생성`

    - `socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)`

    - socket(AF_INET, SOCK_STREAM, 0)

    - 소켓 함수의 2번째 인자로 소켓의 종류 선택

    - 3번째 인자로 0을 선택하면 해당 소켓의 종류를 대표하는 프로토콜이 선택된다. 
        
        - SOCK_STREAM 의 경우 TCP 프로토콜


--- 

### 서버 소켓 주소 초기화 및 설정

```
#define PORT 9001
int main()
{
int srvSd;
struct sockaddr_in srvAddr;

...

srvSd = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성 

...

srvAddr.sin_family = AF_INET;
srvAddr.sin_addr.s_addr = htonl(INADDR_ANY); // IP주소 0.0.0.0으로 설정, 모든 IP로부터 접속 허용 srvAddr.sin_port = htons(PORT);

...

if(bind(srvSd, (struct sockaddr *) &srvAddr, sizeof(srvAddr)) == -1) // 소켓 바인드 

...

if(listen(srvSd, 5) == -1) // 수동형 열기 (CLOSED → LISTEN)

...

}
```

<img width="666" alt="스크린샷 2023-04-11 오후 3 06 29" src="https://user-images.githubusercontent.com/87372606/231070709-e7170ce5-eb09-4bc4-95c8-222edf2babd7.png">

- OS 를 통해 TCP 모듈에서 listening 하고 있고, 프로세스에서 소켓을 만들면 파일 디스크립터가 만들어지고 클라이언트에서 connect 를 통해 연결한다.

---

### 클라이언트 연결 요청

```
#define PORT 9001

int main() 
{
    int clntSd;
    struct sockaddr_in clntAddr;

...

clntSd = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성 ...
clntAddr.sin_family = AF_INET;
clntAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 127.0.0.1 은 호스트 자신을 가리킴 
clntAddr.sin_port = htons(PORT); // 소켓 구조체 초기화 (접속할 주소 및 포트 설정)

...

if(connect(clntSd, (struct sockaddr *) &clntAddr, sizeof(clntAddr)) == -1) //소켓 연결 
// 소켓 연결 시, TCP 상태는 ESTABLISH 로 변함

...

}
```

---

### 서버의 TCP 연결과 읽기 / 쓰기

```
...

clntSd = accept(srvSd, (struct sockaddr*)&clntAddr, &clntAddrLen); // TCP 연결 파일디스크립터 반환

...

readLen = read(clntSd, rBuff, sizeof(rBuff)-1); // TCP 읽기

...

write(clntSd, wBuff, sizeof(wBuff)); // TCP 쓰기
```

---

## Iterative 서버 모델

<img width="445" alt="스크린샷 2023-04-11 오후 3 15 07" src="https://user-images.githubusercontent.com/87372606/231072208-fddb69b4-79b1-4803-9435-e42c4ced341f.png">

- `Concurrent` 서버 모델 : 동시에 여러 클라이언트에게 서비스를 제공한다.

- `Iterative` 서버 모델 : 다수의 클라이언트들에게 순차적인 서비스를 제공한다.

    - `대기`하고 있는 커넥션이 있을 때, 지금 서비스하고 있는 애가 끝나면 다시 돌아와서 서비스하는 것을 반복한다.



- [서버 : tcpfirstsrv.c](https://github.com/jjaehwi/2023_Network/blob/main/Week3/tcpfirstsrv.c)

- [클라이언트 : tcpfirstclnt](https://github.com/jjaehwi/2023_Network/blob/main/Week3/tcpfirstclnt.c)
