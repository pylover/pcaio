#ifndef CORE_TASK_H_
#define CORE_TASK_H_


#include "pcaio/core.h"


struct pcaio_task *
task_new(const char *id, pcaio_entrypoint_t func, int argc, va_list args);


int
task_createcontext(struct pcaio_task *t, struct ucontext_t *parent,
        size_t size);


int
task_dispose(struct pcaio_task *t);


#endif  // CORE_TASK_H_
