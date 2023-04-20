# Basic Chatting App

- [TCP client](#a-tcp-client)

  - [tcp_client.c 코드 분석](#tcp_clientc)

- [TCP server](#a-tcp-server)

  - [tcp_serve_toupper.c 코드 분석](#tcp_serve_toupperc)

- [Building a chat room](#building-a-chat-room)

---

## A TCP client

- Basic Program Flow

<img width="423" alt="스크린샷 2023-04-20 오후 7 09 47" src="https://user-images.githubusercontent.com/87372606/233335193-6d5712dc-9f4d-4a80-8907-a133a872b864.png">

- 보통 클라이언트 프로그램은 실제 IP 주소를 입력해서 접속하는 것이 아닌 해당 서버의 도메인 이름을 받아서 프로그램이 IP 주소로 바꾸는 `getaddrinfo()` 함수가 사용된다.

  - 이후 socket() 함수를 통해 소켓을 만들어서 connect() 함수로 connection 을 한다.

  - stdin 으로 data 를 받아서 서버에 데이터를 보내고

  - 소켓으로부터 데이터가 오면 그것을 받아서 처리하는 루틴

---

### tcp_client.c

- 입력값으로 client 가 접속할 **host name 과 port number** 를 입력함

- Client 가 접속하고자 하는 host name 을 가진 서버에 접속을 한 후, 데이터를 주고 받음

```
// argument 를 check
if (argc < 3){
    fprintf(stderr, "usage: tcp_client hostname port\n");
    return 1;
}
```

---

- 접속하고자 하는 address 에 domain name 을 통해 가서 IP 주소를 받아와야 한다.

```
// hint 정보를 통해 peer_address 에 갖고 오고자 하는 정보들이 저장된다.
printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
```

---

- 일반적으로 서버 클라이언트 프로그램을 할 때 로그를 남기는 것.

  - 접근하고자 하는 서버의 정보를 출력하고자 할 때, IP 주소와 서비스에 대한 정보를 출력 (`getnameinfo`)

  - peer_address 에 할당됐던 정보를 출력 가능하게 바꿔주는 것.

    - IP 주소와 port number 가 숫자로 들어가 있는데 `NI_NUMERICHOST` 옵션을 통해 버퍼에 내용을 저장하면 dot 형태의 IP 주소와 숫자가 아닌 서비스의 이름이 저장된다.

```
printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);
```

---

- 소켓을 만든다. 소켓을 만들고 에러 체크를 하는 것.

```
printf("Creating socket...\n");
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family,
            peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
```

- connection 을 한다. getaddrinfo 를 통해 받아 온 peer_address 안에 있는 address 와 service name 을 통해 소켓에 접근을 한다.

  - 서버에 접근이 되면 peer_address 는 메모리 해제된다. (freeaddrinfo)

```
printf("Connecting...\n");
    if (connect(socket_peer,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(peer_address);
```

---

- 그리고 클라이언트가 데이터를 읽어오는 부분

  - 데이터를 사용자로부터 데이터를 서버로 보내는 부분

  - 서버로부터 데이터를 받아오는 부분

  - `select` 를 이용하여 구현했다.

  - 파일 디스크립터 set 에 관련된 변수를 설정하고 세팅한다. (socket_peer 에 소켓 connect 를 했기 때문에 socket_peer 에 대한 파일 디스크립터를 등록)

  - 사용자로부터 입력값을 받기 위해 FD_SET(0, &reads) (stdin 의 파일디스크립터는 0 이기 때문)

    - or FD_SET(fileno(stdin), &reads)

  - select 함수가 timeout 을 할 수 있기 때문에 값을 할당 (100 milliseconds)

  - 그리고 select 함수를 통해 파일 디스크립터 set 을 돈다.

  - &reads 에 event 가 발생하면 socket_peer 일 때, 데이터를 받아와서 출력한다.

    - recv() 함수로 받아온 데이터는 null 로 끝나는 형식이 아니라 읽은 만큼만 출력해야한다. 그래서 %.\*s format specifier 를 통해 출력한다.

  - socket_peer event 가 아닌 stdin 을 통해 event 가 발생한 것이라면 사용자가 데이터를 입력한 것이므로 fgets 를 통해 사용자 입력값을 읽어오고 서버로 보낸다.

```
while(1) {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);
        FD_SET(0, &reads);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(socket_peer+1, &reads, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        if (FD_ISSET(socket_peer, &reads)) {
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                break;
            }
            printf("Received (%d bytes): %.*s",
                    bytes_received, bytes_received, read);
        }

        if(FD_ISSET(0, &reads)) {
            char read[4096];
            if (!fgets(read, 4096, stdin)) break;
            printf("Sending: %s", read);
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
            printf("Sent %d bytes.\n", bytes_sent);
        }
    }
```

<img width="557" alt="스크린샷 2023-04-20 오후 7 45 04" src="https://user-images.githubusercontent.com/87372606/233343382-5094703d-1395-46a9-ac29-46fd80fbc935.png">

---

## A TCP server

- Basic Program Flow

<img width="312" alt="스크린샷 2023-04-20 오후 7 45 19" src="https://user-images.githubusercontent.com/87372606/233343425-8db457c9-8376-40bf-bf5b-04531b1692f0.png">

- getaddrinfo 를 통해 자신이 listen 할 network interface card 와 port number 를 설정하고 정보를 socket 에 넣어서 생성된 파일 디스크립터를 통해 bind() 로 binding 한다.

- 클라이언트의 접속을 기다리기 위해 listen 하고 select 를 통해 listen 중이던 소켓을 등록하면서 client 들의 요청을 받는다.

  - 새로운 connection 이라면 accept 을 하고

  - socket 에 대한 이벤트면 처리를 한다.

- 상대방에게 보내줄 땐 toupper() 함수를 통해 자신이 받은 데이터를 대문자로 출력한다.

---

### tcp_serve_toupper.c

- 서버의 IP 주소를 받아오고 포트 넘버를 세팅하는 부분

  - addr 구조체를 통해 서버의 주소랑 포트 넘버를 bind_address 에 세팅

- socket_listen 을 생성

  - bind_address 에 저장된 소켓의 특징, 타입, 프로토콜을 가져와서 세팅하고 socket_listen 에 대한 파일 디스크립터를 생성한다.

```
printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);


    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
```

---

- binding 을 한다. bind_address 에 세팅했던 서버의 주소와 포트 넘버를 저장한다.

- 10개 까지 받는 listen 을 수행한다.

```
printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
```

---

- select 를 통해 listen 을 하기 때문에 select 를 수행하기 위한 파일 디스크립터 set 을 만들어야한다.

  - 지금 생성한 socket_listen 이 가장 큰 파일 디스크립터 일 것이기 대문에 max_socket 을 변수에 저장하여 관리

```
fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");
```

---

- 그 다음 while 문을 통해 select 를 이용하여 새로 들어오는 connection 에 대한 aceept 을 하고 계속 관찰해야하기 때문에 아까 등록한 파일 디스크립터 set 에 등록한다.

- select 를 수행하기 전에 만들었던 master 파일 디스크립터 set 의 **복사본인 reads 파일 디스크립터 set 을 만든다**.

  - select 함수는 max_socket + 1 만큼 관찰해야하고 관찰 대상을 저장하는 곳은 복사본인 reads 에 저장을 한다.

  - `select 는 파일 디스크립터 set 을 modified 하기 때문`에 복사본을 사용하는 것 (원본을 유지해야함)

  - 관찰하는 것 중 이벤트가 소켓을 listen 하는 파일 디스크립터 였다면 새로운 클라이언트에 대한 요청이므로

    - accept 을 해준다.

    - accept 된 소켓의 새로운 파일 디스크립터를 마스터에 새로 등록을 한다. (FD_SET)

    - 그리고 출력한다.

  - 새로운 클라이언트가 요청이 아닌 이벤트는 기존의 관리 대상인 클라이언트들의 데이터가 온 것이므로 read 배열을 통해 recv 한다.

    - 받은 데이터의 크기가 1 보다 작다는 것은 소켓이 종료되거나 에러가 발생한 것이기 때문에 소켓을 닫는다.

      - 닫았으므로 select 에서 관리하고 있던 파일 디스크립터는 관리 대상이 아니게 되므로 마스터에서 제거해준다. (FD_CLR)

    - 정상적인 데이터라면 해당 데이터를 (소문자) 대문자로 바꿔서 클라이언트에게 다시 send 한다.

```
while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i) {
            if (FD_ISSET(i, &reads)) {

                if (i == socket_listen) {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);

                } else {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }

                    int j;
                    for (j = 0; j < bytes_received; ++j)
                        read[j] = toupper(read[j]);
                    send(i, read, bytes_received, 0);
                }
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)
```

---

## Building a chat room

- 채팅 서버를 만드는 것은 방금 살펴본 tcp_serve_toupper 코드 에서 정상적인 데이터를 다시 클라이언트에게 보내주는 부분에 대해서 수정을 하면 된다.

- 관찰하고 있는 파일 디스크립터에 이벤트가 발생했을 때, 소켓에 대한 변수를 다시 설정하고 1 ~ max_socket 까지 돌면서

  - 해당 j 가 마스터에 등록된 얘고,

  - 현재 listen 하고 있지 않고 while 문에 들어온 애가 아니면 (listen 소켓과 자기 자신이 아니면)

  - 채팅 상대방인 것이므로 데이터를 보낸다.

```
SOCKET j;
    for (j = 1; j <= max_socket; ++j) {
        if (FD_ISSET(j, &master)) {
            if (j == socket_listen || j == i)
                continue;
            else
                send(j, read, bytes_received, 0);
        }
    }
```

---

- References : Hands-On Network Programming with C, Lewis Van Winkle
