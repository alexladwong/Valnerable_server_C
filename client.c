#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main(void)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    const char *message = "Hello, Server!\n";
    ssize_t bytes_read;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation error");
        return EXIT_FAILURE;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        close(sock);
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        close(sock);
        return EXIT_FAILURE;
    }

    bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read failed");
        close(sock);
        return EXIT_FAILURE;
    }

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Server: %s", buffer);
    }

    if (send(sock, message, strlen(message), 0) < 0) {
        perror("send failed");
        close(sock);
        return EXIT_FAILURE;
    }

    bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read failed");
        close(sock);
        return EXIT_FAILURE;
    }

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Server reply: %s", buffer);
    }

    close(sock);
    return EXIT_SUCCESS;
}
    