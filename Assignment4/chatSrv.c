#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

// 접속 가능 클라이언트 수
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// 소켓 및 사용자 이름 (username) 을 포함하여 클라이언트 정보를 저장하는 구조를 정의
struct ClientInfo {
    int socket;
    char username[256];
};

// client_sockets 는 connection 된 정보를 저장하는 데 사용되는 ClientInfo 구조의 배열
struct ClientInfo client_sockets[MAX_CLIENTS];
// 연결된 클라이언트의 수를 tracking
int client_count = 0;
// 서버가 이벤트를 확인해야 하는 파일 디스크립터를 포함하는 집합 (set)
fd_set readfds;
// select 함수의 인수로 사용되는 maximum socket descriptor 를 tracking
int max_sd = -1;

/* handle_new_connection 함수
들어오는 연결을 허용
클라이언트로부터 사용자 이름 (username) 을 수신
클라이언트 소켓을 client_sockets 배열에 추가한 다음 클라이언트에 시작 메시지를 보낸다. 
또한 메시지를 브로드캐스트하여 다른 클라이언트에 새 연결에 대해 알립니다. */
void handle_new_connection(int server_socket);

/* handle_client_activity 함수
클라이언트에서 메시지를 수신하여 다른 클라이언트로 브로드캐스트하는 등의 클라이언트 활동을 처리. */
void handle_client_activity(int client_socket);

/* handle_client_disconnection 함수
클라이언트 연결 해제를 처리.
다른 클라이언트에 연결 끊김을 알리고 client_sockets 배열에서 클라이언트를 제거한다. */
void handle_client_disconnection(int client_socket);

/* broadcast_message 함수
보낸 사람을 제외하고 연결된 모든 클라이언트에 메시지를 보낸다. */
void broadcast_message(const char* message, int sender_socket);

int main() {
    int server_socket, activity;
    struct sockaddr_in address;
    // 서버 소켓 생성 + 에러 처리
    // TCP 프로토콜(SOCK_STREAM) 이 있는 소켓을 사용하여 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }
    // 서버 주소 설정, 포트번호 9020
    // 서버 주소는 sockaddr_in을 사용하여 IP 주소와 포트 번호를 지정하여 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9020);
    // 서버 소켓 바인딩
    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Failed to bind server socket");
        exit(EXIT_FAILURE);
    }
    // connection 을 listen
    if (listen(server_socket, 5) == -1) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }
    // client_sockets 배열이 초기화
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i].socket = -1;
        strcpy(client_sockets[i].username, "");
    }
    // 서버 소켓을 초기화하고 readfds 집합에 추가
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);
    max_sd = server_socket;

    printf("Chat server is running. Waiting for connections...\n");

    // 클라이언트 활동을 대기하고 처리하기 위한 무한 루프
    // 1. 서버 소켓에 작업이 있는 경우 handle_new_connection 에서 새 연결을 처리
    // 2. 클라이언트 소켓에 활동이 있는 경우 해당 클라이언트 활동은 handle_client_activity 에 의해 처리
    // 3. 루프가 종료되면 모든 클라이언트 소켓이 닫히고 서버 소켓이 닫힘
    while (1) {
        // 임의의 set 을 만듣나. (select 가 set 을 modify 하기 때문)
        fd_set temp_fds = readfds;
        // select 함수를 이용하여 소켓 set 에 등록된 소켓 디스크립터의 이벤트를 wait (readfds의 소켓 activity 를 모니터링)
        activity = select(max_sd + 1, &temp_fds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }
        // 서버 소켓에 수신 연결 요청이 있는지 확인
        if (FD_ISSET(server_socket, &temp_fds)) {
            handle_new_connection(server_socket);
        }
        // 연결된 각 클라이언트 소켓의 이벤트 (activity) 확인
        for (int i = 0; i < client_count; i++) {
            int client_socket = client_sockets[i].socket;
            if (FD_ISSET(client_socket, &temp_fds)) {
                handle_client_activity(client_socket);
            }
        }
    }
    // 모든 클라이언트 소켓을 close
    for (int i = 0; i < client_count; i++) {
        close(client_sockets[i].socket);
    }
    // 서버 소켓 close
    close(server_socket);
    return 0;
}

void handle_new_connection(int server_socket) {
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    // 들어오는 연결 accept
    int new_socket = accept(server_socket, (struct sockaddr*)&client_address, &addrlen);
    if (new_socket == -1) {
        perror("Failed to accept connection");
        exit(EXIT_FAILURE);
    }
    // 배열에 새 클라이언트 소켓 추가
    if (client_count < MAX_CLIENTS) {
        // 배열에서 사용 가능한 첫 번째 슬롯 찾기
        int index = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i].socket == -1) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            client_sockets[index].socket = new_socket;
            // 클라이언트로부터 사용자 이름 수신
            char username[BUFFER_SIZE];
            int valread = recv(new_socket, username, BUFFER_SIZE - 1, 0);
            if (valread <= 0) {
                perror("Failed to receive username");
                exit(EXIT_FAILURE);
            }
            username[valread] = '\0';
            strcpy(client_sockets[index].username, username);
            client_count++;
            // 다른 클라이언트에 새 연결 알림
            char message[BUFFER_SIZE];
            sprintf(message, "New client connected\nsocket: %d\nusername: %s\n",new_socket, client_sockets[index].username);
            broadcast_message(message, new_socket);

        } else {
            printf("Maximum number of clients reached. Rejecting new connection.\n");
            close(new_socket);
            return;
        }
    } else {
        printf("Maximum number of clients reached. Rejecting new connection.\n");
        close(new_socket);
        return;
    }
    // set 에 새 소켓 추가
    FD_SET(new_socket, &readfds);
    // 필요한 경우 max socket 디스크립터 업데이트
    if (new_socket > max_sd) {
        max_sd = new_socket;
    }
    // 새 클라이언트에 시작 메시지 보내기
    char* message = "Welcome to the chat room!\n";
    if (send(new_socket, message, strlen(message), 0) == -1) {
        perror("Failed to send welcome message");
        exit(EXIT_FAILURE);
    }
}

void handle_client_activity(int client_socket) {
    char buffer[BUFFER_SIZE];
    int valread;
    valread = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (valread <= 0) {
        handle_client_disconnection(client_socket);
    } else {
        buffer[valread] = '\0'; // 수신된 메시지에 null 종료자 추가
        // 모든 클라이언트에 메시지 브로드캐스트
        broadcast_message(buffer, client_socket);
    }
}

void handle_client_disconnection(int client_socket) {
    // 다른 클라이언트에 연결 끊김 알림
    char message[BUFFER_SIZE];
    int disconnected_client_index = -1;

    // client_sockets 배열에서 연결이 끊긴 클라이언트 찾기
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i].socket == client_socket) {
            disconnected_client_index = i;
            break;
        }
    }

    // 연결이 끊긴 클라이언트를 발견한 경우 다른 클라이언트에 알린다.
    if (disconnected_client_index != -1) {
        sprintf(message, "Client socket: %d, username: %shas left.\n", client_sockets[disconnected_client_index].socket, client_sockets[disconnected_client_index].username);
        broadcast_message(message, client_socket);

        // 클라이언트 소켓 닫기
        close(client_socket);

        // 배열에서 클라이언트 제거
        client_sockets[disconnected_client_index].socket = -1;
        strcpy(client_sockets[disconnected_client_index].username, "");

        // set 에서 클라이언트 소켓 제거
        FD_CLR(client_socket, &readfds);
    }
}

void broadcast_message(const char* message, int sender_socket) {
    for (int i = 0; i < client_count; i++) {
        int client_socket = client_sockets[i].socket;
        if (client_socket != -1 && client_socket != sender_socket) {
            if (send(client_socket, message, strlen(message), 0) == -1) {
                perror("Failed to send message");
                exit(EXIT_FAILURE);
            }
        }
    }
    // 서버에서 메시지 출력
    printf("%s\n", message);
}