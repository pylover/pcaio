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

    if (condition_init(&q->condition)) {
        mutex_deinit(&q->mutex);
        return -1;
    }

    q->head = NULL;
    q->tail = NULL;
    return 0;
}


void
QNAME(queue_deinit) (struct QNAME(queue) *q) {
    mutex_deinit(&q->mutex);
    condition_deinit(&q->condition);
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

    condition_signal(&q->condition);
    mutex_release(&q->mutex);
}


void
QNAME(queue_pushall) (struct QNAME(queue) *q, QELTYP() *v[], size_t count) {
    int i;

    /* some guards */
    if (count == 0) {
        return;
    }

    /* take the ownership */
    mutex_acquire(&q->mutex);

    if (q->tail == NULL) {
        /* queue is empty */
        q->tail = v[0];
        q->head = v[0];
    }

    /* chain the rest */
    for (i = 1; i < count, i++) {
        q->tail->next = v[i];
        q->tail = v[i];
    }

    q->tail->next = NULL;

    condition_broadcast(&q->condition);
    mutex_release(&q->mutex);
}


int
QNAME(queue_waitpop) (struct QNAME(queue) *q, QELTYP() **out) {
    int ret = 0;
    QELTYP() *o;

    mutex_acquire(&q->mutex);
    if ((q->head == NULL) && (condition_wait(&q->condition, &q->mutex))) {
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
