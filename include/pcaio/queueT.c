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


/* standard */
#include <stdint.h>  // NOLINT
#include <errno.h>

/* posix */
#include <pthread.h>


void
QNAME(queue_init) (struct QNAME(queue) *q) {
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->condition, NULL);

    q->head = NULL;
    q->tail = NULL;
}


void
QNAME(queue_deinit) (struct QNAME(queue) *q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->condition);
    q->head = NULL;
    q->tail = NULL;
}


int
QNAME(queue_push) (struct QNAME(queue) *q, QTYPE() *v) {
    if (v->QNAME(queue_next) || (v == q->tail)) {
        return -1;
    }

    pthread_mutex_lock(&q->mutex);
    if (q->tail == NULL) {
        q->head = v;
        q->tail = v;
    }
    else {
        q->tail->QNAME(queue_next) = v;
        q->tail = v;
    }

    v->QNAME(queue_next) = NULL;

    pthread_cond_signal(&q->condition);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}


int
QNAME(queue_pushall) (struct QNAME(queue) *q, QTYPE() *v[], size_t count) {
    int i;

    /* some guards */
    if (count == 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        if (v[i]->QNAME(queue_next) || (v[i] == q->tail)) {
            return -1;
        }
    }

    /* take the ownership */
    pthread_mutex_lock(&q->mutex);

    if (q->tail == NULL) {
        /* queue is empty */
        q->tail = v[0];
        q->head = v[0];
    }

    /* chain the rest */
    for (i = 1; i < count; i++) {
        q->tail->QNAME(queue_next) = v[i];
        q->tail = v[i];
    }

    q->tail->QNAME(queue_next) = NULL;

    pthread_cond_broadcast(&q->condition);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}


int
QNAME(queue_pop) (struct QNAME(queue) *q, QTYPE() **out, int flags) {
    int ret = 0;
    QTYPE() *o;

    pthread_mutex_lock(&q->mutex);
    if (flags & QWAIT) {
        while (q->head == NULL) {
            if (pthread_cond_wait(&q->condition, &q->mutex)) {
                ret = -1;
                goto done;
            }
        }
    }
    else if (q->head == NULL) {
        ret = -1;
        goto done;
    }

    o = q->head;
    if (o->QNAME(queue_next) == NULL) {
        q->head = NULL;
        q->tail = NULL;
    }
    else {
        q->head = o->QNAME(queue_next);
        o->QNAME(queue_next) = NULL;
    }

    *out = o;

done:
    pthread_mutex_unlock(&q->mutex);
    return ret;
}
