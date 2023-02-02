#include <stdlib.h>
#include "./thread_pool.h"
#include "../testing_h/logger.h"

int task_queue_front(task_queue_t *queue, task_t *ret)
{
    if (ret == NULL) {
        lprintf(LOG_ERROR, "Thread pool is returning a task to a NULL pointer\n");
        return 0;
    }

    int r = 0;
    pthread_mutex_lock(&queue->lock);
    if (queue->head == NULL) {
        goto cleanup;
    }

    // Get the head of the queue
    task_node_t *node = queue->head;

    // Sanity check that a valid node was found
    if (node == NULL) {
        lprintf(LOG_ERROR, "Thread pool is empty\n");
        goto cleanup;
    }
    // Update the head of the queue
    queue->head = queue->head->next;

    // Update tail if the queue is now empty
    if (queue->tail == node) {
        queue->tail = NULL;
    }

    // free the old node and, return the value
    task_t task = node->payload;
    free(node);
    *ret = task;

    r = 1;

cleanup:
    pthread_mutex_unlock(&queue->lock);
    return r;
}

int task_queue_enque(task_queue_t *queue, task_t task)
{

}
