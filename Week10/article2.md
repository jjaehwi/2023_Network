# HTTP and Web Client (1)

- [HTTP protocol](#http-protocol)

  - [HTTP Request](#http-request)

  - [HTTP Response](#http-response)

  - [What's in a URL](#whats-in-a-url)

- A web client

- HTTP POST requests

---

## HTTP protocol

- **`HTTP` 는 text - based client - server protocol 이고 TCP 기반으로 동작**한다.

  - HTTP 서버가 구동될 때 일반적으로 `80 포트`를 사용

- 하지만 HTTP 는 임호화되지 않은 상태에서 데이터가 오가기 때문에 보안이 취약하다.

  - 스니퍼 같은 도청 공격자에게 데이터가 쉽게 노출된다.

  - `HTTPS` 는 `Transport Layer Security (TLS) 계층`을 통해 HTTP 프로토콜을 실행하는 것 만으로 HTTP 를 보호한다. (HTTPS secures HTTP by merely running the HTTP protocol through a Transport Layer Security (TLS) layer)

    - 서버와 데이터를 주고받을 때 암호화되기 때문에 유의미한 데이터를 공격자가 가져갈 수 없다.

<img width="557" alt="스크린샷 2023-05-10 오전 1 12 12" src="https://github.com/jjaehwi/2023_Network/assets/87372606/500e1cb0-8799-4a66-80cc-5ba5a91fd5e5">

- 클라이언트가 원하는 소스를 request 를 통해 보낸다. (`HTTP Request`)

- 웹 서버가 request 를 받아서 정당한 요청이라면 response 를 보내서 사용자 브라우저에게 띄운다. (`HTTP Response`)

### HTTP Request

- `HTTP Request types`

  - `GET` : when the client wants to download a resource

  - `HEAD` : client only wants information about the resource instead of the resource itself

    - 리소스가 아닌 리소스에 대한 정보를 가져올 때

  - `POST` : client needs to send information to the server

    - 클라이언트가 서버에 정보를 보내서 웹 서버가 받게끔 하는 것

    - 정보를 줄 때 위치를 지정하지 않아서 중복된 데이터를 보내도 계속 받는다.

  - `PUT` : most-often utilized for **update** capabilities

    - 업데이트를 할 때 사용

    - 서버의 실제 저장소 위치를 명확하게 해야한다. URI 포맷을 통해 접근하고자하는 자원의 위치를 명시하고 전달한다.

    - 새로운 데이터라면 생성되고, 있는 데이터라면 업데이트한다.

  - `DELETE` : used to **delete** a resource identified by a URI

<img width="592" alt="스크린샷 2023-05-10 오전 1 21 54" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a550ff80-b41d-464c-becf-20195a081f89">

---

- `HTTP Request format`

  - (ex) 웹 브라우저에서 http://www.example.com/page1.html 이라는 링크를 클릭하면 웹 브라우저에서 웹 서버로 가는 데이터는 HTTP request 가 되고 이에 대한 정보를 달라고 요청한다.

  - GET 을 통해 접근하고자 하는 example.com 의 page1 resource 를 달라고 요청하면 서버가 page1 에 대한 html 코드를 클라이언트에게 보내준다.

  - GET 에는 HTTP body 가 없지만, POST 나 PUT 에는 HTTP body 가 존재한다.

    - HTTP Request Header 가 끝난 후 body 가 나온다.

  - 보이지 않아도 항상 \r\n 이 존재한다.

  - Connection : Keep-Alive : 추가적인 request 를 보낼 수 있으니 response 를 보내고나서도 connection 을 유지하라는 뜻

    - Close 라면 response 를 보내고 connection 을 종료하라는 뜻

<img width="608" alt="스크린샷 2023-05-10 오전 1 23 51" src="https://github.com/jjaehwi/2023_Network/assets/87372606/01ea823b-361a-4f10-8c1c-facfd8f71736">

---

### HTTP Response

<img width="583" alt="스크린샷 2023-05-10 오전 1 30 27" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c4ce828d-8ab6-435b-8278-d128746f02f5">

- Like the HTTP request, the HTTP response also consists of `a header part and a body part.`

  - Status Line : 프로토콜 버전과 성공했는지 실패했는지 알려주는 response code 와 response code 에 대한 명세

  - Content-Type : 클라이언트에게 어떤 리소스를 보내고 있는지 명시됨

  - Date : 리소스가 언제 만들어 졌고 수정됐는지 클라이언트에게 알려주기위해 존재

  - Content-Length : HTTP response body 의 크기

  - 이후 response 의 헤더가 가리키고 있는 body 에 대한 내용이 나온다. (header 와 body 사이에는 \r\n\r\n 이 존재)

<img width="577" alt="스크린샷 2023-05-10 오전 1 35 46" src="https://github.com/jjaehwi/2023_Network/assets/87372606/a8719184-8125-429a-aa47-b3e7219f7ec1">

- 경우에 따라 `Content-Length 가 없는 상황`이 있다. (body length 를 모르는 상황)

  - body length 를 모르니 Content-Length 를 쓸 수 없다.

  - 이런 경우 **Transfer-Encoding: chunked header line** 을 쓴다.

  - chunk 단위로 데이터를 보내고 나중에 합쳤을 때, 그 때가 전체 body 의 길이가 된다.

  - HTTP body 는 zero-length chunk 로 끝나게 된다.

<img width="577" alt="스크린샷 2023-05-10 오전 1 37 26" src="https://github.com/jjaehwi/2023_Network/assets/87372606/e97916cf-1905-46f9-b41c-725137e5eb67">

- HTTP request 를 보내면 서버가 상황에 따라 response code 를 보낸다.

  - 200 OK : 서버가 성공적으로 클라이언트의 요청을 받아서 response 를 보냄

  - 301 Moved Permanently : 리소스가 완전 이동된 경우, 새로운 location 을 찾아서 request 를 보내라.

  - 400 Bad Request : 서버가 이해하지 못했거나 클라이언트의 요청을 support 할 수 없다.

  - 401 Unauthorized : 클라이언트가 리소스를 받도록 인가되지 않았다. (클라이언트가 권한이 없는 경우)

  - 403 Forbidden : 클라이언트의 접근이 금지됨

  - 404 Page Not Found or File Not Found error message : 페이지가 없음

  - 500 Internal Server Error

---

### What's in a URL

<img width="521" alt="스크린샷 2023-05-10 오전 1 41 26" src="https://github.com/jjaehwi/2023_Network/assets/87372606/0086e793-5a46-4e73-a11b-0dcb270a64bb">

- `URL (Uniform Resource Locators)` : **리소스의 위치를 명시**한 것

  - also known as `web addresses`, **provide a convenient way to specify particular web resources**
