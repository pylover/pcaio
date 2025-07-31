/* standard */
/* system */
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/core.h"

/* local private */


int
worker(int argc, char *argv[]) {
    return -1;
}


void
test_api() {
    struct pcaio *p;
    struct pcaio_task *t;

    p = pcaio_new();
    isnotnull(p);

    t = pcaio_spawn(p, worker, 2, (long)"foo", 5);
    isnotnull(t);

    eqint(0, pcaio_await(t));
    eqint(0, pcaio_free(p));
}


int
main() {
    test_api();
}
