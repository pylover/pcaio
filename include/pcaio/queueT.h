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
/* queueT common definitions */
#ifndef PCAIO_QUEUET_H_


/* generic stuff (must included once) */
#define QUEUET_NAME_PASTER(x, y) x ## y
#define QUEUET_NAME_EVALUATOR(x, y)  QUEUET_NAME_PASTER(x, y)
#define QUEUET_NAME(n) QUEUET_NAME_EVALUATOR(queue_t, n)


/* helper macros */
#define QUEUET_COUNT(q) ((q->tail - q->head) & q->mask)
#define QUEUET_AVAIL(q) ((q->head - q->tail - 1) & q->mask)


#endif  // PCAIO_QUEUET_H_


/* generic<template> definitions */
struct QUEUET_NAME(queue) {
    unsigned char mask;
    int head;
    int tail;
    queue_t blob[];
};


struct QUEUET_NAME(queue) *
QUEUET_NAME(queue_create) (unsigned char bits);


int
QUEUET_NAME(queue_dispose) (struct QUEUET_NAME(queue) *);
