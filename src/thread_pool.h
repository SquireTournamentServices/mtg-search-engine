#pragma once
#include <pthread.h>
#include <stddef.h>
#include <semaphore.h>
/**
 * The thread pool is a set of threads and a queue, tasks are added to the queue and, then executed
 * when the threads are woken up by a mutex.
 **/

struct thread_pool_t; // Forward declare.

/// This represents a task to execute with the thread pool
typedef struct task_t {
    /// This is a user defined data for the task, it is allocated by the user
    void *data;
    /// This function is called when the task is at the front of the queue
    void (*exec_func)(void *data, struct thread_pool_t *pool);
} task_t;

typedef struct task_node_t {
    task_t payload;
    struct task_node_t *next;
} task_node_t;

typedef struct task_queue_t {
    /// This is a lock used to allow for thread safe operation on the queue
    pthread_mutex_t lock;
    /// This is a counter semaphore used to wake threads up when there are jobs to do
    sem_t semaphore;
    task_node_t *head;
    task_node_t *tail;
} task_queue_t;

/// Inits a queue
int init_queue(task_queue_t *queue);
// There is no free as that is done by the pool to make sure that consumers are killed

/// Takes the front of the queue and removes it, places it into ret.
/// 0 on failure
///
/// This is a blocking call. The caller will wait until the semaphore is non-zero
/// then it will lock the mutex and, grab the first task as described.
int task_queue_front(task_queue_t *queue, task_t *ret);

/// Inserts task into the queue
/// 0 on failure
///
/// This is a blocking call. The caller will be locked whilst the queue is locked.
/// This will wake up any waiting threads.
int task_queue_enque(task_queue_t *queue, task_t task);

typedef struct thread_pool_t {
    size_t threads_count;
    pthread_t *threads;
    task_queue_t queue;
} thread_pool_t;

/// Starts some consumer thrads and, inits an empty pool
int init_pool(thread_pool_t *p);

/// Frees the pool and, kills all consumer threads
int free_pool(thread_pool_t *p);

/// The definition of the consumer thread
void *thread_pool_consumer_func(void *pool);
