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
#include <stdbool.h>
#include <errno.h>
#include <ucontext.h>

/* posix */
#include <pthread.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "worker.h"
#include "context.h"
#include "task.h"


static void
_cleanup(struct taskqueue *q) {
    struct pcaio_task *t;

    t = threadlocaltask_get();
    if (t) {
        task_free(t);
        threadlocaltask_set(NULL);
    }

    pthread_mutex_unlock(&q->mutex);
    threadlocaltask_delete();
    threadlocalucontext_delete();
}


int
worker(struct taskqueue *q) {
    int ret;
    int exitstatus = 0;
    ucontext_t landing;
    struct pcaio_task *t;

    /* storing the main thread's context inside the thread specific storage */
    threadlocalucontext_set(&landing);

    /* register the cleanup handler */
    pthread_cleanup_push((void(*)(void*))_cleanup, q);

    /* wait, pop, calculate and start over */
    while (taskqueue_pop(q, &t, QWAIT) == 0) {
        /* update the task's thread local storage, master_currenttask*
         * functions needs it. */
        threadlocaltask_set(t);

        /* switch to task's context */
        ret = swapcontext(&landing, &t->context);
        threadlocaltask_set(NULL);
        if (ret) {
            /* out of memory or something like this. trying to quit to free up
             * some resources. */
            ERROR("insufficient memory for swapcontext(3)");
            exitstatus = ENOMEM;
            goto done;
        }

        if (t->flags & TASK_TERMINATED) {
            /* freeup  */
            task_free(t);

            /* task is terminated and just disposed. so, do not schedule it
             * again and continue to pop the next task to step forward */
            continue;
        }

        if (t->flags & TASK_NOSCHEDULE) {
            /* task is holding by a module. so, do not schedule it until the
             * module do it. */
            t->flags &= ~TASK_NOSCHEDULE;
            continue;
        }

        /* re-schedule the task */
        if (taskqueue_push(q, t)) {
            /*  panic */
            exitstatus = EINVAL;
            goto done;
        }
    }

done:
    pthread_cleanup_pop(true);
    return exitstatus;
}
