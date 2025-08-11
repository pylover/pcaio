/* standard */
/* system */
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/core.h"


#undef queueT
#define queueT int
#include "pcaio/queueT.h"
#include "pcaio/queueT.c"


void
test_queueT() {
    struct intqueue *q = intqueue_create(3);
    isnotnull(q);
    eqint(0, intqueue_dispose(q));
}


int
main() {
    test_queueT();
    return EXIT_SUCCESS;
}
