/* standard */
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <ucontext.h>

/* system */
/* thirdparty */
#include <cutest.h>

/* local private */
#include "task.h"

/* local public */
#include "pcaio/core.h"


struct pcaio {
    const struct pcaio_config *config;
    struct ucontext_t uctx;
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


int
pcaio_schedule(struct pcaio *p, struct pcaio_task *t) {
    return taskqueue_push(p->tasks, t);
}


struct pcaio_task *
pcaio_spawn(struct pcaio *p, const char *id, pcaio_entrypoint_t func,
        int argc, ...) {
    va_list args;
    struct pcaio_task *t;

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

    if (task_createcontext(t, &p->uctx, p->config->task_stacksize)) {
        task_dispose(t);
        return NULL;
    }

    if (pcaio_schedule(p, t) != -1)  {
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
