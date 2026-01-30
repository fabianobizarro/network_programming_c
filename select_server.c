#include <sys/select.h>
#include <stdio.h>
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
    fd_set readfds;
    int listen_fd, fd1, fd2, nfds, conn_fd = 0;
    int client_len;
    struct sockaddr_in serverAddress = {0};
    struct sockaddr_in clientAddress = {0};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    fd1 = socket(AF_INET, SOCK_STREAM, 0);
    fd2 = socket(AF_INET, SOCK_STREAM, 0);

    nfds = (fd1 > fd2 ? fd1 : fd2) + 1;

    if (bind(listen_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("bind");
        return -1;
    }

    if (listen(listen_fd, BACKLOG) == -1)
    {
        perror("listen");
        close(fd1);
        return -1;
    }

    puts("listening at 0.0.0.0:5555 ...");

    printf("%i\n", fd1);
    printf("%i\n", fd2);
    printf("%i\n", listen_fd);
    printf("%i\n", nfds);

    while (1)
    {

        FD_ZERO(&readfds);
        // FD_SET(fd1, &readfds);
        // FD_SET(fd2, &readfds);

        FD_SET(listen_fd, &readfds);
        // nfds = listen_fd + 1;

        // Wait for an input on either socket
        if (select(nfds, &readfds, NULL, NULL, NULL) > 0)
        {
            if (FD_ISSET(fd1, &readfds))
            {
                if ((conn_fd = accept(listen_fd, (struct sockaddr *)&clientAddress, &client_len)) == -1)
                {
                    perror("accept");
                    continue;
                }

                printf("New connection from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

                // Handle input on fd1
            }
            if (FD_ISSET(fd2, &readfds))
            {
                // Handle input on fd2
            }
        }
        else
        {
            perror("select");
            break;
            // Handle timeout or error
        }
    }

    // while (1)
    // {
    // }

    return 0;
}
