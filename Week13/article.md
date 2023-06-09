# 네트워크 보안 기초 (TLS)

- [Backgrounds](#backgrounds)

  - [Link encryption & End - to - End encryption](#link-encryption--end---to---end-encryption)

  - [키 동의(Key Agreement) 프로토콜 (or Key exchange)](#키-동의key-agreement-프로토콜-or-key-exchange)

- [SSL / TLS 프로토콜](#ssl--tls-프로토콜)

  - [SSL / TLS 프로토콜 구조](#ssl--tls-프로토콜-구조)

    - [Handshake Protocol](#handshake-protocol)

    - [ChangeCipherSpec Protocol](#changecipherspec-protocol)

    - [Alert Protocol](#alert-protocol)

    - [Record Protocol](#record-protocol)

  - [SSL / TLS 패킷 구조](#ssl--tls-패킷-구조)

---

## Backgrounds

<img width="558" alt="스크린샷 2023-06-10 오전 1 29 27" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2367ddb4-88d9-4541-9cc9-5df219dd160b">

- 네트워크 계층은 OSI 7 계층으로 구성된다.

<img width="579" alt="스크린샷 2023-06-10 오전 1 29 44" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d3750a39-b1a2-47ad-b1d3-b3bf66045784">

<img width="549" alt="스크린샷 2023-06-10 오전 1 30 47" src="https://github.com/jjaehwi/2023_Network/assets/87372606/f1220c1a-79fc-487c-a27b-6f0b5a35b6d2">

- 각 계층을 내려갈 때마다 헤더가 붙어서 패킷이 만들어진다.

- `TCP/IP` is a **client-server model**, i.e. when the client requests for service
  it is provided by the server. Whereas, `OSI` is a **conceptual model**.

- `TCP/IP` is a **standard protocol** used for every network including the Internet, whereas, `OSI` is not a protocol but **a reference model** used for understanding and designing the system architecture.

- `TCP/IP` is a **four layered model**, whereas, `OSI` has **seven layers**.

### Link encryption & End - to - End encryption

<img width="603" alt="스크린샷 2023-06-10 오전 2 11 05" src="https://github.com/jjaehwi/2023_Network/assets/87372606/db3c45ce-17e5-4279-bf27-122acf0728c2">

- `Link encryption`

  - 암호화가 링크 단위로 이뤄짐

  - 암호화가 모든 링크에서 독립적으로 수행됨

  - 모든 링크간의 사전에 공유된 키가 필요함

  - (ex) 모바일 기기들이 무선 AP 에 p/w 를 통해 접근하고 통신함 -> p/w 를 기반으로 암호화한 것 (무선 링크간 암호화)

- `End‐to‐End encryption`

  - 암호화가 시작점 (source) 과 도착점 (Destination) 에서 수행됨

  - 중간 노드들은 데이터를 볼 수 없다.

  - (ex) 카카오 비밀 채팅 (중간의 노드들은 데이터를 보지 못하고 end - to - end 로 통신)

- **_end - to - end encryption 을 하게 되면 IP 주소 같은 것 traffic flow 들은 보호되지 않는다._**

  - 컨텐츠는 암호화되어 보이지 않지만 왔다갔다하는 traffic 은 알 수 있다.

  - When using End‐to‐End encryption must leave headers (e.g., IP address) in clear

  - 헤더에 포함된 정보 (i.e., IP 주소)를 통해 패킷 라우팅을 하게 됨

  - **But, traffic flows are not protected**

    - 만약 A와 B가 End-to-End 암호화를 통해 통신을 하고 있는 상황이라면, 트래픽 분석을 통해 A와 B와 통신을 하고 있다는 것은 알게 됨 (통신 컨텐츠는 암호화를 통해 보호)

- Link encryption 과 End - to - End encryption 을 같이 사용하면 컨텐츠도 보호되고 traffic flow 도 어느정도 보호할 수 있다. (ex. VPN)

  - `End‐to‐End` **protects data contents** over the entire path

  - `Link encryption` **protects traffic flows** from monitoring

- 메시지 인증을 위해 네트워크 패킷에 메시지 인증 알고리즘도 적용되어야 한다. (ex. MAC)

---

### 키 동의(Key Agreement) 프로토콜 (or Key exchange)

<img width="544" alt="스크린샷 2023-06-10 오전 2 19 01" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c5d8ee7f-e581-4870-85dc-bb42ad23cf50">

- 키 동의 or 키 교환 프로토콜이 필요한 이유 : 인터넷이 발전하면서 공격자들이 많이 생겨났고 유저 사이 안전하게 통신하기 위해 보안이 꼭 필요하고, 보안의 핵심은 key 세팅을 어떻게 하느냐에 관한 것이다.

  - A 와 B 가 만나서 키를 교환하는 것은 너무 비효율적이기 때문에 신뢰되지 않는 인터넷 채널을 통해 키를 교환하는 방법들이 설계되었고 안전하다는 것이 증명되었다.

  - 그림에서 **A 와 B 는 서로 랜덤한 값 (nonce) 를 주고받으면서 값에 대해 guarantee 를 하고 키를 교환**한다.

  - 랜덤한 값을 기반으로 특정 키 유도 함수 (hash) 를 통과시켜 키를 생성 (각 A, B 에서 생성되는 키는 동일함)

<img width="622" alt="스크린샷 2023-06-10 오전 2 23 10" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d9fdb467-1af9-48af-a783-73e96ca968ce">

- `Diffie-Hellman 키 교환 프로토콜`

  - 수학적 배경 : 모듈러 연산을 통한 **DLP 문제**

  - The problem of finding 𝑥 such that `𝑎^𝑥 ≡ 𝑏 (𝑚𝑜𝑑 𝑝)` is called the discrete logarithm problem. (DLP)

  - 숫자가 엄청 커지면 공격자가 유추할 수 없다.

<img width="491" alt="스크린샷 2023-06-10 오전 2 28 58" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a2e6bb9d-1bc1-49a1-b551-4c6c3b208950">

<img width="569" alt="스크린샷 2023-06-10 오전 2 31 56" src="https://github.com/jjaehwi/2023_Network/assets/87372606/8ad2af8a-5659-44c1-aec1-48e476c8236d">

- `STS (Station - To - Station) 프로토콜`

  - 중간자 공격이 있을 수도 있으므로 키 교환을 할 때, PKI 서명 (인증서 교환) 을 검증 해야한다.

---

## SSL / TLS 프로토콜

- `SSL/TLS 프로토콜`은 **_클라이언트와 서버의 통신에 `메시지 인증`, `기밀성을 제공`하고 `가용성도 제공`하는 기술이다._**

<img width="523" alt="스크린샷 2023-06-10 오전 2 35 45" src="https://github.com/jjaehwi/2023_Network/assets/87372606/880cdc26-07dd-4397-9458-49d6c5b6856b">

- **암호화 되지 않은 패킷 (e.g., HTTP) 은 SSL 패킷으로 인코딩 되면서 암호화가 진행**된다. **(e.g., HTTPS)**

  - **Transport layer 를 보호**하기 위해 존재.

- 지금까지 만들었던 패킷은 보안적으로 안전하지 않은 패킷이었다. SSL 로 버전업이 되면서 발전했는데, 뭔가 취약점이 계속 있어서 아예 새로운 기술을 만들어서 완전 다름을 인식. (TLS 가 최근 버전인 것)

<img width="557" alt="스크린샷 2023-06-10 오전 2 38 46" src="https://github.com/jjaehwi/2023_Network/assets/87372606/7f8fa474-4240-4140-ab41-f3d113f0dca8">

<img width="509" alt="스크린샷 2023-06-10 오전 2 39 11" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d5f211ed-4d1b-4719-a8a3-382f1d861afa">

- 기존 http 에서는 데이터가 암호화되지 않아서 다 보였다.

- https 에서는 **서버 인증서와, 브라우저 인증서를 통해 키 교환을 해서 암호화**를 한다. (TLS 보안 통신)

---

### SSL / TLS 프로토콜 구조

<img width="574" alt="스크린샷 2023-06-10 오전 2 40 53" src="https://github.com/jjaehwi/2023_Network/assets/87372606/75d2286d-c853-4799-bab2-88df6e23391c">

- App layer 와 Transport layer 사이에 존재하며 TLS 에는 총 4 가지의 프로토콜이 세부적으로 구성되어있다.

  1. `Handshake Protocol`

     - 서로 뭘 쓸지 확인하고 맞추는 것

  2. `ChangeCipherSpec Protocol`

     - 쓰기로 했던 알고리즘과 알고리즘에 필요한 key 값이 제대로 세팅되었는지 confirm 하는 과정

  3. `Record Protocol`

     - 암호화 + 인증 이 적용된 통신

  4. `Alert Protocol`

     - 1, 2, 3 의 과정에서 에러가 발생했을 때의 에러를 정의하고 에러가 발생했음을 알리는 프로토콜

---

### Handshake Protocol

<img width="589" alt="스크린샷 2023-06-10 오전 3 05 23" src="https://github.com/jjaehwi/2023_Network/assets/87372606/b6c583a5-fa71-403d-8434-cadb8d99da48">

- 어떤 암호 알고리즘을 사용할 것인지, 사용하는 암호 알고리즘에 사용되는 인증 키를 어떻게 만들 것인지 negotiation 하는 단계 (The negotiation of the cipher suite and the generation of cryptographic secrets)

  1. 클라이언트가 ClientHello 를 서버에게 보내고 서버는 ServerHello 를 클라이언트에 보낸다.

  2. 서버의 TLS 인증서를 클라이언트에 보내고, 서버가 키를 교환하고 싶다고 키 교환 메시지를 보내고, 인증서가 있으면 달라고 요청을 보내면서 ServerHello 를 끝낸다.

  3. 클라이언트도 자신의 인증서를 보내고, 키 교환 메시지를 보내고, 인증에 대한 Verify 결과를 보낸다.

  4. ChangeCipherSpec 을 통해 사용할 수 있는 알고리즘이 뭔지 어떤 것을 선택할 것인지 confirm.

  - 키 교환에 쓰이는 프로토콜이 Diffie-Hellman 키 교환 프로토콜 이다.

---

<img width="589" alt="스크린샷 2023-06-10 오전 3 06 59" src="https://github.com/jjaehwi/2023_Network/assets/87372606/913a65fa-f0f9-4d35-8d73-975d85f556d1">

- Phase I 종료 후, 클라이언트와 서버는 아래 정보를 교환하게 됨

  - TLS 버전, 키 교환 알고리즘, 인증 알고리즘, 암호화 알고리즘, 압축 알고리즘, 랜덤한 값

  - **서로 지원가능한 알고리즘 리스트를 공유**하는 것

  - 서버가 클라이언트에 맞춰주는데, 서버의 최소보안요구사항을 만족하는 알고리즘이 클라이언트에 존재하면 선택 (보통 서버가 powerful)

---

<img width="589" alt="스크린샷 2023-06-10 오전 3 07 49" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d09fa69d-b968-4762-ba2c-31c7eabf9fec">

<img width="420" alt="스크린샷 2023-06-10 오전 3 10 11" src="https://github.com/jjaehwi/2023_Network/assets/87372606/3c97a231-0170-4646-b45a-2405a7ab70bc">

- 서버는 **인증서와 키 교환 메시지**를 전송 하며, 필요할 경우 **클라이언트로부터 인증서 요청 메시지를 전송함**

  - After Phase II, **클라이언트는 전송받은 인증서를 통해 서버를 인증**하게 되고, `클라이언트는 서버의 공개키를 알게 됨`

- 서버가 클라이언트에 데이터를 보낼 때, 서버가 s 라는 랜덤한 값을 만든 후 서버와 클라이언트 사이에 공유된 g 와 p 를 바탕으로 g^s mod p 를 해서 g^s 로 서버의 개인키에 서명을 한다.

  - 서버는 이 서명값과 서명을 인증할 수 있는 인증서를 클라이언트에 보낸다.

<img width="541" alt="스크린샷 2023-06-10 오전 3 12 05" src="https://github.com/jjaehwi/2023_Network/assets/87372606/b38003c7-caa7-4a74-964c-963517af94e4">

---

<img width="572" alt="스크린샷 2023-06-10 오전 3 13 14" src="https://github.com/jjaehwi/2023_Network/assets/87372606/65688a30-5d22-4681-8abe-6196ff7967f4">

<img width="487" alt="스크린샷 2023-06-10 오전 3 13 41" src="https://github.com/jjaehwi/2023_Network/assets/87372606/deb520d4-f092-4e0f-9847-979a8ba3095b">

- **_서버로부터 Certificate Request 메시지를 수신하였을 경우, 클라이언트의 인증서를 보내줌_**

  - After Phase III, **서버는 클라이언트를 인증할 수 있게 되고**, **서버와 클라이언트는 pre - master secret(PM)를 공유하게 됨**

- 클라이언트도 랜덤한 값 c 를 만들고, g^c mod p 에 서명을 한다.

  - 클라이언트는 이 키 교환 파라미터에 대한 서명값과, 인증서를 서버에 보낸다.

  - 그러면 **서로 공유하는 키가 생기게 된다. (g^cs)** = `pre - master secret (PM)`

<img width="556" alt="스크린샷 2023-06-10 오전 3 16 24" src="https://github.com/jjaehwi/2023_Network/assets/87372606/6625fd4b-984b-4286-9d05-c6d59a6dd019">

- phase II 와 phase III 에서 동일한 방법을 사용해야한다.

---

<img width="556" alt="스크린샷 2023-06-10 오전 3 17 41" src="https://github.com/jjaehwi/2023_Network/assets/87372606/9172711d-70e3-415f-a51a-b0fa5732241b">

- 클라이언트와 서버는 암호 알고리즘과 안 파라미터를 변경하는 메시지를 주고 받고, Handshake 프로토콜 종료 메시지 를 주고 받음으로써 Handshake 프로토콜을 종료한다.

  - After Phase IV, **the client and server are ready to exchange data.**

- finish 과정이 끝나면 앞으로 사용할 알고리즘과 알고리즘에 사용될 key 들이 다 세팅된 상태.

---

<img width="527" alt="스크린샷 2023-06-10 오전 3 20 04" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2c62fe8a-abaf-446b-9e06-b03f0910b1bb">

- DLP 키 교환 알고리즘을 통해 PM 을 얻고 키 생성 함수 (hash) 를 통해 원하는 충분한 길이의 output 값을 얻고 순서대로 키의 역할을 매긴다.

---

### ChangeCipherSpec Protocol

<img width="632" alt="스크린샷 2023-06-10 오전 3 22 29" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2cf9f276-8c60-4570-aef5-0dba8a846773">

- Handshake Protocol 이 끝나면 클라이언트와 서버는 암호화 통신을 할 준비가 끝난 것이다.

  - 어떤 암호화 알고리즘을 사용할지, 어떤 인증 알고리즘을 사용할지, 암호화 알고리즘에 사용되는 key, 인증에 사용되는 key, initial 값이 뭔지 세팅이 되어있는 상태

        - 암호 알고리즘

        - MAC 알고리즘

        - 암호화 키

        - MAC 키

        - 초기벡터 IV

  - 이 값들은 pending 상태에서 active 한 상태로 만들어준다.

- **ChangeCipherSpec 을 보낸다는 것 = writing 관련**

- **ChangeCipherSpec 을 받는 것 = Read 관련**

  - 클라이언트가 ChangeCipherSpec 를 보내면 pending 상태에 있던 Write 값이 active 상태로 옮겨진다.

  - ChangeCipherSpec 를 받은 서버에서는 Read 상태가 enable 되어 pending 에 있던 Read 값들이 active 상태로 옮겨진다.

  - 반대도 마찬가지..

---

### Alert Protocol

<img width="597" alt="스크린샷 2023-06-10 오전 3 28 51" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a006008d-e7d7-41b8-9382-453d08ca4287">

- **SSL / TLS 시스템이 비정상적으로 동작하였을 때 `에러처리`를 하기 위하여 에러나 경고 메시지를 `Alert 프로토콜`에서 정의**

  - 에러 값들이 정의되어 있음

---

### Record Protocol

<img width="544" alt="스크린샷 2023-06-10 오전 3 32 29" src="https://github.com/jjaehwi/2023_Network/assets/87372606/031a4098-2fc4-4786-bc5c-307db1d26e9d">

- 암호화 통신을 하면 됨. -> 암호화랑 인증 데이터를 보내는 것

- App 에서 데이터가 만들어지면 `쪼개지고` `압축`된다.

- 압축된 데이터에 **MAC 을 통해 인증값을 추가**한다.

  - pending 상태에서 active 상태가 된 **MAC 키를 같이 넣어줌**. (**_데이터와 키가 들어가서 인증값이 되는 것_**)

- **압축된 데이터와 인증값이 붙은 데이터를 전체적으로 암호화**한다.

- 암호화된 데이터가 되고 `SSL / TLS payload` 가 된다. (**_암호화와 인증이 적용된 패킷이 만들어진 것_**)

---

### SSL / TLS 패킷 구조

- `암호화 x`

<img width="420" alt="스크린샷 2023-06-10 오전 3 35 19" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c9ae385e-dc8f-4090-8bd1-93ce311e3cff">

- `암호화 o`

<img width="356" alt="스크린샷 2023-06-10 오전 3 34 16" src="https://github.com/jjaehwi/2023_Network/assets/87372606/25f31261-a005-45a3-9aad-8d7fa965221a">

- **TCP Data 자리에 Encrypted data (암호화된 데이터), MAC (인증값) 이 들어간다.**
