// Copyright 2025 Vahid Mardani
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


typedef struct foo {
    int val;
} foo_t;


/* local public */
#undef T
#define T foo
#include "pcaio/ringT.h"
#include "pcaio/ringT.c"


void
test_ringT() {
    struct foo *tmp;
    struct foo first = {1};
    struct foo second = {2};
    struct foo third = {3};
    struct foo other = {99};
    struct fooring r;

    /* create */
    fooring_init(&r, 2);
    eqint(0, r.head);
    eqint(0, r.tail);
    eqint(3, r.mask);
    eqint(0, RINGT_USED(&r));
    eqint(3, RINGT_AVAIL(&r));

    eqint(0, fooring_push(&r, &first));
    eqint(0, r.head);
    eqint(1, r.tail);

    eqint(0, fooring_push(&r, &second));
    eqint(0, fooring_push(&r, &third));
    eqint(0, r.head);
    eqint(3, r.tail);

    eqint(-1, fooring_push(&r, &other));
    eqint(0, fooring_pop(&r, &tmp));
    eqptr(&first, tmp);

    eqint(0, fooring_push(&r, &other));
    eqint(0, fooring_pop(&r, &tmp));
    eqptr(&second, tmp);
    eqint(0, fooring_pop(&r, &tmp));
    eqptr(&third, tmp);

    /* freeup */
    fooring_deinit(&r);
}


int
main() {
    test_ringT();
    return EXIT_SUCCESS;
}
