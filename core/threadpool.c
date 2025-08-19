/* standard */
#include <string.h>
#include <stdlib.h>

/* local private */
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


int
threadpool_init(struct threadpool *p, struct pcaio_config *c,
        thread_start_t starter, void *arg) {
    thread_t *t;

    t = malloc(sizeof(thread_t *) * c->workers_max);
    if (t == NULL) {
        return -1;
    }

    p->threads = t;
    p->starter = starter;
    p->starter_arg = arg;
    p->min = c->workers_min;
    p->max = c->workers_max;
    p->count = 0;
    return 0;
}


int
threadpool_deinit(struct threadpool *p) {
    if (p == NULL) {
        return -1;
    }

    if (p->threads) {
        free(p->threads);
    }

    memset(p, 0, sizeof(struct threadpool));
    return -1;
}


int
threadpool_waitall(struct threadpool *p) {
    // TODO: wait for all threads
    return -1;
}


int
threadpool_startall(struct threadpool *p) {
    // TODO: start all threads
    return -1;
}
