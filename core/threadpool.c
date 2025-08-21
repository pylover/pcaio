// Copyright 2025 Vahid Mardani
/*
 * This file is part of pcaio.
 *  pcaio is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  pcaio is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with pcaio. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
/* standard */
#include <string.h>
#include <stdlib.h>

/* posix */
#include <pthread.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


int
threadpool_init(struct threadpool *tp, struct pcaio_config *c,
        worker_t starter, struct taskqueue *q) {
    pthread_t *thrds;

    thrds = calloc(tp->min, sizeof(pthread_t));
    if (thrds == NULL) {
        return -1;
    }

    tp->threads = thrds;
    tp->starter = starter;
    tp->taskq = q;
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

    if (tp->count) {
        return -1;
    }

    if (tp->threads) {
        free(tp->threads);
    }

    return 0;
}


static void
_fire(struct threadpool *tp, size_t count) {
    // TODO: implement
}


int
threadpool_cancelall(struct threadpool *tp) {
    // TODO: implement
    return -1;
}


int
threadpool_tune(struct threadpool *tp, unsigned short count) {
    int i;
    unsigned short backup;

    if (tp == NULL) {
        return -1;
    }

    backup = tp->count;
    if (tp->count > count) {
        _fire(tp, tp->count - count);
        return 0;
    }

    for (i = tp->count; i < count; i++) {
        if (pthread_create(&tp->threads[i], NULL,
                    (void*(*)(void*))tp->starter, &tp->taskq)) {
            ERROR("pthread_create");
            goto rollback;
        }
        atomic_fetch_add(&tp->count, 1);
    }

    return 0;

rollback:
    if (threadpool_tune(tp, backup)) {
        ERROR("rollback: threadpool_tune");
    }
    return -1;
}
