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
#include <errno.h>

/* posix */
#include <pthread.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


typedef void *(*pthread_start_t)(void *);


int
threadpool_init(struct threadpool *tp, struct pcaio_config *c,
        worker_t starter, struct taskqueue *q) {
    int err;
    pthread_t *thrds;

    if (tp == NULL) {
        return -1;
    }

    thrds = calloc(c->workers_max, sizeof(pthread_t));
    if (thrds == NULL) {
        return -1;
    }

    err = pthread_mutex_init(&tp->mutex, NULL);
    if (err) {
        free(thrds);
        errno = err;
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

    pthread_mutex_destroy(&tp->mutex);
    if (tp->threads) {
        free(tp->threads);
    }

    return 0;
}


static int
_scaleup(struct threadpool *tp, unsigned short count) {
    int err;
    int ret = 0;
    int i;

    if (tp == NULL) {
        return -1;
    }

    pthread_mutex_lock(&tp->mutex);
    for (i = 0; i < count; i++) {
        err = pthread_create(&tp->threads[tp->count + i], NULL,
                    (pthread_start_t)tp->starter, tp->taskq);
        if (err) {
            errno = err;
            ret = -1;
            goto done;
        }
        atomic_fetch_add(&tp->count, 1);
    }

done:
    pthread_mutex_unlock(&tp->mutex);
    return ret;
}


static int
_scaledown(struct threadpool *tp, unsigned short count) {
    void *status;
    int ret = 0;
    int err;
    int i;
    int target;

    if (tp == NULL) {
        return -1;
    }

    if (count > tp->count) {
        return -1;
    }

    pthread_mutex_lock(&tp->mutex);

    /* send cancel request */
    target = tp->count - count;
    for (i = tp->count - 1; i >= target; i--) {
        err = pthread_cancel(tp->threads[i]);
        if (err) {
            errno = err;
            ret = -1;
            goto done;
        }
    }

    /* wait for them */
    for (i = tp->count - 1; i >= target; i--) {
        err = pthread_join(tp->threads[i], &status);
        if (err) {
            errno = err;
            ret = -1;
            goto done;
        }

        if (status == PTHREAD_CANCELED) {
            INFO("thread: %lu has been canceled successfully.",
                    pthread_self());
        }
        else {
            INFO("thread: %lu exitted with status: %ld", pthread_self(),
                    (long)status);
        }

        atomic_fetch_sub(&tp->count, 1);
    }

done:
    pthread_mutex_unlock(&tp->mutex);
    return ret;
}


int
threadpool_scale(struct threadpool *tp, unsigned short count) {
    if (tp == NULL) {
        return -1;
    }

    if (tp->count > count) {
        return _scaledown(tp, tp->count - count);
    }

    if (tp->count < count) {
        return _scaleup(tp, count - tp->count);
    }

    return 0;
}
