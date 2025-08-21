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

/* thirdparty */
#include <clog.h>

/* local private */
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


int
threadpool_init(struct threadpool *tp, struct pcaio_config *c,
        thread_start_t starter, struct taskqueue *q) {
    tp->starter = starter;
    tp->taskq = q;
    tp->min = c->workers_min;
    tp->max = c->workers_max;
    tp->count = 0;
    return 0;
}


static void
_fire(struct threadpool *tp, size_t count) {
    taskqueue_headpush(tp->taskq, NULL);
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
    thread_t tid;

    if (tp == NULL) {
        return -1;
    }

    backup = tp->count;
    if (tp->count > count) {
        _fire(tp, tp->count - count);
        return 0;
    }

    for (i = tp->count; i < count; i++) {
        if (thread_new(&tid, tp->starter, &tp->taskq)) {
            ERROR("thread_new");
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
