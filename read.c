#include "main.h"

void *th_read(void *arg)
{
    thread_arg_t *rArg = (thread_arg_t *)arg;
    int head_buf_size = 5;
    int bytes_received, rv;

    while (1)
    {
        bytes_received = 0;
        char head_buf[head_buf_size];
        memset(head_buf, 0, head_buf_size);
        // if this is allocated only once
        socket_context_t ctx;
        tsq_dequeue(rArg->reader_queue, &ctx);
        printf("reading from socket: %i\n", ctx.sockfd);

        // first fill the header buffer
        while (1)
        {
            rv = read(ctx.sockfd, head_buf + bytes_received, head_buf_size - bytes_received - 1);
            if (rv < 1)
                break;
            bytes_received += rv;
            head_buf[bytes_received] = '\n';
        }

        char *ptr;
        long expected_length;
        // parse header and check if its valid
        printf("header:\n%s\n", head_buf);
        expected_length = strtol(head_buf, &ptr, 10);
        printf("parsed: %ld - %s", expected_length, ptr);

        // then create body buffer of size specified in header
        ctx.recv = (char *)malloc(expected_length + 1);
        memset(ctx.recv, 0, expected_length + 1);

        // then read until buffer is full or timeout is reached
        bytes_received = 0;
        while (1)
        {
            rv = read(ctx.sockfd, ctx.recv + bytes_received, expected_length - bytes_received);
            if (rv < 1)
                break;
            bytes_received += rv;
            ctx.recv[bytes_received] = '\n';
        }
        perror("read");
        tsq_enqueue(rArg->writer_queue, &ctx);
    }
}