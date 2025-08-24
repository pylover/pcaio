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
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "master.h"
#include "task.h"
#include "context.h"
#include "worker.h"
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


struct master *__master__;


int
master_init(struct pcaio_config *config) {
    struct master *m;

    if (__master__) {
        ERROR("already initialized");
        return -1;
    }

    m = malloc(sizeof(struct master));
    if (m == NULL) {
        return -1;
    }

    if (taskqueue_init(&m->taskq)) {
        free(m);
        return -1;
    }

    if (threadpool_init(&m->pool, config->workers_max, worker, &m->taskq)) {
        taskqueue_deinit(&m->taskq);
        free(m);
        return -1;
    }

    /* threadlocal storages */
    if (threadlocaltask_init(task_free)
            || threadlocalucontext_init(NULL)) {
        threadpool_deinit(&m->pool);
        taskqueue_deinit(&m->taskq);
        free(m);
        return -1;
    }

    m->config = config;
    m->cancel = false;
    m->tasks = 0;
    __master__ = m;
    return 0;
}


int
master_deinit() {
    struct master *m;

    if (__master__ == NULL) {
        return -1;
    }
    m = __master__;

    threadlocaltask_delete();
    threadlocalucontext_delete();
    threadpool_deinit(&m->pool);
    taskqueue_deinit(&m->taskq);
    free(m);
    __master__ = NULL;
    return 0;
}


int
master() {
    struct threadpool *tp;

    /* scale the threadpool to minimum capacity */
    tp = &__master__->pool;
    if (threadpool_scale(tp, __master__->config->workers_min)) {
        ERROR("threadpool_scale");
        return -1;
    }

    while ((!__master__->cancel) && (__master__->tasks)) {
        sleep(.3);
    }

    INFO("shutting down all workers...");
    if (threadpool_scale(tp, 0)) {
        ERROR("threadpool_scale");
        return -1;
    }

    INFO("all workers have been shut down.");
    return 0;
}
