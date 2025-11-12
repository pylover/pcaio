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
#include <stdbool.h>
#include <errno.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "task.h"
#include "context.h"
#include "master.h"

/* local public */
#include "pcaio/pcaio.h"


static void
_signal(int sig) {
    WARN("signal received: %d", sig);
    state.cancel = true;
}


int
pcaio_schedule(struct pcaio_task *t) {
    if (t == NULL) {
        return -1;
    }

    return taskqueue_push(&state.taskq, t);
}


struct pcaio_task *
pcaio_fschedule(pcaio_taskmain_t func, int *status, int argc, ...) {
    va_list args;
    struct pcaio_task *t;

    /* create a new task*/
    va_start(args, argc);
    t = task_new(func, status, argc, args);
    va_end(args);

    if (t == NULL) {
        return NULL;
    }

    if (taskqueue_push(&state.taskq, t)) {
        task_free(t);
        return NULL;
    }

    return t;
}


struct pcaio_task *
pcaio_task_new(pcaio_taskmain_t func, int *status, int argc, ...) {
    va_list args;
    struct pcaio_task *t;

    /* create a new task*/
    va_start(args, argc);
    t = task_new(func, status, argc, args);
    va_end(args);

    return t;
}


void
pcaio_task_callback(struct pcaio_task *t, pcaio_taskcb_t cb) {
    t->callback = cb;
}


int
pcaio_task_free(struct pcaio_task *t) {
    if (t == NULL) {
        return -1;
    }

    task_free(t);
    return 0;
}


struct pcaio_task *
pcaio_self() {
    struct pcaio_task *t;

    t = threadlocaltask_get();
    if (t == NULL) {
        /* there is no active task inside the thread specific storage, a
         * wierd situtation. ignoring it silenthly, fatal or at least a
         * warning?
         */
        WARN("thread's local active task is null.");
        return NULL;
    }

    return t;
}


/** this function will be called from worker threads.
 */
int
pcaio_relaxA(int flags) {
    ucontext_t *ctx;
    struct pcaio_task *t;

    /* retrieve the thread local task */
    t = threadlocaltask_get();
    if (t == NULL) {
        /* there is no active task inside the thread specific storage, a
         * wierd situtation. ignoring it silenthly, fatal or at least a
         * warning?
         */
        ERROR("trying to feed on a thread which it's local task is null");
        return -1;
    }

    /* retrieve the thread local ucontext and task */
    ctx = threadlocalucontext_get();
    if (ctx == NULL) {
        ERROR("trying to feed on a thread which it's local context is null");
        return -1;
    }

    /* task flags */
    t->flags |= flags;

    /* hollaaa, do the magic! */
    if (swapcontext(&(t->context), ctx)) {
        ERROR("out of memory for swapcontext(3)");
        return -1;
    }

    errno = 0;
    return 0;
}


int
pcaio_module_install(struct pcaio_module *m) {
    if (state.modulescount == CONFIG_PCAIO_MODULES_MAX) {
        return -1;
    }

    m->flags = 0;
    state.modules[state.modulescount++] = m;
    return 0;
}


int
pcaio(unsigned short workers, struct pcaio_task *tasks[],
        unsigned short count) {
    int masterstatus;
    struct sigaction sigact;

    if (count < 1) {
        ERROR("at least one task must be provided");
        return -1;
    }

    if (master_init(workers)) {
        ERROR("master_init");
        return -1;
    }

    /* catch signal(s) */
    sigact.sa_handler = _signal;
    sigact.sa_flags = 0;
    if (sigemptyset(&sigact.sa_mask)) {
        ERROR("sigemptyset");
        return -1;
    }

    if (sigaction(SIGINT, &sigact, NULL)) {
        ERROR("sigaction");
        return -1;
    }

    if (taskqueue_pushall(&state.taskq, tasks, count)) {
        ERROR("taskqueue_pushall");
        return -1;
    }

    masterstatus = master();
    master_deinit();
    return masterstatus;
}
