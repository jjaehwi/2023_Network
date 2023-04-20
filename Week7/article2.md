# Epoll Raw Socket

- [epoll](#epoll)

  - [epoll 객체 생성](#epoll-객체-생성)

  - [epoll 객체 제어](#epoll-객체-제어)

  - [epoll 모니터링](#epoll-모니터링)

  - [epolltcpsrv.c 예제](#epolltcpsrvc-예제)

  - [epoll mode](#epoll-mode)

    - [Synchronous blocking I/O](#synchronous-blocking-io)

    - [Synchronous non - blocking I/O](#synchronous-non-blocking-io)

    - [Asynchronous blocking I/O](#asynchronous-blocking-io)

    - [Asynchronous non - blocking I/O](#asynchronous-non-blocking-io)

  - [epolltecsrv2.c 예제](#epolltecsrv2c-예제)

  - [epoll 의 장/단점](#epoll-의-장--단점)

- [Raw Socket](#raw-socket)

  - [ip.c 예제](#ipc-예제)

  - [Checksum](#checksum)

- [Raw Socket 을 이용한 Sniffer 프로그램](#sniffer)

  - [sniffer.c 예제](#snifferc-예제)

---

## epoll

- `epoll` 도 select() 함수와 마찬가지로 `다수의 file descriptor 를 관찰하며 사용가능한 descriptor 를 발견할 경우 작업을 수행한다.`

- **스레드나 프로세스의 도움 없이 epoll (리눅스) 이나 select (윈도우, 리눅스) 를 통해 multiplexing 을 구현할 수 있다.**

- **epoll 절차**

  - epoll 객체 생성

  - epoll 객체의 제어

  - 모니터링 시작

  - 조건에 맞는 파일 디스크립터의 I/O 수행

---

### epoll 객체 생성

<img width="629" alt="스크린샷 2023-04-20 오후 8 38 10" src="https://user-images.githubusercontent.com/87372606/233354880-df79e7c7-344e-4426-a403-ac1a44a0c5df.png">

- `EPOLL_CLOEXEC` 옵션을 정의할 수 있다.

  - 새로운 프로세스가 열렸을 때, 지금 열린 파일 디스크립터를 어떻게 처리할 것인지에 대한 옵션

  - 프로세스가 관리하는 파일 디스크립터 테이블에 파일 디스크립터의 속성을 정의하기 위한 flag 값

  - 해당 flag에 close-on-exec을 EPOLL_CLOEXEC 통해 세팅할 수 있음

  - 만약 파일 디스크립터 A 에 close-on-exec 세팅이 되어 있으면 exec 계열 함수를 호출하여 다른 프로그램을 실행시킬 때 파일 디스크립터 A 는 닫힌 상태가 됨

  - 그렇지 않을경우 exec 계열 함수를 호출하여 다른 프로그램을 실행시켰을 때, 파일 디스크립터 A는 열린 상태를 유지함

---

### epoll 객체 제어

- epoll 객체에 관찰하려는 파일 디스크립터를 등록해야한다.

<img width="629" alt="스크린샷 2023-04-20 오후 8 41 49" src="https://user-images.githubusercontent.com/87372606/233355704-1d27fbfd-f326-4fd3-a8e9-df0112f43fb4.png">

- epoll_event 는 event 를 저장할 수 있는 자료형

  - 관리하고자 하는 이벤트를 지정할 수 있다. (read 이벤트, write 이벤트 ,,,)

  - 이벤트에 넘겨줄 데이터가 존재한다. (일반적으로 파일 디스크립터가 넘어간다.)

<img width="636" alt="스크린샷 2023-04-20 오후 8 42 08" src="https://user-images.githubusercontent.com/87372606/233355767-b9fc6838-a0db-47e7-bc7d-fa26a75f8d6e.png">

- `epoll_ctl` 함수를 통해 epoll 과 관련된 객체 제어를 할 수 있다.

  - epoll 파일 디스크립터

  - epoll 파일 디스크립터와 관련된 operation 이 지정된다. (어떤 것을 epoll 파일 디스크립터로 관리할 것인지에 대한 operation)

    - epoll 이 관리하고자 하는 파일 디스크립터를 등록하는 EPOLL_CTL_ADD

    - 이미 등록된 파일 디스크립터의 세팅을 변경하는 EPOLL_CTL_MOD

    - 관리되는 리스트에서 해당 디스크립터를 제거하는 EPOLL_CTL_DEL

  - operation 을 통해 하고자하는 작업을 지정하고 operation 과 관련된 파일 디스크립터를 넘겨준다.

  - epoll_event 를 전달해준다.

---

### epoll 모니터링

<img width="636" alt="스크린샷 2023-04-20 오후 8 46 50" src="https://user-images.githubusercontent.com/87372606/233356725-bf5b98c4-6602-4ff7-82b7-1507ae2c9bd6.png">

- operation 이벤트를 등록한 파일 디스크립터를 모니터링 하기 위해 `epoll_wait` 함수를 사용한다.

- epoll_ctl 함수를 통해 관리하고자 하는 이벤트가 등록이 되면

  - 해당 파일 디스크립터 (epfd) 를 통해 관리하고자 하는 이벤트를 모니터링 하고 있다가 그 이벤트가 포착이 되면 그 값을 선언된 \*events 에 넣어주면서 이벤트를 처리한다.

- event 는 파일 디스크립터가 동작하고 있다가 해당 관심 이벤트가 발생하면 해당 이벤트를 일으킨 파일 디스크립터와 관련된 정보를 넣는다.

- maxevnets 는 최대 몇 번의 event 를 볼 수 있는지에 대한 것

- timeout 은 wait 함수의 특성

  - 0 일 경우 한번만 체크 (wait 함수 한번 호출 후 끝)

  - -1 일 경우 무한 대기

  - 0 이상의 값은 타임아웃 시간

- 리턴 값은 성공 시, 요청된 I/O 가 ready 됐을 때 그 ready 된 파일 디스크립터의 갯수를 리턴한다.

  - wait 함수를 call 했을 때 epfd 가 관리하고 있는 파일 디스크립터에서, I/O 가 필요한 애들의 숫자가 리턴된다.

  - 0 이면 파일 디스크립터가 ready 되지 않은 것 (이벤트가 발생하지 않은 것)

  - 숫자가 리턴 되면 그 수만큼 파일 디스크립터들이 관리를 기다리고 있는 것.

- `epoll 동작 정의`

<img width="549" alt="스크린샷 2023-04-20 오후 8 54 37" src="https://user-images.githubusercontent.com/87372606/233358418-ae25e711-6423-435f-9a72-60daa2dc3c6d.png">

---

### epolltcpsrv.c 예제

<img width="637" alt="스크린샷 2023-04-20 오후 8 56 13" src="https://user-images.githubusercontent.com/87372606/233358750-3a9aaee7-c346-4215-8e4a-d0e8e946ce0d.png">

- 최대 처리 가능한 이벤트의 개수를 정의

- epoll 파일 디스크립터를 사용할 수 있는 int 형태의 변수를 선언

- epoll_event 라는 구조체와 구조체의 배열을 선언한다. (10 까지 허용하도록 선언)

- epoll_create 를 통해 epoll 객체를 생성해서 선언한 epfd 에 반환

  - 이후 listen, bind 동작은 동일

- 관심있는 이벤트를 정의해야한다.

  - epoll event 구조체에 이벤트 자료의 변수에 원하는 이벤트를 할당한다. (클라이언트로부터 요청이 오는 것을 read 하는 것이기 때문에 read 이벤트를 정의했다.)

- 관심있는 데이터는 소켓을 listen 해야하기 때문에 관심있는 파일 디스크립터는 listen 파일 디스크립터가 된다.

- epoll_ctl 을 통해 생성한 epoll 파일 디스크립터 객체에 EPOLL_CTL_ADD 라는 operation 을 줘서 listen 파일 디스크립터를 epfd 가 관리할 수 있도록 등록한다.

  - 이 때, 관심있는 이벤트는 앞서 정의했다.

<img width="642" alt="스크린샷 2023-04-20 오후 9 02 31" src="https://user-images.githubusercontent.com/87372606/233360128-050b03c1-18a2-4628-ab15-22038d654ac5.png">

- epoll_wait 를 통해 event 를 wait 한다.

  - epoll fd 객체가 들어가고 관리되는 것들이 이벤트로 복사될 수 있도록 이벤트 배열이 들어간다.

  - 그리고 MAX_EVNETS 를 쓰고 -1 을 썼으므로 무한 대기를 한다. (클라이언트 요청이 올 때까지 무한 대기)

- 클라이언트가 접속을 하면 wait 함수를 통해 ready 라는 변수에 얼마나 많은 클라이언트들이 요청했는지 개수가 return 이 될 것이고 그 만큼 for 문을 돈다.

  - for 문을 돌면서 이벤트에 정의된 것들에 대해 접근한다.

    - 처음 등록했을 때는 listen 소켓밖에 없으므로 event[0] 에는 listen 소켓 파일 디스크립터가 있을 것이고, listen 소켓일 경우 소켓에 accept 을 처리하고 출력한다. 그리고 connection 소켓을 만드는데 새로운 connection 소켓을 앞에 등록한 것처럼 epoll_ctl 함수를 통해 epfd 에 등록한다.

  - 그 외에는 기존 등록된 클라이언트들의 I/O 일 것이다.

    - readfd 라는 파일 디스크립터에 다른 이벤트의 배열 구조체를 이용해 저장된 파일 디스크립터를 불러온다. (connection 소켓을 통해 만들어진 클라이언트들의 파일 디스크립터)

    - read 로 데이터를 읽어와서 출력하고 그 파일 디스크립터를 통해 클라이언트에게 받은 데이터를 다시 전송한다.

---

## epoll mode

<img width="617" alt="스크린샷 2023-04-20 오후 9 12 23" src="https://user-images.githubusercontent.com/87372606/233362282-7be90d5c-d168-49ac-97e6-8a527eaf4bfc.png">

- epoll 에는 **두 가지 mode** 가 존재한다.

  - `Edge - triggerd mode` : **무엇인가 일어났을 때** 이벤트를 주는 것

    - 수신 버퍼에 데이터가 있을 때, 한 번 읽은 후 다 읽지 못했다면

    - 새로운 이벤트가 와야지 trigger 가 되어서 남은 데이터에 대해 접근할 수 있다.

  - `Level - triggerd mode` : **데이터가 사용 가능할 때** 이벤트를 주는 것

    - 방금 예제에 대한 것. default

    - 이벤트가 없더라도 수신 버퍼에 데이터가 남아 있으면 read 를 통해 계속 읽어올 수 있다. (level 이 유지되는 것)

- `blocking I/O` and `non-blocking I/O`

  - 일반적으로 I/O 는 커널에서 수행한다.

  - 유저 프로세스가 커널 프로세스에 I/O 요청을 하고 결과를 기다리는 것 : `blocking`

    - 유저 프로세스가 멈추고 네트워크 데이터나 input 을 기다리고 있는 것

  - 유저 프로세스가 커널 프로세스에 요청을 하고 다른 작업을 하는 것 : `non-blocking`

- `Synchronous` vs `Asynchronous`

<img width="529" alt="스크린샷 2023-04-20 오후 9 18 42" src="https://user-images.githubusercontent.com/87372606/233363632-4e53ab19-16c2-4d0d-8b6d-c7c9a5d6aba3.png">

---

### Synchronous blocking I/O

<img width="489" alt="스크린샷 2023-04-20 오후 9 29 06" src="https://user-images.githubusercontent.com/87372606/233366040-8d330d66-c306-43a9-84e3-f4179a2e68f1.png">

- 어플리케이션 프로세스가 수행이 되다가 읽을 것이 필요하면 커널에 read I/O 작업을 요청한다.

- 커널에서 데이터를 읽어올 때, 어플리케이션은 `blocking 이고 synchronous 이기 때문에 system call 후 기다린다.`

- 비효율적이지만 구현이 용이

---

### Synchronous non-blocking I/O

<img width="452" alt="스크린샷 2023-04-20 오후 9 31 06" src="https://user-images.githubusercontent.com/87372606/233366487-9230d502-870c-4d75-ae2c-88fdec65415c.png">

- 어플리케이션에서 데이터를 커널에 요청하고 non - blocking 이기 때문에 다른 작업을 할 수 있다.

- read 를 요청했지만 바로 읽을 것이 없기 때문에 error 가 발생하다가, kernel 로 부터 작업 이 끝나면 읽어 올 수 있게 된다.

- **커널의 I/O 작업이 끝났을 떄 App 이 확인할 방법이 없어서 read 를 통해서 확인**하는 것이다. 실패를 통해 확인을 하는 것이고 read 요청 사이사이에 다른 작업을 할 수 있다.

- synchronous 이기 때문에 I/O 작업 완료에 대한 signal 이 없어서 마냥 기다리다가 중간중간 간헐적으로 요청을 날리는 것.

---

### asynchronous blocking I/O

<img width="588" alt="스크린샷 2023-04-20 오후 9 35 51" src="https://user-images.githubusercontent.com/87372606/233367619-cbfa2d7b-72ad-40aa-b79c-2cc4c86c41f2.png">

- select 모델과 유사하다.

- 처음 확인하는 과정은 non - blocking 구성이다.

- kernel 에 요청을 하고 바로 read 를 실행하는데 데이터를 읽을 것이 없기 때문에 에러가 난다. 에러를 받으면 blocking 이기 때문에 select 를 호출한다.

  - select 로 부터 select 만 수행하도록 blocking 이 되고 이벤트가 발생하는지 모니터링 하고 있는 것.

  - I/O 처리가 끝나면 select 에서 데이터를 받을 수 있음을 알고 read 를 요청하여 데이터를 받는다.

---

### Asynchronous non-blocking I/O

<img width="588" alt="스크린샷 2023-04-20 오후 9 42 24" src="https://user-images.githubusercontent.com/87372606/233369233-4e3f5e31-d85b-4b6a-85fb-b8aaf0bf4afd.png">

- 어플리케이션에서 데이터를 읽을 때, asynchronous I/O 를 요청하면 커널이 데이터를 읽을 준비를 해서 데이터를 읽어오고 그 동안 어플리케이션은 다른 작업을 한다.

- 데이터 작업이 끝나면 signal 을 보내서 I/O processing 을 할 수 있다.

- 가장 효율적이지만 signal 을 처리하고 프로세스를 멈추고 다시 시작하는 작업의 구현이 복잡하다.

---

### Edge - triggerd epoll, Non - blocking socket

- epoll 의 edge - trigger 모드를 위해 epoll_event 구조체의 events 변수에 EPOLLET 을 설정한다.

- epol_wait 를 호출할 떄 무한 blocking 이 되어서 기다리는 것이 아닌 `값이 있는지를 확인하여 없으면 에러를 return 할 수 있도록 구성`된다.

  - Level Triggered 에 비해 Edge Triggered 는 필요 이상의 epoll_wait 함수 호출을 줄일 수 있다.

- **edge - triggerd epoll 을 사용하기 위해 소켓을 blocking 이 아닌 `non - blocking` 으로 만들어야한다.**

  - Non - blocking Socket 의 경우 `수신 버퍼에 데이터가 없을 경우, 에러코드를 반환` (수신데이터가 올 때까지 waiting 을 하지 않음)

  - **Non - blocking Socket** 을 위해서는 `fcntl` 함수를 이용

    - F_FETFL 을 통해 소켓의 정보를 가져오고 가져온 정보에 `NONBLOCK 속성을 추가`해서 정보에 다시 추가한다.

```
int res;
res = fcntl(socket, F_GETFL, 0);
if (res == -1) errProc("fcntl");
res |= O_NONBLOCK;
res = fcntl(socket, F_SETFL, res);
if (res == -1) errProc("fcntl");
```

<img width="565" alt="스크린샷 2023-04-20 오후 9 49 41" src="https://user-images.githubusercontent.com/87372606/233371018-fd73c815-d3b4-4cff-93e5-c07aaacc13b1.png">

---

### epolltecsrv2.c 예제

<img width="416" alt="스크린샷 2023-04-20 오후 9 51 04" src="https://user-images.githubusercontent.com/87372606/233371351-5fb7f130-be74-4a15-842a-edc60838183d.png">

- NONBLOCK 속성을 추가하여 소켓을 설정

<img width="617" alt="스크린샷 2023-04-20 오후 9 51 51" src="https://user-images.githubusercontent.com/87372606/233371562-75f2f538-d574-41fb-a23a-cb6a837dd6e5.png">

- 소켓을 만들고 앞에서 정의한 makeNbSocket() 함수로 non - blocking 소켓으로 만든다.

- epoll 이벤트를 만들 때, read 만 설정하는 것이 아닌 edge - triggerd 를 설정하는 EPOLLET 도 설정한다.

- 그리고 epoll_ctl 을 통해 listen 소켓을 등록한다.

<img width="527" alt="스크린샷 2023-04-20 오후 9 53 28" src="https://user-images.githubusercontent.com/87372606/233371920-11bc5213-de47-44a8-860b-accb8e23b0d4.png">

- waiting 을 통해 이벤트가 오는 것을 기다렸는데 edge - triggerd 이기 때문에 데이터가 오면 호출이 된다.

  - accept 하여 읽으려고 들어갔는데 데이터가 올 때 약간의 딜레이가 있을 수 있고 커널로부터 아직 안 넘어왔을 수도 있기 때문에 그런 에러 상황을 대비하여 에러처리 부분이 있다.

  - 데이터가 있어서 connection 을 할 땐, connection 소켓도 Non - blocking 소켓으로 만들어준다. 파일 디스크립터도 connect 소켓으로 바꾸고 EPOLLET 도 명시를 해서 epoll_ctl 함수를 통해 등록을 한다.

<img width="527" alt="스크린샷 2023-04-20 오후 9 56 43" src="https://user-images.githubusercontent.com/87372606/233372888-671bca77-25d8-41b4-81d4-6aa08e107ce1.png">

- 등록된 소켓으로부터 데이터가 오면 이벤트에 저장된 파일 디스크립터를 readfd 로 복사를 하고 데이터를 읽는다.

- 처음 edge - triggerd 로 데이터를 발생시켰을 땐, 데이터가 있으니 데이터를 write 하는 것이고 그렇지 않은 경우 데이터가 없을 경우 소켓이 닫히는 상황이므로 close 소켓을 하는 것

  - 그 다음 read 에 접근할 때는 read 를 non - blocking 함수로 선언했기 때문에 데이터를 읽으려고 접근했을 때 무조건 기다리는 것이 아닌 데이터가 없으면 data unavailable 을 출력한다. read 에러 발생 시 read error 를 출력한다.

- edge - triggerd 의 특성상 예상치 못한 에러가 발생할 수 있다. 퍼포먼스가 중요하고 잘 컨트롤할 수 있다면 edge - triggerd 방식으로 프로그램을 구성해도 됨.

---

## epoll 의 장 / 단점

<img width="544" alt="스크린샷 2023-04-20 오후 10 02 07" src="https://user-images.githubusercontent.com/87372606/233374192-480c8203-697b-4b44-80de-e7874cc11209.png">

- 모든 리눅스 시스템의 파일들은 파일 디스크립터로 관리할 수 있다.

- select 같은 경우는 파일 디스크립터를 select 에 등록 할 때, 가장 큰 파일 디스크립터를 기준으로 for 문을 돌면서 MAX_NUM + 1 을 넣어서 MAX_NUM 까지 체크를 한다. (0 ~ 끝까지 순차적으로 확인)

- epoll 은 원하는 파일 디스크립터만 등록하여 select 와 달리 epoll 에 등록된 파일 디스크립터가 1, 3, 5 라면 등록된 파일 디스크립터 3 개만 관찰을 한다.

- 즉, `select 는 MAX 까지 모두 도는 방식이라면 epoll 은 등록된 것만 보기 때문에 더 효율적`이다.

- select 는 리눅스, 윈도우에서 동작하지만 epoll 은 리눅스에서만 동작한다. 그래서 porting 할 땐 select 가 더 편하다.

---

## Raw Socket

<img width="620" alt="스크린샷 2023-04-20 오후 10 06 52" src="https://user-images.githubusercontent.com/87372606/233375446-5c239c3c-783d-4394-a8c2-e9829eb5fcdf.png">

- Raw 소켓을 사용하기 위해 type 에 지정한다.

  - 일반적으로는 헤더에 접근을 못하는데, raw 소켓은 데이터를 받았을 때 원시 데이터 그대로 App 에 올라오기 때문에 헤더를 조정하거나 읽어낼 수 있다.

- `Raw Socket` 과 `IP_HDRINCL 옵션`을 사용하면, **`IP 헤더정보`를 사용자가 수정할 수 있다.**

  - 만약 IP_HDRINCL 옵션이 활성화되어 있지 않다면, 소켓을 통해 전달받은 데이터는 운영체제를 통해 자동적으로 적절한 헤더를 세팅한다.

<img width="604" alt="스크린샷 2023-04-20 오후 10 09 35" src="https://user-images.githubusercontent.com/87372606/233376148-caf15c47-d8db-4a2a-975d-cfa4bd4f5337.png">

```
// IP 헤더 구조체 정의

struct ip_hdr
{
#if __BYTE_ORDER__ == __LITTLE_ENDIAN
    uint8_t ip_hdr_len:4; // (IP Header Length)
    uint8_t ip_version:4; // (IP Version)

#else
    uint8_t ip_version:4; // 구조체에서 “:”를 사용하여 사용될 비트 수를 정의할 수 있음
    uint8_t ip_hdr_len:4;

#endif
    uint8_t ip_tos; // (TOS Field)
    uint16_t ip_len; // (Payload Field= header + SDU)

    uint16_t ip_id; // (Identification Field)
    uint16_t ip_off; // (Flag(DF,MF) + Fragment offset Field)

    uint8_t ip_ttl; // (Time to Live)
    uint8_t ip_proto; // (Upper Layer Protocol)
    uint16_t ip_check; // (IP Checksum)

    uint32_t ip_src; //(Source Address)

    uint32_t ip_dst; //(Destination Address)
};
```

- `Raw 소켓은 TCP 헤더의 값도 변경할 수 있다.`

<img width="599" alt="스크린샷 2023-04-20 오후 10 11 17" src="https://user-images.githubusercontent.com/87372606/233376626-d47f94dd-0e62-4606-a018-dc3d327bc67a.png">

```
struct tcp_hdr {
    uint16_t tcp_src; // (Source Port)
    uint16_t tcp_dst; // (Destination Port)
    uint32_t tcp_seq; // (Sequence Number Field)
    uint32_t tcp_ackno; // (Acknowledgment Number Field)

#if __BYTE_ORDER__ == __LITTLE_ENDIAN
    uint8_t tcp_rsv1:4; // (Reserved 4bits)
    uint8_t tcp_hdr_len:4; // (Header Length)
    uint8_t tcp_fin:1; // (6bit flags = U/A/P/R/S/F)
    uint8_t tcp_syn:1;
    uint8_t tcp_rst:1;
    uint8_t tcp_psh:1;
    uint8_t tcp_ack:1;
    uint8_t tcp_urg:1;
    uint8_t tcp_rsv2:2;//(Reserved 2bits)

#else
    uint8_t tcp_hdr_len:4;
    uint8_t tcp_rsv1:4; // (Reserved 4bit)
    uint8_t tcp_rsv2:2; // (Reserved 4bit)
    uint8_t tcp_urg:1; // (6bit flags = U/A/P/R/S/F)
    uint8_t tcp_ack:1;
    uint8_t tcp_psh:1;
    uint8_t tcp_rst:1;
    uint8_t tcp_syn:1;
    uint8_t tcp_fin:1;

#endif
    uint16_t tcp_win_size; // (Window Size)
    uint16_t tcp_check; // (TCP Checksum)
    uint16_t tcp_urg_ptr; // (Urgent Pointer)
};
```

---

### ip.c 예제

<img width="570" alt="스크린샷 2023-04-20 오후 10 14 59" src="https://user-images.githubusercontent.com/87372606/233377559-adb3070f-ce51-446b-aa1f-225b3cfdd39b.png">

- IP 헤더와 TCP 헤더에 관한 구조체 포인터를 선언한 다음

- 소켓을 생성할 때 SOCK_RAW 로 생성한다.

- setsockopt 함수를 통해 IP_HDRINCL 를 설정해서 헤더를 조작할 수 있다.

<img width="612" alt="스크린샷 2023-04-20 오후 10 16 41" src="https://user-images.githubusercontent.com/87372606/233378012-80896360-6800-495b-af92-cecaa579b73b.png">

- 패킷에 대한 헤더들을 조작할 수 있다.

- checksum 은 인터넷에서의 데이터에 대한 오류 체크를 하는 것. 임의의 값을 넣더라도 실제 OS 에서 값이 맞지 않으면 원래 계산되어야하는 값으로 변경되어서 들어간다.

<img width="548" alt="스크린샷 2023-04-20 오후 10 18 15" src="https://user-images.githubusercontent.com/87372606/233378419-f88fcda7-7e09-4128-a390-1dc0c336b554.png">

---

### Checksum

<img width="561" alt="스크린샷 2023-04-20 오후 10 22 58" src="https://user-images.githubusercontent.com/87372606/233379695-4f901798-35ce-4cc9-8bc7-ab3df76cbd84.png">

- Sender 가 Recevier 에게 데이터를 보낼 때, 메시지와 Checksum 을 같이 보낸다.

  - Checksum 은 표준화된 함수를 사용

- 메시지를 Checksum 에 넣으면 메시지에 대한 Checksum Value (CV) 가 나와서 이 값을 Recevier 에게 같이 보내는 것.

- **Receiver 는 데이터를 받아 메시지에 대한 Checksum 을 자신이 계산하여 받은 거랑 같은지 확인**을 한다.

  - 같으면 메시지를 담은 데이터가 전달이 될 때, 중간에 네트워크 오류가 없었다는 것을 확인할 수 있다.

- 하지만 공격이 있었는지 없었는지는 확인할 수 없는데, 공격자도 Checksum 을 데이터를 바꾸고 올바르게 계산하여 보낼 수 있기 때문에 네트워크 상 오류가 있었는지 없었는지만 체크 가능하다.

---

## Sniffer

- 데이터를 엿듣는 것, 데이터를 도청하는 프로그램을 Raw 소켓을 통해 만들어 볼 수 있다.

  - Raw 소켓은 TCP 헤더와 IP 헤더에 접근할 수 있기 때문에 원하는 정보를 볼 수 있다.

<img width="606" alt="스크린샷 2023-04-20 오후 10 26 39" src="https://user-images.githubusercontent.com/87372606/233380731-89a207b1-9765-4ef2-ad00-aea689eeeb19.png">

- 일반적인 NIC 는 자신의 MAC 주소에 할당된 패킷만 읽어서 App 계층에 전달해주는데, 전체 네트워크를 모니터링 하기 위해서는 나의 MAC 주소가 아닌 다른 데이터들도 모니터링 해야한다.

  - 전체적인 모니터링 하기 위해 `NIC 를 Promiscuous 모드로 설정`해야한다.

  - 나를 거쳐지나가는 패킷들을 전부 모니터링 하고 싶으면 Promiscuous 모드를 설정하면 된다.

---

### sniffer.c 예제

- SOCK_RAW 로 설정을 하고 데이터를 받으면 IP 랑 TCP 헤더를 뿌린다.

- parseTCPHeader 함수를 통해 TCP 헤더에 관련된 정보들을 출력한다.

<img width="619" alt="스크린샷 2023-04-20 오후 10 31 02" src="https://user-images.githubusercontent.com/87372606/233381935-a2a115ea-8345-4b16-931d-9e9f349ec082.png">

---

- **References**

  - Hands-On Network Programming with C, Lewis Van Winkle

  - [IBM developer](https://developer.ibm.com/technologies/linux/articles/l-async/)
