#include <arpa/inet.h>
#include <float.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 500
unsigned recv_total;

char buffer[BUFFER_SIZE];

void log_and_exit(const char* msg){
    printf("Error on %s", msg);
    exit(0);
}


int main(int argc, char **argv)
{
    struct sockaddr_in servaddr = {0};
    struct sockaddr_in cliaddr = {0};
    int socket_fd1, socket_fd2;
    int ret;

    char server1[10];
    char server2[10];

    sprintf(server1, "%d", atoi(argv[1]));
    sprintf(server2, "%d", atoi(argv[1]) + 1);

    socket_fd1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd1 == -1)
    {
        log_and_exit("socket");
    }
    socket_fd2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd2 == -1)
    {
        log_and_exit("socket");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    servaddr.sin_port = htons(atoi(argv[1]));
    ret = bind(socket_fd1, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret != 0)
    {
        log_and_exit("bind");
    }
    servaddr.sin_port = htons(atoi(argv[1]) + 1);
    ret = bind(socket_fd2, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret != 0)
    {
        log_and_exit("bind");
    }
    
    printf("Waiting connections...\n");

    socklen_t len = 0;

    fd_set rfds;
    FD_ZERO(&rfds);
    struct timeval tv = {1, 0};
    
    while (1)
    {
        int server_rcvd = 0;
        memset(buffer, 0, BUFFER_SIZE);

        FD_SET(socket_fd1, &rfds);
        select(BUFFER_SIZE, &rfds, NULL, NULL, &tv);
        if (FD_ISSET(socket_fd1, &rfds))
        {
            len = sizeof(cliaddr);
            recv_total = recvfrom(socket_fd1, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
            server_rcvd = 1;
            buffer[recv_total] = '\n';
        }

        FD_SET(socket_fd2, &rfds);
        select(BUFFER_SIZE, &rfds, NULL, NULL, &tv);
        if (FD_ISSET(socket_fd2, &rfds))
        {
            len = sizeof(cliaddr);
            recv_total = recvfrom(socket_fd2, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
            server_rcvd = 2;
            buffer[recv_total] = '\n';
        }

        if(server_rcvd != 0){
            printf("Server %d received: %s", server_rcvd, buffer);
            sprintf(buffer, "Server %d received: %s", server_rcvd, buffer);
            sendto(socket_fd1, (const char *)buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        }
    }
}
