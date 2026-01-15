#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 0
#define PORT 5555

int
main (int argc, char const *argv[])
{
    int fd;
    unsigned  clientSize = 0;
    struct sockaddr_in serverAddress = { 0 };
    struct sockaddr_in clientAddress = { 0 };
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons (PORT);

    fd = socket (AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        {
            perror ("socket");
            return -1;
        }

    if (bind (fd, (struct sockaddr *)&serverAddress, sizeof (serverAddress))
        == -1)
        {
            perror ("bind");
            close (fd);
            return -1;
        }

    if (listen (fd, BACKLOG) == -1)
        {
            perror ("listen");
            close (fd);
            return -1;
        }

    puts ("listening at 0.0.0.0:5555 ...");
    int cfd = accept (fd, (struct sockaddr *)&clientAddress, &clientSize);
    if (cfd == -1)
        {
            perror ("accept");
            close (fd);
            return -1;
        }

    close (cfd);

    return 0;
}
