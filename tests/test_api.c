/* standard */
/* system */
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/core.h"

/* local private */


static volatile int ret = 0;


int
worker(int argc, void *argv[]) {
    int i;

    i = 4;
    pcaio_task_relax();

    i *= 3;
    pcaio_task_relax();

    ret = i;
    return i;
}


void
test_api() {
    struct pcaio *p;
    struct pcaio_task *t;

    p = pcaio_new(NULL);
    isnotnull(p);

    t = pcaio_task("worker#1", worker, 2, "foo", "bar");
    isnotnull(t);

    eqint(0, pcaio(p));
    eqint(0, pcaio_free(p));
    eqint(12, ret);
}


int
main() {
    test_api();
}
