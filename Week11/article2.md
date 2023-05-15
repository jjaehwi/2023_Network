# 보안 기본 개념 (1)

- 기초 보안 개념들을 배우고 네트워크 보안 protocol 과 보안 + 네트워크 프로그래밍을 할 예정

- [정보 보호 기초](#정보-보호-기초)

  - [Symmetric vs Public Key Algorithms](#symmetric-vs-public-key-algorithms)

  - [정보 보호 기초 서비스](#정보-보호-기초-서비스)

- [암호학적 해쉬 함수 (Cryptographic Hash Function)](#암호학적-해쉬-함수)

  - [함수의 관점](#함수의-관점)

  - [MDC](#mdc-message-modification-code)

  - [MAC](#mac-message-authentication-code)

- 대칭키 암호 시스템 (Symmetric Crypto System)

- 공개키 암호 시스템 (Public Crypto System)

- 공개키 기반 구조 (Public Key Infrastructure)

---

## 정보 보호 기초

- `암호기법 (cryptography)` : 그리스어로 “비밀(secret)”을 의미하는 kryptos와 “쓰다(write)”를 의미하는 gráphō의 합성어

  - `메시지의 기밀성(confidentiality)`을 제공하기 위하여 사용

  - 현재는 메시지를 공격자로부터 안전하게 보호하기 위하여 메시지를 변화하는 과학이나 기술을 의미.

  - **Key: uniformly distributed random string**

    - random string 값에 대해 복호화하는 key

  - `Symmetric` (**대칭키**) if k1 = k2 (A 의 키와 B 의 키가 같은 경우), Otherwise, `asymmetric` (**공개키 public key**)

- **_암호화 관련 대표 용어 정리_**

  - Plaintext (평문): 암호화되기 전의 메시지를 의미함

  - Encryption algorithm (암호화 알고리즘)

  - Secret key (암호화 키): 암호 알고리즘의 안정성은 암호화 키에 의존함

  - Ciphertext (암호문): 암호화 된 후의 메시지를 의미함

  - Decryption algorithm (복호화 알고리즘)

<img width="467" alt="스크린샷 2023-05-16 오전 2 13 25" src="https://github.com/jjaehwi/2023_Network/assets/87372606/db89897c-4b66-433a-b56d-b85a761d08ff">

---

### Symmetric vs Public Key Algorithms

- `symmetric key 암호화`는 **`encryption` or `cipher` 라고 부른다.**

  - Alice → Bob : Ek(m) = c

  - Bob decrypts c by using k

- How can Alice and Bob share k?

  - Public Key Algorithms solves this problem!

  - Encrypt k using Bob’s public key k𝑝𝑢𝑏 , i.e., E𝑝𝑢𝑏𝑘(k).

  - Bob then decrypts E𝑝𝑢𝑏𝑘(k) using his private key k𝑝𝑟𝑖𝑣 to obtain k.

---

### 정보 보호 기초 서비스

- 안전한 암호란?

  - 공격자가 암호시스템에서의 비밀키를 얻을 수 있다면, 그 암호시스템은 완전히 파괴되었다고 말할 수 있다.

  - 하지만 공격자가 키를 알아낼 수는 없지만 암호문의 일부분, 혹은 한 글자만 알 수 있는 경우, 안전하다고 할 수 있을까?

- 3대 정보보호 서비스 (NIST)

  - `기밀성` (Confidentiality)

    - 정보를 오직 인가(authorization)된 사람들에게만 공개

  - `무결성` (Integrity)

    - 컨텐츠가 훼손 되어선 안됨

  - `가용성` (Availability)

    - 원할 때 접근할 수 있어야한다.

- 정보보호 서비스 이외에 환경에 따라 요구되는 서비스는 다양하다.

  - `인증` (Authentication)

  - `부인방지` (Non-Repudiation)

    - 전자서명

  - `접근제어` (Access Control)

---

## 암호학적 해쉬 함수

<img width="467" alt="스크린샷 2023-05-16 오전 2 40 04" src="https://github.com/jjaehwi/2023_Network/assets/87372606/b0d76756-0fcb-447f-9cef-9615e0bd9a15">

- `메시지 무결성 (Message Integrity) 을 제공하는 정보 보호 솔루션`

  - 원본 메시지의 변조를 방지하기 위한 서비스

  - 대칭키 & 공개키 암호 시스템은 `기밀성 (데이터를 숨기는 것) 을 제공`

  - 무결성 (데이터를 보존하는 것) 을 위해 Modification Detection Code(MDC) 사용

    - 변조 감지 코드를 생성하기 위한 방법으로 암호학적 해쉬 함수 (Cryptographic Hash Function) 사용

- (해쉬함수 ex) Secure Hash Algorithm (SHA)

<img width="599" alt="스크린샷 2023-05-16 오전 2 42 07" src="https://github.com/jjaehwi/2023_Network/assets/87372606/e20abf26-8322-4777-a387-f90ee6744efb">

- SHA 256 bit --> output 이 32 바이트

- 암호학적 해쉬 함수: `h = H(M)`

  - **임의의 크기의 데이터를 입력할 수 있다.** (variable 한 사이즈의 input 을 넣고 고정된 output 을 낸다.)

  - **고정된 크기의 출력 값**

  - 하드웨어 및 소프트웨어의 적용이 쉬워야 하며, 어떤 입력 데이터에 대해서도 출력 값 을 계산하기 용이

  - 해쉬 함수는 **공개된 함수이며 키가 사용되지 않음** (**무결성 검증만 할 뿐**)

  - `키를 사용한 해쉬 함수: h = H(k,M)` -> **MAC (Message Authentication Code)**

    - `메시지 무결성 (Integrity) 뿐만 아니라 인증 (Authentication) 제공`

---

### 함수의 관점

<img width="575" alt="스크린샷 2023-05-16 오전 2 51 59" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a5f274d3-5980-4905-a9e6-1ad4b8f4cbb7">

<img width="575" alt="스크린샷 2023-05-16 오전 2 52 18" src="https://github.com/jjaehwi/2023_Network/assets/87372606/be80c035-4209-4657-b056-59307a04fa52">

- 역방향으로 가기는 어렵다.

<img width="557" alt="스크린샷 2023-05-16 오전 2 54 08" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a8861bb0-ce32-4170-b4e8-15a9954d3a76">

- `Avalanche effect 가 있게끔 설계`된다. (ex. SHA1, SHA256)

  - **Input의 한 비트 차이는 Output의 완전한 변화를 야기함**

  - If there is one-bit difference between inputs, the corresponding outputs are totally different!

- [SHA Test Link](https://passwordsgenerator.net/sha1-hash-generator/)

<img width="579" alt="스크린샷 2023-05-16 오전 2 55 31" src="https://github.com/jjaehwi/2023_Network/assets/87372606/d7cf4442-1ce2-4c69-8e25-d17b59016877">

- 목적 : 무결성 제공 (MDC 역할)

---

### MDC (Message Modification Code)

- `메시지 변조 감지 코드: MDC` (Message Modification Code)

  - 메시지 변조 여부를 검증하기 위해 암호학적 해쉬 함수를 사용할 수 있음

  - 메시지 m 과 MDC h(m) 은 분리될 수 있으며, h(m) 은 변조되지 않도록 하는 것이 중요!

<img width="579" alt="스크린샷 2023-05-16 오전 2 57 28" src="https://github.com/jjaehwi/2023_Network/assets/87372606/e79f6e8c-03c7-4233-9719-6863f4000a7a">

- 앨리스가 메시지 m 을 만들어서 암호학적 해쉬 함수 결과 h(m) 를 만든다.

  - 이 결과 h(m) 을 안전한 채널 (ex. 클라우드) 로 보낸다.

  - 앨리스가 메시지 m 를 밥 에게 보내면 밥은 이 메시지가 앨리스가 보낸 메시지가 맞는지 확인을 하고 싶다.

  - 밥은 앨리스에게 받은 메시지를 동일한 암호학적 해시 함수에 넣어서 결과 h'(m) 을 만들고 안전한 채널에 접근해서 앨리스가 만들어낸 h(m) 을 다운받아 비교한다.

  - 같다면 메시지가 훼손되지 않은 것이고, 다르다면 앨리스가 만든 메시지가 전송도중 훼손되었다는 것을 알 수 있다.

<img width="608" alt="스크린샷 2023-05-16 오전 3 01 18" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c32c2259-9ae3-4f71-ac73-bbdb0047bb23">

- MDC 를 이용한 메시지 변조 감지 코드 활용 예

  - 클라우드 서비스에서 원본 메시지에 대한 무결성을 제공

  - 디지털 포렌식에서도 사용됨

    - 수사하기 전 원본에 대한 해시값을 만들어서 보관..

---

### MAC (Message Authentication Code)

- MAC 부터 (2) 진행..
