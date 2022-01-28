#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>

#define BUFFER_SIZE 500

int socket_fd;
char buffer[BUFFER_SIZE];
char server[BUFFER_SIZE];

int main(int argc, char **argv)
{
    struct sockaddr_in servaddr = {0};
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1)
    {
        exit(0);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    while(1){
        printf("> Server: ");
        fgets(server, BUFFER_SIZE, stdin);
        printf("> Message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        servaddr.sin_port = htons(atoi(server));
        int ret = sendto(socket_fd, (const char *)buffer, BUFFER_SIZE, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        if (ret == -1)
        {
            exit(0);
        }
        socklen_t len = 0;
        int n = recvfrom(socket_fd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\n';
        printf("< %s", buffer);
    }
    return 0;
}