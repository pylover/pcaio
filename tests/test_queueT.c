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
    struct foo *next;
} foo_t;

/* local public */
#undef T
#define T foo
#include "pcaio/queueT.h"
#include "pcaio/queueT.c"


void
test_queueT() {
    struct fooqueue q;
    struct foo *out;
    struct foo foo73 = {73};
    struct foo foo74 = {74};
    struct foo foo75 = {75};

    /* create */
    eqint(0, fooqueue_init(&q));
    isnull(q.head);
    isnull(q.tail);

    /* let's play with the Sheldon Cooper's favorite number. */
    fooqueue_push(&q, &foo73);
    isnotnull(q.head);
    isnotnull(q.tail);
    isnull(foo73.next);
    eqptr(q.head, q.tail);
    eqptr(q.head, &foo73);

    eqint(0, fooqueue_pop(&q, &out));
    eqint(73, out->val);
    isnull(q.head);
    isnull(q.tail);
    isnull(out->next);

    /* enqueue more than one item */
    fooqueue_push(&q, &foo73);
    fooqueue_push(&q, &foo74);
    fooqueue_push(&q, &foo75);
    eqptr(q.head, &foo73);
    eqptr(q.tail, &foo75);
    eqptr(foo73.next, &foo74);
    eqptr(foo74.next, &foo75);
    isnull(foo75.next);

    eqint(0, fooqueue_pop(&q, &out));
    eqint(73, out->val);
    eqint(0, fooqueue_pop(&q, &out));
    eqint(74, out->val);
    eqint(0, fooqueue_pop(&q, &out));
    eqint(75, out->val);

    isnull(q.head);
    isnull(q.tail);
    isnull(out->next);
    /* freeup */
    fooqueue_deinit(&q);
}


int
main() {
    test_queueT();
    return EXIT_SUCCESS;
}
