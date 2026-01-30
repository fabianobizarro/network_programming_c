#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <protocol.h>

#define BACKLOG 0
#define PORT 5555

void handle_client(int fd)
{
    char buf[4096] = {0};
    proto_hdr_t *hdr = buf;

    hdr->type = htonl(PROTO_HELLO);
    hdr->len = sizeof(int);

    int reallen = hdr->len;

    hdr->len = htons(hdr->len);

    int *data = &hdr[1];
    *data = htons(1);

    write(fd, hdr, sizeof(hdr) + reallen);
}

int main(int argc, char const *argv[])
{
    int fd;
    unsigned clientSize = 0;
    struct sockaddr_in serverAddress = {0};
    struct sockaddr_in clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }

    if (bind(fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, BACKLOG) == -1)
    {
        perror("listen");
        close(fd);
        return -1;
    }

    puts("listening at 0.0.0.0:5555 ...");

    while (1)
    {

        int cfd = accept(fd, (struct sockaddr *)&clientAddress, &clientSize);
        if (cfd == -1)
        {
            perror("accept");
            close(fd);
            return -1;
        }

        puts("Connection received");

        handle_client(cfd);

        close(cfd);
    }
}
