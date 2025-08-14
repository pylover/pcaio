// Copyright 2023 Vahid Mardani
/*
 * This file is part of pcaio.
 *  pcaio is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  pcaio is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with pcaio. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
/* thirdparty */
#include <cutest.h>

/* local public */
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
    eqint(6, intqueue_push(q, 73, 0));
    eqint(0, intqueue_pop(q, &out));
    eqint(73, out);
    eqint(1, q->head);
    eqint(1, q->tail);
    eqint(0, QUEUET_COUNT(q));
    eqint(7, QUEUET_AVAIL(q));

    for (int i = 1; i < 0xF; i++) {
        eqint(QUEUET_AVAIL(q), intqueue_push(q, i, 0));
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
