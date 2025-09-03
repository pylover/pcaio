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
#ifndef INCLUDE_PCAIO_RINGT_H_
#define INCLUDE_PCAIO_RINGT_H_


/* generic stuff (must included once) */
#define RNAME_PASTE(x, y) x ## y
#define RNAME_EVAL(x, y)  RNAME_PASTE(x, y)
#define RNAME(n) RNAME_EVAL(T, n)
#define RELTYP() RNAME(_t)


#define RINGT_USED(r) (((r)->tail - (r)->head) & (r)->mask)
#define RINGT_AVAIL(r) (((r)->head - (r)->tail - 1) & (r)->mask)
#define RINGT_ISEMPTY(r) (RINGT_USED(r) == 0)
#define RINGT_ISFULL(r) (RINGT_AVAIL(r) == 0)


#endif  // INCLUDE_PCAIO_RINGT_H_


/**
 *  h   t
 * 01234567 tail
 *  ....
 *  ht
 * 01234567 tail
 */
struct RNAME(ring) {
    int head;
    int tail;
    int mask;
    pthread_mutex_t headx;
    pthread_mutex_t tailx;

    RELTYP() **backend;
};


int
RNAME(ring_init)(struct RNAME(ring) *r, unsigned char bits);


int
RNAME(ring_deinit)(struct RNAME(ring) *r);


int
RNAME(ring_push)(struct RNAME(ring) *r, RELTYP() *e);


int
RNAME(ring_pop)(struct RNAME(ring) *r, RELTYP() **o);
