#ifndef CORE_TASK_H_
#define CORE_TASK_H_


/* standard */
#include <ucontext.h>

/* local private */
typedef struct pcaio_task * task;
#undef queue_t
#define queue_t task
#include "pcaio/queueT.h"

/* local public */
#include "pcaio/pcaio.h"


enum taskstatus {
    TS_NAIVE,
    TS_ARTFUL,
    TS_RELAXING,
    TS_TERMINATING,
};


struct pcaio_task {
    const char *id;
    size_t stacksize;
    struct ucontext_t context;
    enum taskstatus status;
    pcaio_entrypoint_t func;
    int argc;
    void *argv[];
};


struct pcaio_task *
task_vnew(const char *id, pcaio_entrypoint_t func, int argc, va_list args);


int
task_createcontext(struct pcaio_task *t, ucontext_t *successor);


int
task_free(struct pcaio_task *t);


#endif  // CORE_TASK_H_
