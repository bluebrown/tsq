#include "tsq/tsq.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define BACKLOG 10
#define PORT 8080

void *listener_func(void *arg);
void *sender_func(void *arg);
void *receiver_func(void *arg);

typedef struct thread_arg
{
    tsq_t *q_recv;
    tsq_t *q_send;
    int socket;
    struct sockaddr_in addr;
    char *task;
} thread_arg_t;

int main(int argc, char const *argv[])
{
    tsq_t q_recv, q_send;
    tsq_init_struct(&q_recv);
    tsq_init_struct(&q_send);

    int sockfd, new_fd, sin_size;
    struct sockaddr_in server, client;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if ((sockfd = socket(server.sin_family, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return 2;
    }

    pthread_t listener, receiver, sender;

    pthread_create(&listener, NULL, listener_func, &(thread_arg_t){&q_recv, &q_send, sockfd, server, "listener"});
    pthread_create(&receiver, NULL, receiver_func, &(thread_arg_t){&q_recv, &q_send, sockfd, server, "receiver"});
    pthread_create(&sender, NULL, sender_func, &(thread_arg_t){&q_recv, &q_send, sockfd, server, "sender"});

    pthread_join(listener, NULL);

    return 0;
}

void *listener_func(void *arg)
{
    thread_arg_t *ta = (thread_arg_t *)arg;
    tsq_t *q_recv = ta->q_recv;
    tsq_t *q_send = ta->q_send;
    char *task = ta->task;
    int sockfd = ta->socket;
    struct sockaddr_in server = ta->addr;
    struct sockaddr_in client;
    int new_fd, sin_size;

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind");
        return NULL;
    }
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        return NULL;
    }

    while (1)
    {
        sin_size = sizeof client;
        new_fd = accept(sockfd, (struct sockaddr *)&client, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }
        printf("new socket connected: %i\n", new_fd);
        tsq_enqueue(q_recv, new_fd);
    }
}

void *receiver_func(void *arg)
{
    thread_arg_t *ta = (thread_arg_t *)arg;
    tsq_t *q_recv = ta->q_recv;
    tsq_t *q_send = ta->q_send;
    char *task = ta->task;
    int sockfd = ta->socket;
    int client_sockfd, read_size;
    int pageSize = getpagesize();
    char buffer[pageSize];
    char *pBuf;
    int bytesLeft;
    int msg_end;

    while (1)
    {
        tsq_dequeue(q_recv, &client_sockfd);
        printf("[%s] reading from client %i\n", task, client_sockfd);
        memset(buffer, 0, sizeof(buffer));

        pBuf = buffer;
        bytesLeft = sizeof(buffer) - sizeof(char);
        msg_end = 0;

        while (bytesLeft > 0 && msg_end != 1)
        {
            int bytes_read = read(client_sockfd, pBuf, bytesLeft);
            if (bytes_read == 0)
            {
                printf("[%s] EOF from socket %i\n", task, client_sockfd);
                break;
            }
            if (bytes_read == -1)
            {
                printf("[%s]recv failed from socket %i\n", task, client_sockfd);
                break;
            }
            bytesLeft -= bytes_read;
            if (strstr(pBuf, "\r") != NULL)
            {
                printf("[%s] end of head\n%s", task, pBuf);
                // msg_end = 1;
                tsq_enqueue(q_send, client_sockfd);
                memset(buffer, 0, sizeof(buffer));
            }
        }
        if (msg_end == 1)
            tsq_enqueue(q_send, client_sockfd);
    }
}

void *sender_func(void *arg)
{
    thread_arg_t *ta = (thread_arg_t *)arg;
    tsq_t *q_send = ta->q_send;
    char *task = ta->task;
    int sockfd = ta->socket;
    int client_sockfd;
    char msg[] = "Hello, world!\r\n";
    while (1)
    {
        tsq_dequeue(q_send, &client_sockfd);
        if (send(client_sockfd, msg, strlen(msg), 0) == -1)
        {
            perror("send");
            close(client_sockfd);
        }
        printf("[%s] message sent to client %i\n", task, client_sockfd);
    }
}
