# Basic OpenSSL

- [Review](#review)

  - [HTTPS overview](#https-overview)

  - [Symmetric Cipher](#symmetric-cipher)

  - [Asymmetric Cipher](#asymmetric-cipher)

  - [Digital Signature](#digital-signature)

- [OpenSSL](#openssl)

  - [Basic](#openssl--basic)

  - [Cipher and Certificate](#openssl--cipher-and-certificate)

---

## Review

### HTTPS overview

<img width="656" alt="스크린샷 2023-06-10 오후 6 02 41" src="https://github.com/jjaehwi/2023_OS/assets/87372606/3bc6b39a-68df-49a3-8ddb-43e3027df7ee">

- HTTP 와 HTTPS 의 차이점 : TLS layer 가 있다는 것.

---

### Symmetric Cipher

<img width="673" alt="스크린샷 2023-06-10 오후 6 06 12" src="https://github.com/jjaehwi/2023_OS/assets/87372606/14938697-a29d-4857-ad4e-ca11e00aaf2a">

- **_대칭키 암호는 암호화할 때와 복호화할 때, 사용되는 키가 같은 암호화 시스템이다._**

- `대칭키 암호 알고리즘 구성`

  - 암호화를 하는 데이터 송신자 / 복호화를 하는 데이터 수신자가 동일한 키를 갖고 있어야한다.

- 키를 통해 송신자가 평문을 암호화하면 랜덤한 값인 Ciphertext 가 나오고 수신자에서 같은 키로 복호화하면 다시 평문이 나온다.

---

### Asymmetric Cipher

<img width="673" alt="스크린샷 2023-06-10 오후 6 06 21" src="https://github.com/jjaehwi/2023_OS/assets/87372606/5b103913-e148-41b2-bfd1-181f6ba622c4">

- `공개키 암호 알고리즘 구성` (**Public key**)

  - Public key 와 Private key 가 한 쌍으로 존재하지만 서로 같지 않고 수학적인 관계만 존재한다.

- 평문을 암호화할 때는 **수신자의 Public key 를 통해 암호화를 하고 암호화된 데이터를 받은 수신자는 자신의 public key 로 암호화된 데이터를 자신만 갖고 있는 Private key 로 복호화**할 수 있다.

- 속도가 느리다.

- 키 교환 분야에 사용된다.

---

### Digital Signature

<img width="673" alt="스크린샷 2023-06-10 오후 6 06 28" src="https://github.com/jjaehwi/2023_OS/assets/87372606/e621777a-afc9-4912-9b35-f0fa9891ec39">

- 공개키 crypto system 은 1. 암호 기술, 2. 전자서명 기술 로 나뉜다.

- `전자서명 알고리즘 구성`

  - 데이터를 **송신자의 Private key (개인키) 로 암호화**한다. -> 데이터에 서명값이 들어가는 것 (Msg + sign value)

  - **_검증을 할 때는 송신자 (서명자) 의 공개키로 한다._**

  - 검증에 통과하면 YES, 아니면 NO

- `PKI` : **_공개 키 기반 구조_**

  - **공개키를 어떻게 신뢰되게 알릴 것인지에 관한 것.**

---

## OpenSSL

- `OpenSSL` is a widely used **open source library** that **_provides SSL and TLS services to applications._**

  - `OpenSSL` : **오픈소스 라이브러리로 SSL 이나 TLS 서비스를 Application 에게 제공**한다.

```
openssl version // 버전 확인

sudo apt-get install libssl-dev // openssl 개발을 위한 패키지 설치
```

---

### OpenSSL : Basic

- OpenSSL Basic operation

```
SSL_library_init();
OpenSSL_add_all_algorithms();
SSL_load_error_strings();
```

1. `SSL_library_init()`

   - OpenSSL 라이브러리를 쓸 수 있게끔 initialize

2. `OpenSSL_add_all_algorithms()`

   - 모든 가능한 알고리즘을 load

3. `SSL_load_error_strings()`

   - 에러와 관련된 것들을 출력할 때

---

```
SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
if (!ctx) {
   fprintf(stderr, "SSL_CTX_new() failed.\n");
    return 1;
}
```

- 위 세 함수로 초기화하고 나면 `SSL context` 를 만들어야한다.

  - `SSL_CTX_new() function` 사용.

  - **_SSL_CTX object 는 SSL / TLS connection 을 관리해주는 factory_**

    - **initial 세팅에 관련된 정보를 SSL_CTX object 에 저장**한다. (초기 세팅한 암호화 키나 인증키 같은 것을 저장한다)

    - SSL_CTX object 를 만들면 **connection 동안 유지되면서 사용**된다.

  - SSL_CTX_new() function 은 하나의 argument 를 받는다.

    - TLS_client_method() -> TLS method 를 사용하기 위해

  - **클라이언트가 자동적으로 가장 필요한 알고리즘이랑 그와 관련된 키들을 서버와 통신하면서 SSL_CTX \*ctx 에 세팅**한다. (Our client automatically negotiates the best mutually supported algorithm with the server upon connecting.)

---

```
SSL *ssl = SSL_new(ctx);
if (!ctx) {
    fprintf(stderr, "SSL_new() failed.\n");
    return 1;
}
if (!SSL_set_tlsext_host_name(ssl, hostname)) {
    fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");
    ERR_print_errors_fp(stderr);
    return 1;
}
SSL_set_fd(ssl, server); //int SSL_set_fd(SSL *ssl, int fd);
if (SSL_connect(ssl) == -1) {
    fprintf(stderr, "SSL_connect() failed.\n");
    ERR_print_errors_fp(stderr);
    return 1;
}
```

- TCP connection 이 성공적으로 되면 이후 TLS connection 을 만들어야한다.

  - ctx object 를 통해 ssl object 를 만든 후 ssl object 를 이용해서 서버 소켓에 붙힌다.

  - SSL_connect 함수를 통해 서버가 ssl 접근을 시도한다.

- `SSL_new()` is used to **create an SSL object**. This object is used to track the new SSL/TLS connection.

  - SSL_new 는 ssl object 를 만들고, ssl object 를 통해서 만든 변수를 통해 ssl connection 을 tracking

- `SSL_set_tlsext_host_name()` to **set the domain for the server we are trying to connect to.**

  - 접속하고자하는 도메인 name 을 세팅

  - This call is optional, but without it, the server does not know which certificate to send in the case that it hosts more than one site.

- `SSL_set_fd()` and `SSL_connect()` to **initiate the new TLS/SSL connection on our existing TCP socket.**

  - SSL_set_fd() : 만든 TCP 소켓에 ssl 변수를 세팅

  - SSL_connect() : ssl object 를 이용해서 해당 서버에 connection 을 만듬

---

```
char buffer[2048];

sprintf(buffer, "GET / HTTP/1.1\r\n");
sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", hostname, port);
sprintf(buffer + strlen(buffer), "Connection: close\r\n");
sprintf(buffer + strlen(buffer), "User-Agent: https_simple\r\n");
sprintf(buffer + strlen(buffer), "\r\n");

SSL_write(ssl, buffer, strlen(buffer));
printf("Sent Headers:\n%s", buffer);

while(1) {
    int bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes_received < 1) {
            printf("\nConnection closed by peer.\n");
            break;
        }

        printf("Received (%d bytes): '%.*s'\n", bytes_received, bytes_received, buffer);
    } // end while(1)
```

- `SSL_write()` : 데이터를 보낼 때 사용

- `SSL_read()` : 데이터를 읽을 때 사용

---

```
SSL_shutdown(ssl);
SSL_free(ssl);
SSL_CTX_free(ctx);
```

- ssl 관련 connection 변수나 ctx 변수가 계속 메모리에 할당되어있으면 메모리 누수가 발생할 수 있기 때문에 ssl 관련 함수들이 종료되면 `SSL_shutdown`, `SSL_free`, `SSL_CTX_free` 를 통해 할당된 메모리를 해지해야한다.

---

### OpenSSL : Cipher and Certificate

```
printf ("SSL/TLS using %s\n", SSL_get_cipher(ssl));
```

- SSL_get_cipher 함수는 우리가 만든 ssl 변수를 통해 세팅했던 알고리즘 리스트를 출력할 수 있다.

---

```
X509 *cert = SSL_get_peer_certificate(ssl);
if (!cert) {
    fprintf(stderr, "SSL_get_peer_certificate() failed.\n");
    return 1;
}

char *tmp;
if ((tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0))) {
    printf("subject: %s\n", tmp);
    OPENSSL_free(tmp);
}

if ((tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0))) {
    printf("issuer: %s\n", tmp);
    OPENSSL_free(tmp);
}

X509_free(cert);
```

- SSL_get_peer_certificate 함수는 접속한 서버에 인증서를 가져올 수 있다.

  - X509 는 인증서와 관련된 변수로, 포인터를 만들어서 ssl 과 관련된 서버 인증서를 받아오고

  - X509 와 관련된 함수를 통해 필요한 정보를 출력할 수 있다.

    - X509_get_subject_name 은 인증서의 주인 name 을 가져온다.

    - X509_get_issuer_name 은 인증서를 발행한 CA 의 이름을 가져온다.

---

<img width="673" alt="스크린샷 2023-06-10 오후 6 48 12" src="https://github.com/jjaehwi/2023_OS/assets/87372606/c4ea9b0c-3c5f-4d34-a372-9d5c090c9d52">

- 컴파일 할 때는 -lssl - lcrypto 라는 옵션을 줘야한다.

<img width="1062" alt="스크린샷 2023-06-10 오후 6 52 39" src="https://github.com/jjaehwi/2023_OS/assets/87372606/5cf9a0af-5357-4616-aef0-6cbfeae300e3">

- 접속하고자하는 사이트 name 과 TLS 포트인 443 을 넣으면 프로그램은 TLS connection 을 맺고 TLS 가 사용하고 있는 알고리즘의 이름과 subject 와 issuer name 을 가져온다.

- 데이터를 보내고 받음도 확인 가능

<img width="1062" alt="스크린샷 2023-06-10 오후 6 52 28" src="https://github.com/jjaehwi/2023_OS/assets/87372606/e9ace6ca-8757-47ca-b827-4c04c08984aa">

- TCP connection 이 3 - way - handshake 를 한 후 클라이언트가 서버에게 Client Hello 를 보내서 TLS initialization 을 시작한다.

  - 서버가 바로 받지 못하고 Hello Retry Request 를 보내면서 Change Cipher Spec 메시지를 보낸다.

  - 그러면 클라이언트는 다시 Change Cipher Spec 메시지를 보내면서 Client Hello 를 보낸다.

  - 이후 서버는 Server Hello 를 보내고 Application Data 를 보낸다.
