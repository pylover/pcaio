/* standard */
#include <threads.h>

/* thirdparty */
#include <clog.h>

/* local public */
#include "pcaio/mutex.h"


int
mutex_init(mutex_t *m) {
    if (mtx_init(m, mtx_plain) != thrd_success) {
        return -1;
    }

    return 0;
}


void
mutex_deinit(mutex_t *m) {
    mtx_destroy(m);
}


void
mutex_acquire(mutex_t *m) {
    if (mtx_lock(m) != thrd_success) {
        FATAL("mtx_lock");
    }
}


void
mutex_release(mutex_t *m) {
    if (mtx_unlock(m) != thrd_success) {
        FATAL("mtx_lock");
    }
}
