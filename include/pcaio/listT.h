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
#ifndef INCLUDE_PCAIO_LISTT_H_
#define INCLUDE_PCAIO_LISTT_H_


/* posix */
#include <pthread.h>


/* generic stuff (must included once) */
#define LNAME_PASTE(x, y) x ## y
#define LNAME_EVAL(x, y)  LNAME_PASTE(x, y)
#define LNAME(n) LNAME_EVAL(T, n)
#define LTYPE() LNAME_EVAL(T, _t)


#endif  // INCLUDE_PCAIO_LISTT_H_


/* generic<template> definitions */
struct LNAME(list) {
    LTYPE() *head;
    LTYPE() *tail;
    size_t count;
    pthread_mutex_t mutex;
};


void
LNAME(list_init) (struct LNAME(list) *q);


void
LNAME(list_deinit) (struct LNAME(list) *q);


int
LNAME(list_append) (struct LNAME(list) *q, LTYPE() *v);


int
LNAME(list_delete) (struct LNAME(list) *q, LTYPE() *v);


// int
// LNAME(_pushall) (struct LNAME() *q, T *v[], size_t count);
//
//
// int
// LNAME(_pop) (struct LNAME() *q, T **out, int flags);
