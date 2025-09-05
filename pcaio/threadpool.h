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
#ifndef CORE_THREADPOOL_H_
#define CORE_THREADPOOL_H_

/* standard */
#include <stdatomic.h>

/* posix */
#include <pthread.h>

/* local private */
#include "worker.h"

/* local public */
#include "pcaio/pcaio.h"


struct threadpool {
    worker_t starter;
    struct taskqueue *taskq;
    atomic_ushort count;
    pthread_t *threads;
    pthread_mutex_t mutex;
};


int
threadpool_init(struct threadpool *tp, unsigned short maxworkers,
        worker_t starter, struct taskqueue *q);


int
threadpool_deinit(struct threadpool *tp);


int
threadpool_scale(struct threadpool *p, unsigned short count);


int
threadpool_killall(struct threadpool *p, int sig);


#endif  // CORE_THREADPOOL_H_
