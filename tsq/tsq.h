#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct p_context
{
    int *sockfd;
    char *received;
} p_context_t;

#define QUEUE_TYPE p_context_t

typedef struct node
{
    QUEUE_TYPE val;
    struct node *nxt;
} node_t;

typedef struct queue
{
    node_t *head;
    node_t *tail;
} queue_t;

typedef struct tsq
{
    pthread_cond_t ready;
    pthread_mutex_t mutex;
    int sig_close;
    queue_t *queue;
} tsq_t;

void tsq_init_struct(tsq_t *tsq);
int tsq_enqueue(tsq_t *tsq, QUEUE_TYPE value);
int tsq_dequeue(tsq_t *tsq, QUEUE_TYPE *result);
void tsq_close(tsq_t *tsq);
