# DNS (Domain Name Service)

- [How hostname resolution works](#how-hostname-resolution-works)

  - [Overview of getaddrinfo()](#overview-of-getaddrinfo)

- [DNS Domain Hierarchy](#dns-domain-hierarchy)

  - [DNS ROOT Server](#dns-root-server)

  - [Top Level Domain (TLD)](#top-level-domain-tld)

- [DNS protocol](#dns-protocol)

  - [Emulating Local DNS Server](#emulating-local-dns-server)

- [DNS message format](#dns-message-format)

  - [DNS message format : Header](#dns-message-format--header)

- A DNS query program

---

## How hostname resolution works

- The DNS is used to assign names to computers and systems connected to the internet

  - `host name 과 IP address 를 link 시켜준다.`

- getaddrinfo() 함수를 통해 IP 주소를 가져왔었는데, 이것이 가능한 이유는 **OS 시스템상에서 getaddrinfo 를 하기 위해 DNS protocol 을 실행시키고 있는 것**이다.

---

### Overview of getaddrinfo()

1. www.example.com 에 접속하고 싶을 때, www.example.com 를 입력한다.

2. OS 가 시스템 내의 www.example.com 에 대한 IP 주소가 있는지 없는지 체크한다.

   - OS 는 local cache 에서 이 hostname 이 최근에 사용되었는지 확인한다.

3. local cache 에 hostname 에 대한 IP 주소가 없으면 OS 는 hostname 에 해당하는 IP 주소를 DNS 서버에 물어본다.

   - IP 주소를 알아야 라우팅을 해서 먼 곳까지 네트워크 패킷이 도달할 수 있는 것

   - DNS 서버는 ISP (Internet Service Provider) 가 제공하기도 하고, 사설 DNS 서버에 연결해서 서비스 받아도 된다.

---

## DNS Domain Hierarchy

<img width="610" alt="스크린샷 2023-05-05 오후 11 16 46" src="https://user-images.githubusercontent.com/87372606/236483609-45f35003-dc3f-4ac3-b9c2-2c3c97795c1c.png">

- `도메인 name 은 계층적으로 관리`가 된다. (Domain namespace is organized in a hierarchical tree-like structure)

  - (ex) www.example.com 에서 TLD 는 .com 이다.

<img width="610" alt="스크린샷 2023-05-05 오후 11 19 46" src="https://user-images.githubusercontent.com/87372606/236484315-6ddff0ae-fc56-4ec6-b13b-f4a3194fbd41.png">

- `DNS 는 zone 에 따라 구성이 된다.` (DNS is organized according to zones.)

- `zone` 은 **관련있는 도메인들끼리 묶은 것**이다.

  - zone 또한 계층적으로 구성되어있다.

- `각 DNS zone 에는 Authoritative Name Servers 가 존재`한다.

  - zone 에 대한 존재를 publish 한다.

  - DNS query 에 대한 original 하고 명확한 답을 한다.

  - Authoritative Name Servers 는 master server (primary) 가 될 수도 있고 slave server (secondary) 가 될 수도 있다.

    - master server : 모든 zone 에 대한 정보를 관리하고 있는 중앙 서버

    - slave server : 유사시 master record 를 복구할 수 있는 정보를 갖고 있는 서버

---

### DNS ROOT Server

<img width="610" alt="스크린샷 2023-05-05 오후 11 23 42" src="https://user-images.githubusercontent.com/87372606/236485297-b248ade8-2836-4c88-9eaf-3dfc6f078dd0.png">

- internet 을 함에 있어서 Domain name service 를 해주는 서버이므로 매우 중요한 infrastructure 이다.

  - They are the most critical infrastructure on the Internet.

---

### Top Level Domain (TLD)

- Infrastructure TLD : .arpa

- Generic TLD (gTLD) : .com, .net,

- Sponsored TLD (sTLD) : .edu, .gov, .mil, .travel, .jobs (특수 목적)

- Country Code TLD (ccTLD) : .kr (Korea), .au (Australia), .cn (China), .fr (France)

- Reserved TLD (예약된 TLD) : .example, .test, .localhost, .invalid

---

## DNS protocol

<img width="610" alt="스크린샷 2023-05-05 오후 11 37 01" src="https://user-images.githubusercontent.com/87372606/236488705-ae7ba134-dfb9-4c67-9252-d40f0728ce72.png">

- **`DNS` 는 `host name` 을 통해 `IP address` 를 가져오는 절차**이다.

- User Machine 에서 host name 을 통해 IP address 를 가져오려고 Local DNS server 에 물어보고 local 에 없으면 상위 DNS server 에 물어봐서 가져오는 절차이다.

<img width="610" alt="스크린샷 2023-05-05 오후 11 38 56" src="https://user-images.githubusercontent.com/87372606/236489176-62beadb2-ae7e-4b2a-8123-4fa8b22f1662.png">

- `클라이언트는 DNS server 에게 DNS query 를 보내야한다.`

- When a client wants to resolve a hostname into an IP address, it sends a DNS query to a DNS server.

  - **DNS 프로토콜은 UDP 를 통해 동작하고 포트번호는 53 을 사용**한다.

  - query 의 길이가 커지면 TCP 를 사용하기도 한다.

- 클라이언트는 적어도 하나의 DNS 서버의 IP 주소를 알아야 DNS 서비스를 받을 수 있다.

  - 클라이언트가 DNS 서버 주소를 모르면, chicken-and-egg problem 발생

  - ISP 를 통해 디폴트로 세팅되기도 하고, 사설 DNS 서버를 사용하기도 한다.

<img width="610" alt="스크린샷 2023-05-05 오후 11 42 15" src="https://user-images.githubusercontent.com/87372606/236490074-ad9c72cc-1437-4e6b-bd84-13dade540c53.png">

- 2 에서 root DNS 서버에 물어보고 .com DNS 서버로 가라는 정보를 받는다.

- 4 에서 .com DNS 서버는 specific 한 정보를 DNS 서버에 주고

- 6 에서 이 정보를 받아 example.com DNS 서버에 접근하여 IP 주소를 받아올 수 있다.

---

### Emulating Local DNS Server

<img width="610" alt="스크린샷 2023-05-05 오후 11 46 18" src="https://user-images.githubusercontent.com/87372606/236491087-ffe5b733-26f7-4c1d-bb02-1c7baeddb663.png">

- dig 명령어를 통해 root 서버가 보내주는 DNS response 에 대한 내용을 확인할 수 있다.

- `QUESTION SECTION` : 물어봤던 질문에 대한 정보 를 보여주는 section

- `ANSWER SECTION` : DNS name 서버가 query 에 대한 답을 알고 있으면 IP 주소를 알려주는 section

  - a.root-servers.net 이 답을 알고 있었으면 답안이 있었을 것이다.

- `AUTHORITY SECTION` : IP 주소를 모른다면 다음 애들에게 물어보라고 정보를 주는 section

  - 모르기 때문에 아래 애들에게 물어보라고 하는 것

- `ADDITIONAL SECTION` : query 관련된 추가적인 section

<img width="610" alt="스크린샷 2023-05-05 오후 11 48 27" src="https://user-images.githubusercontent.com/87372606/236491630-862324c9-130f-47de-8a34-57710b671c60.png">

- e.gtld-servers.net 에 가서 물어봤더니 또 ANSWER 가 없다.

<img width="610" alt="스크린샷 2023-05-05 오후 11 49 27" src="https://user-images.githubusercontent.com/87372606/236491882-47ca505e-b5a0-41df-8953-f7b07cd26999.png">

- ANSWER 를 받은 것을 확인할 수 있다. www.example.net 에 대해 93.184.216.34 라는 IP 주소를 알아낸 것

- dig 를 통해 파고파서 알아봤지만 `일반적으로 DNS 프로토콜은 query 만 던지면 DNS 서버가 자동적으로 순차적으로 응답해서 IP 주소를 받아서 클라이언트에게 알려주게 된다.`

---

## DNS message format

<img width="519" alt="스크린샷 2023-05-05 오후 11 53 49" src="https://user-images.githubusercontent.com/87372606/236493058-b58d9d04-fe81-4d7a-81ea-bb2928da876b.png">

- 모든 메세지에는 헤더가 존재한다.

- question, answer, authority, additional section 이 존재한다.

---

### DNS message format : Header

<img width="479" alt="스크린샷 2023-05-05 오후 11 58 53" src="https://user-images.githubusercontent.com/87372606/236494377-7869c5fd-ff8f-472f-88ef-ffe51dc7baa5.png">

- `DNS message 의 헤더는 12 bytes 길이`이다.

  - **DNS query 랑 DNS response 랑 정확히 일치**한다.

- `ID 필드`는 2 bytes (16 - bit) value 이고, DNS 메세지를 식별하기 위한 필드이다.

- `QR 필드`는 1 - bit 필드로 DNS query 인지, DNS response 인지 indicate 해주기 위한 필드이다.

  - 0 : DNS query,

  - 1 : DNS response

- `Opcode` 는 4 - bit 필드로, query 의 타입을 명세하기 위해 정의된다.

  - 0 : standard query

  - 1 : reverse query to resolve an IP address into a name

  - 2 : server status request

- `AA 비트`는 authoritative answer section

- `TC 비트`는 메세지가 truncated (축약) 됐다는 것을 의미

  - 이 경우 TCP 를 통해 re-sent 해야한다.

- `RD 비트`는 recursion 과 관련된 필드

  - 세팅이 되면 query 를 줬을 때, 응답으로 온 것이 아직 IP 주소가 세팅 안됐다는 것을 의미한다.

  - IP 주소를 받아오기 위해 additional server 에게 추가적으로 DNS query 를 보내야한다.

- `RA 비트`는 response 메세지에 존재하는 DNS 서버가 recursion 을 지원하는지 안하는지 식별하는 비트

- `Z 비트`는 unused bit

<img width="273" alt="스크린샷 2023-05-06 오전 12 06 40" src="https://user-images.githubusercontent.com/87372606/236496300-d1aea3f9-2dbb-4ae5-ac60-b9071bea4839.png">

- `RCODE` 는 DNS 의 error condition

- `QDCOUNT` : DNS query 를 날릴 때, 날리는 query 의 개수

  - 표준으로는 여러 개의 question 을 한 번에 보낼 수 있는데, 실제 구현할 때는 하나

- `ANCOUNT` : answer section 을 의미

  - 서버가 여러 개의 IP 주소를 가질 수 있기 때문에 answer section 은 복수 개가 될 수 있다.

---

- DNS Question Format, DNS Answer Format 과 A DNS query program 는 10 주차로..
