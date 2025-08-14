#ifndef CORE_WORKER_H_
#define CORE_WORKER_H_


/* standard */
#include <ucontext.h>


typedef struct worker {
    struct pcaio *pcaio;
    struct ucontext_t maincontext;
    struct pcaio_task *currenttask;
} worker_t;


#undef TL
#define TL worker
#include "pcaio/threadlocal.h"


struct worker *
worker_create();


int
worker_destroy(struct worker *worker);


#endif  // CORE_WORKER_H_
