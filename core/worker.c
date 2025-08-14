/* standard */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ucontext.h>

/* thirdparty */
#include <clog.h>

/* local private */
#include "core.h"
#include "worker.h"
#include "task.h"

/* local public */
#undef TL
#define TL worker
#include "pcaio/threadlocalT.c"


struct worker *
worker_new() {
    struct worker *w = malloc(sizeof(struct worker));
    if (w == NULL) {
        return NULL;
    }

    memset(w, 0, sizeof(struct worker));
    return w;
}


int
worker_free(struct worker *w) {
    if (w == NULL) {
        return -1;
    }

    free(w);
    return 0;
}


static int
_stepforward(struct worker *worker, struct pcaio_task *task) {
    if ((task->status == TS_NAIVE) &&
            // FIXME: maincontext must be thread-local
            (task_createcontext(task, &worker->maincontext))) {
        return -1;
    }

    worker->currenttask = task;
    asm volatile("": : :"memory");

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
worker_loop(struct worker *w) {
    struct taskqueue *tasks = w->pcaio->tasks;
    struct pcaio_task *t;

    // FIXME: thread-safe pop
    while (taskqueue_pop(tasks, &t) >= 0) {
        if (_stepforward(w, t)) {
            /* task is terminated and disposed. so, do not schedule it again
             * and continue to pop the next task to execute */
            continue;
        }

        // FIXME: thread-safe push
        /* re-schedule the task */
        taskqueue_push(tasks, t, 0);
    }

    return 0;
}
