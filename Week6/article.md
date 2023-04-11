# Multi Thread & Multiplexing

- [Thread](#thread)

    - [pthread_create](#pthread_create)

    - [pthread_join](#pthread_join)

    - [세마포어](#세마포어-semaphore)

        - [sem_open](#sem_open)

        - [sem_wait](#sem_wait)

        - [sem_post](#sem_post)

        - [sem_unlink](#sem_unlink)

    - [mutex](#뮤텍스-mutex-init-lock-unlock-destroy)

- [Socket programming with thread](#socket-programming-with-thread)

    - [예제](#예제--multithreadtcpsrvc)

- [Multiplexing](#multiplexing)

    - [select 함수의 기능과 호출 순서](#select-함수의-기능과-호출-순서)

    - [select 함수](#select-함수)

    - [시간 구조체 timeval](#select-함수의-시간-구조체-timeval)

    - [파일 디스크립터 집합 설정을 위한 매크로](#fd_set-설정을-위한-매크로)

    - [⭐️select 함수 총 정리](#select-함수-총-정리)

- [Socket programming with select](#socket-programming-with-select)

    - [예제](#예제--selecttcpsrvc)

    - [멀티 프로세스, 스레드 vs 멀티 플렉싱 (select 함수)](#멀티-프로세스-스레드-vs-멀티-플렉싱-select-함수)

---

## Thread

<img width="487" alt="스크린샷 2023-04-11 오후 11 23 24" src="https://user-images.githubusercontent.com/87372606/231193597-cea635ea-040d-4d5b-bec3-d6ea98c11148.png">

- 단일 프로세스에서 스레드를 여러개 만들어서 서버가 여러 클라이언트에게 요청을 받을 수 있도록 해본다.

<img width="690" alt="스크린샷 2023-04-11 오후 11 24 19" src="https://user-images.githubusercontent.com/87372606/231193840-11303f8e-e8f8-444f-902f-face0b8b37c6.png">

- 프로세스를 `fork` 해서 child 프로세스를 만들면 **parent 프로세스의 전체적인 메모리 구조를 자식 프로세스가 복사**해서 가져간다. 

     - 물리적인 메모리는 제한적인데 다양한 프로세스가 상주하면 메모리 낭비가 발생한다. 그리고 이들간 왔다갔다하는 context switching overheading 도 무시하지못한다.

- 경우에 따라서는 멀티 스레드로 구현을 할 때도 있다. `스레드랑 프로세스는 code, data, heap 영역을 같이 공유`한다. 그리고 `스레드 자신의 stack 만 따로 가지고 있다`. 

    - **프로세스에 비해 스레드가 메인 메인 프로세스에서 공유되는 영역이 많아서 자원 관리상 더 효율적**이다.

---

### pthread_create

<img width="733" alt="스크린샷 2023-04-11 오후 11 25 49" src="https://user-images.githubusercontent.com/87372606/231194243-cd9829a8-56b3-438f-a750-fb1889e8e86e.png">

- `pthread_create` 함수 : `스레드를 만드는 함수`

    - thread ID

    - thread 의 속성값을 저장하고 있는 attribute

    - 스레드와 함께 호출되는 함수

    - 그 함수에 전해줄 매개변수

- 컴파일 할 때 -lpthread 와 함께 컴파일해야한다.

```
- thread_1.c 예제 - 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *init_thread(void *parm)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		printf("Counter: %d \n", i);
		sleep(1);
	}
	printf("Thread is now terminated. \n");		
}

int main(int argc, char** argv)
{
	pthread_t thread_id;

	if(pthread_create(&thread_id, NULL, init_thread, NULL) != 0)
	{
		fprintf(stderr,"PThread Creation Error \n");
		exit(0);
	}

	sleep(5);
	printf("Main function is terminated.\n");	
	return 0;
}
```

- 메인에서 thread_id 에 대한 변수를 선언한다.

- attribute 는 NULL 로 세팅가능하다. 

- 호출되는 함수는 init_thread 로 명령했다. 

- 이 함수에 전달할 매개변수는 없으므로 NULL 이다.

- 호출이 되면 에러가 없다면 if 문을 지나가고 init_thread 함수를 보면 counter 가 1씩 증가하고 sleep(1) 을 한다.

- counter 를 세기 전에 main 함수가 종료되는 것을 확인할 수 있다.

---

### pthread_join

- 이전 예제에서 스레드를 만든 후 메인 함수가 종료될 때 만들었던 스레드도 같이 종료되는 상황을 확인했다. 

    - 스레드의 목적을 달성하지 못했는데 메인 함수가 종료되면서 같이 종료되어 버린 것이다.

- 그래서 메인 스레드에서 작업이 끝날 때 까지 기다리도록 하는 함수인 `pthread_join` 함수가 존재한다.

- create 후 pthread_join 을 호출하면 ***thread 가 수행되는 동안 자신이 만든 스레드가 일을 다 수행하고 종료될 때까지 기다린다.*** 종료가 되면 자신의 일을 수행한다.

<img width="733" alt="스크린샷 2023-04-11 오후 11 31 12" src="https://user-images.githubusercontent.com/87372606/231195859-afc609f4-59a7-4656-ab40-b8e3186ede93.png">

<img width="733" alt="스크린샷 2023-04-11 오후 11 31 01" src="https://user-images.githubusercontent.com/87372606/231195807-1c0fe0ca-4700-42e6-a4d4-f19eb9d47ded.png">

- thread_id

- thread 가 종료되면서 반환하는 값에 접근할 수 있는 더블 포인터

- 성공적으로 진행되면 0 return

    - 실패시, 여러 개의 thread 가 동시에 호출하면 교착상태가 발생할 수 있기 때문에 deadlock 에러발생 가능

```
- thread_2.c 예제 -

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *init_thread(void *parm)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		printf("Counter: %d \n", i);
		sleep(1);
	}
	printf("Thread is now terminated. \n");	
}

int main(int argc, char** argv)
{
	pthread_t thread_id;
	int res;

	if(pthread_create(&thread_id, NULL, init_thread, NULL) != 0)
	{
		fprintf(stderr,"PThread Creation Error \n");
		exit(0);
	}

	if(pthread_join(thread_id, (void **) &res) != 0)
	{
		fprintf(stderr,"PThread Join Error \n");
		exit(0);		
	} 
	printf("Main function is terminated.\n");	
	return 0;
}
```

- pthread_create 를 통해 스레드를 만들고 pthread_join 을 호출한다. 

- 메인 스레드에서 만든 스레드가 init_thread 를 다 수행할 때까지 기다린다. 

    - counter 가 0~9 까지 출력이 되면 종료가 되고 그러면 pthread join 이 잘 종료되었으니 0 을 return 해서 에러에 빠지지 않고 메인 스레드도 종료가 되게끔 한다.

---

### 세마포어 (semaphore)

<img width="733" alt="스크린샷 2023-04-11 오후 11 35 52" src="https://user-images.githubusercontent.com/87372606/231197162-14f534af-535e-4ddf-a61e-356f63358532.png">

- 스레드 프로그래밍에서는 우리가 공통 변수에 접근할 때 `동기화`를 잘 해야한다. 

- 스레드는 스택 영역을 제외하고는 같은 프로세스에 있는 메모리 공간을 공유하기 때문에 `공통변수에 접근`해서 작업을 하는 경우가 많다. 

- 동기화를 잘 하지 않으면 결과 반영이 잘 안되고 예상치 못한 결과가 나올 수 있다.

- 공통 자원을 관리하기 위해서 다양한 방법이 있는데 그 중 하나가 **`세마포어`를 통해 공통자원을 관리**하는 것

- ***리눅스 시스템에서 세마포어는 `파일 형태로 관리`가 된다. 파일에 접근할 수 있는 변수를 만들고 그 변수를 통해 공통자원을 관리하게된다.***

---

### sem_open

<img width="733" alt="스크린샷 2023-04-11 오후 11 40 10" src="https://user-images.githubusercontent.com/87372606/231198365-a80560bd-337e-40bd-b9bc-426307bd49fd.png">

- 파일을 만들어서 접근해야하기 때문에 파일 open 함수와 똑같게 sem_open 함수가 존재한다.

    - name: 파일의 이름 (세마포어 이름)

    - oflag: 세마포어의 특성

    - mode: 권한 정보

    - value: 세마포어 초기 값

- 성공시 세마포어를 가리킬 수 있는 포인터가 반환, 실패시 에러

<img width="733" alt="스크린샷 2023-04-11 오후 11 40 27" src="https://user-images.githubusercontent.com/87372606/231198446-cb85057c-9d19-4b8f-8bff-3ef43b30e5aa.png">

- argument 로 들어가는 flage 와 mode에 대해 살펴보면 리눅스에서 파일을 만들 때 볼 수 있는 것들이다.

---

### sem_wait

<img width="742" alt="스크린샷 2023-04-11 오후 11 41 48" src="https://user-images.githubusercontent.com/87372606/231198897-fcc5675c-388d-4e6c-9f93-8053ef0afdfd.png">

- 세마포어를 만드는 이유는 세마포어를 기준으로 `공통자원에 접근을 제어`하는 역할을 하기 위함이다.

- 세마포어 wait 함수는 공통자원이 있는 `critical` 한 영역에 들어가려고 할 때, **wait 함수를 통해 세마포어 값이 0 인지 아닌지를 체크**하게 된다. 

    - **값이 0 인 경우 그 값이 양수가 될 때 까지 공통 자원이 있는 섹션에 들어가려고 했던 스레드는 `대기상태`**가 된다.

-	argument 는 세마포어 영역을 가리키는 포인터가 들어간다.

---

### sem_post

<img width="742" alt="스크린샷 2023-04-11 오후 11 41 56" src="https://user-images.githubusercontent.com/87372606/231198922-1ab9203a-2989-4b30-b7bc-3ec83da5c8a0.png">

- 스레드가 어떠한 critical 세션에 들어가서 작업을 하다가 `끝내고 나올 때` **다른 스레드들이 이 영역에 접근할 수 있도록 해줘야한다.**

- **영역에 들어올 때 세마포어의 값을 0 으로 하면서 들어왔는데** ***나갈 때는 0 이 아닌 값으로 바꿔주고 나가야한다. ***

    - 그래야 다른 스레드들이 세마포어에 접근할 수 있다. 
    
    - 그래야 `세마포어 wait 를 하고 있던 애들`이 `세마포어 값이 0 이 아니기 때문에 들어올 수 있다.`

- 기존 critical 세션에서 작업을 하던 스레드들은 ***세마포어 post 함수를 호출해서 세마포어 값을 양수로 만들어준다.***

---

### sem_unlink

<img width="742" alt="스크린샷 2023-04-11 오후 11 45 34" src="https://user-images.githubusercontent.com/87372606/231199931-ad39207b-8773-428a-b589-b5dadd7301f9.png">

- open 으로 세마포어 를 만들었듯이 `unlink` 를 통해 **만들어진 세마포어를 제거**할 수 있다.

---

- [thread_3.c 예제](https://github.com/jjaehwi/2023_Network/blob/main/Week6/thread_3.c)

- 스레드 2개를 만들고 전역변수로 세마포어 포인터와 카운터가 있다. 

    - 이 전역변수는 스레드들이 공유하는 값이다.

- 메인함수에서 스레드들 호출하기 떄문에 id 를 두개 만들고 세마포어 open 을 통해 세마포어를 만든다. (파일처럼 관리를 하기 때문에 /Test 라는 파일경로로 세팅을 하고, 플래그와 권한, 초기값을 세팅한다.)

- 그리고 pthread_create 를 통해 스레드 A,B 를 호출하고 pthread_join 을 통해 스레드가 끝날 때 까지 기다린 다음 스레드 A,B 가 끝나면 세마포어 unlink 를 통해 세마포어를 해제한다.

- 스레드 A, B 함수를 보면, A 가 먼저 호출이 되게 되는데 세마포어 wait 함수에 들어오면 세마포어의 초기값은 1 이기 때문에 이 값을 0 으로 바궈주고 카운터를 증가시키는 작업을 수행한다. 

    - 카운터는 전역변수 이기 때문에 다른 스레드들이 접근할 수 있다. (counter 가 critical 세션인 것)

- 스레드 B 도 만들어진 후 B 함수가 실행되는데 카운터 증가시키는 작업을 하기 위해 세마포어 wait 함수를 체크한다. 

    - 세마포어 값을 확인할 때 A 가 작업 중이기 때문에 세마포어 값은 0 일 것이고 이 스레드에 대한 작업은 대기상태가 된다. 
    
    - 그리고 A 의 작업 (counter 가 60000번 증가) 이 끝나면 A 는 세마포어 post 를 통해 세마포어 값을 양수로 만들고 B 가 세마포어 wait 로 확인하고 있던 도중 세마포어 값이 양수가 된 것을 보고 critical 세션에 접근하여 세마포어를 0 으로 만들고 counter 를 증가시키는 작업을 진행하고 끝나면 다시 세마포어 post 를 통해 세마포어 값을 양수로 만들어준다.

---

### 뮤텍스 (mutex) init, lock, unlock, destroy

- 스레드를 동기화를 위해서는 세마포어 외에 `mutex` 를 사용할 수도 있다.

<img width="742" alt="스크린샷 2023-04-11 오후 11 51 11" src="https://user-images.githubusercontent.com/87372606/231201607-1231e246-85d9-4b67-8e92-1cba3a42fe8d.png">

- 인자

    - mutex 관련 구조체 포인터

    - 뮤텍스 객체에 대한 속성,, NULL 로 세팅해서 사용

<img width="742" alt="스크린샷 2023-04-11 오후 11 52 31" src="https://user-images.githubusercontent.com/87372606/231201999-9a1ac1f4-471c-4bc2-b7ef-a2d9c4c01b95.png">

- lock 과 unlock 이 존재해서 만든 뮤텍스 구조체 변수에 `lock` 을 하면 ***뮤텍스에 locking*** 이 되고 `unlock` 은 ***locking 을 해제***한다.

<img width="742" alt="스크린샷 2023-04-11 오후 11 53 11" src="https://user-images.githubusercontent.com/87372606/231202200-c5cc51b8-5889-4e26-91f4-fa95bbebde8a.png">

- 사용이 끝나면 뮤텍스 `destroy` 를 통해 해제할 수 있다.

---

- [thread_4.c 예제](https://github.com/jjaehwi/2023_Network/blob/main/Week6/thread_4.c)

- 세마포어가 아니라 pthread_mutex_init 을 통해 뮤텍스를 초기화시킨다.

    - pthread_join 을 통해 스레드의 작업이 끝날 때 까지 기다리다가 다 끝나면 pthread_mutex_destroy 를 통해 뮤텍스를 해제한다.

- 세마포어와 비슷하게 작업에 들어가기 전 `mutex_lock 을 하고 작업을 수행`한다. 그리고 `작업이 끝나면 unlock` 을 한다. ***locking 된 상태이기 때문에 스레드 B 에서 접근을 할 수 없다.***

***(H/W : 세마포어와 뮤텍스의 차이를 알아보자.)***

---

## Socket programming with thread

<img width="742" alt="스크린샷 2023-04-11 오후 11 57 00" src="https://user-images.githubusercontent.com/87372606/231203357-4cb8a923-2d92-4f6c-aa97-67528bf431bb.png">

- `pthread_detach` 함수를 통해 스레드의 호출이 종료되면 종료되는 시점에서 스레드가 사용한 자원을 자동적으로 반납하게 하도록한다.

    - pthread id

---

### [예제 : multithreadtcpsrv.c](https://github.com/jjaehwi/2023_Network/blob/main/Week6/multithreadtcpsrv.c)

- 스레드를 만들 때 호출할 client_module 함수 : 클라이언트로부터 데이터를 받으면 소켓을 통해 데이터를 읽어온다.

- 클라이언트로부터 요청이 올 때 단일 프로세스로 처리하면 클라이언트 여러명을 받을 수 없게 된다. 그래서 클라이언트 모듈을 통해 단일 프로세스지만 `멀티 스레드`를 이용해서 **여러 대의 클라이언트를 받게 해줄 수 있다.**

- 메인 함수는 기존과 같다.

- connectSd 을 accept 을 받을 때가 달라진다. 

    - accept 을 받으면 저번에는 fork 를 해서 프로세스를 만들었는데 이 예제에서는 `pthread_create` 를 통해 스레드를 호출하고 **client_module 함수에 우리가 받은 connectSd 디스크립터를 넣는다.**
    
    - 그러면 **클라이언트에서 이 디스크럽터를 받아서 connect 소켓 이 된다.** (소켓의 파일 디스크럽터가 넘어온 것) 넘어 온 것을 통해 클라이언트 모듈에서 read 를 할 수 있다.

- while 문을 돌 때 마다 스레드가 만들어진다. **클라이언트가 많아져도 스레드가 처리할 수 있게 되는 것**이다. 그리고 `스레드가 종료가 되면 스레드 detach` 가 된다.

---

## Multiplexing

<img width="645" alt="스크린샷 2023-04-12 오전 12 01 03" src="https://user-images.githubusercontent.com/87372606/231204560-69f1e263-03e8-4553-abe4-1dd9a984e8a2.png">

- 다수의 클라이언트 요청을 처리하기 위해 멀티 프로세스와 멀티 스레드를 배웠다.

- 그러나 **`한 개의 프로세스에서도 다수의 클라이언트를 처리하기 위한 방법이 존재`하고 `select 라는 함수`를 통해 처리할 수 있고 이것이 `multiplexing`**

    - `select` 라는 함수로 ***다수의 클라이언트 요청에 대해 처리***할 수 있다.

---

### select 함수의 기능과 호출 순서

<img width="261" alt="스크린샷 2023-04-12 오전 12 03 36" src="https://user-images.githubusercontent.com/87372606/231205370-472c7bb1-c62e-4e1e-9483-e4d35f778274.png">

- 멀티플렉싱을 하려면 `select 함수를 소켓과 연결지어 활용`해야한다. 

    - **select 함수는 문제가 있거나 전송해야하거나, 읽어야하는 소켓을 selection** 한다.

- 단일 프로세스에서 여러개의 소켓이 만들어 졌을 때, select 함수를 통해 처리해야될 소켓들을 선택하여 처리한다.

    - select 함수를 어떻게 사용해야하는지에 대한 정의, 호출 순서가 존재하고 이대로 구현을 하면 되는 것.

1. 어떠한 소켓들을 볼 것인지, 그러한 파일 디스크럽터를 선택하고, 어떤 것을 검사할 것인지 범위를 설정하고, 검사를 어느 정도 시간동안 할지 timeout 을 설정해야한다.

2. 설정이 되면 select 함수를 호출한다.

3. 어떠한 소켓에 event 가 발생했는지 호출 결과를 확인한다.

---

### select 함수

<img width="754" alt="스크린샷 2023-04-12 오전 12 05 37" src="https://user-images.githubusercontent.com/87372606/231205934-46e73ffd-7822-4875-8910-1ac0fccad0b8.png">

- select 함수를 통해 소켓에 어떠한 데이터가 있는지 없는지, 에러가 있는지 없는지 확인한다.

- 5개의 argument가 있다.

    - 파일디스크럽터 중 `가장 높은 값을 설정`해야한다.

        - for 문 같은 형태로 돌면서 모든 파일 디스크럽터에 대해 검사를 하는 느낌임 (***0 ~ nfds -1 만큼 examined***)

    - 관심있는 파일 디스크럽터 set 에 대해 설정한다.

    - timeout 값 설정한다. select 함수는 소켓들을 관찰해서 event 가 있는지 없는지 처리를 하는데, timeout 이 NULL 이면 event 가 발생할 때 까지 기다리는 select 함수가 될 것이고 timeout 값이 설정되면 그 기간만큼만 기다리는 select 함수가 될 것이다.

- 성공 시 select 로 선택된 파일디스크럽터의 값 반환된다. (0이상의 정수)

---

### select 함수의 시간 구조체 (timeval)

<img width="745" alt="스크린샷 2023-04-12 오전 12 28 05" src="https://user-images.githubusercontent.com/87372606/231212653-493f2549-9176-454c-847f-f5ad2b41ca1b.png">

- select 함수에는 time 과 관련된 구조체가 있다. 시간을 세팅하고 싶으면 구조체로 시간을 세팅하면된다.

---

### fd_set 설정을 위한 매크로

<img width="754" alt="스크린샷 2023-04-12 오전 12 19 10" src="https://user-images.githubusercontent.com/87372606/231210009-afc32d61-4760-4f47-b9dd-f3e228b2a26c.png">

- `파일 디스크럽터 set (fd_set) 을 설정`해야한다. 

    - 파일 디스크럽터 set 은 관심있는 파일디스크럽터의 집합이다. 이런 것들을 하나의 자료형으로 관리하는 것 이다. 
    
    - 일일이 bit 단위로 관리하기 복잡하기 때문에 파일 디스크립터를 세팅할 수 있는 매크로를 지원한다. 이 매크로를 활용하여 관심있는 파일디스크럽터 set 을 지정할 수 있다.

- `FD_SET` 을 통해 관심있는 파일 디스크립터의 부분의 비트를 1 로 설정

- `FD_CLR` 를 통해 관심이 없어 졌으므로 그 부분의 비트를 0 으로 설정 

- `FD_ISSET` 함수를 통해 fdset 에서 어느 부분이 1 인지 알 수 있다. (매개변수로 들어가는 fd 부분의 비트가 1 이면 양수를 반환한다.)

---

### ⭐️ select 함수 총 정리 ⭐️

- After `select()` has returned, ***readfds*** will be cleared of all file descriptors except for those that are ready for reading. After calling `select()`, the `FD_ISSET()` macro can be used to test if a file descriptor is still present in a set. 

- select 함수는 관심있는 파일디스크럽터를 보고 있다가 어떤 event 가 발생한 애를 리턴해주는 함수이다.

    - 해당 event 가 발생한 파일 디스크럽터를 return 하는데,,,

    - 우리가 관심있는 애들을 보려면 select 함수에 넘겨주는 파일 디스크럽터 set 을 만들어줘야한다.

- 예를 들어, `FD_SET` 이라는 매크로를 통해 `FD_SET(1,fdset)`, `FD_SET(2,fdset)`, `FD_SET(3,fdset)` : 파일 디스크립터 1,2,3 을 fd set 에 넣고 관찰을 한다.

- select 함수에는 4 를 넣어줘야한다. (0 ~ 4-1 까지 파일디스크립터를 관찰하기 떄문) (가장 큰 값에다가 + 1 한 값을 넣어준다.) : select(4,fdset, ....)

- 만약 2 번 파일 디스크립터에서 이벤트가 발생했다고 한다면, select 함수는 2 를 return 할 것 이다.

- ***fdset 에 비트열로 관리된다고 하면 0 ~ 4 중 1 2 3 에 대해서 비트 1 로 설정이 되어있을텐데, select 함수에 의해 return 값이 2 가 됐다는 것은 비트열이 0,0,1,0,0 이 됐다는 것이다.***

    - `이벤트가 발생한 것을 제외하고는 모든 파일 디스크립터의 값은 0 으로 세팅이 된다.` **그렇기 때문에 2 가 리턴 될 수 있는 것이다.**

- input -> 0 1 1 1 0 에서, output -> 0 0 1 0 0 이 되는 것

- 그래서 FD_ISSET 이 select 함수 이후 호출을 할 때, FD_ISSET(2, fdset) 을 하면 양수가 반환이 될 것이다.

- 만약 FD_ISSET(1,fdset) 을 하면 양수가 반환되지 않을 것이다. (fdset 은 select 함수 이후 변화했기 때문)

- **중요 : `select 함수는 input 으로 들어온 fdset 의 값들을 modify 한다.`**

---

## Socket programming with select

- `multiplexing` 을 지원하는 `select() 함수`를 이용해서 `socket programming`

<img width="761" alt="스크린샷 2023-04-12 오전 12 31 20" src="https://user-images.githubusercontent.com/87372606/231213508-96d0ccb4-ee1b-47b8-a20b-f15485cde9e0.png">

- 멀티프로세스 혹은 스레드로 소켓 프로그래밍 하는 것과 멀티플렉싱으로 하는 것을 `비교`해봐야한다. 그 전에 멀티플렉싱 코드를 먼저 살펴보자.

---

### [예제 : selecttcpsrv.c](https://github.com/jjaehwi/2023_Network/blob/main/Week6/selecttcpsrv.c)

- selecttcpsrv 코드는 `select` 함수를 이용하는 서버 프로그래밍 코드이다.

- maxFd 를 선언하고 초기화 되어있다. 그리고 파일 디스크립터 set 을 정의하기 위한 변수가 있다.

- 소켓을 만들고 bind 한뒤 listen 하고 있는 상황에서,,

    - 관심있는 **파일 디스크립터 set 을 세팅**해야한다.

    - default 로 defaultFds 라는 변수를 쓰기 때문에 FD_ZERO 로 세팅을 하고

    - 관심이 있는 파일 디스크립터를 FD_SET 로 추가한다. (관심이 있는 파일디스크립터는 우리가 만든 listen 소켓을 담당하고 있는 파일디스크립터 일 것)

    - maxFd 의 값은 `listen 소켓의 파일디스크립터가 최댓값`이 된다.

- 그리고 while 문을 돌게된다. `select 를 통해 관심 있는 곳에 event 가 발생하면 처리`할 것이다.

- `select 함수는 인자로 들어가는 것을 modify 하기 때문에 복사본을 만들어야한다.` 

    - 그게 아까 만든 변수 두 개중 디폴트 변수가 아닌 **rFds 라는 변수**일 것. (디폴트Fds 가 원본)

    - 그리고 select 호출할 땐 복사본을 넣는다. 그리고 max 값에 + 1 한 것을 디스크립터 max 값으로 넣는다.

- for 문을 돈다. (0 부터 maxFd 까지 반복문이 돌아간다)

    - select 를 통해 뭔가 호출이 되면 connection 이 들어왔다는 것..! (listen 소켓 파일디스크립터 이므로)

    - FD_ISSET(I, &rFds) 를 통해 우리가 저장한 set 에 I 번 째 파일 디스크립터에 변화가 있었는지 를 물어본다. 그러면 그 때의 I 를 listen 소켓 아이디와 비교하여 동일하면 그 땐 accept 하여 `connection 파일 디스크립터`를 만든다. 

    - 이 connection 파일 디스크립터도 통신을 하면서 계속 주의깊게 봐야하는 파일디스크립터 이기 때문에 **FD_SET 을 통해 원본 디스크립터 set 에 세팅을 한다.**

    - 그리고 만약 이 connection 파일 디스크립터가 max 파일 디스크립터보다 크면 max 값을 갱신해준다.

- 그리고 다시 while 문을 도는데 디스크립터 set 에 connection 파일 디스크립터도 설정이 되었기 때문에 이 connection 파일 디스크립터에서 이벤트가 발생할 수 도 있다. 

    - 그래서 I 번째에서 이벤트가 발생이 되고 확인을 할 때 `listen 하고 있는 소켓에 대한 디스크립터와 맞지 않으면` ***connection 파일 디스크립터에서 이벤트가 발생한 것 (I/O 이벤트)*** 이므로 connection 파일 디스크립터로 부터 read 하게 된다. 
    
    - 그리고 그 값을 출력한다. 만약 connection 이 종료되면 FD_CLR 를 통해 원본 디스크립터 set 에서 제거를 하고 소켓을 close 한다.

- 이런 식으로 스레드와 프로세스를 사용하지 않고 멀티플렉싱 개념을 사용해서 소켓 프로그래밍을할 수 있다.

---

### 멀티 프로세스, 스레드 vs 멀티 플렉싱 (select 함수)

- `멀티 프로세스랑 스레드`는 ***socket 을 만들고***, ***bind*** 를 호출하고 ***listen*** 을 한 뒤 ***accept*** 시점에서 `fork` 를 하던지, `pthread_create` 를 한다.

- `멀티 플렉싱`은 ***듣기 socket 을 관심으로 설정***을 먼저 해 놓은 뒤 `이벤트가 발생`하면 ***accept*** 을 호출해서 ***connection 소켓을 만들고*** 이 `connection 소켓도 관심대상으로 설정`을 해서 관리를 한다.

    - **소켓이 종료**되면 `관심 대상에서 제거`를 해주면 된다.
