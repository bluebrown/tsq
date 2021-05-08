#include <stdio.h>
#include "tsq/tsq.h"

void *produce(void *arg)
{
    printf("start producer\n");
    tsq_t *tsq = (tsq_t *)arg;
    int i = 1;
    for (int i = 1; i < 11; i++)
    {
        if (write_to_fifo(tsq, i * 1) == -1)
            return NULL;
        if (write_to_fifo(tsq, i * 2) == -1)
            return NULL;
        if (write_to_fifo(tsq, i * 3) == -1)
            return NULL;
        sleep(1);
    }
    printf("producer done\n");
    return NULL;
}

void *consume(void *arg)
{
    printf("start consumer\n");
    tsq_t *tsq = (tsq_t *)arg;
    int result;
    while (1)
    {
        if (read_from_fifo(tsq, &result) == -1)
        {
            printf("consumer cancelled\n");
            return NULL;
        }
        printf("value received: %i\n", result);
    }
}

int main(int argc, char const *argv[])
{
    tsq_t tsq;
    init_tsq(&tsq);

    pthread_t consumer1, consumer2, consumer3, producer1, producer2;

    pthread_create(&consumer1, NULL, consume, &tsq);
    pthread_create(&consumer2, NULL, consume, &tsq);
    pthread_create(&consumer3, NULL, consume, &tsq);

    pthread_create(&producer1, NULL, produce, &tsq);
    pthread_create(&producer2, NULL, produce, &tsq);
    pthread_join(producer2, NULL);
    printf("producer threads returned\n");

    cancel_fifo(&tsq);
    printf("fifo cancelled\n");

    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
    pthread_join(consumer3, NULL);
    printf("consumer threads returned\n");

    return 0;
}
