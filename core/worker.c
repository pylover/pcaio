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
#include "config.h"
#include "context.h"
#include "master.h"
#include "task.h"
#include "worker.h"


static void
_cleanup(struct taskqueue *q) {
    pthread_mutex_unlock(&q->mutex);
    threadlocaltask_delete();
    threadlocalucontext_delete();
}


int
worker(struct taskqueue *q) {
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
        if (swapcontext(&landing, &t->context)) {
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

        /* re-schedule the task */
        taskqueue_push(q, t);
    }

done:
    pthread_cleanup_pop(true);
    return exitstatus;
}
