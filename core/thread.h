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
#ifndef CORE_THREAD_H_
#define CORE_THREAD_H_


/* standard */
#include <threads.h>
#include <stdatomic.h>


typedef thrd_t thread_t;
typedef int (*thread_start_t) (struct taskqueue *q);


int
thread_new(thread_t *tid, thread_start_t f, void *arg);


int
thread_join(thread_t tid, int *status);


#endif  // CORE_THREAD_H_
