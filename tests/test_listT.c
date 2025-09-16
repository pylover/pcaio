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
    struct foo *foolist_next;
    struct foo *foolist_prev;
} foo_t;


/* local public */
#undef T
#define T foo
#include "pcaio/listT.h"
#include "pcaio/listT.c"


static void
test_listT() {
    struct foo first = {7, NULL, NULL};
    struct foo second = {8, NULL, NULL};
    struct foo third = {9, NULL, NULL};
    struct foolist l = {NULL, NULL, 0};

    foolist_init(&l);
    isnull(l.head);
    isnull(l.tail);
    eqint(0, l.count);

    eqint(0, foolist_append(&l, &first));
    isnotnull(l.head);
    isnotnull(l.tail);
    isnull(first.foolist_prev);
    isnull(first.foolist_next);
    eqint(1, l.count);

    eqint(0, foolist_append(&l, &second));
    isnotnull(l.head);
    isnotnull(l.tail);
    eqptr(&first, second.foolist_prev);
    eqptr(&second, first.foolist_next);
    isnull(second.foolist_next);
    eqint(2, l.count);

    eqint(0, foolist_append(&l, &third));
    eqint(3, l.count);

    eqint(0, foolist_delete(&l, &third));
    eqptr(&first, l.head);
    eqptr(&second, l.tail);
    isnull(second.foolist_next);
    isnull(third.foolist_prev);
    isnull(third.foolist_next);
    eqint(2, l.count);

    eqint(0, foolist_append(&l, &third));
    eqint(0, foolist_delete(&l, &second));
    eqptr(&first, l.head);
    eqptr(&third, l.tail);
    eqptr(first.foolist_next, &third);
    eqptr(&first, third.foolist_prev);
    isnull(third.foolist_next);
    isnull(second.foolist_prev);
    isnull(second.foolist_next);
    eqint(2, l.count);

    eqint(0, foolist_delete(&l, &first));
    eqptr(&third, l.head);
    eqptr(&third, l.tail);
    isnull(third.foolist_next);
    isnull(third.foolist_prev);
    isnull(first.foolist_next);
    isnull(first.foolist_prev);
    eqint(1, l.count);

    eqint(0, foolist_delete(&l, &third));
    isnull(l.head);
    isnull(l.tail);
    isnull(third.foolist_next);
    isnull(third.foolist_prev);
    eqint(0, l.count);

    foolist_deinit(&l);
}


int
main() {
    test_listT();
    return EXIT_SUCCESS;
}
