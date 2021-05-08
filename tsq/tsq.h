#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define FIFO_TYPE int

typedef struct node
{
    FIFO_TYPE val;
    struct node *nxt;
} node_t;

typedef struct fifo
{
    node_t *head;
    node_t *tail;
} fifo_t;

typedef struct tsq
{
    pthread_cond_t ready;
    pthread_mutex_t mutex;
    int sig_stop;
    fifo_t *fifo;
} tsq_t;

void init_tsq(tsq_t *tsq);
int write_to_fifo(tsq_t *tsq, FIFO_TYPE value);
int read_from_fifo(tsq_t *tsq, FIFO_TYPE *result);
void cancel_fifo(tsq_t *tsq);
