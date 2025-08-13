/* standard */
#include <stdlib.h>
#include <ucontext.h>

/* local private */
#include "worker.h"


#undef TL
#define TL worker
#include "pcaio/threadlocal.c"


struct worker *
worker_create() {
    struct worker *w = malloc(sizeof(struct worker));
    if (w == NULL) {
        return NULL;
    }

    // TODO: backup context?
    return w;
}


int
worker_destroy(struct worker *worker) {
    if (worker == NULL) {
        return -1;
    }

    free(worker);
    return 0;
}
