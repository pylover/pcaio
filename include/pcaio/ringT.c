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
#include <stdio.h>  // NOLINT

/* posix */
#include <pthread.h>


int
RNAME(ring_init)(struct RNAME(ring) *r, unsigned char bits) {
    int size;
    RELTYP() **b;

    if (r == NULL) {
        return -1;
    }

    if (bits > 32) {
        return -1;
    }

    size = 1 << bits;
    b = malloc(sizeof(RELTYP()*) * size);
    if (b == NULL) {
        return -1;
    }

    r->backend = b;
    r->head = 0;
    r->tail = 0;
    r->mask = size - 1;
    pthread_mutex_init(&r->headx, NULL);
    pthread_mutex_init(&r->tailx, NULL);
    return 0;
}


int
RNAME(ring_deinit)(struct RNAME(ring) *r) {
    if (r == NULL) {
        return -1;
    }

    free(r->backend);
    r->backend = NULL;
    return 0;
}


int
RNAME(ring_push)(struct RNAME(ring) *r, RELTYP() *e) {
    if (RINGT_ISFULL(r)) {
        return -1;
    }

    pthread_mutex_lock(&r->tailx);
    r->backend[r->tail] = e;
    r->tail++;
    r->tail = r->tail & r->mask;
    pthread_mutex_unlock(&r->tailx);
    return 0;
}


int
RNAME(ring_pop)(struct RNAME(ring) *r, RELTYP() **o) {
    if (RINGT_ISEMPTY(r)) {
        return -1;
    }

    pthread_mutex_lock(&r->headx);
    *o = r->backend[r->head];
    r->head++;
    r->head = r->head & r->mask;
    pthread_mutex_unlock(&r->headx);
    return 0;
}
