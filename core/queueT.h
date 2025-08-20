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
#ifndef CORE_QUEUET_H_
#define CORE_QUEUET_H_


#include "pcaio/mutex.h"


/* generic stuff (must included once) */
#define QNAME_PASTE(x, y) x ## y
#define QNAME_EVAL(x, y)  QNAME_PASTE(x, y)
#define QNAME(n) QNAME_EVAL(T, n)
#define QELTYP() QNAME(_t)


#endif  // CORE_QUEUET_H_


/* generic<template> definitions */
struct QNAME(queue) {
    QELTYP() *head;
    QELTYP() *tail;
    mutex_t mutex;
};


int
QNAME(queue_init) (struct QNAME(queue) *q);


void
QNAME(queue_deinit) (struct QNAME(queue) *q);


void
QNAME(queue_push) (struct QNAME(queue) *q, QELTYP() *v);


int
QNAME(queue_pop) (struct QNAME(queue) *q, QELTYP() **out);
