# 소켓

- [소켓 (Socket)](#네트워크에서의-소켓-socket-이란)

- [소켓과 입출력](#소켓과-입출력)

- [통신 프로토콜](#통신-프로토콜)

## 네트워크에서의 소켓 (Socket) 이란?

-	`프로세스 사이 연결을 네트워크 해주는 통신 접점`, `네트워크 (인터넷) 의 연결 도구`

-	우분투를 포함한 많은 OS 가 제공해주는 `소프트웨어 적인 API`, `커널 단에서 제공해주는 함수`

-	소켓을 사용하면 어플리케이션 프로그래밍을 잘 다룰 수 있다.

-	네트워크를 통해 프로그램들이 통신을 하려면 소켓을 통한다.

    - 네트워크 프로그래밍이라는 것은 `소켓 API` 를 통해 컴퓨터 네트워크 프로그램을 만드는 것이다.

    - 소켓 API 를 이용하면 서로 다른 (또는 같은) **두 개의 호스트에서 동작하는 프로세스 간 통신을 지원**할 수 있다.

<img width="938" alt="스크린샷 2023-03-23 오전 11 56 58" src="https://user-images.githubusercontent.com/87372606/227089012-de4a9747-126a-4915-9f11-acdbf09e6bcb.png">

## 소켓과 입출력

- `소켓을 다룬다는 것`은 **OS 에서 제공되는 소켓이랑 프로그래밍하는 어플리케이션 레이어의 프로그램이랑 서로 통신을 해야한다는 것**이다. 즉, 서로 메세지를 주고 받는 것 = 소켓과 입출력을 하는 것

- 프로세스의 관점에서 소켓과 관련된 작업 (입출력) 은 다른 프로세스로부터 데이터를 받는 작업과 데이터를 주는 작업 (네트워크 프로그래밍은 입출력 작업이 소켓 기반으로 이뤄짐)

- 소켓은 여러 리눅스에서 제공하는 여러 입출력 방법론 중 하나이다.

    - 표준 입출력, 파일 입출력, 장치 입출력, 터미널, 소켓

    - 리눅스 시스템에서는 이런 입출력 작업을 파일로 다루는 API 로 추상화

    - 따라서 소켓 입출력 작업도 파일로 다루기 때문에 `파일 디스크럽터` 가 필요하다.

---

### 파일 디스크럽터 (File Descripter)

- 프로세스가 파일에 접근하기 위해 사용하는 하나의 `숫자 정보`

- 프로세스가 파일에 접근하기 위해서는 파일이 프로세스에 의해 `열린 상태`가 되어야 한다.

- 파일 디스크럽터는 `열린 파일들을 식별하기 위한 정보 (숫자)` 이다.

<img width="663" alt="스크린샷 2023-03-23 오후 12 00 28" src="https://user-images.githubusercontent.com/87372606/227089588-42463335-9eae-4d4d-b683-1e0b53dc620f.png">

---

### 리눅스의 기본 입출력 함수들

- `open`, `read`, `write`, `close`

1. ***open***

```
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *path, int oflag, ...);
```

- `const char *path` : 파일을 열고자 하는 경로

- `int oflag` : 파일을 어떻게 열 건지에 대한 플래그

<img width="875" alt="스크린샷 2023-03-23 오후 12 04 13" src="https://user-images.githubusercontent.com/87372606/227090134-d159f7c2-9ded-4830-b735-87c03a09ccbf.png">

- 성공적으로 실행되면, 함수는 파일을 열고 사용되지 않은 파일 디스크립터의 가장 낮은 번호를 나타내는 음이 아닌 정수를 반환해야 한다. 그렇지 않다면 -1

2. ***read***

```
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

- int fd : 파일 디스크립터

- void *buf : 버퍼 포인터

- size_t count : read 함수의 maximum number 바이트

- 성공하면 읽은 바이트 수 만큼 리턴값을 주고, (`0 은 파일의 끝을 나타낸다`) file position 이 이 숫자만큼 advanced. 에러 발생시 -1 을 반환한다.

3. ***write***

```
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

- int fd : 파일 디스크립터

- void *buf : 버퍼 포인터

- size_t count : write 함수의 maximum number 바이트

- 성공하면 쓴 바이트 수 만큼 리턴 값을 주고, 에러 발생시 -1 을 반환한다.

4. ***close***

```
#include <unistd.h>

int close(int fd);
```

- close (닫기) 는 파일 디스크립터를 닫아서 더 이상 어떤 파일도 참조하지 않으며 재사용할 수 없다.

- 성공하면 0 을 반환하고, 에러 발생시 -1 을 반환한다.

---

### [실습 01 : io-syscall.c](https://github.com/jjaehwi/2023_Network/blob/main/Week2/io-syscall.c)

- integer 자료형 선언 후 버퍼사이즈를 default 로 세팅한다.

- if 문에서 argument size 를 측정해서 정확하지 않다면 에러를 출력

- 파일을 읽을 때 `file descripter 자리에 0 으로 세팅을 하면` 그 의미는 읽어오는 것이 `standard input 으로 들어오는 것을 BUFSIZ-1 만큼 rBuff 에 저장`한다는 것.
 
    - 최대 8192 바이트까지 입력하면 rBuff 에 저장하는 read 함수 인 것이고, 그 때 standard input 을 통해 읽어서 버퍼에 저장한 만큼의 바이트 수를 리턴하는데 그 리턴 값을 readLen 에 저장하는 것.

- readLen == -1 이라는 것은 standard input 에 있어서 에러가 발생 한 것이므로 read error 메세지를 준다.

- 에러가 없이 standard input 을 통해 버퍼에 저장이 잘 된다면 총 몇개를 읽었는지를 출력한다.

- 이어서 ‘\0’ (null 문자) 를 쓰고, argument 로 입력한 값을 기본으로 해서 (argv[1]) 파일을 만든다. 

    - O_WRONLY | O_CREAT | O_TRUNC : write only 파일을 만드는데 파일이 존재하면 무시하고 존재하지 않으면 만든다. 만약 존재한다면 파일 length 를 0 으로 만들어서 처음 파일이름으로 새롭게 쓰게끔하는 조합이다.

- open 을 통해 return 되는 것은 file descripter 값인데 이를 fD에 저장한다. 
    
    - fD == -1 이면 open 이 성공적으로 수행되지 않았기 때문에 조건문으로 에러를 출력한다.

- 성공적으로 open 이 되었으면 그 파일에 아까 저장했던 버퍼에 있는 값을 아까 읽었던 길이 만큼 쓸 수 있을 것이다. (널 문자를 포함하기 때문에 readLen + 1 인 것)

- 그래서 총 쓰여진 바이트 를 리턴 값으로 writeLen 에 저장하고 == -1 이면 write 에러이므로 에러메세지 출력한다.

- 제대로 write 했으면 총 얼마나 쓰였는지를 출력한다. 그리고 close 로 파일을 닫는다.

---

### 소켓 API 

<img width="956" alt="스크린샷 2023-03-23 오후 1 31 03" src="https://user-images.githubusercontent.com/87372606/227103606-a48f5fa5-7995-4b21-ba30-a82c0602f3c5.png">

- 소켓에 들어가는 argument 

    - 도메인 : 프로토콜 체계 정보 

    - 타입 : 소켓의 데이터 전송방식 - TCP / UDP / RAW (맞춰서 TCP 헤더나 UDP 헤더를 수정할 때)

    - 프로토콜 : TCP 프로토콜 / UDP 프로토콜

- 성공하면 파일 디스크럽터 반환, 에러 발생 시 -1 반환

---

### [실습 02 : socket-syscall.c](https://github.com/jjaehwi/2023_Network/blob/main/Week2/socket-syscall.c)

- 파일을 4개 만든다.

    - sD1 - 소켓을 만든다. IPv4 인터넷프로토콜 체계, tcp 프로토콜, 0 이면 tcp 라고 명시하는 것

    - fD1 – 파일을 하나 만드는 것

    - sD2 – 또 다른 소켓을 만듬

    - fD2 – 또 다른 파일을 만듬

- 하나씩 출력해보는 것. 

    - 소켓이라는 것도 파일형태로 관리되고 파일 디스크럽터 리턴 값이 소켓이 성공적으로 열렸을 때 마찬가지로 반환된다 는 것을 확인할 수 있다.

- 순차적으로 증가하는 것을 봐서 파일 디스크럽터 값이 순차적으로 프로세스에서 할당해주는 것을 확인할 수 있다.

## 통신 프로토콜

- 컴퓨터 네트워크의 궁극적인 목적은 `다른 호스트상에서 실행되고 있는 프로세스 간의 통신` 이다.

- 프로세스는 `프로세스 ID (PID)` 로 식별 가능하다.

- 동일 호스트에서는 프로세스 id 로 프로세스를 구분할 수 있지만 서로 원격으로 있는 호스트에서는 서로 관리하는 프로세스 아이디가 다르다.

    - 서로 통신 하려면 소켓을 통해 통신을 해야하는데, 소켓은 transport layer 에 포트와 연결이 되어있다.

    - 그리고 ip 계층에서 ip 가 할당되고 data link 계층에서 맥주소가 할당되고 피지컬 계층에서 넘어간다.

    - **서로 원격으로 있는 호스트의 프로세스는 프로세스 아이디로 식별하지 못하고 `소켓에 할당된 소켓 포트 넘버로 식별`한다.**

        - 네트워크 계층 주소 (IP주소) 를 통해 전달된 데이터는 `전송계층에서 제공하는 포트를 통해 프로세스에 전달`된다.

<img width="655" alt="스크린샷 2023-03-23 오후 1 42 45" src="https://user-images.githubusercontent.com/87372606/227105242-c0b1a317-d6e8-49e6-a2f0-113137f8aee3.png">

<img width="746" alt="스크린샷 2023-03-23 오후 1 44 41" src="https://user-images.githubusercontent.com/87372606/227105486-3181ecd0-0268-4380-bb22-dda0a492a92a.png">

---

### PF_INET vs AF_INET

- PF_INET는 프로토콜 체계 (프로토콜 패밀리) 중 하나이고, AF_INET는 주소 체계 (주소 패밀리) 중 하나이다.

- 192.168.0.1 같은 IP 주소 체계를 인터넷 프로토콜뿐만 아니라 다른 프로토콜에서도 사용할 수 있도록, `IP 주소 체계를 지칭할 때는 AF_INET`, `IP 자체를 가리킬 때는 PF_INET` 을 사용하기로 했다.

<img width="950" alt="스크린샷 2023-03-23 오후 1 47 43" src="https://user-images.githubusercontent.com/87372606/227105981-ae2943c3-61a5-40f9-b1e2-12115308ac54.png">

---

***Reference***

- 네트워크 프로그래밍/최지훈/프리렉

- TCP/IP 소켓 프로그래밍/윤성우/오렌지미디어