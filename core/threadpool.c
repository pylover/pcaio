/* standard */
#include <string.h>
#include <stdlib.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


int
threadpool_init(struct threadpool *tp, struct pcaio_config *c,
        thread_start_t starter) {
    struct thread *t;

    t = malloc(sizeof(struct thread) * c->workers_max);
    if (t == NULL) {
        return -1;
    }

    tp->threads = t;
    tp->starter = starter;
    tp->min = c->workers_min;
    tp->max = c->workers_max;
    tp->count = 0;
    return 0;
}


int
threadpool_deinit(struct threadpool *tp) {
    if (tp == NULL) {
        return -1;
    }

    if (tp->threads) {
        free(tp->threads);
    }

    memset(tp, 0, sizeof(struct threadpool));
    return -1;
}


static int
_cancelone(struct threadpool *tp) {
    int status;
    struct thread *th;

    th = &tp->threads[--tp->count];
    atomic_store(&th->cancel, true);
    if (thread_join(th->id, &status)) {
        FATAL("thread_join");
    }

    ERROR("thread: %lu exited with status: %d", th->id, status);
    return status;
}


int
threadpool_cancelall(struct threadpool *tp) {
    while (tp->count) {
        if (_cancelone(tp)) {
            return -1;
        }
    }

    return 0;
}


int
threadpool_tune(struct threadpool *tp, unsigned short count) {
    int i;
    struct thread *t;
    unsigned short backup;

    if (tp == NULL) {
        return -1;
    }

    backup = tp->count;
    while (tp->count > count) {
        _cancelone(tp);
    }

    for (i = tp->count; i < count; i++) {
        t = tp->threads + i;
        if (thread_new(&t->id, tp->starter, &t->cancel)) {
            ERROR("thread_new");
            goto rollback;
        }
        tp->count++;
    }

    return 0;

rollback:
    if (threadpool_tune(tp, backup)) {
        ERROR("rollback: threadpool_tune");
    }
    return -1;
}
