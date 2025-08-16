// Copyright 2023 Vahid Mardani
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
#include <stdarg.h>
#include <ucontext.h>

/* thirdparty */
#include <cutest.h>
#include <clog.h>

/* local private */
#include "config.h"
#include "core.h"
#include "task.h"
#include "worker.h"

/* local public */
#include "pcaio/pcaio.h"


struct pcaio_config *
pcaio_config_default() {
    struct pcaio_config *c;

    c = malloc(sizeof(struct pcaio_config));
    if (c == NULL) {
        return NULL;
    }

    c->taskqueue_size = 16;
    c->workers = 1;
    return c;
}


struct pcaio *
pcaio_new(struct pcaio_config *config) {
    struct pcaio *p;
    p = malloc(sizeof(struct pcaio));
    if (p == NULL) {
        return NULL;
    }

    if (config == NULL) {
        p->config = pcaio_config_default();
    }
    else {
        p->config = config;
    }

    p->tasks = taskqueue_create(CONFIG_PCAIO_TASKQUEUE_BITS);
    struct worker *worker = worker_new();
    worker->pcaio = p;
    threadlocalworker_set(worker);

    return p;
}


int
pcaio_free() {
    struct pcaio *p;
    struct worker *w = threadlocalworker_get();

    if (w == NULL) {
        return -1;
    }

    p = w->pcaio;
    if (p == NULL) {
        return -1;
    }

    worker_free(w);
    free(p);
    return 0;
}


int
pcaio_task_schedule(struct pcaio_task *t) {
    int avail;
    struct worker *worker = threadlocalworker_get();
    struct pcaio *p = worker->pcaio;

    // FIXME: thread-safe push
    avail = taskqueue_push(p->tasks, t, (int)p->config->workers);
    if (avail == -1) {
        return -1;
    }

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
    struct worker *w = threadlocalworker_get();
    struct pcaio *p = w->pcaio;
    struct pcaio_config *c = p->config;

    for (int i = 1; i < c->workers; i++) {
        // TODO: create thread
    }
    return worker_loop(w);
}
