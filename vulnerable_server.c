#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define PORT 8080
#define BUFFER_SIZE 1024

static void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    const char *welcome = "Welcome to the vulnerable server!\n";
    ssize_t received;

    if (send(client_socket, welcome, strlen(welcome), 0) < 0) {
        perror("send welcome failed");
        close(client_socket);
        return;
    }

    received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (received < 0) {
        perror("recv failed");
        close(client_socket);
        return;
    }

    if (received == 0) {
        printf("Client disconnected before sending data.\n");
        close(client_socket);
        return;
    }

    buffer[received] = '\0';
    printf("Received: %s\n", buffer);

    const char *goodbye = "Goodbye!\n";
    if (send(client_socket, goodbye, strlen(goodbye), 0) < 0) {
        perror("send reply failed");
    }

    close(client_socket);
}

int main(void)
{
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);
    printf("This is the practical lab server.\n");

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_fd);
    return 0;
}