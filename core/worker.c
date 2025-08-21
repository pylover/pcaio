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
#include <unistd.h>
#include <ucontext.h>

/* posix */
#include <pthread.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "config.h"
#include "context.h"
#include "master.h"
#include "task.h"
#include "worker.h"


static void
_cleanup(void *) {
    /* master is telling me to die as soon as possible. */
    INFO("worker: 0x%lx is dying...", pthread_self());
    threadlocaltask_delete();
    threadlocalucontext_delete();
}


static int
_stepforward(struct pcaio_task *t, ucontext_t *landing) {
    threadlocaltask_set(t);
    if (swapcontext(landing, &t->context)) {
        FATAL("swapcontext to task");
    }

    /* after working a bit on task */
    if (t->status == TS_TERMINATING) {
        task_free(t);
        master_tasks_decrease();
        /* then clear the thread local task to indicate the worker is idle */
        threadlocaltask_set(NULL);
        return 1;
    }

    return 0;
}


/** spawns and start a new thread for the worker's loop. sets the tidout and
 * then immediately returns 0 if enrything was ok. otherwise -1.
 */
int
worker(struct taskqueue *q) {
    static ucontext_t landing;
    struct pcaio_task *t;

    if (threadlocalucontext_set(&landing)) {
        FATAL("threadlocalucontext_set");
    }

    /* register the cleanup handler */
    pthread_cleanup_push(_cleanup, NULL);

    /* wait, pop, calculate and start over */
    while (taskqueue_pop(q, &t, QWAIT) == 0) {
        /* work on as short as possible */
        if (_stepforward(t, &landing)) {
            /* task is terminated and disposed. so, do not schedule it again
             * and continue to pop the next task to execute */
            continue;
        }

        /* re-schedule the task */
        taskqueue_push(q, t);
    }

    pthread_cleanup_pop(true);
    return 0;
}
