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
#include <stdint.h>  // NOLINT


int
QNAME(queue_init) (struct QNAME(queue) *q) {
    if (q == NULL) {
        return -1;
    }

    if (mutex_init(&q->mutex) != thrd_success) {
        return -1;
    }

    q->head = NULL;
    q->tail = NULL;
    return 0;
}


void
QNAME(queue_deinit) (struct QNAME(queue) *q) {
    mutex_deinit(&q->mutex);
    q->head = NULL;
    q->tail = NULL;
}


void
QNAME(queue_push) (struct QNAME(queue) *q, QELTYP() *v) {
    mutex_acquire(&q->mutex);

    if (q->tail == NULL) {
        q->head = v;
        q->tail = v;
    }
    else {
        q->tail->next = v;
        q->tail = v;
    }

    v->next = NULL;

    mutex_release(&q->mutex);
}


int
QNAME(queue_pop) (struct QNAME(queue) *q, QELTYP() **out) {
    int ret = 0;
    QELTYP() *o;

    mutex_acquire(&q->mutex);
    if (q->head == NULL) {
        ret = -1;
        goto done;
    }

    o = q->head;

    if (o->next == NULL) {
        q->head = NULL;
        q->tail = NULL;
    }
    else {
        q->head = o->next;
        o->next = NULL;
    }

    *out = o;

done:
    mutex_release(&q->mutex);

    return ret;
}
