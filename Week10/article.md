# DNS (Domain Name Service) - 2

- [DNS message format](#dns-message-format)

  - [DNS message format : Question](#dns-message-format--question)

  - [DNS message format : Answer](#dns-message-format--answer)

- [A DNS query program](#a-dns-query-program)

  - [A simple DNS query](#a-simple-dns-query)

  - [A DNS query program : Print a DNS message name](#a-dns-query-program--print-a-dns-message-name)

  - [A DNS query program : print_name 함수](#a-dns-query-program--print_name-함수)

  - [A DNS query program : print_dns_message 함수](#a-dns-query-program--print_dns_message-함수)

  - [A DNS query program : main 함수](#a-dns-query-program--main-함수)

---

## DNS message format

<img width="519" alt="스크린샷 2023-05-05 오후 11 53 49" src="https://user-images.githubusercontent.com/87372606/236493058-b58d9d04-fe81-4d7a-81ea-bb2928da876b.png">

---

### DNS message format : Question

<img width="613" alt="스크린샷 2023-05-09 오전 1 04 14" src="https://user-images.githubusercontent.com/87372606/236873298-2e7231f2-9cff-418c-bbde-2b1b75ee15fb.png">

- NAME, QTYPE, QCLASS field 가 존재한다.

- host name 으로 ip address 를 알아내는 것이기 때문에 `NAME` 필드에 host name 이 들어간다.

- `QTYPE` 은 IPv4, IPv6 와 같은 형태를 명시하는 타입 (indicates the record type we are asking for)

  - A 는 IPv4 에 대한 address record

- `QCLASS` 는 일반적으로 1 로 세팅하고 1 은 internet 을 의미한다.

<img width="613" alt="스크린샷 2023-05-09 오전 1 07 57" src="https://user-images.githubusercontent.com/87372606/236874107-b272040e-dead-49a7-ac22-c83b0dc40986.png">

- DNS 표준에는 NAME 필드를 세팅하는 특별한 encoding rule 이 존재한다.

  - length 와 value 로 이뤄짐

  - (ex) www.example.com 를 www, example, com 으로 나누고 length 를 세팅하고 value 를 넣고 마지막엔 0 byte 로 세팅한다.

  - 일반적으로 data 를 다룰 땐 TLV (Type, Length, Value) 세팅을 많이 한다.

    - type 을 통해 어떤 메시지인지 알고 length 와 value 를 통해 메시지를 파악

  - DNS message format 에서는 type 없이 length, value 로 이뤄진 encoding rule 을 따라간다.

---

### DNS message format : Answer

- **client 가 DNS server 에 물어보는 것이 question 이고, 이에 대한 답이 answer 이다.**

<img width="336" alt="스크린샷 2023-05-09 오전 1 15 15" src="https://user-images.githubusercontent.com/87372606/236875606-78033d77-b660-4cf1-bb19-fb77f6ff2ecf.png">

- NAME, TYPE, CLASS 는 question 의 필드와 똑같다.

- `TTL` : 32 - bit 로 이뤄진 ttl 정보

  - 얼마나 많은 시간동안 캐시에 머무르는지..

  - 그 시간만큼 DNS 정보를 임시 저장하고 있다.

- `RDLENGTH` : DATA 에 대한 길이

- `RDATA` : 특성이 TYPE 에 따라 달라진다. (interpretation is dependent upon the type specified by TYPE)

---

## A DNS query program

- DNS query 를 만들고, DNS response 를 받아서, print 해본다.

  - DNS query : 원하는 host name 에 대한 IP 주소를 얻기위한 query

  - DNS response : 세팅한 host name 에 대한 IP 주소를 DNS 서버에서 보내주는 패킷

- DNS response 를 print 하기 위해 두 함수를 정의한다.

  - `print_dns_message()` 함수 : DNS message 를 printing

    - 전체 DNS message format 을 printing

  - `print_name()` : DNS message 의 name 을 printing

    - name section 은 특별한 encoding rule (Length Value 기반의 인코딩 tagging) 이 있기 때문에 이를 위한 print_name

<img width="548" alt="스크린샷 2023-05-09 오전 2 12 42" src="https://user-images.githubusercontent.com/87372606/236887399-451a8d18-64b2-472e-b5c3-c48150fcdaf4.png">

- DNS query 는 Header 와 Question 만 가지고 있고

- DNS response 는 query 의 Question 을 복사해서 보내주고, Answer 와 경우에 따라 Authority, Additional 이 존재한다.

---

### A simple DNS query

- DNS query 를 만들기 위해 byte 배열로 만들어본다.

<img width="488" alt="스크린샷 2023-05-09 오전 2 01 00" src="https://user-images.githubusercontent.com/87372606/236885204-e8f9a331-8643-41ce-a981-2dab5f86f80d.png">

- example.com 에 대해 물어보기 위해 DNS question 에 써준다.

- UDP 패킷이 DNS 서버의 port 53 으로 전달이 되고, DNS query 가 성공적으로 전송되면 DNS 서버는 UDP 패킷을 response 로 다시 클라이언트에게 보낸다.

  - ID 가 일치할 것이고, QR 이 response 로 세팅된다. (query 를 보낼 땐 0, response 일 땐 1)

  - QDCOUNT 도 1 로 세팅된다.

  - 보냈던 question 도 response 가 올 때 같이 온다.

  - ANCOUNT 는 IP 주소를 가져오는 형태에 따라 여러 section 으로 나뉠 수 있다.

---

### A DNS query program : Print a DNS message name

<img width="548" alt="스크린샷 2023-05-09 오후 9 22 04" src="https://github.com/jjaehwi/2023_Network/assets/87372606/67354a3c-1abe-4d69-96f0-9bcfc7ebce72">

- Question format 에서 NAME 부분을 출력하는 프로그램

- DNS encoding rule 은 length 와 value 로 되어있다.

  - **`네트워킹 딜레이 (network bandwidth) 를 줄이기 위해 메시지를 최대한 줄여서 보내야한다`.**

    - **중복되는 구간은 패킷 내에 원래 부분을 가리키도록 포인터를 지정**하도록 한다.

    - **length 의 `최상위 비트가 0xc0 (1100 0000)` 이라면 value 를 의미하는 것이아니라 어떤 위치를 지정한다는 의미**이다. ((If a length has its two highest bits set (that is, 0xc0 (1100 0000)), then it and the next byte should be interpreted as a pointer instead.))

  - Question 의 NAME 이랑 Answer 의 Name 이랑 같다. NAME 필드에 host name 이 중복되어 사용된 것이므로 Answer section 에서는 Question section 의 NAME 을 포인팅해서 사용하면 된다.

  - 위 그림에서 Answer section 의 Domain Name 은 패킷에 전송할 때 중복되는 값이 전송되는 것이 아닌 앞에 패킷의 내용을 가리키는 포인터가 저장된 것이다. (최상위 비트가 11 로 세팅되고 그 뒤에 오는 사용되지 않은 6 비트와 뒤에 오는 8 비트가 합쳐서 14 비트가 포인팅할 위치를 지정한 것)

---

### A DNS query program : print_name 함수

<img width="633" alt="스크린샷 2023-05-09 오후 9 33 25" src="https://github.com/jjaehwi/2023_Network/assets/87372606/df983057-4832-44f5-85b1-287a8fdb5b1a">

- `print_name 함수` 의 파라미터

  - **DNS 메시지** msg

    - 메시지 위치는 실제 헤더가 시작되는 부분

  - **출력할 부분을 의미하는 포인터** p

    - Question 에서 NAME 부분, Answer 에서 NAME 부분 을 포인팅

  - **메시지의 끝** end

    - DNS 메시지가 끝나는 부분

- if 문을 통해 메시지의 끝 부분인지 체크를 한다. (출력할 부분은 NAME 부분이고 length 와 value 는 최소 1 바이트 무조건 있을텐데 읽고자하는 위치에서 2 바이트 더한 것이 end 보다 크면 패킷 포맷 에러임)

- print 하고자 하는 NAME 이 포인팅하고 있는 포인터가 있는지, 실제 length - value tag 인지 체크하는 것

  - 0xC0 (1100 0000) 이랑 & 연산을 통해 최상위 비트가 11 로 세팅되어있는지 확인한다.

    - 11 로 세팅되어있다면 p 포인트에 있는 최상위비트를 제외한 6 비트 값과 p 포인트 다음 값을 더해서 14 비트 포인터가 되고 이것을 k 에 저장

    - 포인터는 현재 위치와 p[1] 의 위치를 읽었기 때문에 + 2

    - `포인터의 의미는 메시지 시작위치에서 얼마나 떨어져 있는지 의미`한다. (**메시지의 시작으로부터 얼마만큼 떨어진 부분을 읽어야하는지를 나타내는 것 = `헤더부터 Query Name 까지의 거리`**)

  - 포인팅 하고 있지 않다면 실제 length - value 이므로 읽고자하는 위치의 length 를 저장하고 그 다음값을 읽어야하므로 포인터는 1 씩 증가한다. (const int len = \*p++)

    - 1 증가한 부분에서 저장된 len 만큼 출력한다.

    - 포인터를 len 만큼 증가시킨다.

    - p 가 0 (termination) 이 아니라면 . 을 찍고 recursive 하게 동작

    - 0 이라면 포인터를 1 증가 (0 을 읽었으므로) 시켜서 return

---

### A DNS query program : print_dns_message 함수

<img width="638" alt="스크린샷 2023-05-09 오후 11 45 43" src="https://github.com/jjaehwi/2023_Network/assets/87372606/8f6c2b97-aef1-4701-8a37-211a47639f4d">

- `print_dns_message 함수` 의 파라미터

  - DNS 메시지

    - 패킷을 받았을 때 패킷을 저장한 배열의 주소

  - 메시지의 길이

    - 길이 12 바이트보다 작으면 에러 (헤더가 무조건 12 바이트 이기 때문)

- input 을 char 형 포인터로 넘긴다.

- ID 가 2 바이트 이므로 msg[0] 가 8 비트 ,msg[1] 이 8 비트 총 16 비트를 출력한다.

  - `ID 의 절반씩 msg[0], msg[1] 인 것`

- QR 을 해석한다. `QR ~ RA 까지 msg[2]` 이다.

  - QR 부분만 필요하고 나머지는 지워야하기 때문에 **1000 0000 (0x80) 과 & 연산을 통해 처음에 나오는 QR 부분만 살린다.**

  - 이 값을 숫자로 읽어야하기 때문에 **8 비트의 최상위 비트에 두면 안되고 >> 7 을 통해 최하위 비트로 옮긴 후 저장**한다.

  - QR 값에 따라 출력한다.

- Opcode 를 해석한다.

  - 1 ~ 4 비트 사이의 값이기 때문에 **0111 1000 (0x78) 과 & 연산을 통해 Opcode 부분만 살린다.**

  - 이 값을 읽기 위해 **>> 3 을 통해 최하위 비트로 옮기고 저장**한다.

  - Opcode 값에 따라 출력한다.

---

<img width="638" alt="스크린샷 2023-05-09 오후 11 53 44" src="https://github.com/jjaehwi/2023_Network/assets/87372606/cbfa6cef-3a3c-4f7d-bd2f-b925c2d05c7f">

- AA, TC, RD 도 똑같이 해석한다.

- **RCODE 는 response 일 때 즉, QR 값에 1 이 세팅되어있을 때만 존재**하므로 if(qr) 을 통해 조건 판별 후 해석한다.

---

<img width="638" alt="스크린샷 2023-05-09 오후 11 56 20" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2a7de970-e087-4dbe-8056-84c4f0a4842d">

- QDCOUNT, ANCOUNT, NSCOUNT, ARCOUNT 는 2 바이트이고 char 형인데, 이 값은 int 형 4 바이트 자료형에 저장한다.

  - msg << 8 + msg 를 통해 **16 비트를 만들고 4 바이트 자료형에 저장**하는 것.

- `원래 메시지의 시작점에서 12 바이트 만큼 내려온 것이므로 포인터를 메시지 시작점에서 + 12` 해준다.

- `end 는 메시지 시작점 + 함수의 매개변수로 입력받은 메시지의 길이`

---

<img width="638" alt="스크린샷 2023-05-10 오전 12 00 27" src="https://github.com/jjaehwi/2023_Network/assets/87372606/b9f9feb5-288f-4768-a6ae-0c7aaa7f73d6">

- Question section 이 있다면 (qdcount 가 1 이상) query 를 출력하도록 한다.

  - NAME 을 출력하기 위해 앞서 본 print_name 함수를 사용한다. (포인터까지 같이 업데이트된다.)

  - 이후 QTYPE 과 QCLASS 를 출력하면서 포인터를 업데이트한다.

---

<img width="638" alt="스크린샷 2023-05-10 오전 12 02 10" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c2898824-679e-4968-b47f-e02bbf550854">

- 출력하고자하는 DNS 메시지가 Answer section 까지 있다면 (ANCOUNT, NSCOUNT, ARCOUNT 가 존재하는 경우) 출력하도록 한다.

  - NAME 을 출력하기 위해 앞서 본 print_name 함수를 사용한다.

    - 실제 host name value 가 있는 것이아니라 포인터가 있을 것이다. (포인터까지 같이 업데이트된다.)

  - 이후 TYPE, QCLASS, TTL, RDLENGTH 를 출력하면서 읽은만큼 포인터를 업데이트한다.

---

<img width="638" alt="스크린샷 2023-05-10 오전 12 05 39" src="https://github.com/jjaehwi/2023_Network/assets/87372606/5db65ccb-c7bd-4f82-a7a4-0bf462a1ea5b">

- RDATA 에 담긴 host name 에 대한 IP 정보를 출력해야한다. (각 타입별로 조건 나눠놓은 것)

---

### A DNS query program : main 함수

<img width="638" alt="스크린샷 2023-05-10 오전 12 08 48" src="https://github.com/jjaehwi/2023_Network/assets/87372606/cc3b3a44-cf8d-4bb4-bbe4-cda1a02903ed">

- 3 개의 argument 가 들어온다.

  - 함수명

  - ip 주소를 알고싶은 host name

  - 원하는 type

- 에러 체크 후 **type 에 따라 어떠한 type 으로 출력할 것인지 설정**한다. (`표준에 따름`)

---

<img width="638" alt="스크린샷 2023-05-10 오전 12 11 19" src="https://github.com/jjaehwi/2023_Network/assets/87372606/8196711d-9dc8-46a9-ac04-520b6a88dfc3">

- 입력한 host name 에 대한 IP 주소를 물어봐야한다. (DNS 서버에게)

- 구글에서 제공하는 DNS 서버를 이용하기 때문에 UDP 로 IP 8.8.8.8, 포트번호 53 으로 설정

- DNS 서버와 소켓을 만들고 이전에 만들어봤던 simple DNS query 를 보낸다.

  - ARCOUNT 까지 하드하게 세팅 후 나머지 부분을 채우기 위해 \*p 에 query 부터 헤더 길이를 더한만큼을 업데이트한다.

    - query 란 배열의 12 바이트 부터는 p 가 가리키게 된다. 12 바이트 까지는 설정한 헤더가 들어간 것

  - h 포인터는 argument 를 받은 hostname 을 가리키게한다.

---

<img width="638" alt="스크린샷 2023-05-10 오전 12 15 45" src="https://github.com/jjaehwi/2023_Network/assets/87372606/3bd0820d-6864-4754-8f6d-54f27ae3e0f3">

- 포인터에 받았던 argument 를 넣어준다.

  - . 을 제외한 hostname 을 저장한다.

- p 포인터를 termination 하고, 받았던 type 에 대해 type 값을 지정하고, QCLASS 는 하드 코딩

- `p 가 이동한 만큼 (상위 주소) 에서 query 의 시작 주소를 빼서 query 배열의 사이즈를 구할 수 있다.`

- sendto 함수를 통해 query 를 배열의 query size 만큼 DNS 서버에게 보낸다.

  - 얼만큼 보냈는지 출력하고, 무엇을 보냈는지 print_dns_message 함수로 출력한다.

  - **query 가 잘 만들어졌고 DNS 서버로 잘 보내졌다면 response 가 올 것**이다.

- response 를 recvfrom 함수를 통해 받는다.

  - 얼만큼 받았는지 메시지 길이를 출력한다.

  - pinrt_dns_message 를 통해 read 배열의 시작점부터 메시지 길이만큼 출력한다. (시작점부터 끝까지 메시지 출력하는 것)

    - DNS 메시지가 호출되면서 **메시지 배열 read 에서 끝까지 포인터 p 를 움직이면서 출력하게 될 것**

---

### 실행 결과

<img width="557" alt="스크린샷 2023-05-10 오전 12 26 39" src="https://github.com/jjaehwi/2023_Network/assets/87372606/1be2953f-eccb-42bb-8f3a-415cdee30605">

- dns_query 를 보내고, example.com 의 a 타입 (IPv4) 으로 query 를 만들어서 보낸다.

- DNS 서버 접속 후, class: 1 까지 query 가 된다. (29 바이트 만큼 보낸 것)

- DNS 서버로부터 45 바이트를 받고 받은 형태가 나온다.

  - ID ~ ARCOUNT 까지가 헤더

  - QUERY 1 ~ class 까지 Query section

  - Answer 부터 끝까지 Answer section

    - `name 을 출력할 때 포인터를 통해 출력했다는 것을 확인할 수 있다.`

---

- References : Hands-On Network Programming with C, Lewis Van Winkle
