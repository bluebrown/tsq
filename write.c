#include "main.h"

void *th_write(void *arg)
{
    socket_context_t ctx;
    thread_arg_t *wArg = (thread_arg_t *)arg;
    socket_context_t ctx_list[100];
    while (1)
    {
        tsq_dequeue(wArg->writer_queue, &ctx);
        printf("message from socket: %i\nmessage:\n%s\n", ctx.sockfd, ctx.recv);
        close(ctx.sockfd);
        free(ctx.recv);
    }
}