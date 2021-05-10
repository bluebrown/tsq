#include "main.h"

int tcp_listen(struct sockaddr_in *server, tsq_t *handler_queue)
{
    int sockfd, new_fd, sin_size;
    struct sockaddr_in client;
    sin_size = sizeof client;

    if ((sockfd = socket(server->sin_family, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return 2;
    }
    if (bind(sockfd, (struct sockaddr *)server, sizeof(*server)) < 0)
    {
        perror("bind");
        return 2;
    }
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        return 2;
    }
    while (1)
    {
        new_fd = accept(sockfd, (struct sockaddr *)&client, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }
        tsq_enqueue(handler_queue, &(socket_context_t){new_fd});
    }
    return 0;
}