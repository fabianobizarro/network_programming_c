#include <arpa/inet.h>
#include <netinet/in.h>
#include <protocol.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 0
#define PORT 5555

void
handle_client (int fd)
{
    char buf[4096] = { 0 };

    read (fd, buf, sizeof (proto_hdr_t) + sizeof (int));

    proto_hdr_t *hdr = buf;
    hdr->type = ntohl (hdr->type);
    hdr->len = ntohs (hdr->len);

    int *data = &hdr[1];
    *data = ntohl (*data);

    if (hdr->type != PROTO_HELLO)
        {
            printf ("Protocol mismatch, failing.\n");
            return;
        }

    if (*data != 1)
        {
            printf ("Protocol version mismatch, failing.\n");
            return;
        }

    scanf ("Connection done successfully! Press any key to finish...");
}

int
main (int argc, char const *argv[])
{
    if (argc != 2)
        {
            printf ("Usage: %s <ip or host>\n", argv[0]);
            return -1;
        }

    int fd;
    int clientSize = 0;
    struct sockaddr_in serverAddress = { 0 };

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr (argv[1]);
    serverAddress.sin_port = htons (PORT);

    fd = socket (AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        {
            perror ("socket");
            return -1;
        }

    if (connect (fd, (struct sockaddr *)&serverAddress, sizeof (serverAddress))
        == -1)
        {
            perror ("connect");
            close (fd);
            return -1;
        }

    handle_client (fd);
    
    return 0;
}
