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
#include "pcaio/threadlocalT.h"


struct worker *
worker_new();


int
worker_free(struct worker *w);


int
worker_loop(struct worker *w);


#endif  // CORE_WORKER_H_
