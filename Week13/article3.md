# TLS 서버 구축

- [Certificate](#certificate)

  - [Certificate : 인증서 및 개인키 생성](#certificate--인증서-및-개인키-생성)

- [TLS 서버](#tls-서버)

- [github : TLS 서버 코드](https://github.com/IamLupo/openssl-examples/blob/master/tlsv1.2/https/src/server.c)

---

## Certificate

- 인증서와 전자서명을 통해 접속하고자하는 서버를 인증한다.

<img width="772" alt="스크린샷 2023-06-10 오후 7 10 03" src="https://github.com/jjaehwi/2023_Network/assets/87372606/2ff827d2-56a6-49f5-a2bf-bdc13b46b4d8">

- 그림에서 유저가 www.abcd.com 에 TLS connection 을 맺는다.

  - 브라우저는 신뢰되는 도메인 서버인지 체크해야한다.

  - 서버가 인증서와 함께 서명값을 보내면 유저는 검증을 통해 올바른 서버인지 확인할 수 있다.

- 인증서를 받은 브라우저는 서버가 보낸 인증서가 적합한 인증서인지 인증서에 포함된 서명 검증을 해야한다.

  - CA1 에 대한 검증을 해야해서 CA1 의 공개키가 필요하다. (서버의 인증서가 CA1 에서 발행됐으므로)

  - 브라우저는 CA1 의 인증서도 필요하다. (CA1 의 인증서에 공개키가 있으므로)

  - CA1 의 인증서를 보면 CA1 인증서가 적합한지 알려주는 전자서명을 RootCA 에서 했다. (발행기관이 RootCA 이므로)

  - 따라서 브라우저는 다시 RootCA 의 인증서를 확인해야한다. (RootCA 는 최상위 인증기관)

    - self sign

  - 모든 브라우저는 대표적인 RootCA 의 인증서를 저장하고 있기 때문에 RootCA 를 검증할 수 있다.

  - 노란키로 RootCA 를 검증하면 CA1 의 인증서에 대한 내용이 유효함을 알 수 있고 파란색 공개키가 CA1 의 공개키가 맞으므로 이 키로 Server 의 인증서를 검증할 수 있고 이를 통해 서버의 공개키가 초록키임을 확인할 수 있다.

<img width="683" alt="스크린샷 2023-06-10 오후 7 17 19" src="https://github.com/jjaehwi/2023_Network/assets/87372606/7c05414b-8c82-47be-8cc9-106708d66f3b">

- RootCA 끼리는 서로 Peer - to - Peer 형태로 검증을 한다.

---

### Certificate : 인증서 및 개인키 생성

- `TLS 서버를 구축하려면 서버에 인증서와 개인키를 세팅`해야한다.

  - **_클라이언트로부터 서버를 인증시키기 위해 인증서가 필요하고 전자서명을 해야하기 때문에 개인키가 필요한 것._**

<img width="860" alt="스크린샷 2023-06-10 오후 7 26 59" src="https://github.com/jjaehwi/2023_Network/assets/87372606/0550161f-f79b-4ecb-87d0-49754c354dd9">

- key 의 directory 를 정한다.

- `private_key` 의 저장 경로를 세팅

  - private_key 를 만드는 즉시 비밀번호를 만들어준다.

- `certificate` 의 저장 경로 세팅

  - certificate 에 들어갈 정보가 명시됨

- 기존 private_key 나 certificate 가 있으면 remove

- openssl 라이브러리를 활용하여 **x509 인증서 표준에 근거하는 인증서를 만든다.**

  - sha1 알고리즘을 사용,

  - newkey 를 만드는데 2048 bit 의 rsa 공개키 알고리즘을 사용하고 -keyout 으로 설정해둔 private_key 경로에 key 를 만들거고, -out 으로 설정해둔 certificate 경로에 인증서를 만들 것임

  - outform 으로 인증서와 키의 형태는 PEM 임을 알림

  - 유효기간은 1825 (5년)

  - 인증서에 들어가는 정보 (subject) 는 $certificate_info 임

  - 키에 대한 비밀번호는 $private_password 임

<img width="748" alt="스크린샷 2023-06-10 오후 7 30 05" src="https://github.com/jjaehwi/2023_Network/assets/87372606/1526d727-d8e8-4509-92e2-9a1b08afc466">

- 이렇게 실행하면 인증서와 키를 만들 수 있고 매번 실행할 때마다 private key 와 public key 는 매번 달라질 것이다.

  - openssl 에서 키를 만들 때 랜덤하게 만들 것이기 때문에

- 결과 subject name 과 issuer name 이 같다. (테스트이기 때문에 self sign 한 것)

- 전자서명 값이 포함. (Signature)

- 신뢰되는 기관으로부터 받은 인증서를 써야한다.

<img width="599" alt="스크린샷 2023-06-10 오후 7 31 19" src="https://github.com/jjaehwi/2023_Network/assets/87372606/9d06b8b3-ac08-4e60-9461-d70edad2e2a6">

<img width="728" alt="스크린샷 2023-06-10 오후 7 31 29" src="https://github.com/jjaehwi/2023_Network/assets/87372606/9fcd9eca-68bf-452e-956c-9e1546ab4bd8">

---

## TLS 서버

<img width="804" alt="스크린샷 2023-06-10 오후 7 34 56" src="https://github.com/jjaehwi/2023_Network/assets/87372606/8d0d35dc-e43b-4690-9e1f-d228c0e66a69">

- TLS 서버를 구축해야하기 때문에 SSL_CTX context 선언

- 5000 번 portnum

- TLS 서버를 위해 필요한 인증서와 개인키 세팅

- SSL_library_init 함수 으로 OpenSSL library 초기화

- ctx 를 할당하기 위해 `InitServerCTX` 함수 사용

- `LoadCertificates` 로 인증서를 load

- `OpenListener` 로 5000 portnum 으로 listen (서버 소켓 생성)

- while 문

  - ssl connection 에 사용될 포인터를 만들어서 서버 요청을 받은 후 ssl 을 SSL_new 로 만들어서 서버 서비스를 한다. -> `Servlet` 함수

---

<img width="804" alt="스크린샷 2023-06-10 오후 7 39 56" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a20401ef-9236-44fa-ab91-a19ec8e3280a">

- `InitServerCTX 함수`

- CTX 를 만들기 위해 포인터를 선언하고 모든 라이브러리의 알고리즘을 load 하고 error 관련 string 도 load

- TLS_server_method 사용 : 클라이언트가 아니고 서버이기 때문

  - 서버가 구동되는 action : 클라이언트와 handshake 를 하기 위해 준비

- SSL_CTX_new 에 위에서 만든 method 를 넣어서 ctx 를 만든다.

- 사용할 cipher 를 세팅할 수 있다.

  - 서버는 모든 알고리즘을 list 할 수 있기 때문에 모든 알고리즘을 준비하게 할 수 있다.

---

<img width="804" alt="스크린샷 2023-06-10 오후 7 43 37" src="https://github.com/jjaehwi/2023_Network/assets/87372606/690dd5bd-a983-4be5-8ec6-4830dc432caa">

- `LoadCertificates` 함수

- 넘겨준 인증서와 keyfile 의 경로를 이용해서 인증서와 private key 를 load 한다.

- SSL_CTX_load_verify_locations 는 **self sign** 이기 때문에 자신의 경로를 넣는다.

  - 원래는 라이브러리 별로 인증서를 관리하는 기관이 있다. (Local Trust Store) 그 경로를 세팅한다.

- SSL_CTX_use_certificate_file 을 통해 인증서를 load

- SSL_CTX_set_default_passwd_cb_userdata 를 통해 비밀번호가 필요한 부분 세팅

- SSL_CTX_use_PrivateKey_file 을 통해 private key 를 load

- !SSL_CTX_check_private_key 를 통해 인증서와 private key 가 검증이 통과되면 쓸 준비가 된 것.

---

<img width="804" alt="스크린샷 2023-06-10 오후 7 48 44" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d7ea5846-b3f4-4f84-bb96-295860bafcdd">

- `OpenListener` 함수

- portnumber 로 할당해서 서버에 있는 모든 address 중 필요한 address 를 가져와서 bind 하고 listen 하는 것

- TLS 서버도 클라이언트와 마찬가지로 TCP connection 을 받아 connection 이 형성된 후 SSL connection 을 맺어야한다.

---

<img width="804" alt="스크린샷 2023-06-10 오후 7 50 25" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a1f63b8b-8a9a-4043-9628-7d8e5e5e60ca">

- `Servlet` 함수 : 서버와 통신하기 위한 기능

- 클라이언트가 http get request 를 보낸다라고 가정

  - http get request 가 클라이언트로부터 오면 세팅한 메시지를 보내주겠다는 것

  - 브라우저는 Hello Wolrd! 를 출력하게 된다.

- 위 메시지는 **평문이기 때문에 암호화해서 보내야하고 클라이언트는 서버에게 SSL / TLS connection 을 할 것**이다. 그러면 `서버는 SSL_accept` 을 한다.

  - accept 되면 ShowCerts 를 통해 인증서를 보여준다.

  - **SSL_read 를 통해 클라이언트가 암호화해서 보낸 get request 를 복호화해서 버퍼에 저장**한다.

  - 이후 Client msg : 로 출력

  - 그 메시지에 대한 응답으로 **SSL_write 를 통해 세팅해둔 메시지를 암호화해서 클라이언트에게 보낸다.**

---

<img width="804" alt="스크린샷 2023-06-10 오후 7 56 37" src="https://github.com/jjaehwi/2023_Network/assets/87372606/fc861d67-509d-4573-b64d-bbb1e8bee3bc">

- `ShowCerts` 함수 : 접속하고자했던 클라이언트가 갖고있는 인증서를 확인하는 기능

---

<img width="1096" alt="스크린샷 2023-06-10 오후 7 58 32" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c36993ae-4428-412f-a5e0-56271bddea0c">

<img width="1096" alt="스크린샷 2023-06-10 오후 8 00 06" src="https://github.com/jjaehwi/2023_Network/assets/87372606/dd63179d-bf28-4279-9184-0c360235201b">

- Loopback IP 에 설정된 포트 번호로 접근하면 Warning 이 뜨는데, self sign 인증서라서 신뢰되지 않은 사이트인 것이다. (브라우저가 인증할 수 없는 것)

- 경고를 무시하고 accept 하면 Hello World! 가 출력되는 것을 확인할 수 있음

---

```
github 에서 코드를 다운받고,,

bash gen_key    // key 생성
bash compile    // compile
cd bin/         // 이동해서
./server        // 서버 실행
```
