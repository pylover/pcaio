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
#include <stdarg.h>
#include <ucontext.h>
#include <errno.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "config.h"
#include "core.h"
#include "task.h"
#include "worker.h"
#include "threadpool.h"

/* local public */
#include "pcaio/pcaio.h"


static struct pcaio *_pcaio;


struct pcaio_config *
pcaio_config_default() {
    struct pcaio_config *c;

    c = malloc(sizeof(struct pcaio_config));
    if (c == NULL) {
        return NULL;
    }

    c->workers_min = 1;
    c->workers_max = 1;
    return c;
}


int
pcaio_init(struct pcaio_config *config) {
    struct pcaio *p;

    if (_pcaio) {
        ERROR("already initialized");
        return -1;
    }

    p = malloc(sizeof(struct pcaio));
    if (p == NULL) {
        return -1;
    }

    if (config == NULL) {
        p->config = pcaio_config_default();
    }
    else {
        p->config = config;
    }

    if (taskqueue_init(&p->tasks)) {
        free(p);
        return -1;
    }

    if (threadpool_init(&p->pool, p->config, (thread_start_t)worker)) {
        return -1;
    }

    _pcaio = p;
    return 0;
}


int
pcaio_deinit() {
    if (_pcaio == NULL) {
        return -1;
    }

    taskqueue_deinit(&_pcaio->tasks);
    free(_pcaio);
    _pcaio = NULL;
    return 0;
}


int
pcaio_task_schedule(struct pcaio_task *t) {
    taskqueue_push(&_pcaio->tasks, t);

    return 0;
}


struct pcaio_task *
pcaio_schedule(const char *id, pcaio_entrypoint_t func, int argc, ...) {
    va_list args;
    struct pcaio_task *t;

    /* create a new task*/
    va_start(args, argc);
    t = task_vnew(id, func, argc, args);
    va_end(args);

    if (pcaio_task_schedule(t)) {
        task_free(t);
        return NULL;
    }

    return t;
}


struct pcaio_task *
pcaio_task_new(const char *id, pcaio_entrypoint_t func, int argc, ...) {
    va_list args;
    struct pcaio_task *t;

    /* create a new task*/
    va_start(args, argc);
    t = task_vnew(id, func, argc, args);
    va_end(args);

    return t;
}


int
pcaio_task_free(struct pcaio_task *t) {
    // FIXME: do something! to avoid additional function call.
    return task_free(t);
}


void
pcaio_task_relax() {
    struct worker *worker = threadlocalworker_get();
    struct pcaio_task *task = worker->currenttask;
    task->status = TS_RELAXING;
    // asm volatile("": : :"memory");
    // worker->currenttask = NULL;

    if (swapcontext(&(task->context), &worker->maincontext)) {
        FATAL("swapcontext to main");
    }
}


int
pcaio() {
    if (_pcaio == NULL) {
        errno = EINVAL;
        ERROR("call pcaio_init() once before the %s().", __func__);
        return -1;
    }

    if (threadpool_minimum(&_pcaio->pool)) {
        errno = ENOMEM;
        ERROR("threadpool_startall");
        return -1;
    }

    DEBUG("watchdog.......");
    sleep(5);
    // unsigned int i;
    // for (;;i++) {
    // }

    return threadpool_cancelall(&_pcaio->pool);
}
