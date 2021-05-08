#include <stdio.h>
#include "tsq/tsq.h"

void *produce(void *arg);
void *consume(void *arg);

typedef struct thread_arg
{
    tsq_t *tsq;
    char *task;
} thread_arg_t;

int main(int argc, char const *argv[])
{
    tsq_t tsq;
    tsq_init_struct(&tsq);

    pthread_t consumer1, consumer2, consumer3, producer1, producer2;

    pthread_create(&consumer1, NULL, consume, &(thread_arg_t){&tsq, "consumer 1"});
    pthread_create(&consumer2, NULL, consume, &(thread_arg_t){&tsq, "consumer 2"});
    pthread_create(&consumer3, NULL, consume, &(thread_arg_t){&tsq, "consumer 3"});
    pthread_create(&producer1, NULL, produce, &(thread_arg_t){&tsq, "producer 1"});
    pthread_create(&producer2, NULL, produce, &(thread_arg_t){&tsq, "producer 2"});

    pthread_join(producer1, NULL);
    pthread_join(producer2, NULL);
    printf("producer threads returned\n");

    printf("close queue\n");
    tsq_close(&tsq);

    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
    pthread_join(consumer3, NULL);
    printf("consumer threads returned\n");

    return 0;
}

void *produce(void *arg)
{
    thread_arg_t *ta = (thread_arg_t *)arg;
    tsq_t *tsq = ta->tsq;
    char *task = ta->task;
    int i = 1;
    printf("[%s] start\n", task);
    for (int i = 1; i < 4; i++)
    {
        // returns -1 on error or closed queue
        if (tsq_enqueue(tsq, i) == -1)
            return NULL;
    }
    printf("[%s] done\n", task);
    return NULL;
}

void *consume(void *arg)
{
    thread_arg_t *ta = (thread_arg_t *)arg;
    tsq_t *tsq = ta->tsq;
    char *task = ta->task;
    int result, ok;
    printf("[%s] start\n", task);
    while (ok != -1)
    {
        // returns -1 if queue is closed and
        // no values more to read
        ok = tsq_dequeue(tsq, &result);
        sleep(2);
        printf("[%s] value received: %i\n", task, result);
    }
    printf("[%s] done, queue closed\n", task);
    return NULL;
}
