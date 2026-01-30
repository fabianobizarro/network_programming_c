#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <protocol.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 2
#define BACKLOG 0
#define PORT 8080

clientstate_t clientStates[MAX_CLIENTS];

void
init_clients ()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        {
            clientStates[i].fd = -1; // free slot
            clientStates[i].state = STATE_NEW;
            memset (&clientStates[i].buffer, '\0', BUFFER_SIZE);
        }
}

int
find_free_slot ()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clientStates[i].fd == -1)
                return i;
        }

    return -1; // no free slot
}

int
find_slot_by_fd (int fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clientStates[i].fd == fd)
                return i;
        }

    return -1; // not found
}

int
main (int argc, char const *argv[])
{
    int listen_fd, conn_fd, freeSlot;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof (client_addr);

    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1; // number of current fds being tracked
    int opt = 1;

    init_clients ();

    listen_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
        {
            perror ("socket");
            return -1;
        }

    if (setsockopt (listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)))
        {
            perror ("setsockopt");
            return -1;
        }

    memset (&server_addr, 0, sizeof (server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons (PORT);

    if (bind (listen_fd, (struct sockaddr *)&server_addr, sizeof (server_addr))
        == -1)
        {
            perror ("bind");
            return -1;
        }

    if (listen (listen_fd, 10) == -1)
        {
            perror ("listen");
            return -1;
        }

    printf ("Server running on port %d\n", PORT);

    // adding the listeing fd
    memset (fds, 0, sizeof (fds));
    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;
    nfds = 1;

    while (1)
        {

            int ii = 1; // offset by 1 for listen_fd
            for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (clientStates[i].fd != -1)
                        {
                            fds[ii].fd = clientStates[i].fd;
                            fds[ii].events = POLLIN;
                            ii++;
                        }
                }

            int n_events = poll (fds, nfds, -1); // -1 meas no timeout
            if (n_events == -1)
                {
                    perror ("poll");
                    return -1;
                }

            // check for new connections
            // listen_fd
            if (fds[0].revents & POLLIN)
                {
                    if ((conn_fd
                         = accept (listen_fd, (struct sockaddr *)&client_addr,
                                   &client_len))
                        == -1)
                        {
                            perror ("accept");
                            return -1;
                        }

                    printf ("New connection from %s:%d\n",
                            inet_ntoa (client_addr.sin_addr),
                            ntohs (client_addr.sin_port));

                    freeSlot = find_free_slot ();
                    if (freeSlot == -1)
                        {
                            printf ("Server full: closing new connection.\n");
                            close (conn_fd);
                        }
                    else
                        {
                            clientStates[freeSlot].fd = conn_fd;
                            clientStates[freeSlot].state = STATE_CONNECTED;
                            nfds++;
                            printf ("Slot %d has fd %d\n", freeSlot,
                                    clientStates[freeSlot].fd);
                        }

                    n_events--;
                }

            /**
             * check each client for read/write activity
             * offset by 1 for the listen_fd
             */
            for (int i = 1; i <= nfds & n_events > 0; i++)
                {

                    if (fds[i].revents & POLLIN)
                        {
                            n_events--;

                            int fd = fds[i].fd;
                            int slot = find_slot_by_fd (fd);
                            ssize_t bytes_read
                                = read (fd, &clientStates[slot].buffer,
                                        sizeof (clientStates[slot].buffer));
                            if (bytes_read <= 0)
                                {
                                    // connection closed or error
                                    close (fd);
                                    if (slot == -1)
                                        {
                                            printf ("Tried to close fd that "
                                                    "doesn't exist?\n");
                                        }
                                    else
                                        {
                                            clientStates[slot].fd = -1;
                                            clientStates[slot].state
                                                = STATE_DISCONNECTED;
                                            printf ("Client disconnected or "
                                                    "error\n");
                                            nfds--;
                                        }
                                }
                            else
                                {
                                    printf ("Received data from client: %s\n",
                                            clientStates[slot].buffer);
                                }
                        }
                }
        }

    return 0;
}
