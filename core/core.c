/* standard */
#include <stdlib.h>
#include <errno.h>

/* system */
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/core.h"

/* local private */


struct pcaio *
pcaio_new() {
    struct pcaio *p;

    p = malloc(sizeof(struct pcaio));
    if (p == NULL) {
        return NULL;
    }

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


struct pcaio_task *
pcaio_spawn(struct pcaio *p, pcaio_taskentry_t entry, int argc, ...) {
    // TODO: implement
    return NULL;
}


int
pcaio_await(struct pcaio_task *task) {
    if (task == NULL) {
        errno = EINVAL;
        return -1;
    }

    // TODO: implement
    return -1;
}
