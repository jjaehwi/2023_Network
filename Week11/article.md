# HTTP and Web Client (2)

- HTTP protocol

- [A web client](#a-web-client)

  - [send_request 함수](#send_request-함수)

  - [connect_to_host 함수](#connect_to_host-함수)

  - [main 함수](#main-함수)

- [HTTP POST requests](#http-post-requests)

---

## A web client

- HTTP request 를 보내고 서버로부터 response 를 받아와서 내용을 출력하는 프로그램

- 클라이언트는 URL 을 input 으로 받는다.

- `parse_url` 함수

  - url 을 parsing 하는 함수

  - hostname, port number, document path 를 URL 로 부터 받아온다.

- `send_request` 함수

  - HTTP request 를 char 형 버퍼에 넣어서 보낸다.

- `connect_to_host` 함수

  - hostname 과 port number 를 통해 서버에 접속한다.

---

### send_request 함수

<img width="527" alt="스크린샷 2023-05-16 오전 12 18 57" src="https://github.com/jjaehwi/2023_Network/assets/87372606/aab70fce-b372-4ed2-829d-89b15ac5de57">

- 소켓 인자, hostname, port number, 접근하고자하는 path 를 인자로 받는다.

- 받은 input 을 char 형 buffer 에 넣어준다.

- 버퍼에 저장된 내용을 소켓 s 과 send 함수를 통해 보낸다.

- 어떤 데이터를 보냈는데 print 한다.

---

### connect_to_host 함수

<img width="459" alt="스크린샷 2023-05-16 오전 12 21 53" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a0a9f445-0bde-4479-b007-0ec18999cf41">

<img width="450" alt="스크린샷 2023-05-16 오전 12 22 02" src="https://github.com/jjaehwi/2023_Network/assets/87372606/06308608-e3ae-4eac-8f1a-2b1fc45c37a2">

- hostname 과 port number 를 갖고 getaddrinfo 를 통해 정보를 가져온다.

- 입력한 hostname 으로부터 주소를 가져왔으므로, 그 기반으로 소켓을 만들고 만들어지면 connect 한다.

---

### main 함수

- 패킷을 보내고 서버로부터 response 를 받아온다.

<img width="605" alt="스크린샷 2023-05-16 오전 12 23 56" src="https://github.com/jjaehwi/2023_Network/assets/87372606/4ed66ab4-81ee-4870-83b5-f084338c4461">

- argument 로 접근하려고 하는 hostname 을 받고 url 변수에 저장한다.

  - parse_url 함수에 argument 로 받은 url 을 통해 hostname, port, path 를 넣는다.

  - 얻은 hostname 과 port number 를 통해 connect_to_host 로 서버 소켓을 만든다.

  - hostname, port, path 를 넣어서 HTTP GET request 를 서버 소켓으로 보낸다. (send_request 함수)

    - 원하는 패킷이 만들어져서 HTTP GET request 가 hostname 주소로 전달된다.

- response 패킷을 parsing 하기 위해 사이즈를 선언한다.

  - 배열의 크기만큼 response 배열을 만들고

  - 처음 포인터를 가리키는 배열 포인터를 만들어서 response 의 주소를 저장한다.

  - 구현상 도움을 주는 \*q 도 선언

  - end 포인터를 선언해서 response 패킷의 끝 점을 가리키도록 한다. (response 패킷을 받아서 처리할 수 있는 공간의 최대치)

    - 처음 포인터 위치에서 response 배열의 사이즈를 더해서 만듬.

    - end 보다 큰 값이 오면 에러,,

  - HTTP response 에는 헤더 정보와 실제 요청한 정보를 담고있는 body 가 있는데, 이 중 body 메시지를 가리키기 위한 body 포인터

---

<img width="605" alt="스크린샷 2023-05-16 오전 12 38 45" src="https://github.com/jjaehwi/2023_Network/assets/87372606/f9ca4923-9882-4f2a-98b9-5ba6ae12d925">

- `while 문을 통해 response 패킷을 처리`

  - 서버로 부터 response 패킷이 5 초 이상 오지 않으면 에러 처리

  - response 를 다룰 수 있는 메모리가 할당되는데 (response 배열 시작점 포인터 p, 끝 포인터 end), p 가 end 가 만나면 더 이상 패킷을 처리할 수 없기 때문에 에러 처리

  - `select 문을 통해 패킷을 받을 준비`를 한다.

---

<img width="621" alt="스크린샷 2023-05-16 오전 12 40 47" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2209b7e0-c3ca-4eb2-b7b6-acf476f13dac">

- FD_ISSET 을 통해 서버가 관심있는 파일 디스크립터에 읽을 거리가 있으면 호출이 되고 recv 함수를 통해 서버 소켓의 데이터를 읽어온다.

  - 읽어온 값을 **p 라는 주소에 저장하고 end - p 만큼의 데이터를 읽어온다.**

  - `받은 바이트가 1 보다 작으면 서버로부터 close 요청`이 온 것이므로, enum 자료형을 통해 connection 으로 처리하고 body 가 있으면 body 를 출력한다.

    - 출력한 이후 서버로부터 connection 이 종료됐음을 출력한다.

    - `encoding 으로 처리하는 경우`

      - HTTP response 가 왔을 때 length 가 정해져 있는 경우

      - length 가 정해지지 않고 chunked 로 오는 경우

      - close 되는 상황 (received byte 가 < 1 인 경우)

    - `byte 를 읽은만큼 p 포인터를 증가`시킨다.

      - 증가된 p 포인터에 마지막에 0 이라는 값을 넣어줘서 string 관련 값을 쓸 수 있도록 세팅해준다. (null - terminated string)

---

<img width="621" alt="스크린샷 2023-05-16 오전 12 46 20" src="https://github.com/jjaehwi/2023_Network/assets/87372606/acc29c9f-68ff-435a-ae0d-e6345a3b28f4">

- Header 부분과 Body 영역 중 Body 부분을 처리하는 부분

  - `strstr 함수`를 이해해야한다.

  - response 를 받고 **Body 부분을 찾아야하기 때문에** Header 와 Body 의 구분이 되는 곳을 가리키도록 해야한다.

  - \r\n\r\n 으로 구분이 되기 때문에 이 부분이 있는지를 체크해서 포인터가 **\r\n\r\n 의 시작 부분을 가리키도록 하면 Header 와 Body 를 나눌 수 있다.**

  - 실제로 \r\n\r\n 은 의미가 없으므로 **그 시작점에서부터 포인터위치를 +4 해야 실제 Body 데이터**일 것이다.

  - **\r\n\r\n 앞에 0 을 넣어서 null termination 시켜서 헤더를 출력**할 수 있도록 한다.

```
Response = Header \r\n\r\n Body
```

<img width="614" alt="스크린샷 2023-05-16 오전 12 47 23" src="https://github.com/jjaehwi/2023_Network/assets/87372606/ad582eab-91cb-465b-bf76-7aab72dc45e3">

- `string 이 들어와서 그 문자열이 있으면 문자열이 시작하는 포인터를 반환하는 함수`

  - A Gar0den Diary (0 위치에 ptr 포인터가 위치할 것)

  - 실행 결과 : den Diary

---

<img width="607" alt="스크린샷 2023-05-16 오전 12 54 12" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2bce54d5-8e16-4996-b1ec-8fe5cca6c562">

- **enum 변수에 length, chunked, connection 이 있고, 세 타입에 따라 받은 response 패킷의 Body 의 길이를 정할 수 있다.**

```
Content-Length: 1270
```

- strstr 함수를 통해 `Content-Length` 가 있는지 확인한다.

  - 있다면 그 값을 읽어서 받은 response 패킷의 Body 부분의 총 길이를 알 수 있다.

    - 받은 만큼의 길이를 처리하기 위해 `strchr` 함수와 `strtol` 함수를 알아야한다.

    - q 라는 포인터에 strchr 함수를 통해 공백을 찾고 가리키도록 한다. (공백은 Content-Length:(여기)1270)

    - q += 1 을 하면 숫자를 가리키는 시작점으로 포인터가 업데이트된다.

    - 값을 10 진수로 읽어올 것이기 때문에 strtol 함수를 통해 remaining 에 Content-Length: 다음 오는 숫자를 저장한다.

  - 없다면 두 번째로 살펴볼 것은 `Transfer-Encoding: chunked` 가 있는지 확인한다.

    - chunked 라면 encoding 을 chunked 로 세팅한다.

    - 데이터를 받으면서 숫자를 체크하기 때문에 remaining Length 를 측정할 수 없어서 0 으로 세팅한다.

  - 그것도 없다면 `connection` 으로 세팅되게 된다.

<img width="559" alt="스크린샷 2023-05-16 오전 12 59 02" src="https://github.com/jjaehwi/2023_Network/assets/87372606/5a12d633-1fe7-4965-ba99-8c28c32b71af">

- **문자열 내에 일치하는 문자가 있는지 검사**한다. (있다면 문자가 있는 위치를 반환한다.)

<img width="575" alt="스크린샷 2023-05-16 오전 12 59 59" src="https://github.com/jjaehwi/2023_Network/assets/87372606/4412293b-63e8-46d0-b715-1dbc24d5fe55">

- **인자로 넘어온 string 값을 원하는 진수에 의거하여 숫자 형태로 반환**한다.

  - end 포인터는 읽었던 부분을 저장하기 위함.

---

<img width="619" alt="스크린샷 2023-05-16 오전 1 06 27" src="https://github.com/jjaehwi/2023_Network/assets/87372606/19846b50-38e4-408c-b7e7-813d41cb23f5">

- Body 의 시작 위치를 알았고 Body 를 출력해야한다.

  - encoding 이 length 라면 받아야할 Body 의 크기를 알 수 있다.

  - remaining 에 저장된 값은 body 의 길이일 것이고, p 는 우리가 데이터를 어디까지 읽었는지 알 수 있는 포인터, body 는 body 데이터의 시작점

  - p - body 가 remaining (body 길이) 보다 크면 body 를 다 읽은 것이기 때문에 그 부분을 출력한다.

---

<img width="619" alt="스크린샷 2023-05-16 오전 1 11 12" src="https://github.com/jjaehwi/2023_Network/assets/87372606/9567df28-81aa-49d4-8cfe-508ba2027976">

- chunked 라면 length 후 데이터 가 반복되고 끝나면 0 이 온다.

  - remaining 이 0 이라면 chunked 를 아직 확인하지 못한 것이므로 new chunk length 를 읽어야한다.

  - 현재 body 는 44 앞 부분을 가리키고 있을 것이다. q 를 body 로 부터 \r\n 부분을 찾아서 그 부분을 가리키도록 한다. (ex. (body)44(q)\r\n)

  - remaining 은 44 (new chunk length) 를 16 진수로 읽어온다.

  - remaining 이 0 이면 termination 이기 때문에 finish 로 가야하고, 0 이 아니라면 body 를 업데이트한다.

    - q + 2 를 하면 \r\n 의 끝 부분을 가리키게 되기 때문에 데이터를 읽을 수 있게 세팅된다. (44(q)\r\n(body))

  - p - body 가 remaining (body 길이) 보다 크면 body 를 다 읽은 것이기 때문에 그 부분을 출력하고,

  - body 는 데이터를 다 읽고 다음 chunk length 를 가리키도록 해야하기 때문에 remaining 만큼 증가시키고, \r\n 을 지나기 위해 + 2 를 해줘서 다음 new chunk length 를 가리키도록 한다. ((body)37)

  - while 이므로 이 과정을 반복한다.

<img width="582" alt="스크린샷 2023-05-16 오전 1 19 19" src="https://github.com/jjaehwi/2023_Network/assets/87372606/93355328-57d2-459a-b8cc-1bb61358352b">

---

## HTTP POST requests

- `HTTP POST request 는 웹 클라이언트에서 생성한 데이터를 웹 서버로 전달하는 역할`을 한다. (An HTTP POST request sends data from the web client to the web server)

- **POST body format 은 Content-Type 헤더에 의해 다양하게 정의**된다.

<img width="606" alt="스크린샷 2023-05-16 오전 1 26 46" src="https://github.com/jjaehwi/2023_Network/assets/87372606/0568b47a-6edf-4b33-bdc1-cc050a6a4ec8">

- 웹 사이트에서 특정 폼 형태가 있을 때 POST 를 사용한다. (such as a login form)

  - (ex) ID 와 P/W 를 POST 형태로 보내는 것..

<img width="606" alt="스크린샷 2023-05-16 오전 1 27 48" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d11c1985-ccc1-4c78-90ba-4bbf1069c063">

- 보안 issue 발생

---

- 추가 참고 문서

  - RFC 7230: Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing (https://tools.ietf.org/html/rfc7230)

  - RFC 7231: Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content (https://tools.ietf.org/html/rfc7231)

  - RFC 1866: Hypertext Markup Language – 2.0 (https://tools.ietf.org/html/rfc1866)

  - RFC 3986: Uniform Resource Identifier (URI): Generic Syntax (https://tools.ietf.org/ html/rfc3986)

- References : Hands-On Network Programming with C, Lewis Van Winkle
