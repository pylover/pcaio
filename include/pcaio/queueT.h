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
#ifndef INCLUDE_PCAIO_QUEUET_H_
#define INCLUDE_PCAIO_QUEUET_H_


/* posix */
#include <pthread.h>


/* generic stuff (must included once) */
#define QNAME_PASTE(x, y) x ## y
#define QNAME_EVAL(x, y)  QNAME_PASTE(x, y)
#define QNAME(n) QNAME_EVAL(T, n)
#define QTYPE() QNAME_EVAL(T, _t)


/* flags */
#define QWAIT 0x1


#endif  // INCLUDE_PCAIO_QUEUET_H_


/* generic<template> definitions */
struct QNAME(queue) {
    QTYPE() *head;
    QTYPE() *tail;
    pthread_cond_t condition;
    pthread_mutex_t mutex;
};


void
QNAME(queue_init) (struct QNAME(queue) *q);


void
QNAME(queue_deinit) (struct QNAME(queue) *q);


int
QNAME(queue_push) (struct QNAME(queue) *q, QTYPE() *v);


int
QNAME(queue_pushall) (struct QNAME(queue) *q, QTYPE() *v[], size_t count);


int
QNAME(queue_pop) (struct QNAME(queue) *q, QTYPE() **out, int flags);
