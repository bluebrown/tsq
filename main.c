#include "main.h"

int main(int argc, char const *argv[])
{
    tsq_t reader_queue, writer_queue;
    pthread_t reader_thread, writer_thread;
    int sockfd, new_fd, sin_size;
    struct sockaddr_in server, client;

    tsq_init_struct(&reader_queue, sizeof(socket_context_t));
    tsq_init_struct(&writer_queue, sizeof(socket_context_t));

    pthread_create(&reader_thread, NULL, th_read, &(thread_arg_t){&reader_queue, &writer_queue});
    pthread_create(&writer_thread, NULL, th_write, &(thread_arg_t){&reader_queue, &writer_queue});

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    return tcp_listen(&server, &reader_queue);
}

