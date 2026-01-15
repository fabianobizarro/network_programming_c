#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 0
#define PORT 5555

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <ip or host>\n", argv[0]);
        return -1;
    }

    int fd;
    int clientSize = 0;
    struct sockaddr_in serverAddress = {0};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(PORT);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }

    if (connect(fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("connect");
        close(fd);
        return -1;
    }

    return 0;
}
