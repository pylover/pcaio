/* standard */
/* system */
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/core.h"

/* local private */


int
worker(int argc, void *argv[]) {
    return -1;
}


void
test_api() {
    struct pcaio *p;
    struct pcaio_task *t;

    p = pcaio_new(NULL);
    isnotnull(p);

    t = pcaio_spawn(p, "worker#1", worker, 2, "foo", "bar");
    isnotnull(t);

    // eqint(0, pcaio_await(t));
    eqint(0, pcaio_free(p));
}


int
main() {
    test_api();
}
