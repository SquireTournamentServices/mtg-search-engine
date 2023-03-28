#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "./thread_pool.h"
#include "../testing_h/testing.h"

#ifdef __WIN32
#include <win32.h>
#endif

#ifdef __unix
#include <unistd.h>
#endif

static int __task_queue_front(task_queue_t *queue, task_t *ret)
{
    // Lock the queue
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
    if (queue->head == NULL) {
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

int task_queue_front(task_queue_t *queue, task_t *ret)
{
    ASSERT(ret != NULL);
    ASSERT(queue != NULL);

    // Get the semaphore and, wait for some data
    sem_wait(&queue->semaphore);
    return __task_queue_front(queue, ret);
}

int task_queue_try_front(task_queue_t *queue, task_t *ret)
{
    ASSERT(ret != NULL);
    ASSERT(queue != NULL);

    // Get the semaphore and, wait for some data
    if (sem_trywait(&queue->semaphore) != 0) {
        return 0;
    }
    return __task_queue_front(queue, ret);
}

int task_queue_enqueue(task_queue_t *queue, task_t task)
{
    ASSERT(queue != NULL);

    // Create new task node
    task_node_t *node = malloc(sizeof * node);
    ASSERT(node != NULL);
    node->next = NULL;
    node->payload = task;

    // Lock the queue
    pthread_mutex_lock(&queue->lock);

    // Add to the end of the queue
    if (queue->tail == NULL) {
        queue->head = queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }

    sem_post(&queue->semaphore);
    pthread_mutex_unlock(&queue->lock);
    return 1;
}

void pool_try_consume(thread_pool_t *pool)
{
    task_t task;
    if (task_queue_try_front(&pool->queue, &task)) {
        task.exec_func(task.data, pool);
    }
}

static void pool_consume(thread_pool_t *pool)
{
    task_t task;
    if (task_queue_front(&pool->queue, &task)) {
        task.exec_func(task.data, pool);
    } else {
        if (pool->running) {
            lprintf(LOG_ERROR, "Cannot consume from the thread pool\n");
        }
    }
}

static void *thread_pool_consumer_func(void *pool_raw)
{
    thread_pool_t *pool = (thread_pool_t *) pool_raw;
    while (pool->running) {
        pool_consume(pool);
    }
    pthread_exit(NULL);
    return NULL;
}

int init_queue(task_queue_t *queue)
{
    ASSERT(queue != NULL);

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    queue->lock = lock;
    queue->head = queue->tail = NULL;
    ASSERT(sem_init(&queue->semaphore, 0, 0) == 0);
    return 1;
}

void reset_pool(task_queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    size_t cnt = 0;

    // Free all of the nodes
    while (queue->head != NULL) {
        task_node_t *node = queue->head;
        queue->head = queue->head->next;
        free(node);
        cnt++;
    }

    if (cnt > 0) {
        lprintf(LOG_WARNING, "%ld tasks are left un-executed\n", cnt);
    }

    // Mark the queue as empty
    queue->head = queue->tail = NULL;
    pthread_mutex_unlock(&queue->lock);
}

int init_pool(thread_pool_t *p)
{
    ASSERT(p != NULL);

#ifdef __WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int cpus = sysinfo.dwNumberOfProcessors;
#endif

#ifdef __unix
    int cpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif

    lprintf(LOG_INFO, "Created a thread pool with %d workers\n", cpus);

    ASSERT(init_queue(&p->queue));
    p->running = 1;
    p->threads_count = cpus;
    p->threads = malloc(sizeof * p->threads * p->threads_count);
    for (size_t i = 0; i < p->threads_count; i++) {
        ASSERT(pthread_create(&p->threads[i], NULL, &thread_pool_consumer_func, (void *) p) == 0);
    }

    return 1;
}

int free_pool(thread_pool_t *p)
{
    lprintf(LOG_INFO, "Stopping all worker threads\n");
    ASSERT(p != NULL);

    // Local alias for the queue
    task_queue_t *queue = &p->queue;
    p->running = 0;

    // Empty the queue
    reset_pool(queue);

    // Wake up all the threads as the queue will be empty they should fail soon
    for (size_t i = 0; i < p->threads_count; i++) {
        sem_post(&queue->semaphore);
    }

    // Slaughter all threads
    for (size_t i = 0; i < p->threads_count; i++) {
        void *__ret;
        ASSERT(pthread_join(p->threads[i], &__ret) == 0);
    }

    // Clear up the IPC  stuff
    pthread_mutex_destroy(&queue->lock);
    sem_destroy(&queue->semaphore);

    free(p->threads);
    memset(p, 0, sizeof * p);
    return 1;
}
