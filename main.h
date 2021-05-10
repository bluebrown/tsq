#include "tsq/tsq.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#define BACKLOG 10
#define PORT 8080

typedef struct socket_context
{
    int sockfd;
    char *recv;
    char *send;
} socket_context_t;

typedef struct thread_arg
{
    tsq_t *reader_queue;
    tsq_t *writer_queue;

} thread_arg_t;

void *th_read(void *arg);
void *th_write(void *arg);
int tcp_listen(struct sockaddr_in *server, tsq_t *handler_queue);