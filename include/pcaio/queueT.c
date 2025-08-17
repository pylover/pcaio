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
#include <stdint.h>  // NOLINT


struct QUEUET_NAME(queue) *
QUEUET_NAME(queue_create) (unsigned char bits) {
    struct QUEUET_NAME(queue) *q;
    size_t size;
    uint32_t mask;

    /* calculate the mask & memory size */
    mask = UINT32_MAX >> (32 - bits);
    size = sizeof(struct QUEUET_NAME(queue)) + sizeof(queue_t) * (mask + 1);

    /* allocate memory */
    q = malloc(size);
    if (q == NULL) {
        return NULL;
    }

    /* initialize it */
    memset(q, 0, size);
    q->mask = mask;
    return q;
}


/** push an item into the queue's tail.
 *
 * return the count of remaining free
 * slots. and -1 if queue is full.
 *
 *
 * head OOOOO tail
 */
int
QUEUET_NAME(queue_push) (struct QUEUET_NAME(queue) *q, queue_t v,
        int reserved) {
    int avail = QUEUET_AVAIL(q) - reserved;

    if (avail < 1) {
        return -1;
    }

    q->blob[q->tail] = v;
    q->tail = (q->tail + 1) & q->mask;
    return avail - 1;
}


/** pop an item from the queue's head.
 *
 * return the count of items inside the queue and -1 on error.
 */
int
QUEUET_NAME(queue_pop) (struct QUEUET_NAME(queue) *q, queue_t *out) {
    int count;

    if (out == NULL) {
        return -1;
    }

    count = QUEUET_COUNT(q);
    if (count == 0) {
        return -1;
    }

    *out = q->blob[q->head];
    q->head = (q->head + 1) & q->mask;
    return count - 1;
}


int
QUEUET_NAME(queue_dispose) (struct QUEUET_NAME(queue) *q) {
    if (q == NULL) {
        return -1;
    }

    free(q);
    return 0;
}
