#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "./thread_pool.h"
#include "../testing_h/testing.h"

#ifdef __WINDOWS
#include <windows.h>
#endif

#ifdef __unix
#include <unistd.h>
#endif

static int __task_queue_front(mse_task_queue_t *queue, mse_task_t *ret)
{
    // Lock the queue
    int r = 0;
    pthread_mutex_lock(&queue->lock);
    if (queue->head == NULL) {
        goto cleanup;
    }

    // Get the head of the queue
    mse_task_node_t *node = queue->head;

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
    mse_task_t task = node->payload;
    free(node);
    *ret = task;

    r = 1;

cleanup:
    pthread_mutex_unlock(&queue->lock);
    return r;
}

int mse_task_queue_front(mse_task_queue_t *queue, mse_task_t *ret)
{
    // Get the semaphore and, wait for some data
    sem_wait(&queue->semaphore);
    return __task_queue_front(queue, ret);
}

int mse_task_queue_try_front(mse_task_queue_t *queue, mse_task_t *ret)
{
    // Get the semaphore and, wait for some data
    if (sem_trywait(&queue->semaphore) != 0) {
        return 0;
    }
    return __task_queue_front(queue, ret);
}

int mse_task_queue_enqueue(mse_task_queue_t *queue, mse_task_t task)
{
    // Create new task node
    mse_task_node_t *node = malloc(sizeof * node);
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

int mse_task_queue_greedy_enqueue(mse_task_queue_t *queue, mse_task_t task)
{
    // Create new task node
    mse_task_node_t *node = malloc(sizeof * node);
    ASSERT(node != NULL);
    node->next = NULL;
    node->payload = task;

    // Lock the queue
    pthread_mutex_lock(&queue->lock);

    // Add to the start of the queue
    if (queue->head == NULL) {
        queue->head = queue->tail = node;
    } else {
        node->next = queue->head;
        queue->head = node;
    }

    sem_post(&queue->semaphore);
    pthread_mutex_unlock(&queue->lock);
    return 1;
}

void mse_pool_try_consume(mse_thread_pool_t *pool)
{
    mse_task_t task;
    if (mse_task_queue_try_front(&pool->queue, &task)) {
        task.exec_func(task.data, pool);
    }
}

static void pool_consume(mse_thread_pool_t *pool)
{
    mse_task_t task;
    if (mse_task_queue_front(&pool->queue, &task)) {
        task.exec_func(task.data, pool);
    } else {
        if (pool->running) {
            lprintf(LOG_ERROR, "Cannot consume from the thread pool\n");
        }
    }
}

static void *thread_pool_consumer_func(void *pool_raw)
{
    mse_thread_pool_t *pool = (mse_thread_pool_t *) pool_raw;
    while (pool->running) {
        pool_consume(pool);
    }
    sem_post(&pool->close_semaphore);
    pthread_exit(NULL);
    return NULL;
}

int mse_init_queue(mse_task_queue_t *queue)
{
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    queue->lock = lock;
    queue->head = queue->tail = NULL;
    ASSERT(sem_init(&queue->semaphore, 0, 0) == 0);
    return 1;
}

void mse_reset_pool(mse_task_queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    size_t cnt = 0;

    // Free all of the nodes
    while (queue->head != NULL) {
        mse_task_node_t *node = queue->head;
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

int mse_init_pool(mse_thread_pool_t *p)
{
    // Default count for unsupported platforms
    int cpus = 10;
#ifdef __WINDOWS
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    cpus = sysinfo.dwNumberOfProcessors;
#endif

#ifdef __unix
    cpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif

    lprintf(LOG_INFO, "Created a thread pool with %d workers\n", cpus);

    ASSERT(mse_init_queue(&p->queue));
    ASSERT(sem_init(&p->close_semaphore, 0, 0) == 0);
    p->running = 1;
    p->threads_count = cpus;
    p->threads = malloc(sizeof * p->threads * p->threads_count);
    for (size_t i = 0; i < p->threads_count; i++) {
        ASSERT(pthread_create(&p->threads[i], NULL, &thread_pool_consumer_func, (void *) p) == 0);
        ASSERT(pthread_detach(p->threads[i]) == 0);
    }

    return 1;
}

int mse_free_pool(mse_thread_pool_t *p)
{
    lprintf(LOG_INFO, "Stopping all worker threads...\n");
    ASSERT(p != NULL);

    // Local alias for the queue
    mse_task_queue_t *queue = &p->queue;
    pthread_mutex_lock(&queue->lock);
    p->running = 0;
    pthread_mutex_unlock(&queue->lock);

    // Empty the queue
    mse_reset_pool(queue);

    // Wake up all the threads as the queue will be empty they should fail soon
    for (size_t i = 0; i < p->threads_count; i++) {
        sem_post(&queue->semaphore);
    }

    // Wait for the threads to clean themselves up and report themselves as done
    lprintf(LOG_INFO, "Waiting for threads to terminate...\n");
    for (size_t i = 0; i < p->threads_count; i++) {
        sem_wait(&p->close_semaphore);
    }

    // Clear up the IPC stuff
    pthread_mutex_destroy(&queue->lock);
    sem_destroy(&queue->semaphore);
    sem_destroy(&p->close_semaphore);

    free(p->threads);
    memset(p, 0, sizeof * p);
    lprintf(LOG_INFO, "Thread pool cleaned.\n");
    return 1;
}
