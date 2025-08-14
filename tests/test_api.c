/* standard */
/* system */
/* thirdparty */
#include <cutest.h>
#include <clog.h>

/* local public */
#include "pcaio/core.h"

/* local private */


int hits = 0;
int logs[6];


int
worker(int argc, char *argv[]) {
    int i = strlen(argv[0]);

    logs[hits++] = i;
    pcaio_task_relax();

    i *= 3;
    logs[hits++] = i;
    pcaio_task_relax();

    i += 7;
    logs[hits++] = i;
    return i;
}


void
test_api() {
    struct pcaio *p;

    p = pcaio_new(NULL);
    isnotnull(p);

    isnotnull(pcaio_task("w #1", (pcaio_entrypoint_t)worker, 1, "foo"));
    isnotnull(pcaio_task("w #2", (pcaio_entrypoint_t)worker, 1, "thud"));

    eqint(0, pcaio(p));
    eqint(0, pcaio_free(p));
    eqint(6, hits);
    eqint(3, logs[0]);
    eqint(4, logs[1]);
    eqint(9, logs[2]);
    eqint(12, logs[3]);
    eqint(16, logs[4]);
    eqint(19, logs[5]);
}


int
main() {
    test_api();
}
