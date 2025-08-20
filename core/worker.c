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

/* thirdparty */
#include <clog.h>

/* local private */
#include "config.h"
#include "context.h"
#include "master.h"
#include "task.h"
#include "worker.h"


static int
_stepforward(struct pcaio_task *t, ucontext_t *landing) {
    if ((t->status == TS_NAIVE) && (task_createcontext(t, landing))) {
        return -1;
    }

    threadlocaltask_set(t);
    if (swapcontext(landing, &t->context)) {
        FATAL("swapcontext to task");
    }

    /* after working a bit on task */
    if (t->status == TS_TERMINATING) {
        task_free(t);
        return 1;
    }

    return 0;
}


/** spawns and start a new thread for the worker's loop. sets the tidout and
 * then immediately returns 0 if enrything was ok. otherwise -1.
 */
int
worker(atomic_bool *cancel) {
    static ucontext_t landing;
    struct pcaio_task *t;
    thread_t tid;

    if (!atomic_is_lock_free(cancel)) {
        FATAL("atomic_bool is not lockfree!");
    }

    if (threadlocalucontext_set(&landing)) {
        FATAL("threadlocalucontext_set");
    }

    tid = thrd_current();
    while (!atomic_load(cancel)) {
        /* get a task from master to work on */
        t = master_assign();
        if (t == NULL) {
            /* task queue is empty, goto deep sleep and continue to pop the
             * next task after wakeup */
            sleep(CONFIG_PCAIO_WORKER_DEEPSLEEP_S);
            continue;
        }

        /* work on as short as possible */
        if (_stepforward(t, &landing)) {
            /* task is terminated and disposed. so, do not schedule it again
             * and continue to pop the next task to execute */
            continue;
        }

        /* re-schedule the task */
        master_report(t);
    }

    INFO("worker: %lu, canceled", tid);
    threadlocaltask_delete();
    threadlocalucontext_delete();
    return -1;
}
