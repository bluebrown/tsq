#include "tsq.h"

void init_tsq(tsq_t *tsq)
{
    pthread_cond_t c = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

    tsq->ready = c;
    tsq->mutex = m;
    tsq->sig_close = 0;

    tsq->fifo = malloc(sizeof(fifo_t));
    tsq->fifo->head = NULL;
    tsq->fifo->tail = NULL;
}

int write_to_fifo(tsq_t *tsq, FIFO_TYPE value)
{
    if (tsq->sig_close)
        // cannot write to closed fifo
        return -1;
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
        // cannot allocate more memory
        return -1;
    new_node->val = value;
    new_node->nxt = NULL;
    pthread_mutex_lock(&tsq->mutex);
    if (tsq->fifo->head == NULL)
    {
        tsq->fifo->head = new_node;
        tsq->fifo->tail = new_node;
    }
    else
    {
        tsq->fifo->tail->nxt = new_node;
        tsq->fifo->tail = new_node;
    }
    pthread_cond_signal(&tsq->ready);
    pthread_mutex_unlock(&tsq->mutex);
    return 0;
}

int read_from_fifo(tsq_t *tsq, FIFO_TYPE *result)
{
    node_t *shifted;
    pthread_mutex_lock(&tsq->mutex);
    while (tsq->fifo->head == NULL && tsq->sig_close != 1)
        pthread_cond_wait(&tsq->ready, &tsq->mutex);
    if (tsq->fifo->head == NULL)
    {
        pthread_cond_signal(&tsq->ready);
        pthread_mutex_unlock(&tsq->mutex);
        // fifo is closed and no more data to read
        return -1;
    }
    shifted = tsq->fifo->head;
    tsq->fifo->head = shifted->nxt;
    if (tsq->fifo->head == NULL)
        tsq->fifo->tail = NULL;
    pthread_mutex_unlock(&tsq->mutex);
    *result = shifted->val;
    free(shifted);
    return 0;
}

void close_fifo(tsq_t *tsq)
{
    pthread_mutex_lock(&tsq->mutex);
    tsq->sig_close = 1;
    pthread_cond_signal(&tsq->ready);
    pthread_mutex_unlock(&tsq->mutex);
}