#ifndef _protocol_h
#define _protocol_h

#define BUFFER_SIZE 4096

typedef enum
{
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED
} state_e;

typedef struct
{
    int fd;
    state_e state;
    char buffer[BUFFER_SIZE];
} clientstate_t;

#endif