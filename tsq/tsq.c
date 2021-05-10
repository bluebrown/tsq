#include "tsq.h"
#include <string.h>

void tsq_init_struct(tsq_t *tsq, int varsize)
{
    pthread_cond_t c = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

    tsq->ready = c;
    tsq->mutex = m;
    tsq->sig_close = 0;

    tsq->queue = malloc(sizeof(queue_t));
    tsq->queue->head = NULL;
    tsq->queue->tail = NULL;
    tsq->queue->memsize = sizeof(queue_t);
    tsq->queue->nodesize = sizeof(node_t);
    tsq->queue->varsize = varsize;
}

int tsq_enqueue(tsq_t *tsq, QUEUE_TYPE value)
{
    if (tsq->sig_close)
        // cannot write to closed queue
        return -1;
    node_t *new_node = malloc(tsq->queue->nodesize);
    if (new_node == NULL)
        // cannot allocate more memory
        return -1;
    new_node->val = value;
    new_node->nxt = NULL;
    pthread_mutex_lock(&tsq->mutex);
    if (tsq->queue->head == NULL)
    {
        tsq->queue->head = new_node;
        tsq->queue->tail = new_node;
    }
    else
    {
        tsq->queue->tail->nxt = new_node;
        tsq->queue->tail = new_node;
    }
    pthread_cond_signal(&tsq->ready);
    pthread_mutex_unlock(&tsq->mutex);
    return 0;
}

int tsq_dequeue(tsq_t *tsq, QUEUE_TYPE result)
{
    node_t *shifted;
    pthread_mutex_lock(&tsq->mutex);
    while (tsq->queue->head == NULL && tsq->sig_close != 1)
        pthread_cond_wait(&tsq->ready, &tsq->mutex);
    if (tsq->queue->head == NULL)
    {
        pthread_cond_signal(&tsq->ready);
        pthread_mutex_unlock(&tsq->mutex);
        // queue is closed and no more data to read
        return -1;
    }
    shifted = tsq->queue->head;
    tsq->queue->head = shifted->nxt;
    if (tsq->queue->head == NULL)
        tsq->queue->tail = NULL;
    pthread_mutex_unlock(&tsq->mutex);
    memcpy(result, shifted->val, tsq->queue->varsize);
    free(shifted);
    return 0;
}

void tsq_close(tsq_t *tsq)
{
    pthread_mutex_lock(&tsq->mutex);
    tsq->sig_close = 1;
    pthread_cond_signal(&tsq->ready);
    pthread_mutex_unlock(&tsq->mutex);
}