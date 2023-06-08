# 보안 기본 개념 (2)

- 암호학적 해쉬 함수, 대칭키 암호 시스템, 공개키 암호 시스템, 공개키 기반 구조 를 통해 네트워크 보안이 이뤄지고 TLS 라는 보안 프로토콜에 대해 배운다.

  - TLS 에 기반한 기술들이 이것들과 관련 있어서 이론적으로 살펴본 후 TLS 를 배우고 TLS 를 응용한 간단한 서버 / 클라이언트 프로그래밍을 실습한다.

- 정보 보호 기초

  - Symmetric vs Public Key Algorithms

  - 정보 보호 기초 서비스

- 암호학적 해쉬 함수 (Cryptographic Hash Function)

  - 함수의 관점

  - MDC

  - [MAC (Message Authentication Code)](#mac)

- [대칭키 암호 시스템 (Symmetric Crypto System)](#대칭키-암호-시스템-symmetric-crypto-system)

  - [대칭키 암호 시스템 : Advanced Encryption Standard (AES)](#대칭키-암호-시스템--advanced-encryption-standard-aes)

    - [암호화 예시](#암호화-예시)

- [공개키 암호 시스템 (Public Crypto System)](#공개키-암호-시스템-public-crypto-system)

  - [공개키 암호시스템과 대칭키 암호시스템의 차이점](#공개키-암호시스템과-대칭키-암호시스템의-차이점)

  - [하이브리드 암호시스템](#하이브리드-암호시스템)

  - [전자 서명 (Digital Signature)](#전자-서명-digital-signature)

  - [전자 서명의 구체적 동작](#전자-서명의-구체적-동작)

- [공개키 기반 구조 (Public Key Infrastructure)](#공개키-기반-구조-public-key-infrastructure)

---

### MAC

- `Hash` : 어떠한 **_고정되지 않은 input 이 들어왔을 때, fix 된 output 을 주는 함수_**

  - SHA1 --> output size = 20 bytes

  - SHA256 --> output size = 32 bytes

  - input 이 다르면 output 이 다를 확률이 매우 높기 때문에 **메시지의 무결성을 제공**한다. (정보가 훼손되었는지 아닌지 체크)

<img width="581" alt="스크린샷 2023-06-08 오후 11 56 13" src="https://github.com/jjaehwi/2023_Network/assets/87372606/ba71d3ef-08b7-4e1f-af5c-23bc9477a6b1">

- `Hash 함수는 표준화된 공개된 함수`여서 **input 이 뭐가 오던 output 을 구할 수 있다.** 하지만 **hash 함수에 input 을 넣을 때 어떠한 `미리 정해진 key 값`을 같이 넣는다면, `key 값을 알고 있는 사람만 함수를 통해 output 을 얻을 수 있을 것`이다.**

  - 이런 점에서 착안된 기술이 `메시지 인증 코드 (Message Authentication Code)` 이다.

    - **메시지를 인증할 때 사용하는 알고리즘**

    - **hash 함수의 key 를 사용해서 메시지를 인증할 때 사용**된다.

- 위 그림에서보면 **Alice 가 Bob 이 메시지를 주고받는데 Alice 가 Bob 에게 메시지를 보냈을 때 Bob 이 자신이 받은 메시지가 Alice 로 부터 왔음을 인증하고 싶은 것.**

  - 메시지에 식별자를 둬서 메시지가 누구에게 왔는지 확인해야하고 이런 기능을 제공하는 것이 MAC 이다.

  - **_Alice 와 Bob 은 서로 메시지 인증을 하기 위해 사용되는 key 값을 사전에 공유하고 있다고 가정한다._**

  - Alice 가 메시지를 보낼 때, 메시지만 보내는 것이 아니라 메시지를 인증해줄 수 있는 태그를 만들어서 보낸다. 태그를 만들기 위해 **메시지에다가 Bob 과 공유한 key 를 추가하여 hash 함수에 넣는다**.

  - `메시지와 key 값을 넣어 hash 함수를 돌린 output 이 MAC` 이 된다.

  - 이후 메시지와 MAC 태그를 안전하지않은 채널을 통해 Bob 에게 보낸다.

  - 이것을 받은 Bob 이 메시지를 받아 메시지가 중간 공격자에 의해 훼손되었는지, Alice 가 보낸 것인지 확인하기 위해 hash 함수에 넣는다. 이 때, Alice 와 공유한 key 도 같이 넣는다. 이렇게 되면 **_output 은 MAC 이 된다._** (메시지와 key 가 input 이기때문)

  - 그리고 **네트워크를 통해 전달된 MAC 태그와 자신이 만든 MAC 태그를 비교**한다. 비교해서 같다면 accept, 다르다면 reject

    - **key 를 공유한 사람은 Alice 와 Bob 뿐이기 때문에 파란색 태그는 Alice 와 Bob 만 만들 수 있어서 같다면 Bob 은 보낸 사람이 Alice 임을 확신할 수 있는 것.**

- 네트워크의 모든 패킷들은 이런식으로 MAC 값을 지니고 다닌다.

  - 클라이언트가 서버가 통신할 때, **key 를 공유**하게 되고 (key 를 공유하는 기술 (알고리즘) 존재), 공유된 key 를 통해 패킷을 보낼 때 MAC 값을 같이 보내는 것이다. 그래서 패킷을 받았을 때, MAC 값이 검증이 되면 클라이언트와 서버는 서로 공유한 key 로 MAC 값을 확인할 수 있기 때문에 패킷이 클라이언트로부터 왔고 서버로부터 왔고 를 알 수 있게된다.

- `정리`

  - **_MAC 은 송/수신자가 서로 공유한 키를 이용하여 메시지 인증을 수행함_**

    - 서로 **공유한 key 를 통해 어떤한 값을 만들어두는 것.**

  - 공격으로부터 안전한 채널을 형성할 필요가 없음

  - 대표적인 MAC의 형태는 암호학적 해쉬 함수에 인증 키를 사용하는 구조임

---

## 대칭키 암호 시스템 (Symmetric Crypto System)

- 암호 시스템에는 `1. 대칭키 암호`와 `2. 공개키 암호`가 존재한다.

- 대칭키 암호 - 기밀성 제공

<img width="594" alt="스크린샷 2023-06-09 오전 12 24 11" src="https://github.com/jjaehwi/2023_Network/assets/87372606/7f937521-606e-4008-bea2-067ade0c506c">

- `대칭키 암호 시스템` : 가운데를 나눴을 때, 서로 **_대칭인 상황에서 동일한 key_** 가 있기 때문에 대칭키 암호

  - key 공유 알고리즘을 통해 key 를 공유한다. (안전한 key 공유가 가능하다.)

  - key 가 공유되면 Alice 와 Bob 은 암호화를 할 수 있다.

  - 암호화에 사용되는 알고리즘은 encryption 알고리즘이다.

  - Plaintext 가 들어오면 서로 공유된 key 와 같이 encryption 알고리즘에 들어가면 Ciphertext 가 나오고 이 암호문을 Isecure channel (네트워크) 을 통해 보낸다.

  - 암호문과 공유된 key 를 통해 decryption 알고리즘에 넣으면 Plaintext 가 나오는 구조

- TLS (네트워크 보안 프로토콜) 에도 대칭키 암호 알고리즘이 사용된다.

<img width="371" alt="스크린샷 2023-06-09 오전 12 56 14" src="https://github.com/jjaehwi/2023_Network/assets/87372606/93fd2033-93f1-414b-8b29-403ad1ab6dcb">

- `혼돈 (Confusion)` : 암호문을 만들 때, 치환 (Substitution) 을 사용하는 것

- `확산 (Diffusion)` : 문자의 순서를 바꾸는 것 (Transposition)

- `Product 시스템` : Substitution 과 Transposition 이 함께 사용되는 암호

  - 현대에 사용되는 암호 시스템

---

### 대칭키 암호 시스템 : Advanced Encryption Standard (AES)

- AES의 공모 시 요구사항

  - **블록의 크기는 128 비트**

  - **대칭키 암호이며 세 종류의 키(128 비트, 192 비트, 256 비트)를 사용할 수 있어야 함** – 소프트웨어와 하드웨어로 구현될 경우 모두 효율적

  - 모든 키를 다 찾는 전수 키 조사 이외에 현재 알려진 다른 암호 분석 공격에 강해야 함

<img width="395" alt="스크린샷 2023-06-09 오전 12 59 27" src="https://github.com/jjaehwi/2023_Network/assets/87372606/115fd5bf-6996-4a3e-a8bf-82823c0c76f9">

- `AES 구조` : **_블록 형태로 데이터가 운용되는 암호 시스템_**

  - 블록으로 쪼개고 한 블록이 128 비트 (16 bytes) 인 것.

  - **128, 192, 256 비트의 비밀키를 선택할 수 있고 선택된 비밀키 비트에 따라 10, 12, 14 라운드가 실행**된다. (몇 번 반복하는지,,)

    - 데이터를 transposition, substitution 하는 반복을 진행한 후 정해진 반복횟수가 끝나면 비밀키 비트에 따라 암호문을 내뱉는 것

---

- AES 는 어떠한 input 을 갖는데 그 input 은 암호화되어야하는 파일이므로, 다양한 사이즈가 있을 것이다. 하지만 **AES() 에 들어갈 때는 16 bytes (128 bits) 씩 쪼개져서 들어가고 16 bytes 단위로 output 이 나올 것**이다.

  - 16 bytes 는 원소가 한 byte 인 4 x 4 행렬로 변환된다. 이 행렬을 상태 (state) 라 부른다.

<img width="452" alt="스크린샷 2023-06-09 오전 1 10 08" src="https://github.com/jjaehwi/2023_Network/assets/87372606/1e887e88-4c07-4aba-8a7a-cb0b44759f83">

- 한 라운드는 4 가지 계층 (layer) 로 구성된다.

  - SubBytes : subtitution 을 수행하는 함수

  - ShiftRows : transposition 을 수행하는 함수

  - MixColumns : transposition 을 수행하는 함수

  - AddRoundKey : 세팅한 key 랑 state 와 XOR 연산을 통해 라운드의 새로운 output 을 만드는 함수

<img width="452" alt="스크린샷 2023-06-09 오전 1 11 45" src="https://github.com/jjaehwi/2023_Network/assets/87372606/849da84b-3cc0-4e03-97fb-a7200d33484a">

- 라운드는 세팅한 key 의 사이즈에 의거하여 결정된다.

  - 그림에서는 128 bit 의 AES 이고 10 라운드가 있는 것

  - **0 ~ 10 라운드까지 11 개의 128 bit 의 key 가 필요**하기 때문에 확장함수를 통해 라운드 별로 key 를 만들어준다. (w[?, ?])

---

### 암호화 예시

<img width="622" alt="스크린샷 2023-06-09 오전 1 13 51" src="https://github.com/jjaehwi/2023_Network/assets/87372606/3564ca18-1280-4b20-bb79-9d65bd0d7cb8">

- AES 의 한 블록이 “EASYCRYPTOGRAPHY” 인 경우

  - 16 bytes 의 input 을 4 x 4 행렬 상태 (state) 로 바꾼 것.

<img width="622" alt="스크린샷 2023-06-09 오전 1 16 04" src="https://github.com/jjaehwi/2023_Network/assets/87372606/7686f3cb-2f66-4276-889b-8648417cc590">

- 첫 라운드의 SubBytes 함수로 오면...

  - **_S - box 라고 하는 표준화된 table_** 이 있다. (AES 를 만든 대학원생들이 설계한 것)

  - state 를 S - box 를 통해 SubByte 를 수행하는 것.

---

<img width="539" alt="스크린샷 2023-06-09 오전 1 21 33" src="https://github.com/jjaehwi/2023_Network/assets/87372606/3ba37015-0863-418f-9753-00bf67915b5e">

- ShiftRows 계층에서는 그림과 같은 방식으로 shiftRows 동작.

---

<img width="550" alt="스크린샷 2023-06-09 오전 1 23 14" src="https://github.com/jjaehwi/2023_Network/assets/87372606/01e94dd2-9b0d-4d85-b8ea-334db9167c5b">

- MixColumns 계층 에서는 행렬 연산을 수행함.

  - SubBytes 계층과 ShiftRows 계층은 바이트 단위로 처리했다면

  - 충분한 분산 효과를 발생시키기 위하여, MixColumns 계층에서는 상태의 각 열을 비트 단위로 섞어 주는 것

---

<img width="490" alt="스크린샷 2023-06-09 오전 1 23 56" src="https://github.com/jjaehwi/2023_Network/assets/87372606/75411ff5-982a-4c0e-bce6-b241098fbc1c">

- Inverse MixColumns 계층 은 역행렬 수행 (**복호화할 때 필요**하다.)

---

<img width="554" alt="스크린샷 2023-06-09 오전 1 25 00" src="https://github.com/jjaehwi/2023_Network/assets/87372606/4810e569-37a7-44c7-a8a1-24f548f89e67">

- AddRoundKey 계층 에서는 하나의 state 에 해당 round 의 key 가 들어오면 열 단위로 XOR 연산을 해서 새로운 state 로 업데이트 된다.

---

<img width="482" alt="스크린샷 2023-06-09 오전 1 26 37" src="https://github.com/jjaehwi/2023_Network/assets/87372606/aa82618c-af45-48be-bd54-7e14b4337760">

- 키 확장 (key expansion) 함수는 128 bit 암호화 key (또는 다른 선택된 암호화 bit 의 key) 가 시드가 되어 expansion 하는 것

  - 128 bit 암호화 key 를 기준으로 총 11 개의 key 가 필요..

  - (ex) 16bytes x 11 개의 string 을 뽑아내어 key 로 사용하는 것

---

- [AES 과정 영상 참고](https://www.youtube.com/watch?v=gP4PqVGudtg) : Animation of AES

---

## 공개키 암호 시스템 (Public Crypto System)

- 기밀성, 인증 제공

- `공개키 (Public key)` 는 `1. 암호화에 사용`가능하고, `2. 전자서명에 사용` 가능하다. 대칭키와는 차이가 있다.

  - **공개키 (Public key)** or **비대칭키 (Asymmetric key)**

- **_각 사람마다 한 쌍의 키(공개키 𝑝𝑘, 개인키 𝑠𝑘)_**

  - `공개키는 모두에게 공개`되고, `개인키는 비밀로 보관`

  - **공개키 𝑝𝑘 로부터 개인키 𝑠𝑘 를 도출하는 것은 계산적으로 불가능 (Computationally Infeasible)**

<img width="552" alt="스크린샷 2023-06-09 오전 2 10 17" src="https://github.com/jjaehwi/2023_Network/assets/87372606/63f61848-0670-4675-a0c3-d9754d4685e3">

- Bob 이 Alice 에게 메시지를 보내려고 할 때, 대칭키를 사용하려면 Bob 과 Alice 가 키를 공유해야하는데 이런 상황이 되지 않는다면 공개키를 쓰게된다.

  - Alice 는 Alice 의 `공개키`와 `개인키`를 만든다. 공개키는 공개키 집합 (ex. 클라우드) 에 업데이트한다.

    - **공개키 집합은 데이터의 무결성이 보장**된다고 가정.

  - Bob 은 Alice 에게 암호통신을 해야하고, 공개키를 사용하기 위해 **공개키 집합에 Alice 의 공개키를 물어봐서 다운받고 Alice 의 공개키를 사용하여 자신이 보내고자하는 메시지를 암호화**를 한다.

  - **_공개키와 개인키는 쌍이기 때문에 공개키로 암호화된 것은 개인키로 복호화가 된다._**

  - 따라서 Alice 가 Bob 에게 받은 암호문을 복호화할 때 Alice 의 개인키로 복화화한다. **암호문을 복호화할 수 있는 사람은 Alice 의 공개키에 대한 쌍인 개인키를 가지고 있는 Alice 뿐**이다.

---

### 공개키 암호시스템과 대칭키 암호시스템의 차이점

<img width="556" alt="스크린샷 2023-06-09 오전 2 16 34" src="https://github.com/jjaehwi/2023_Network/assets/87372606/dcb7ecbe-ddc8-4aa2-8f92-56592a84aa08">

- 암호화 & 복호화 속도에서 엄청난 차이가 발생...

  - 따라서 비밀키 분배나 보유 비밀키 개수에서 공개키 암호시스템이 장점이 있어도 속도 때문에 대칭키 암호시스템을 사용한다.

  - 실제로는 두 시스템을 합쳐서 `하이브리드 암호시스템`을 사용한다.

- **_키 공유 모델에 공개키 암호시스템을 사용하는 것_**

  - `TLS (Transport Layer security)` : 공개키 (키 공유) + 대칭키 (공유되면 대칭키 사용)

---

### 하이브리드 암호시스템

<img width="612" alt="스크린샷 2023-06-09 오전 2 21 36" src="https://github.com/jjaehwi/2023_Network/assets/87372606/dca2d4eb-71e3-41c0-88fd-2029f17e9f81">

- 대용량의 데이터를 암호화하기 위해서 대칭키 암호시스템에서 사용되는 **_비밀키 𝑘 를 `공개키 암호시스템`으로 암호화 (𝐸𝑝𝑘 (비밀키 𝒌)) 하여 분배하고_**, **_수신자는 분배된 비밀키를 이용하여 대용량의 데이터를 `대칭키 암호시스템`으로 암호화_**

- 그림에서 보면 Alice 가 랜덤하게 (ex. AES 128 -> 128 bit key) 대칭키 암호화한다. 암호문을 Bob 에게 보내면 Bob 은 현재 Alice 가 랜덤하게 만든 대칭키에 대해 복호화를 진행할 수가 없다.

  - Alice 는 자신이 만든 랜덤한 키에 대해 Bob 이 복호화를 할 수 있도록 해주기 위해 공개키 작업을 통해 키 공유를 한다.

  - Alice 는 Bob 과 통신하기 위해 공개키 집합에서 Bob 의 공개키를 가져오고 Bob 의 공개키를 사용하여 Alice 의 메시지 암호화에 사용했던 대칭키를 암호화한다.

  - **Alice 는 `대칭키로 암호화된 메시지`와 `Bob 의 공개키로 암호화된 Alice 의 대칭키` 를 Bob 에게 보내는 것**

- Bob 은 Alice 가 보낸 것 중 **자신의 공개키로 암호화된 것을 자신 (Bob) 의 개인키로 복호화하여 Alice 의 대칭키를 알아낸다.**

  - Bob 이 대칭키를 알아내면 Alice 가 보낸 암호화된 메시지를 복호화할 수 있다.

---

<img width="330" alt="스크린샷 2023-06-09 오전 2 32 19" src="https://github.com/jjaehwi/2023_Network/assets/87372606/42e8faa2-0427-466b-ab25-0e6e7e878205">

<img width="540" alt="스크린샷 2023-06-09 오전 2 31 11" src="https://github.com/jjaehwi/2023_Network/assets/87372606/b2350056-7e8b-4a03-ba2d-0bec0207905b">

- `일방향 함수 (One - way Function) f`

  - (ex) hash 함수

- `Trapdoor One - Way Function (TOWF)`

  - 공개키 함수

  - 어떤 메시지가 있을 때, **공개키로 암호화해서 암호화한 한 것을 얻는 것은 쉽다.** (공개키는 누구나 접근가능하고 데이터도 알 수 있으므로 암호화된 데이터를 만드는 것은 누구나 할 수 있다.)

  - 하지만 암호문을 복호화하는 과정은 아무나 하지 못한다. (**암호화한 공개키에 대한 쌍인 개인키를 갖고 있는 사람만이 할 수 있다.**)

- (EX) `RSA 암호`: **_가장 많이 사용되고 있는 공개키 암호시스템_**

  - Rivest, Shamir, and Adleman 의 이름에서 RSA

---

### 전자 서명 (Digital Signature)

<img width="593" alt="스크린샷 2023-06-09 오전 2 35 09" src="https://github.com/jjaehwi/2023_Network/assets/87372606/6a0021fa-8b25-46b6-8d7f-edd6d6b952c9">

<img width="422" alt="스크린샷 2023-06-09 오전 2 51 43" src="https://github.com/jjaehwi/2023_Network/assets/87372606/b0dfd833-e5dd-4603-9ded-94b87a7ba955">

- **_Public key crypto system (전자 서명은 공개키 암호 시스템 중 하나)_**

  1. `Public key encryption algorithm (공개키 암호) 가 포함된..`

     - Public key-based encryption algorithm (위에서 봤던 공개키 암호화 복호화 예시,,)

  2. `Digital signature (전자서명)`

     - (public key-based) digital signature algorithm

     - A 가 B 에게 메시지에 대한 서명 태그를 보낼 때,,

       - 서명은 누구나 검증이 가능해야하고, 서명은 나만 할 수 있어야한다.

       - 나만 알고 있는 서명 알고리즘을 돌려야한다. (**A 는 자기의 개인키로 서명 (암호화)**한다.)

       - **B 가 A 의 서명을 받았을 때, A 의 공개키를 통해 검증**을 할 수 있다.

---

### 전자 서명의 구체적 동작

<img width="486" alt="스크린샷 2023-06-09 오전 2 54 44" src="https://github.com/jjaehwi/2023_Network/assets/87372606/efdb88f1-fd1e-4076-a6c2-071dab9bfe5b">

- 키생성 과정을 통해 검증키 𝑝𝑘, 서명키 𝑠𝑘 생성

- 𝑆𝑖𝑔𝑠𝑘() = 서명 생성 알고리즘, 𝑉𝑒𝑟𝑝𝑘() = 검증 알고리즘

1. Alice -> Bob : 메시지 𝑚 에 대한 서명 𝑠 = 𝑆𝑖𝑔𝑠𝑘 (𝑚)

   - **_검증할 때는 공개키를 쓰고, 서명할 때는 개인키를 쓴다._**

2. Bob : 𝑉𝑒𝑟𝑝𝑘(𝑚, 𝑠) 로 검증

---

<img width="581" alt="스크린샷 2023-06-09 오전 2 58 25" src="https://github.com/jjaehwi/2023_Network/assets/87372606/1deb9428-1eac-49da-8f98-65ce94dcd04a">

- 공개키 암호 알고리즘은 느리다는 단점 --> 하이브리드 방식을 사용했었다.

  - 서명의 경우, **서명하는 파일이 매우 클 수도 있다.** (이전의 **하이브리드 방식은 대칭키를 공개키로 암호화했어서 작은 크기**였음.)

  - 공개키로 서명하게 될 경우 오래걸릴 수 있기 때문에 이를 해결하기위해 hash 함수를 사용한다.

    - **_input 이 엄청 크더라도 hash 함수에 의해 고정된 작은 output 으로 만들어 낼 수 있다._** (ex. SHA256 의 output 은 32 bytes)

    - 이후 hash 함수의 output 에 대해 서명을 진행한다.

---

## 공개키 기반 구조 (Public Key Infrastructure)

- 공개키 암호 시스템을 위한 공개키 기반 구조

<img width="489" alt="스크린샷 2023-06-09 오전 3 00 43" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c6bbbb84-6bee-4a76-a2e8-5ba415ce1be3">

- 공개키 암호시스템을 이용한 키 교환에서 **공개된 공간을 활용한 공개키 공개 선언을 하면 `신뢰성 문제`가 발생한다.**

<img width="489" alt="스크린샷 2023-06-09 오전 3 01 23" src="https://github.com/jjaehwi/2023_Network/assets/87372606/e6472b6c-649d-4ca9-8caf-a8e650edc979">

- 신뢰할 수 있는 서버를 사용하더라도 **서버에 과부하**가 발생한다.

---

<img width="553" alt="스크린샷 2023-06-09 오전 3 02 51" src="https://github.com/jjaehwi/2023_Network/assets/87372606/4bd7adb8-39f7-4f3b-ae78-b6787f6e308c">

- 따라서 `공개키 구조`를 생각했다.

  - **_`인증서`를 발행해서 `공개키를 인증`하는 것_**

- Bob 이 자신의 공개키랑 개인키를 만든다.

  - **공개키를 인증 기관에 보내서 인증서를 요청**하고, **인증 기관은 공개키가 Bob 의 것이라는 인증서를 발급**해준다.

- **다른 유저가 Bob 에 대한 공개키가 필요할 경우 인증 기관에 물어보는 것이 아니라 Bob 에게 요청**한다.

  -**_ 공개키와 인증서를 받으면 유저들은 공개키가 Bob 의 것인지 인증서를 통해 확인할 수 있다._**

- 등록 과정은 인증 기관을 거치지만, 공개키를 요청하고 받아오는 과정은 인증 기관 없이도 인증서를 통해 할 수 있게 된 것. (과부하를 막은 것)

---

<img width="615" alt="스크린샷 2023-06-09 오전 3 06 36" src="https://github.com/jjaehwi/2023_Network/assets/87372606/cd5538fe-0449-413b-b290-9e9a1beba726">

- 사용자가 공개키와 개인키를 만들면, 공개키를 이용해서 등록 기관 (RA) 에 인증서를 만들어 달라고 요청한다.

  - 인증서 발행을 대행해주는 `RA` 가 존재. (ex. 은행)

  - RA 는 `CA` 에 인증서 발급을 요청한다. (ex. KISA, 한국정보인증)

  - CA 는 인증서를 사용자에게 준다.

- 인증서는 유효기간이 남아 있더라도 폐기될 수 있다. (ex. 분실)

  - 온라인 서비스를 통해 인증서의 상태를 검증하는 서비스도 CA 영역에서 관리가 된다.

- 내가 다른 사람의 인증서를 요청하고 받고 싶은 경우

  - 내가 받은 인증서가 유효한지 아닌지 검증은 통과할 수 있어도 폐기 되었는지 아닌지에 대한 상태는 온라인 서비스를 통해야한다.

---

<img width="615" alt="스크린샷 2023-06-09 오전 3 12 16" src="https://github.com/jjaehwi/2023_Network/assets/87372606/bdfbd122-618e-4341-8dd8-5cce1a8c1330">

- 손도장을 검증하면 위의 정보들이 검증되는 것 (이 정보들이 CA 가 준 것이라는 사실을 알 수 있게된다.)

- 인증서는 공개키가 누구의 것 (ex. 유저 A) 이 맞다라는 것을 인증해주는 것이고 그런 정보들이 들어가 있고, 이에 대한 hash 값을 구하고 그에 대한 서명을 **CA (Certified Authority) 가 해서 보장**해주는 것. (**_A 의 대한 정보와 이를 보증하는 CA 의 서명 값 (tag) 가 붙는 것_**)

  - **인증서를 검증하는 것은 CA 의 서명을 검증하는 것**, CA 의 서명이 검증되면 A 의 정보라는 것을 확신할 수 있게 되는 것이다.

- 클라이언트와 서버끼리 통신할 때, 클라이언트의 인증서와 서버의 인증서를 교환한다.

  - 이 때, 서로 맞으면 자신의 개인키로 서로 서명을 해서 본인이 인증된 클라이언트와 서버라는 것을 확인한다.

  - 인증이 되지 않으면 (신뢰되지 않은 사이트 어쩌구,, -> 인증서가 폐지되어 유효하지 않은 인증서가 온 것)

---

- 이후 PKI 를 배우고, TLS 를 배우고, HTTPS client (암호, 인증이 있는 통신) 를 실습할 것..

---

## Summary

- Hash 함수

  - 메시지와 같이 key 를 사용하여 hash 함수를 통과한 output : MAC

- 대칭키 암호

  - AES

- 공개키 암호

  1. 암호화 알고리즘

  2. 서명 알고리즘

---

- 암호화를 할 때

  1. 대칭키 암호

  2. 공개키 암호

- 메시지에 대한 인증

  1. MAC

  2. 전자 서명

- 공개키와 전자 서명은 속도가 느리므로 키 공유 모델이나 코드 서명에 사용된다.

- 네트워크는 빠르게 패킷을 주고받아야하기 때문에 네트워크 안의 데이터들에 대한 인증과 암호화는 대칭키 암호와 hash 기반의 MAC 을 사용.

  - 대칭키 암호와 MAC 은 네트워크 traffic 에 사용되는 것.
