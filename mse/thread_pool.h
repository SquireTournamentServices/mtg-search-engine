#pragma once
#include <pthread.h>
#include <stddef.h>
#include <semaphore.h>
/**
 * The thread pool is a set of threads and a queue, tasks are added to the queue and, then executed
 * when the threads are woken up by a mutex.
 **/

struct mse_thread_pool_t; // Forward declare.

/// This represents a task to execute with the thread pool
typedef struct mse_task_t {
    /// This is a user defined data for the task, it is allocated by the user
    void *data;
    /// This function is called when the task is at the front of the queue
    void (*exec_func)(void *data, struct mse_thread_pool_t *pool);
} mse_task_t;

typedef struct mse_task_node_t {
    mse_task_t payload;
    struct mse_task_node_t *next;
} mse_task_node_t;

typedef struct mse_task_queue_t {
    /// This is a lock used to allow for thread safe operation on the queue
    pthread_mutex_t lock;
    /// This is a counter semaphore used to wake threads up when there are jobs to do
    sem_t semaphore;
    mse_task_node_t *head;
    mse_task_node_t *tail;
} mse_task_queue_t;

/// Inits a queue
/// There is no free as that is done by the pool to make sure that consumers are killed
int mse_init_queue(mse_task_queue_t *queue);

/// Takes the front of the queue and removes it, places it into ret.
/// 0 on failure
///
/// This is a blocking call. The caller will wait until the semaphore is non-zero
/// then it will lock the mutex and, grab the first task as described.
int mse_task_queue_front(mse_task_queue_t *queue, mse_task_t *ret);

/// This will try to get the front of the queue, this method is the same as mse_task_queue_front, except
/// 1. it is non-blocking
/// 2. it returns an error if the queue is empty
int mse_task_queue_try_front(mse_task_queue_t *queue, mse_task_t *ret);

/// Inserts task into the queue
/// 0 on failure
///
/// This is a blocking call. The caller will be locked whilst the queue is locked.
/// This will wake up any waiting threads.
int mse_task_queue_enqueue(mse_task_queue_t *queue, mse_task_t task);

/// A greedy implementation of mse_task_queue_enqueue that adds the task to the
/// start of the queue, this is intended for use within tasks spawned by a query.
/// This means that queries are resolved faster.
int mse_task_queue_greedy_enqueue(mse_task_queue_t *queue, mse_task_t task);

/// This will reset the task queue, it is used for freeing the pool safely by cancelling
/// all pending tasks
void mse_reset_pool(mse_task_queue_t *queue);

typedef struct mse_thread_pool_t {
    size_t threads_count;
    pthread_t *threads;
    mse_task_queue_t queue;
    int running;
} mse_thread_pool_t;

/// Starts some consumer thrads and, inits an empty pool
int mse_init_pool(mse_thread_pool_t *p);

/// Frees the pool and, kills all consumer threads
int mse_free_pool(mse_thread_pool_t *p);

/// This is a method that a thread should call whilst it is waiting for other things in the thread pool,
/// it should be polled such that the logic is similar to this:
/// while (waiting_for_task()) pool_consume();
void mse_pool_try_consume(mse_thread_pool_t *p);
