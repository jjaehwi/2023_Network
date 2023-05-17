#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 2048

int client_socket;
char username[256];

// 서버로부터 메시지를 수신하는 스레드를 위한 함수
void* receive_message(void* arg);
// 서버에 메시지를 보내는 함수
void send_message(const char* message);

int main() {
    struct sockaddr_in server_address;

    // 클라이언트 소켓 생성
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create client socket");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정 (Loopback), 포트번호 9020
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9020);
    if (inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr)) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // 채팅 서버에 연결
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Failed to connect to the server");
        exit(EXIT_FAILURE);
    }

    // username 입력 
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  // 후행 개행 문자 제거

    // 메시지를 받을 스레드 생성
    pthread_t thread;
    if (pthread_create(&thread, NULL, receive_message, NULL) != 0) {
        perror("Failed to create receive thread");
        exit(EXIT_FAILURE);
    }
    
    printf("Welcome, %s!\n", username);
    printf("You can start sending messages.\n");

    while (1) {
        // select 을 위한 파일 디스크립터 set 설정
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // select 에 대한 시간 설정
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000; // 10 초

        // select 함수를 통해 stdin 작업을 기다린다.
        int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char message[BUFFER_SIZE];
            fgets(message, BUFFER_SIZE, stdin);
            message[strcspn(message, "\n")] = '\0';

            char formatted_message[BUFFER_SIZE];
            sprintf(formatted_message, "[%s] %s\n", username, message);
            send_message(formatted_message);
        }
    }
    // 클라이언트 소켓 close
    close(client_socket);
    return 0;
}

// 서버로부터 메시지를 수신하는 스레드를 위한 함수
void* receive_message(void* arg) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        if (strncmp(buffer, username, strlen(username)) != 0) {
            printf("%s\n", buffer);
            fflush(stdout); // stdout 을 fflust 를 통해 프롬프트가 표시되는지 확인
        }
    }
    return NULL;
}

// 서버에 메시지를 보내는 함수
void send_message(const char* message) {
    if (send(client_socket, message, strlen(message), 0) == -1)
    {
        perror("Failed to send message");
        exit(EXIT_FAILURE);
    }
}
