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


/* local private */
#include "thread.h"

/* local public */
#include "pcaio/pcaio.h"


struct thread {
    thread_t id;
    atomic_bool cancel;
};


struct threadpool {
    thread_start_t starter;
    unsigned short min;
    unsigned short max;
    unsigned short count;
    struct thread *threads;
};


#define threadpool_minimum(tp) threadpool_tune(tp, (tp)->min)
#define threadpool_maximum(tp) threadpool_tune(tp, (tp)->max)


int
threadpool_init(struct threadpool *p, struct pcaio_config *c,
        thread_start_t starter);


int
threadpool_deinit(struct threadpool *p);


int
threadpool_tune(struct threadpool *p, unsigned short count);


int
threadpool_cancelall(struct threadpool *tp);


#endif  // CORE_THREADPOOL_H_
