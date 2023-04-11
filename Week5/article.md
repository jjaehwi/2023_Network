# Multi - Process

- [단일 프로세스 서버의 한계](#단일-프로세스-서버의-한계)

- [Parent/Child process](#parent--child-process)

- [Multi – Process – based server](#multi---process---based-server)

    - [MultiProcessTcpServer 예제](#예제---multiprocesstcpserverc)

- [Multi – Process – based client](#multi---process---based-client)

    - [ChatServer, ChatClient 예제](#예제--chatserverc--chatclientc)

- [IPC (Inter Process Communications)](#ipc-inter-process-communications)

    - [Signal 종류](#signal-종류)

    - [signal 함수](#signal-함수)

        - [예제](#signal-함수-예제)

    - [sigaction 함수](#sigaction-함수)

        - [예제](#sigaction-함수-예제)

    - [좀비 프로세스](#좀비-defunct-프로세스)

---

## 단일 프로세스 서버의 한계

- 이전 챕터에서 살펴본 Iterative 프로그램은 클라이언트로부터 요청이 오면 accpet 을 하고 클라이언트로부터 입력을 받아 버퍼에 저장해서 출력을하고 다시 클라이언트로 보내주는 프로그램이다.

    - iterative 한 성질 때문에 listen 을 2개까지 허용했다. 즉, 2개까지 접속을 받을 수 있고, 클라이언트가 2개에 접속했을 때 첫번째 일이 끝날 때까지 기다리는 프로그램이다.

- 아래 처럼 코드를 수정 (tcpfirstclnt.c --> tcptestclnt.c) 후 터미널 A, B, C 를 실행해서 두 개의 클라이언트에서 하나의 서버로 메세지를 전달해본다.

```
while(1)
{
    fgets(wBuff, BUFSIZ-1, stdin);
    readLen = strlen(wBuff);
    if(readLen < 2) continue;
    write(clntSd, wBuff, readLen-1);
    wBuff[readLen-1] = '\0';
    if(!strcmp(wBuff,"END")) break;
}
```

<img width="711" alt="스크린샷 2023-04-11 오후 8 38 23" src="https://user-images.githubusercontent.com/87372606/231151367-aac13dca-1e99-4a9b-aa85-d33c8a051111.png">

- iterative 한 서버이기 때문에 1,2,3,4,5 순서로 오는게 아니라 첫번째 접속한 클라이언트가 보낸 데이터가 기록이 된 후 종료가 되면 두번째 클라이언트가 보낸 데이터가 기록이 된다.
    
    - `단일 프로세스로 프로그래밍`을 하면 이 예제처럼 `서버는 클라이언트 하나만 받아서 처리를 하게 되기 때문에 다른 클라이언트는 기다려야하는 상황이 발생`한다.

    - 시간 순서 입력이 다른데도 불구하고 서버에는 먼저 접근한 클라이언트에 대한 데이터를 먼저 받게 되는 것이다.

    - 이런 상황을 해결하기 위해 `멀티 프로세스 프로그래밍`을 해야한다.

---

## Parent / Child process

<img width="738" alt="스크린샷 2023-04-11 오후 8 44 06" src="https://user-images.githubusercontent.com/87372606/231152846-41dc339c-1c8c-4c79-9f52-36dd1ca63c0b.png">

- 부모 프로세스가 `fork 함수` 를 통해 자식 프로세스를 `생성`한다. 

    - fork 함수가 호출되면, 호출한 프로세스가 복사된다.

    - 부모 프로세스와 자식 프로세스는 독립적으로 수행한다.

- 리턴 타입은 pid_t 로 int 값  부모 프로세스는 자식 프로세스의 pid_t 값이 리턴되고, 자식 프로세스는 자신을 가리키는 0 이 리턴된다.

- 실행 예시

<img width="438" alt="스크린샷 2023-04-11 오후 8 45 51" src="https://user-images.githubusercontent.com/87372606/231153245-b1fb30b1-1ab2-4bc1-bca5-2be1fb56d14d.png">

- 부모 프로세스는 else 로 들어가서 lval ++ 로 되어서, 부모 프로세스가 끝나면 gval 은 11, lval 은 26 이 된 상황일 것이다.

- 자식 프로세스는 if(pid==0) 으로 들어가서 gval ++ 를 하고, 자식 프로세스가 끝나면 gval 은 12 가 되고 lval 은 25 가 될 것이다.

---

<img width="652" alt="스크린샷 2023-04-11 오후 8 47 01" src="https://user-images.githubusercontent.com/87372606/231153498-59919fa5-95c4-4231-972b-29eb81551dc8.png">

- child 프로세스가 종료되면, parent 프로세스에 해당 정보가 전송된다.

- parent 프로세스는 wait 함수를 이용하여 child 프로세스의 변화를 알 수 있다.

<img width="745" alt="스크린샷 2023-04-11 오후 8 52 43" src="https://user-images.githubusercontent.com/87372606/231154710-20246045-1cf5-42f7-b0b2-93ef89b2e6b5.png">

- `wait(&status)` 로 자식 프로세스의 상태값을 받아온다.

- 정상 종료되었으면 정상 종료되었음을 출력하고 WEXITSTATUS 로 리턴값이 얼마임을 리턴한다.

- 자식 프로세스는 자식 프로세스가 생성되었음을 출력하고 count 값을 증가시키고 15 초 동안 기다렸다가 종료하면 3 을 리턴한다.

- `부모 프로세스와 자식 프로세스는 서로 독립된 메모리 영역을 갖고 있기 때문에` 부모 프로세스에서 출력하는 count 값은 자식 프로세스에서 +1 했더라도 0 이 출력될 것이다.

```
signal 로 죽이게 되는 경우

else //parent process 
	print(“Child PID %d \n”,pid);
{
    wait(&status); 
    if(WIFEXITED(status)) 
    {
        printf("Child process returns %d \n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
	    printf(“Child process has been terminated by signal\n”);
    }
}

컴파일 후 백그라운드로 실행하고 자식 프로세스의 PID 가 나오면 kill 자식 프로세스의 PID 로 죽일 수 있다.

./fork &
Child PID 2808

kill 2808

하면 Child process has been terminated by signal 가 출력된다.
```

---

## Multi - Process - based server

<img width="779" alt="스크린샷 2023-04-11 오후 8 59 23" src="https://user-images.githubusercontent.com/87372606/231156186-107aafdb-a5e8-4a73-b7a5-873a840a36cf.png">

- 부모 프로세스에서는 듣기 소켓을 통해 대기열에 클라이언트 요청이 오면 백 로그 만큼 받아놓고 하나씩 accept 을 수행한다.

- accept 하는 순간 fork 를 해서 서비스 소켓에 child 프로세스가 담당하게끔 구현을 해서 대기열에서 대기를 하지 않고 `concurrent` 한 시스템을 구동할 수 있다.

<img width="779" alt="스크린샷 2023-04-11 오후 8 59 27" src="https://user-images.githubusercontent.com/87372606/231156176-f528a30a-264f-4873-8908-88bc261e0b88.png">

- fork 한 다음 부모 프로세스는 서비스 소켓을 닫고, 자식 프로세스는 듣기 소켓을 닫으면 병렬적으로 프로그래밍이 가능하다.

- 듣기 소켓에서는 부모 프로세스에서 계속 대기열에 있는 클라이언트를 accept 할 수 있고 accept 된 클라이언트는 자식 프로세스의 서비스 소켓을 통해 서비스를 할 수 있다.

---

### 예제 : multiProcessTcpServer.c

- 변수를 선언하고 주소를 세팅해서 bind 하고 listen 하는 상황은 같다.

- accept 을 할 때 상황이 달라지는데 accept 을 하고 출력을 하고 에러가 없으면 fork 를 한다.

- fork 를 해서 pid == 0 이면 자식 프로세스 이기 때문에 서버 소켓을 닫고, 클라이언트가 보낸 요청을 읽어서 출력하고 다시 write 한다.

    - 끝나면 accept 을 통해 만들어진 클라이언트 소켓을 닫는다.

- 부모 프로세스에서는 클라이언트에 대한 요청을 자식 프로세스가 해주니까 fork 된 이후 바로 클라이언트 소켓을 닫으면 된다. 그러면 다시 while 문을 통해 새로운 클라이언트 소켓을 accept 해서 다른 클라이언트들에 서비스를 할 수 있게된다.

- 이전 단일 프로세스 서버의 한계를 드러냈던 테스트를 이 서버 코드로 실행해본다.

    - 터미널 A, B, C 를 실행해서 두 개의 클라이언트에서 하나의 서버로 메세지를 전달한다.

<img width="625" alt="스크린샷 2023-04-11 오후 9 04 42" src="https://user-images.githubusercontent.com/87372606/231157253-6de447ea-31b3-45fd-9482-959351275802.png">

- 메세지들이 concurrent 하게 감을 확인할 수 있다.

---

## Multi - Process - based client

- 클라이언트에서도 멀티 프로세스 기반 프로그래밍을 할 수 있음을 확인해본다. (구현의 간편함을 위해 클라이언트 리스트는 추가만 가능하고 삭제는 안된다.)

- 서버 자체는 단일 프로세스에서 돌아가도록 UDP 서버로 구현을 했으나 UDP 채팅 클라이언트는 멀티 프로세스로 구동되게 구현하려는데 멀티 프로세스가 필요한 부분은 

    - 사용자로부터 문자열을 입력받는 프로세스 (user delay)

    - 서버로부터 문자열을 수신하는 프로세스 (network delay)

- 둘 다 파일 입출력과 같은 I/O 를 하기 때문에 waiting 시간이 있을 것이다.

- 유저에서 입력받는 동안 서버에서 오는 데이터를 수신 못할 수 있고, 데이터를 수신하는 동안 입력을 못할 수도 때문에 이 부분을 멀티프로세스로 구현해본다.

<img width="768" alt="스크린샷 2023-04-11 오후 9 08 11" src="https://user-images.githubusercontent.com/87372606/231157987-f7a11f42-87ac-428d-ab3a-ee18111b72bf.png">

- 서버는 단일 프로세스로 돌아가는 UDP 서버로 클라이언트 리스트를 관리한다.

- 클라이언트에서는 fgets 를 통해 유저의 입력을 받고 sendto 로 서버로 데이터를 보내고, 서버로부터 데이터를 recvfrom 으로 받고 출력해서 사용자에게 보여준다.

    - fgets 와 recvfrom 이 I/O 관련 작업이기 때문에 서로 concurrent 하게 일어나게 하기 위해 멀티 프로세스로 구현했다. 

- 부모 프로세스는 fgets 를 통해 사용자의 입력을 받고 데이터를 서버로 보내도록, 자식 프로세스는 서버로부터 데이터를 받고 출력하여 사용자에게 보여주도록 구현한다.

---

### 예제 : ChatServer.c / ChatClient.c

`chatServer.c`

- define 을 통해 클라이언트 최대 접속자 수를 정의했고, 클라이언트의 리스트를 관리할 수 있는 소켓 배열을 선언했다. 

- 접속한 클라이언트의 숫자를 나타내는 nClient 가 선언되어있다.

- while 문에서는 udp 클라이언트의 요청을 받아서 관리한다.

    - recvfrom 을 통해 클라이언트로부터 데이터를 받는다. 클라이언트의 주소는 destAddr 구조체에 들어간다. 

    - checkSockList 를 통해 destAddr 저장된 정보랑 서버가 관리하고 있는 소켓 리스트에서 중복된 것이 있는지 확인하고 중복되지 않았으면 넘어가고 반환값을 res 에 저장한다.

    - 현재 클라이언트와 res 가 같으면 (새로운 클라이언트라면) 소켓 리스트에 지금 받은 destAddr 를 복사하고 복사가 끝나면 클라이언트의 숫자를 증가시킨다. (memcpy)

    - 그리고 for 문을 돌면서 접속한 클라이언트의 수 만큼 자신이 받은 데이터를 다른 클라이언트들에게 보내준다.

- checkSockList

    - for 문을 돌면서 자신이 할당한 소켓 address 리스트의 정보랑 지금 받은 entry 정보랑 비교를 한다. (memcmp) 같으면 소켓에 이미 있는 것이기 때문에 해당 index 를 리턴한다.

    - 같은 것이 존재하지 않으면 for 문이 count 까지 증가한 것이기 때문에 그 때의 i 를 반환하고 그 때의 i 는 nClient 와 같을 것이다.

`ChatClient.c`

- 채팅 서버에서 chatClient 는 유저로 부터 입력받는 부분을 부모 프로세스로 하고 서버로부터 입력받는 클라이언트 부분은 자식 프로세스이다.

---

## IPC (Inter Process Communications)

<img width="768" alt="스크린샷 2023-04-11 오후 9 15 59" src="https://user-images.githubusercontent.com/87372606/231159604-8e980094-3e3e-49ca-9b7c-b62409ec7f25.png">

- **`IPC` 는 `프로세스 간 통신`을 지원해주는 것.**

- `시그널 (Signal)` : IPC 에서 사용되는 하나의 메세지 형태, 일반적으로 UNIX 와 같은 **POSIX – compliant** 한 OS 에서 사용됨.

    - 운영체제와 프로세스간 통신에도 활용되고, 프로세스와 프로세스 간 통신에도 활용이 된다.

- (ex) 프로세스는 자신이 관심있는 시그널을 핸들러로 운영체제에 등록을 한다. 등록이 되면 운영체제가 감지를 하고 핸들러를 호출한다. 그럼 프로세스는 일반 루틴을 진행하고 있다가 핸들러가 호출하면 자신이 정한 핸들러 루틴을 수행한다. 그리고 끝나면 원래 했던 루틴을 계속 한다. 

---

### Signal 종류

- signal.h 에 정의된

<img width="647" alt="스크린샷 2023-04-11 오후 9 19 17" src="https://user-images.githubusercontent.com/87372606/231160286-e06d4016-11fe-4273-a3d8-705bbe539615.png">

<img width="693" alt="스크린샷 2023-04-11 오후 9 19 56" src="https://user-images.githubusercontent.com/87372606/231160410-3420b37f-23ae-4834-a3d4-ae726dfc1de3.png">

<img width="498" alt="스크린샷 2023-04-11 오후 9 20 54" src="https://user-images.githubusercontent.com/87372606/231160600-d1873d54-6222-4cee-bce9-cd862e0f7b39.png">

---

### signal 함수

<img width="444" alt="스크린샷 2023-04-11 오후 9 21 07" src="https://user-images.githubusercontent.com/87372606/231160671-ab7b3782-45d2-41c6-baae-52489079566d.png">

- Signal 을 사용하기 위해 등록해야한다. 시그널 등록은 시그널 함수를 통해 수행한다.

- 앞서 본 여러 시그널 종류들에 대한 int 값이 들어가고 시그널이 발생했을 때 호출되는 함수 값을 인자로 받는다.

---

### signal 함수 예제

```
#include <stdio.h>
#include <signal.h>

int global_count = 10;

void sigint_handler(int sig)
{
	if(sig == SIGINT)
	{
		printf("Received SIGINT... %d Lives Left \n", global_count--);
	}
	if(global_count <= 0)
	{
		signal(SIGINT, SIG_DFL);
	}
}

int main(int argc, char **argv)
{
	int i = 0;
	signal(SIGINT, sigint_handler);
	while(1)
	{
		printf("%d: sleep and awake \n", ++i);
		sleep(5);
	}
	return 1;
}
```

- ctrl + c 를 입력하면 SIGINT (시그널 인터럽트) 가 유저에 의해 발생한다. (운영체제가 보내주는 시그널) 

- 일반적으로는 ctrl + c 를 입력하면 프로그램이 바로 종료되지만 이 예제는 10번까지 입력받아서 몇 번 입력받았는지 이벤트 메세지를 출력하도록 하고 11째부터는 프로세스를 종료한다.

- 10 번을 count 할 수 있게 global_count 를 선언했고, main 에서 signal 이라는 함수를 통해 SIGINT 를 처리하고, 이게 왔을 때 메인함수가 호출할 함수는 sigint_handler 함수이다.

- sigint_handler 는 시그널을 받아서 유저가 발생시킨 SIGINT(시그널 인터럽트)면 몇 번 받았는지 print 를 출력한다. global_count 가 10번째 오고 11번째 올 때, 두번째 if 문으로 들어가서 SIGINT 를 본연의 기능으로 설정해서 프로세스를 종료시키도록 한다.

- main 에서는 while 을 계속 돌면서 5초마다 print 가 계속 나올테고 그 다음 ctrl + c 를 10번 이상 누르면 이 무한루프가 종료되게 될 것이다.

---

### sigaction 함수

<img width="752" alt="스크린샷 2023-04-11 오후 9 27 23" src="https://user-images.githubusercontent.com/87372606/231161962-309f7213-62f4-47a6-9f5b-68610361f3cc.png">

```
#include <signal.h>

struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    int sa_flags;       /* signal options */
    sigset_t sa_mask;   /* signal mask to apply blocking signals */
};
```

- sigaction 구조체

    - mask 는 블록할 수 있는 시그널에 대해 정의.

    - 시그널 flag 는 시그널과 관련된 옵션이 들어간다.

---

### sigaction 함수 예제

```
#include <stdio.h>
#include <signal.h>

int main(int argc, char **argv)
{
	int i = 5;
	struct sigaction new_action, old_action;

	new_action.sa_handler = SIG_IGN;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction(SIGINT, &new_action, &old_action);

	while(1)
	{
		printf("%d: sleep and awake \n", i--);
		sleep(1);
		if(i == 0) break;
	}
	return 1;
}
```

- sigemptyset 이라는 메서드는 &new_action.sa_mask 로 들어가는 값을 초기화한다는 뜻 (마스킹을 하지 않을 것이기 때문)

- sigaction 에 대한 변수가 new, old 로 두 개 선언

    - 새롭게 만든 액션에 대해 handler 를 호출하는데 SIG_IGN 로 넣었다.

    - 플래그도 0 으로 세팅

    - new_action 시그널 액션에 SIG_IGN 를 mapping 시키고 플래그랑 mask 는 0 으로 세팅한 것.

- sigaction 을 통해 유저에 의해 발생하는 SIGINT 를 new_action 으로 바꾸는 것. 그리고 본연의 기능을 old_action 에 저장했다.

- ctrl + c 가 SIG_IGN 됐기 때문에 이 시그널을 무시하게 된다.

---

### 좀비 (defunct) 프로세스

<img width="562" alt="스크린샷 2023-04-11 오후 9 32 52" src="https://user-images.githubusercontent.com/87372606/231163278-2c4b4a09-140d-4bfb-8a3e-78d14249386c.png">

- 부모 프로세스는 listen 하고 있고, 접속을 요청해서 데이터를 주고 받는 것은 자식 프로세스가 하고 있다. 

- 클라이언트가 END 로 종료를 하더라도 multiprocessTcpServer 의 상태를 보면 defunct 가 존재한다. 

    - 아직 운영체제에서는 아까 만든 자식 프로세스에 대한 정보를 유지하고 있는 상황이다. 좀비 프로세스인 상태인 것.

<img width="747" alt="스크린샷 2023-04-11 오후 9 33 02" src="https://user-images.githubusercontent.com/87372606/231163324-1e29fae8-5efe-4d76-aa0e-c0d881583cc9.png">

- 부모 프로세스에서 fork 를 하면 운영체제에 fork 한 프로세스 정보를 등록된다.

- 자식 프로세스에 대한 리소스 같은 것들을 운영체제가 할당을 해주고, 그리고 자식 프로세스에서 자식 루틴이 수행된다. 

- 자식 프로세스 자식 루틴이 끝나면 운영체제에 알려준다. 그러면서 자식 루틴을 통한 반환값과 자기가 받았던 리소스를 운영체제에 준다.

- 운영체제는 프로세스 정보와 반납된 리소스를 유지하고 있다가, 부모 프로세스가 `wait 를 호출`하면 부모 프로세스는 자식 프로세스가 종료 될 때 까지 기다린다. 

- 그러면 운영체제는 자식 프로세스가 줬던 정보를 부모 프로세스에게 준다. **그때서야 자식 프로세스에 대한 정보와 반환값을 운영체제에서 제거를 한다.**

- 앞서 본 multiprocessTcpServer 예제는 `wait 함수가 없기 때문`에 END 를 통해 종료를 했음에도 불구하고 자식프로세스의 반환값과 정보가 운영체제에서 유지가 되고 있는 것이다.

- **부모 프로세스에 wait 를 사용하면 자식 프로세스가 끝날 때까지 기다리는 것은 `적절히 자신의 자원을 사용 못하는 상황`이 생기기 때문에, `적절한 타이밍에 wait` 를 해야한다.**
    
    - `시그널을 사용`한다.

- `SIGCHLD` 를 받을 수 있는 핸들러를 등록해서 wait 를 호출하자.

    - 운영체제가 자식 프로세스가 끝났다고 알려주면 시그널이 오고 (SIGCHLD 발생) 

    - 그 시그널을 받는 핸들러에서 wait 함수를 호출한다.

    - wait 함수에서 자식 프로세스의 반환값을 받아오고 운영체제는 자식 프로세스의 자원을 제거하게 된다.

<img width="662" alt="스크린샷 2023-04-11 오후 9 36 57" src="https://user-images.githubusercontent.com/87372606/231164288-93b93eb2-c859-4546-b9b8-80a65ffbc44f.png">

- UDP 채팅 서버 / 클라이언트 프로그램에서 (ChatClient.c) kill (pid, SIGTERM)

    - 부모 프로세스에서 pid 는 fork 를 통해 받은 자식 프로세스의 pid 이고 그 프로세스를 termination 하는 코드였다.
