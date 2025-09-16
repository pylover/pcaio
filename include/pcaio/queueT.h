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
#define QNAME(n) QNAME_EVAL(S, n)


/* flags */
#define QWAIT 0x1


#endif  // INCLUDE_PCAIO_QUEUET_H_


/* generic<template> definitions */
struct QNAME() {
    T *head;
    T *tail;
    pthread_cond_t condition;
    pthread_mutex_t mutex;
};


void
QNAME(_init) (struct QNAME() *q);


void
QNAME(_deinit) (struct QNAME() *q);


int
QNAME(_push) (struct QNAME() *q, T *v);


int
QNAME(_pushall) (struct QNAME() *q, T *v[], size_t count);


int
QNAME(_pop) (struct QNAME() *q, T **out, int flags);
