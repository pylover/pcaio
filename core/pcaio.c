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


/** pcaio public APIs
 */


/* standard */
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ucontext.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "task.h"
#include "context.h"
#include "master.h"

/* local public */
#include "pcaio/pcaio.h"


static struct pcaio_config
_defaultconfig = {
    .workers_min = 1,
    .workers_max = 1,
};


int
pcaio_init(struct pcaio_config *config) {
    if (config == NULL) {
        config = &_defaultconfig;
    }
    else {
        config = config;
    }

    return master_init(config);
}


int
pcaio_deinit() {
    return master_deinit();
}


int
pcaio_task_schedule(struct pcaio_task *t) {
    if (t == NULL) {
        return -1;
    }
    master_report(t);
    return 0;
}


struct pcaio_task *
pcaio_task_newschedule(pcaio_entrypoint_t func, int argc, ...) {
    va_list args;
    struct pcaio_task *t;

    /* create a new task*/
    va_start(args, argc);
    t = task_new(func, argc, args);
    va_end(args);

    if (t == NULL) {
        return NULL;
    }

    master_report(t);
    return t;
}


struct pcaio_task *
pcaio_task_new(pcaio_entrypoint_t func, int argc, ...) {
    va_list args;
    struct pcaio_task *t;

    /* create a new task*/
    va_start(args, argc);
    t = task_new(func, argc, args);
    va_end(args);

    return t;
}


int
pcaio_task_free(struct pcaio_task *t) {
    if (t == NULL) {
        return -1;
    }

    task_free(t);
    return 0;
}


void
pcaio_task_relax(struct pcaio_task *t) {
    ucontext_t *ctx;

    ctx = threadlocalucontext_get();
    if (ctx == NULL) {
        FATAL("threadlocalucontext_get");
    }

    /* then clear the thread local task to indicate the worker is idle */
    threadlocaltask_set(NULL);

    /* hollaaa, do the magic! */
    if (swapcontext(&(t->context), ctx)) {
        FATAL("swapcontext to main");
    }
}


/** this function will be called from worker threads.
 */
void
pcaio_currenttask_relax() {
    struct pcaio_task *t;

    /* retrieve the thread local ucontext and task */
    t = threadlocaltask_get();
    if (t == NULL) {
        FATAL("threadlocaltask_get");
    }

    pcaio_task_relax(t);
}


static void
_signal(int sig) {
    printf("signal received: %d\n", sig);
    master_cancel();
}


int
pcaio() {
    struct sigaction sigact;
    sigact.sa_handler = _signal;
    sigact.sa_flags = 0;

    if (sigemptyset(&sigact.sa_mask)) {
        ERROR("sigemptyset");
    }

    if (sigaction(SIGINT, &sigact, NULL)) {
        ERROR("sigaction");
        return -1;
    }

    return master();
}
