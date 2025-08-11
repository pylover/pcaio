/* standard */
/* system */
/* thirdparty */
#include <cutest.h>

/* local public */
#include "pcaio/core.h"


#undef queue_t
#define queue_t int
#include "pcaio/queueT.h"
#include "pcaio/queueT.c"


void
test_queueT() {
    int out;

    /* create */
    struct intqueue *q = intqueue_create(3);
    isnotnull(q);
    eqint(0, q->head);
    eqint(0, q->tail);

    /* let's play with the Sheldon Cooper's favorite number. */
    eqint(6, intqueue_push(q, 73));
    eqint(0, intqueue_pop(q, &out));
    eqint(73, out);
    eqint(1, q->head);
    eqint(1, q->tail);
    eqint(0, QUEUET_COUNT(q));
    eqint(7, QUEUET_AVAIL(q));

    for (int i = 1; i < 0xF; i++) {
        eqint(QUEUET_AVAIL(q), intqueue_push(q, i));
        eqint(0, intqueue_pop(q, &out));
        eqint(i, out);
    }

    /* freeup */
    eqint(0, intqueue_dispose(q));
}


int
main() {
    test_queueT();
    return EXIT_SUCCESS;
}
