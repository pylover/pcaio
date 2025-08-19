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

/* thirdparty */
#include <clog.h>

/* local private */
#include "core.h"
#include "task.h"
#include "worker.h"

/* local public */
#undef TL
#define TL worker
#include "pcaio/threadlocalT.c"


static int
_stepforward(struct worker *worker, struct pcaio_task *task) {
    if ((task->status == TS_NAIVE) &&
            // FIXME: maincontext must be thread-local
            (task_createcontext(task, &worker->maincontext))) {
        return -1;
    }

    worker->currenttask = task;
    // asm volatile("": : :"memory");

    if (swapcontext(&worker->maincontext, &task->context)) {
        FATAL("swapcontext to task");
    }

    if (task->status == TS_TERMINATING) {
        if (task_free(task)) {
            FATAL("task_free");
        }

        return 1;
    }

    return 0;
}


int
worker_sigterm(thread_t tid) {
    // TODO: send signal
    return 0;
}


// static int
// _loop(struct worker *w) {
//     struct taskqueue *tasks = &w->pcaio->tasks;
//     struct pcaio_task *t;
//
//     while (taskqueue_pop(tasks, &t) >= 0) {
//         if (_stepforward(w, t)) {
//             /* task is terminated and disposed. so, do not schedule it again
//              * and continue to pop the next task to execute */
//             continue;
//         }
//
//         /* re-schedule the task */
//         taskqueue_push(tasks, t);
//     }
//
//     // TODO: cleanup the whole worker and thread
//     return 0;
// }


/** spawns and start a new thread for the worker's loop. sets the tidout and
 * then immediately returns 0 if enrything was ok. otherwise -1.
 */
int
worker(struct taskqueue *q) {
    // w = malloc(sizeof(struct worker));
    // if (w == NULL) {
    //     return -1;
    // }

    // memset(w, 0, sizeof(struct worker));
    // return 0;
    return -1;
}
