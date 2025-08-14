/* standard */
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <ucontext.h>

/* system */
/* thirdparty */
#include <cutest.h>
#include <clog.h>

/* local private */
#include "config.h"
#include "task.h"
#include "worker.h"

/* local public */
#include "pcaio/core.h"


struct pcaio {
    const struct pcaio_config *config;
    struct taskqueue *tasks;
};


static const struct pcaio_config _defaultconfig = {
    .taskqueue_size = 16,
    .task_stacksize = 2048,
};


struct pcaio *
pcaio_new(const struct pcaio_config *config) {
    struct pcaio *p;
    if (config == NULL) {
        config = &_defaultconfig;
    }

    p = malloc(sizeof(struct pcaio));
    if (p == NULL) {
        return NULL;
    }
    p->config = config;

    p->tasks = taskqueue_create(CONFIG_PCAIO_TASKQUEUE_BITS);
    struct worker *worker = worker_create();
    worker->pcaio = p;
    threadlocalworker_set(worker);

    return p;
}


int
pcaio_free(struct pcaio *p) {
    if (p == NULL) {
        return -1;
    }

    free(p);
    return 0;
}


static int
_schedule(struct pcaio *p, struct pcaio_task *t) {
    int avail;

    // FIXME: thread-safe push
    avail = taskqueue_push(p->tasks, t, (int)p->config->workers);
    if (avail == -1) {
        return -1;
    }

    return 0;
}


struct pcaio_task *
pcaio_task(const char *id, pcaio_entrypoint_t func,
        int argc, ...) {
    va_list args;
    struct pcaio_task *t;
    struct pcaio *p = threadlocalworker_get()->pcaio;

    if (p == NULL) {
        errno = EINVAL;
        return NULL;
    }

    /* create a new task*/
    va_start(args, argc);
    t = task_new(id, func, argc, args);
    va_end(args);

    if (t == NULL) {
        return NULL;
    }

    if (task_createcontext(t, p->config->task_stacksize)) {
        task_dispose(t);
        return NULL;
    }

    if (_schedule(p, t))  {
        task_dispose(t);
        return NULL;
    }

    return t;
}


int
pcaio_await(struct pcaio_task *t) {
    if (t == NULL) {
        errno = EINVAL;
        return -1;
    }

    // TODO: implement
    return -1;
}


static int
_stepforward(struct pcaio_task *task) {
    struct worker *worker = threadlocalworker_get();

    /* delete this condition later, to increase the performance */
    if (worker->currenttask) {
        FATAL("worker->task is not NULL!");
    }

    worker->currenttask = task;
    task->context.uc_link = &worker->maincontext;
    if (swapcontext(&worker->maincontext, &task->context)) {
        FATAL("swapcontext to task");
    }

    if (task->status == TS_TERMINATING) {
        if (task_dispose(task)) {
            FATAL("task_dispose");
        }

        return 1;
    }

    return 0;
}


void
pcaio_task_relax() {
    struct worker *worker = threadlocalworker_get();
    struct pcaio_task *task = worker->currenttask;

    task->status = TS_RELAXING;
    worker->currenttask = NULL;

    if (swapcontext(&task->context, &worker->maincontext)) {
        FATAL("swapcontext to main");
    }
}


int
pcaio_task_schedule(struct pcaio_task *t) {
    struct worker *worker = threadlocalworker_get();

    return _schedule(worker->pcaio, t);
}


int
pcaio_loop(struct pcaio *p) {
    struct pcaio_task *t;

    // FIXME: thread-safe pop
    while (taskqueue_pop(p->tasks, &t) >= 0) {
        if (_stepforward(t)) {
            /* task is terminated and disposed. so, do not schedule it again
             * and continue to pop the next task to execute */
            continue;
        }

        _schedule(p, t);
    }

    return 0;
}


int
pcaio(struct pcaio *p) {
    return pcaio_loop(p);
}
