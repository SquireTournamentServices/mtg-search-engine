#pragma once
#include <mse/mse.h>
#include <mse/search.h>
#include <pthread.h>
#include <time.h>

#define MSE_PAGE_SIZE (13 * 4)

typedef struct mse_query_params_t {
    int page_number;
    mse_search_sort_type_t sort;
    int sort_asc;
} mse_query_params_t;

typedef struct mse_query_t {
    mse_query_params_t params;
    mse_t *mse;
    /// Query i.e: "set:m20 and type:goblin"
    char *query;
    /// JSON response of the query
    char *resp;
    struct timespec start, stop;
    int query_time_us;
} mse_query_t;

typedef struct mse_async_query_t {
    int ref_count;
    int ready;
    int err;
    pthread_mutex_t lock;
    mse_query_t mse_query;
} mse_async_query_t;

/// Inits and starts a query, sets the ref count to 2 (1 for the caller and 1
/// for the worker) Query now belongs to the returned objected (if it is not
/// null)
mse_async_query_t *mse_start_async_query(char *query, mse_query_params_t params,
        mse_t *mse);

/// Polling function to see if the query has completed
/// If it has finished then you can now return the resp from the query to the
/// client then decref, otherwise you should poll again soon.
int mse_async_query_poll(mse_async_query_t *query);

/// Decreases the reference count for the query and frees it is the ref count is
/// 0 or less
void mse_async_query_decref(mse_async_query_t *query);

int mse_query(mse_query_t *query);
void mse_query_free(mse_query_t query);
