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
#include <unistd.h>
// #include <ucontext.h>
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


static struct master *_master;


int
master_init(struct pcaio_config *config) {
    struct master *p;

    if (_master) {
        ERROR("already initialized");
        return -1;
    }

    p = malloc(sizeof(struct master));
    if (p == NULL) {
        return -1;
    }

    if (taskqueue_init(&p->tasks)) {
        free(p);
        return -1;
    }

    if (threadpool_init(&p->pool, p->config, (thread_start_t)worker)) {
        taskqueue_deinit(&p->tasks);
        free(p);
        return -1;
    }

    /* threadlocal storages */
    if (threadlocaltask_init(task_free)
            || threadlocalucontext_init(NULL)) {
        threadpool_deinit(&p->pool);
        taskqueue_deinit(&p->tasks);
        free(p);
        return -1;
    }

    p->cancel = false;
    _master = p;
    return 0;
}


int
master_deinit() {
    struct master *p;

    if (_master == NULL) {
        return -1;
    }
    p = _master;

    threadlocaltask_deinit();
    threadlocalucontext_deinit();
    threadpool_deinit(&p->pool);
    taskqueue_deinit(&p->tasks);
    free(p);
    _master = NULL;
    return 0;
}


void
master_schedule(struct pcaio_task *t) {
    taskqueue_push(&_master->tasks, t);
}


void
master_cancel() {
    // TODO: atomic, threadsafe
    _master->cancel = true;
}


int
master() {
    if (_master == NULL) {
        errno = EINVAL;
        ERROR("call pcaio_init() once before the %s().", __func__);
        return -1;
    }

    if (threadpool_minimum(&_master->pool)) {
        errno = ENOMEM;
        ERROR("threadpool_startall");
        return -1;
    }

    while (!_master->cancel) {
        sleep(1);
    }

    INFO("canceling all workers...");
    return threadpool_cancelall(&_master->pool);
}
